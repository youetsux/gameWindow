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
    if (SUCCEEDED(hr)) hr = ReadIndices(mesh);

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

void Fbx::Draw(Transform&) {} // まだ描画しない

HRESULT Fbx::ReadVertices(const ufbx_node* node)
{
    const ufbx_mesh* mesh = node->mesh;
    if (mesh->faces.count == 0 || !mesh->vertex_position.exists)
        return E_FAIL;

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

            // UVと法線はまだ読まない。仮の値を使う。
            vertex.uv = DirectX::XMFLOAT4(0, 0, 0, 1);
            vertex.normal = DirectX::XMFLOAT4(0, 1, 0, 0);

            if (!std::isfinite(vertex.position.x)
                || !std::isfinite(vertex.position.y)
                || !std::isfinite(vertex.position.z)) return E_FAIL;

            vertices_.push_back(vertex);
        }
    }
    return vertices_.empty() ? E_FAIL : S_OK;
}

HRESULT Fbx::ReadIndices(const ufbx_mesh* mesh)
{
    if (mesh->faces.count == 0) return E_FAIL;
    indices_.clear();

    for (size_t faceIndex = 0; faceIndex < mesh->faces.count; faceIndex++)
    {
        const ufbx_face face = mesh->faces.data[faceIndex];
        if (face.num_indices != 3) return E_FAIL;

        for (uint32_t corner = 0; corner < 3; corner++)
        {
            const size_t cornerIndex = face.index_begin + corner;
            if (cornerIndex >= vertices_.size()
                || cornerIndex > UINT32_MAX) return E_FAIL;

            indices_.push_back(static_cast<uint32_t>(cornerIndex));
        }
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
