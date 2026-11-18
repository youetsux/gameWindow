#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <cstdint>
#include <string>
#include <vector>
#include "Transform.h"
#include "Direct3D.h"

struct ufbx_node;

class Fbx
{
public:
    Fbx() = default;
    ~Fbx();
    // GPUのポインタを持つので、オブジェクトのコピーを禁止する。
    Fbx(const Fbx&) = delete;
    Fbx& operator=(const Fbx&) = delete;

    HRESULT Load(const std::string& fileName);
    void Draw(Transform& transform);
    void Release();

private:
    struct MaterialRange
    {
        DirectX::XMFLOAT4 diffuse = { 1, 1, 1, 1 };
        UINT firstIndex = 0;
        UINT indexCount = 0;
    };

    std::vector<MaterialRange> materials_;
    std::vector<Direct3D::Vertex3D> vertices_;
    std::vector<uint32_t> indices_;
    ID3D11Buffer* pVertexBuffer_ = nullptr;
    ID3D11Buffer* pIndexBuffer_ = nullptr;
    ID3D11Buffer* pConstantBuffer_ = nullptr;

    HRESULT ReadVertices(const ufbx_node* node);
    HRESULT ReadIndices(const ufbx_node* node);
    HRESULT CreateBuffers();
};
