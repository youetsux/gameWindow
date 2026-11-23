#include "Fbx.h"
#include "Direct3D.h"
#include "Camera.h"
#include "ufbx.h"
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstring>

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
    options.generate_missing_normals = true;
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

void Fbx::Draw(Transform& transform)
{
    if (!pVertexBuffer_ || !pIndexBuffer_ || !pConstantBuffer_) return;

    Direct3D::SetShader(SHADER_3D);

    CONSTANT_BUFFER cb = {};
    cb.matWVP = DirectX::XMMatrixTranspose(
        transform.GetWorldMatrix()
        * Camera::GetViewMatrix()
        * Camera::GetProjectionMatrix());
    cb.matW = DirectX::XMMatrixTranspose(transform.GetWorldMatrix());
    Direct3D::pContext->UpdateSubresource(
        pConstantBuffer_, 0, nullptr, &cb, 0, 0);

    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    Direct3D::pContext->IASetVertexBuffers(
        0, 1, &pVertexBuffer_, &stride, &offset);
    Direct3D::pContext->IASetIndexBuffer(
        pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);
    Direct3D::pContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Direct3D::pContext->VSSetConstantBuffers(
        0, 1, &pConstantBuffer_);

    // まだマテリアルを読み込まないのでテクスチャは指定しない。
    // 別の描画処理から残ったテクスチャを使わないようにする。
    ID3D11ShaderResourceView* nullSRV = nullptr;
    Direct3D::pContext->PSSetShaderResources(0, 1, &nullSRV);

    for (const auto& material : materials_)
    {
        if (material.indexCount)
        {
            Direct3D::pContext->DrawIndexed(
                material.indexCount, material.firstIndex, 0);
        }
    }
}

HRESULT Fbx::ReadVertices(const ufbx_node* node)
{
    const ufbx_mesh* mesh = node->mesh;
    if (mesh->faces.count == 0 || !mesh->vertex_position.exists
        || !mesh->vertex_normal.exists)
        return E_FAIL;

    const ufbx_matrix normalMatrix =
        ufbx_matrix_for_normals(&node->geometry_to_world);

    vertices_.clear();
    vertices_.reserve(mesh->num_indices);

    // すべての三角形を順番に調べる。
    for (size_t faceIndex = 0; faceIndex < mesh->faces.count; faceIndex++)
    {
        const ufbx_face face = mesh->faces.data[faceIndex];
        if (face.num_indices != 3)
        {
            OutputDebugStringA("Export the FBX with triangulated faces.\n");
            return E_FAIL;
        }

        // 三角形の3つの角を順番に調べる。
        for (uint32_t corner = 0; corner < 3; corner++)
        {
            const size_t cornerIndex = face.index_begin + corner;
            if (cornerIndex >= mesh->num_indices) return E_FAIL;

            // 面の角に対応する頂点座標を取得する。
            ufbx_vec3 p = ufbx_get_vertex_vec3(
                &mesh->vertex_position, cornerIndex);
            p = ufbx_transform_position(&node->geometry_to_world, p);

            VERTEX vertex = {};
            vertex.position = DirectX::XMFLOAT4(
                static_cast<float>(p.x),
                static_cast<float>(p.y),
                static_cast<float>(p.z), 1.0f);

            ufbx_vec2 uv = {};
            if (mesh->vertex_uv.exists)
            {
                uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, cornerIndex);
            }
            vertex.uv = DirectX::XMFLOAT4(
                static_cast<float>(uv.x),
                static_cast<float>(1.0 - uv.y), 0.0f, 1.0f);

            ufbx_vec3 n = ufbx_get_vertex_vec3(
                &mesh->vertex_normal, cornerIndex);
            n = ufbx_transform_direction(&normalMatrix, n);

            // 法線の向きを保ったまま、長さ1にする。
            const double length = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
            if (!std::isfinite(length) || length <= 0.0) return E_FAIL;
            vertex.normal = DirectX::XMFLOAT4(
                static_cast<float>(n.x / length),
                static_cast<float>(n.y / length),
                static_cast<float>(n.z / length), 0.0f);

            if (!std::isfinite(vertex.position.x)
                || !std::isfinite(vertex.position.y)
                || !std::isfinite(vertex.position.z)) return E_FAIL;

            vertices_.push_back(vertex);
        }
    }
    return vertices_.empty() ? E_FAIL : S_OK;
}

HRESULT Fbx::ReadIndices(const ufbx_node* node)
{
    const ufbx_mesh* mesh = node->mesh;
    if (mesh->faces.count == 0
        || mesh->faces.count > UINT_MAX / sizeof(uint32_t) / 3) return E_FAIL;

    indices_.clear();
    materials_.resize(node->materials.count ? node->materials.count : 1);
    for (size_t i = 0; i < node->materials.count; ++i)
    {
        const auto& maps = node->materials.data[i]->fbx;
        const ufbx_vec3 color = maps.diffuse_color.has_value
            ? maps.diffuse_color.value_vec3 : ufbx_vec3 { 1, 1, 1 };
        const double factor = maps.diffuse_factor.has_value
            ? maps.diffuse_factor.value_real : 1.0;

        materials_[i].diffuse = DirectX::XMFLOAT4(
            static_cast<float>(color.x * factor),
            static_cast<float>(color.y * factor),
            static_cast<float>(color.z * factor), 1.0f);
    }
    std::vector<std::vector<uint32_t>> materialIndices(materials_.size());

    for (size_t faceIndex = 0; faceIndex < mesh->faces.count; faceIndex++)
    {
        const ufbx_face face = mesh->faces.data[faceIndex];
        if (face.num_indices != 3) return E_FAIL;

        const uint32_t materialIndex = mesh->face_material.count
            ? mesh->face_material.data[faceIndex] : 0;
        if (materialIndex >= materialIndices.size()) return E_FAIL;

        for (uint32_t corner = 0; corner < 3; corner++)
        {
            const size_t cornerIndex = face.index_begin + corner;
            if (cornerIndex >= vertices_.size()
                || cornerIndex > UINT32_MAX) return E_FAIL;

            materialIndices[materialIndex].push_back(
                static_cast<uint32_t>(cornerIndex));
        }
    }

    indices_.reserve(mesh->faces.count * 3);
    for (size_t i = 0; i < materials_.size(); ++i)
    {
        auto& range = materials_[i];
        const auto& group = materialIndices[i];
        range.firstIndex = static_cast<UINT>(indices_.size());
        range.indexCount = static_cast<UINT>(group.size());
        indices_.insert(indices_.end(), group.begin(), group.end());
    }
    return indices_.size() == vertices_.size() ? S_OK : E_FAIL;
}

HRESULT Fbx::CreateBuffers()
{
    if (!Direct3D::pDevice || vertices_.empty() || indices_.empty())
        return E_FAIL;
    if (vertices_.size() > UINT_MAX / sizeof(VERTEX)
        || indices_.size() > UINT_MAX / sizeof(uint32_t)) return E_FAIL;

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = static_cast<UINT>(sizeof(VERTEX) * vertices_.size());
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = vertices_.data();
    HRESULT hr = Direct3D::pDevice->CreateBuffer(
        &desc, &data, &pVertexBuffer_);
    if (FAILED(hr)) return hr;

    desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indices_.size());
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    data.pSysMem = indices_.data();
    hr = Direct3D::pDevice->CreateBuffer(
        &desc, &data, &pIndexBuffer_);
    if (FAILED(hr)) return hr;

    desc.ByteWidth = sizeof(CONSTANT_BUFFER);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    return Direct3D::pDevice->CreateBuffer(
        &desc, nullptr, &pConstantBuffer_);
}
