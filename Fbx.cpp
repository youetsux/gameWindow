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

void Fbx::Draw(Transform&) {} // まだ描画しない
