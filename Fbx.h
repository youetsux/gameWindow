#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <cstdint>
#include <string>
#include <vector>
#include "Transform.h"
#include "Texture.h"

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
        DirectX::XMFLOAT4 position; // 既存のSHADER_3Dに合わせる
        DirectX::XMFLOAT4 uv;       // 工程8で実データに変更する
        DirectX::XMFLOAT4 normal;   // 工程7で実データに変更する
    };
    struct CONSTANT_BUFFER
    {
        DirectX::XMMATRIX matWVP;
        DirectX::XMMATRIX matW;
    };

    struct MaterialRange
    {
        DirectX::XMFLOAT4 diffuse = { 1, 1, 1, 1 };
        Texture* texture = nullptr;
        UINT firstIndex = 0;
        UINT indexCount = 0;
    };

    std::vector<MaterialRange> materials_;
    std::vector<VERTEX> vertices_;
    std::vector<uint32_t> indices_;
    ID3D11Buffer* pVertexBuffer_ = nullptr;
    ID3D11Buffer* pIndexBuffer_ = nullptr;
    ID3D11Buffer* pConstantBuffer_ = nullptr;

    HRESULT ReadVertices(const ufbx_node* node);
    HRESULT ReadIndices(const ufbx_node* node);
    void ReadTextures(const ufbx_node* node, const std::string& fileName);
    HRESULT CreateBuffers();
};
