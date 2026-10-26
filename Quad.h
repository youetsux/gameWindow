#pragma once
#include "Direct3D.h"
#include <DirectXMath.h>

using namespace DirectX;

//コンスタントバッファー
struct CONSTANT_BUFFER
{
	XMMATRIX	matWVP;
};


class Quad
{
	ID3D11Buffer* pVertexBuffer_;	//頂点バッファ
	ID3D11Buffer* pIndexBuffer_;    //インデックスバッファ
	ID3D11Buffer* pConstantBuffer_;	//コンスタントバッファ
public:
	Quad();
	~Quad();
	// 初期化
	void Initialize();
	void Draw(XMMATRIX& WorldMatrix);
	void Release();
	XMVECTOR vertices[4]; // 頂点座標
};

