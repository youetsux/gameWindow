#include "Fbx.h"
#include "Direct3D.h"
#include "Camera.h"
#include "ufbx.h"
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <string_view>

Fbx::~Fbx()
{
    Release();
}

void Fbx::Release()
{
    if (pConstantBuffer_) { pConstantBuffer_->Release(); pConstantBuffer_ = nullptr; }
    if (pIndexBuffer_) { pIndexBuffer_->Release(); pIndexBuffer_ = nullptr; }
    if (pVertexBuffer_) { pVertexBuffer_->Release(); pVertexBuffer_ = nullptr; }
    materials_.clear();
    vertices_.clear();
    indices_.clear();
}

HRESULT Fbx::Load(const std::string& fileName)
{
    // 再読み込みのときは、前のデータを先に解放する。
    Release();

    ufbx_load_opts options = {};
    options.target_axes = ufbx_axes_left_handed_y_up;
    options.target_unit_meters = 1.0; // 1単位を1mに揃える
    ufbx_error error = {};
    ufbx_scene* scene = ufbx_load_file(fileName.c_str(), &options, &error);
    if (!scene)
    {
        char message[1024];
        ufbx_format_error(message, sizeof(message), &error);
        OutputDebugStringA(message);
        OutputDebugStringA("\n");
        return E_FAIL;
    }

    // 子の先頭がメッシュとは限らない。全ノードから探す。
    ufbx_node* meshNode = nullptr;
    size_t meshNodeCount = 0;
    for (size_t i = 0; i < scene->nodes.count; i++)
    {
        ufbx_node* node = scene->nodes.data[i];
        if (node->mesh)
        {
            meshNode = node;
            meshNodeCount++;
        }
    }

    // この授業では、メッシュを持つノードが1つのFBXを扱う。
    if (meshNodeCount != 1)
    {
        OutputDebugStringA("This lesson requires exactly one mesh node.\n");
        ufbx_free_scene(scene);
        return E_FAIL;
    }

    ufbx_mesh* mesh = meshNode->mesh;
    char message[256];
    sprintf_s(message, "FBX: vertices=%zu faces=%zu triangles=%zu\n",
        mesh->num_vertices, mesh->num_faces, mesh->num_triangles);
    OutputDebugStringA(message);

    HRESULT hr = S_OK;
    hr = ReadVertices(meshNode);
    if (SUCCEEDED(hr)) hr = ReadIndices(meshNode);
    if (SUCCEEDED(hr)) ReadTextures(meshNode, fileName);

    // 必要な値を自分のvectorへコピーした後で、ufbxのデータを解放する。
    ufbx_free_scene(scene);
    if (FAILED(hr))
    {
        Release();
        return hr;
    }

    sprintf_s(message, "Copied: vertices=%zu indices=%zu\n",
        vertices_.size(), indices_.size());
    OutputDebugStringA(message);

    hr = CreateBuffers();
    if (FAILED(hr)) { Release(); return hr; }
    return S_OK;
}

HRESULT Fbx::ReadVertices(const ufbx_node* node)
{
    const ufbx_mesh* mesh = node->mesh;
    if (mesh->num_indices == 0 || mesh->num_indices > UINT_MAX / sizeof(Direct3D::Vertex3D)) return E_FAIL;
    // 位置を共有する面でも、角ごとに異なる頂点カラーを持てる。
    vertices_.resize(mesh->num_indices);

    for (size_t i = 0; i < mesh->num_indices; i++)
    {
        // FBX内の配置・回転・拡大率と、座標系・単位変換を位置へ反映する。
        ufbx_vec3 p = ufbx_transform_position(
            &node->geometry_to_world, ufbx_get_vertex_vec3(&mesh->vertex_position, i));
        DirectX::XMFLOAT3 position(
            static_cast<float>(p.x),
            static_cast<float>(p.y),
            static_cast<float>(p.z));
        if (!std::isfinite(position.x) || !std::isfinite(position.y)
            || !std::isfinite(position.z)) return E_FAIL;
        auto& vertex = vertices_[i];
        vertex.pos = DirectX::XMVectorSet(position.x, position.y, position.z, 1);
        vertex.uv = DirectX::XMVectorZero();
        vertex.normal = DirectX::XMVectorZero();
        if (mesh->vertex_color.exists)
        {
            const ufbx_vec4 color = ufbx_get_vertex_vec4(&mesh->vertex_color, i);
            vertex.color = DirectX::XMVectorSet(static_cast<float>(color.x),
                static_cast<float>(color.y), static_cast<float>(color.z), static_cast<float>(color.w));
        }
    }
    return S_OK;
}

