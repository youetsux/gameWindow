#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <cstdint>
#include <string>
#include <vector>
#include "Transform.h"

struct ufbx_node;
struct ufbx_mesh;

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
    struct VERTEX
    {
        DirectX::XMFLOAT3 position;
    };
    struct CONSTANT_BUFFER
    {
        DirectX::XMMATRIX matWVP;
    };

    std::vector<VERTEX> vertices_;
    std::vector<uint32_t> indices_;
    ID3D11Buffer* pVertexBuffer_ = nullptr;
    ID3D11Buffer* pIndexBuffer_ = nullptr;
    ID3D11Buffer* pConstantBuffer_ = nullptr;

    HRESULT ReadVertices(const ufbx_node* node);
    HRESULT ReadIndices(const ufbx_mesh* mesh);
    HRESULT CreateBuffers();
};
