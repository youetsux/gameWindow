#pragma once
#include "Direct3D.h"
#include <DirectXMath.h>
#include "Texture.h"

using namespace DirectX;

//コンスタントバッファー
struct CONSTANT_BUFFER
{
	XMMATRIX	matWVP;
	XMMATRIX	matW;
};


//頂点構造体
struct VERTEX
{
	XMVECTOR pos; // 頂点座標 (x, y, z, 1)
	XMVECTOR uv; // 頂点UV座標 (u, v, 0, 0)
	XMVECTOR normal; // 頂点法線ベクトル (x, y, z, 0)
};

class Quad
{

public:
	Quad();
	~Quad();
	// 初期化
	void Initialize();
	void Draw(XMMATRIX& WorldMatrix);
	void Release();
	VERTEX vertices[4]; // 頂点座標
private:
	ID3D11Buffer* pVertexBuffer_;	//頂点バッファ
	ID3D11Buffer* pIndexBuffer_;    //インデックスバッファ
	ID3D11Buffer* pConstantBuffer_;	//コンスタントバッファ
	Texture* pTexture_; // テクスチャ
};

