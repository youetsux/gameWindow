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
    return S_OK;
}

HRESULT Fbx::ReadVertices(const ufbx_node* node)
{
    const ufbx_mesh* mesh = node->mesh;
    if (mesh->num_vertices == 0) return E_FAIL;
    vertices_.resize(mesh->num_vertices);

    for (size_t i = 0; i < mesh->num_vertices; i++)
    {
        // FBX内の配置・回転・拡大率と、座標系・単位変換を位置へ反映する。
        ufbx_vec3 p = ufbx_transform_position(
            &node->geometry_to_world, mesh->vertices.data[i]);
        DirectX::XMFLOAT3 position(
            static_cast<float>(p.x),
            static_cast<float>(p.y),
            static_cast<float>(p.z));
        if (!std::isfinite(position.x) || !std::isfinite(position.y)
            || !std::isfinite(position.z)) return E_FAIL;
        vertices_[i].position = position;
    }
    return S_OK;
}

HRESULT Fbx::ReadIndices(const ufbx_mesh* mesh)
{
    if (mesh->faces.count == 0) return E_FAIL;
    indices_.clear();

    // モデラー側で三角形化してから出力したFBXを使う。
    for (size_t faceIndex = 0; faceIndex < mesh->faces.count; faceIndex++)
    {
        ufbx_face face = mesh->faces.data[faceIndex];
        if (face.num_indices != 3)
        {
            OutputDebugStringA("Export the FBX with triangulated faces.\n");
            return E_FAIL;
        }

        // 1つの三角形を構成する、3個の頂点番号を順に取得する。
        for (uint32_t corner = 0; corner < 3; corner++)
        {
            uint32_t cornerIndex = face.index_begin + corner;
            if (cornerIndex >= mesh->vertex_indices.count) return E_FAIL;
            // 面の角の番号を、位置の頂点番号に変換する。
            uint32_t vertexIndex = mesh->vertex_indices.data[cornerIndex];
            if (vertexIndex >= vertices_.size()) return E_FAIL;
            indices_.push_back(vertexIndex);
        }
    }
    return S_OK;
}

void Fbx::Draw(Transform&) {} // まだ描画しない