HRESULT Fbx::ReadIndices(const ufbx_node* node)
{
    const ufbx_mesh* mesh = node->mesh;
    if (mesh->faces.count == 0
        || mesh->faces.count > UINT_MAX / sizeof(uint32_t) / 3) return E_FAIL;

    // マテリアルを割り当てていないメッシュも、既定の1グループで描く。
    materials_.resize(node->materials.count ? node->materials.count : 1);
    for (size_t i = 0; i < node->materials.count; ++i)
    {
        const auto& maps = node->materials.data[i]->fbx;
        const ufbx_vec3 color = maps.diffuse_color.has_value
            ? maps.diffuse_color.value_vec3 : ufbx_vec3 { 1, 1, 1 };
        const double factor = maps.diffuse_factor.has_value ? maps.diffuse_factor.value_real : 1.0;
        materials_[i].diffuse = DirectX::XMFLOAT4(static_cast<float>(color.x * factor),
            static_cast<float>(color.y * factor), static_cast<float>(color.z * factor), 1);
    }
    std::vector<std::vector<uint32_t>> materialIndices(materials_.size());
    // 座標系の変換や負のスケールで表裏が反転したら、面の頂点順も反転する。
    const bool flipWinding = ufbx_matrix_determinant(&node->geometry_to_world) < 0;

    // モデラー側で三角形化してから出力したFBXを使う。
    for (size_t faceIndex = 0; faceIndex < mesh->faces.count; faceIndex++)
    {
        ufbx_face face = mesh->faces.data[faceIndex];
        if (face.num_indices != 3)
        {
            OutputDebugStringA("Export the FBX with triangulated faces.\n");
            return E_FAIL;
        }

        const uint32_t materialIndex = mesh->face_material.count
            ? mesh->face_material.data[faceIndex] : 0;
        if (materialIndex >= materialIndices.size()) return E_FAIL;

        // 1つの三角形を構成する、3個の頂点番号を順に取得する。
        for (uint32_t corner = 0; corner < 3; corner++)
        {
            uint32_t cornerIndex = face.index_begin + corner;
            if (cornerIndex >= vertices_.size()) return E_FAIL;
            materialIndices[materialIndex].push_back(cornerIndex);
        }
        if (flipWinding)
        {
            auto& group = materialIndices[materialIndex];
            std::swap(group[group.size() - 2], group.back());
        }
    }
    // グループを連結して、1本のインデックスバッファに入れる。
    indices_.reserve(mesh->faces.count * 3);
    for (size_t i = 0; i < materials_.size(); ++i)
    {
        auto& range = materials_[i];
        const auto& group = materialIndices[i];
        range.firstIndex = static_cast<UINT>(indices_.size());
        range.indexCount = static_cast<UINT>(group.size());
        indices_.insert(indices_.end(), group.begin(), group.end());
    }
    return S_OK;
}

