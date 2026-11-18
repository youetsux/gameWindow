#pragma once
#include "Transform.h"
#include "Direct3D.h"
#include <DirectXMath.h>
#include "Texture.h"

using namespace DirectX;

class Quad
{

public:
	Quad();
	virtual ~Quad();
	// 初期化
	virtual void Initialize(const char* textureFileName = "image.jpg");
	virtual void Draw(Transform& transform);
	void Release();
	Direct3D::Vertex3D vertices[4]; // 頂点座標
private:
	ID3D11Buffer* pVertexBuffer_;	//頂点バッファ
	ID3D11Buffer* pIndexBuffer_;    //インデックスバッファ
	ID3D11Buffer* pConstantBuffer_;	//コンスタントバッファ
	Texture* pTexture_; // テクスチャ
};

