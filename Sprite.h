#pragma once
#include "Transform.h"
#include "Direct3D.h"
#include <DirectXMath.h>
#include <string>

using namespace DirectX;
class Texture;//前方宣言

//コンスタントバッファー

class Sprite
{
public:
	struct CONSTANT_BUFFER
	{
		XMMATRIX	matW;
	};


	//頂点構造体
	struct VERTEX
	{
		XMVECTOR pos; // 頂点座標 (x, y, z, 1)
		XMVECTOR uv; // 頂点UV座標 (u, v, 0, 0)
	};

	//Sprite();
	Sprite(const std::wstring& filename);
	~Sprite();
	void Draw(Transform& transform);
private:
	ID3D11Buffer* pVertexBuffer_;	//頂点バッファ
	ID3D11Buffer* pIndexBuffer_;    //インデックスバッファ
	ID3D11Buffer* pConstantBuffer_;	//コンスタントバッファ
	Texture* pTexture_; // テクスチャ

	HRESULT InitBuffers();
	void LoadTexture(const std::wstring& filename);

};