void Fbx::ReadTextures(const ufbx_node* node, const std::string& fileName)
{
    const auto folder = std::filesystem::path(std::u8string(fileName.begin(), fileName.end())).parent_path();
    for (size_t i = 0; i < node->materials.count; ++i)
    {
        auto& material = materials_[i];
        const auto& diffuse = node->materials.data[i]->fbx.diffuse_color;
        const ufbx_texture* source = diffuse.texture;
        if (!material.indexCount || !diffuse.texture_enabled || !source
            || source->type != UFBX_TEXTURE_FILE) continue;

        const ufbx_vertex_vec2* uv = &node->mesh->vertex_uv;
        if (source->uv_set.length)
        {
            for (const auto& set : node->mesh->uv_sets)
            {
                if (std::string_view(set.name.data, set.name.length)
                    == std::string_view(source->uv_set.data, source->uv_set.length))
                {
                    uv = &set.vertex_uv;
                    break;
                }
            }
        }
        if (!uv->exists) continue;

        // FBXの参照から画像名を取り、モデルと同じフォルダーだけを参照する。
        const ufbx_string name = source->relative_filename.length
            ? source->relative_filename : source->filename;
        if (!name.length) continue;
        const auto image = folder / std::filesystem::path(std::u8string(name.data, name.data + name.length)).filename();
        auto texture = std::make_unique<Texture>();
        // Texture::Loadは読み込み失敗時にS_FALSEを返す。
        if (texture->Load(image.wstring()) != S_OK)
        {
            OutputDebugStringW((L"FBX Diffuse texture could not be loaded: " + image.wstring() + L"\n").c_str());
            continue;
        }

        // 同じ位置でも面の角が違えばUVが違う。Vは画像の原点に合わせる。
        for (UINT j = 0; j < material.indexCount; ++j)
        {
            const uint32_t corner = indices_[material.firstIndex + j];
            const ufbx_vec2 value = ufbx_get_vertex_vec2(uv, corner);
            vertices_[corner].uv = DirectX::XMVectorSet(static_cast<float>(value.x),
                static_cast<float>(1.0 - value.y), 0, 0);
        }
        material.texture = std::move(texture);
    }
}

HRESULT Fbx::CreateBuffers()
{
    if (!Direct3D::pDevice || vertices_.empty() || indices_.empty()) return E_FAIL;
    // ByteWidthはUINTなので、キャストの前に収まるか確認する。
    if (vertices_.size() > UINT_MAX / sizeof(Direct3D::Vertex3D)
        || indices_.size() > UINT_MAX / sizeof(uint32_t)) return E_FAIL;

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = static_cast<UINT>(sizeof(Direct3D::Vertex3D) * vertices_.size());
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = vertices_.data();
    HRESULT hr = Direct3D::pDevice->CreateBuffer(&desc, &data, &pVertexBuffer_);
    if (FAILED(hr)) return hr;

    desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indices_.size());
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    data.pSysMem = indices_.data();
    hr = Direct3D::pDevice->CreateBuffer(&desc, &data, &pIndexBuffer_);
    if (FAILED(hr)) return hr;

    desc.ByteWidth = sizeof(Direct3D::Constants3D);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    return Direct3D::pDevice->CreateBuffer(&desc, nullptr, &pConstantBuffer_);
}

void Fbx::Draw(Transform& transform)
{
    // 読み込み失敗後や、まだLoadしていない場合は描かない。
    if (!pVertexBuffer_ || !pIndexBuffer_ || !pConstantBuffer_) return;

    Direct3D::SetShader(SHADER_3D);
    Direct3D::UseDepthBuffer(true);
    Direct3D::Constants3D cb = {};
    cb.useLighting = 0;
    cb.matWVP = DirectX::XMMatrixTranspose(
        transform.GetWorldMatrix()
        * Camera::GetViewMatrix()
        * Camera::GetProjectionMatrix());

    UINT stride = sizeof(Direct3D::Vertex3D);
    UINT offset = 0;
    Direct3D::pContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);
    Direct3D::pContext->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);
    Direct3D::pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Direct3D::pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer_);
    Direct3D::pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer_);
    for (const auto& material : materials_)
    {
        if (!material.indexCount) continue;
        cb.diffuse = material.diffuse;
        cb.useTexture = material.texture ? 1 : 0;
        ID3D11ShaderResourceView* srv = material.texture ? material.texture->GetSRV() : nullptr;
        ID3D11SamplerState* sampler = material.texture ? material.texture->GetSampler() : nullptr;
        Direct3D::pContext->PSSetShaderResources(0, 1, &srv);
        Direct3D::pContext->PSSetSamplers(0, 1, &sampler);
        Direct3D::pContext->UpdateSubresource(pConstantBuffer_, 0, nullptr, &cb, 0, 0);
        Direct3D::pContext->DrawIndexed(material.indexCount, material.firstIndex, 0);
    }
    ID3D11ShaderResourceView* srv = nullptr;
    ID3D11SamplerState* sampler = nullptr;
    Direct3D::pContext->PSSetShaderResources(0, 1, &srv);
    Direct3D::pContext->PSSetSamplers(0, 1, &sampler);
    Direct3D::UseDepthBuffer(false);
}
