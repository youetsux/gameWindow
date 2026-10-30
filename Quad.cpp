#include "Quad.h"
#include "Camera.h"

Quad::Quad()
	:pVertexBuffer_(nullptr),
	 pIndexBuffer_(nullptr),
	 pConstantBuffer_(nullptr),
	 pTexture_(nullptr)

{
	//vertices[0] = {XMVectorSet(-1.0f,  1.0f, 0.0f, 1.0f),   
	//	           XMVectorSet(0,0,0,0),
	//			   XMVectorSet(0, 0, -1, 1) };	// 四角形の頂点（左上）
	//vertices[1] = {XMVectorSet( 1.0f,  1.0f, 0.0f, 1.0f),   
	//	           XMVectorSet(1, 0, 0, 0), 
	//	           XMVectorSet(0, 0, -1, 1)};	// 四角形の頂点（右上）
	//vertices[2] = {XMVectorSet( 1.0f, -1.0f, 0.0f, 1.0f),   
	//	           XMVectorSet(1, 1, 0, 0), 
	//	           XMVectorSet(0, 0, -1, 1)};	// 四角形の頂点（右下）
	//vertices[3] = {XMVectorSet(-1.0f, -1.0f, 0.0f, 1.0f),   
	//	           XMVectorSet(0, 1, 0, 0), 
	//	           XMVectorSet(0, 0, -1, 1)};	// 四角形の頂点（左下）	
	// 手前（-Z）：1
	vertices[0] = {
		XMVectorSet(-1.0f,  1.0f, -1.0f, 1.0f),
		XMVectorSet(0, 0, 0, 0),
		XMVectorSet(0, 0, -1, 0)
	};
	vertices[1] = {
		XMVectorSet(1.0f,  1.0f, -1.0f, 1.0f),
		XMVectorSet(1.0f / 3.0f, 0, 0, 0),
		XMVectorSet(0, 0, -1, 0)
	};
	vertices[2] = {
		XMVectorSet(1.0f, -1.0f, -1.0f, 1.0f),
		XMVectorSet(1.0f / 3.0f, 0.5f, 0, 0),
		XMVectorSet(0, 0, -1, 0)
	};
	vertices[3] = {
		XMVectorSet(-1.0f, -1.0f, -1.0f, 1.0f),
		XMVectorSet(0, 0.5f, 0, 0),
		XMVectorSet(0, 0, -1, 0)
	};

	// 奥（+Z）：6
	vertices[4] = {
		XMVectorSet(1.0f,  1.0f, 1.0f, 1.0f),
		XMVectorSet(2.0f / 3.0f, 0.5f, 0, 0),
		XMVectorSet(0, 0, 1, 0)
	};
	vertices[5] = {
		XMVectorSet(-1.0f,  1.0f, 1.0f, 1.0f),
		XMVectorSet(1, 0.5f, 0, 0),
		XMVectorSet(0, 0, 1, 0)
	};
	vertices[6] = {
		XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f),
		XMVectorSet(1, 1, 0, 0),
		XMVectorSet(0, 0, 1, 0)
	};
	vertices[7] = {
		XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f),
		XMVectorSet(2.0f / 3.0f, 1, 0, 0),
		XMVectorSet(0, 0, 1, 0)
	};

	// 右（+X）：2
	vertices[8] = {
		XMVectorSet(1.0f,  1.0f, -1.0f, 1.0f),
		XMVectorSet(1.0f / 3.0f, 0, 0, 0),
		XMVectorSet(1, 0, 0, 0)
	};
	vertices[9] = {
		XMVectorSet(1.0f,  1.0f,  1.0f, 1.0f),
		XMVectorSet(2.0f / 3.0f, 0, 0, 0),
		XMVectorSet(1, 0, 0, 0)
	};
	vertices[10] = {
		XMVectorSet(1.0f, -1.0f,  1.0f, 1.0f),
		XMVectorSet(2.0f / 3.0f, 0.5f, 0, 0),
		XMVectorSet(1, 0, 0, 0)
	};
	vertices[11] = {
		XMVectorSet(1.0f, -1.0f, -1.0f, 1.0f),
		XMVectorSet(1.0f / 3.0f, 0.5f, 0, 0),
		XMVectorSet(1, 0, 0, 0)
	};

	// 左（-X）：5
	vertices[12] = {
		XMVectorSet(-1.0f,  1.0f,  1.0f, 1.0f),
		XMVectorSet(1.0f / 3.0f, 0.5f, 0, 0),
		XMVectorSet(-1, 0, 0, 0)
	};
	vertices[13] = {
		XMVectorSet(-1.0f,  1.0f, -1.0f, 1.0f),
		XMVectorSet(2.0f / 3.0f, 0.5f, 0, 0),
		XMVectorSet(-1, 0, 0, 0)
	};
	vertices[14] = {
		XMVectorSet(-1.0f, -1.0f, -1.0f, 1.0f),
		XMVectorSet(2.0f / 3.0f, 1, 0, 0),
		XMVectorSet(-1, 0, 0, 0)
	};
	vertices[15] = {
		XMVectorSet(-1.0f, -1.0f,  1.0f, 1.0f),
		XMVectorSet(1.0f / 3.0f, 1, 0, 0),
		XMVectorSet(-1, 0, 0, 0)
	};

	// 上（+Y）：3
	vertices[16] = {
		XMVectorSet(-1.0f, 1.0f,  1.0f, 1.0f),
		XMVectorSet(2.0f / 3.0f, 0, 0, 0),
		XMVectorSet(0, 1, 0, 0)
	};
	vertices[17] = {
		XMVectorSet(1.0f, 1.0f,  1.0f, 1.0f),
		XMVectorSet(1, 0, 0, 0),
		XMVectorSet(0, 1, 0, 0)
	};
	vertices[18] = {
		XMVectorSet(1.0f, 1.0f, -1.0f, 1.0f),
		XMVectorSet(1, 0.5f, 0, 0),
		XMVectorSet(0, 1, 0, 0)
	};
	vertices[19] = {
		XMVectorSet(-1.0f, 1.0f, -1.0f, 1.0f),
		XMVectorSet(2.0f / 3.0f, 0.5f, 0, 0),
		XMVectorSet(0, 1, 0, 0)
	};

	// 下（-Y）：4
	vertices[20] = {
		XMVectorSet(-1.0f, -1.0f, -1.0f, 1.0f),
		XMVectorSet(0, 0.5f, 0, 0),
		XMVectorSet(0, -1, 0, 0)
	};
	vertices[21] = {
		XMVectorSet(1.0f, -1.0f, -1.0f, 1.0f),
		XMVectorSet(1.0f / 3.0f, 0.5f, 0, 0),
		XMVectorSet(0, -1, 0, 0)
	};
	vertices[22] = {
		XMVectorSet(1.0f, -1.0f,  1.0f, 1.0f),
		XMVectorSet(1.0f / 3.0f, 1, 0, 0),
		XMVectorSet(0, -1, 0, 0)
	};
	vertices[23] = {
		XMVectorSet(-1.0f, -1.0f,  1.0f, 1.0f),
		XMVectorSet(0, 1, 0, 0),
		XMVectorSet(0, -1, 0, 0)
	};
}

Quad::~Quad()
{}

void Quad::Initialize()
{
	// 頂点データ用バッファの設定
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(vertices); // バッファのサイズ
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices;// バッファにコピーするデータのアドレス
	Direct3D::pDevice->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);

	//インデックス情報
	//int index[] = { 0,2,3, 0,1,2 };
	int index[] = {
	 0,  2,  3,   0,  1,  2,
	 4,  6,  7,   4,  5,  6,
	 8, 10, 11,   8,  9, 10,
	12, 14, 15,  12, 13, 14,
	16, 18, 19,  16, 17, 18,
	20, 22, 23,  20, 21, 22
	};

	
	// インデックスバッファを生成する
	D3D11_BUFFER_DESC   bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(index);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = index;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	Direct3D::pDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer_);

	//コンスタントバッファ作成
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// コンスタントバッファの作成
	Direct3D::pDevice->CreateBuffer(&cb, nullptr, &pConstantBuffer_);


	//テクスチャの読み込み
	pTexture_ = new Texture;
	HRESULT hr = pTexture_->Load("Assets\\sainome.png");
	if(FAILED(hr))
	{
		// エラーハンドリング
		return;
	}

}

void Quad::Draw(XMMATRIX& WorldMatrix)
{
	////コンスタントバッファに渡す情報
	//XMVECTOR position = { 0, 3, -10, 0 };	//カメラの位置
	//XMVECTOR target = { 0, 0, 0, 0 };	//カメラの焦点
	//XMMATRIX view = XMMatrixLookAtLH(position, target, XMVectorSet(0, 1, 0, 0));	//ビュー行列
	//XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 800.0f / 600.0f, 0.1f, 100.0f);//射影行列

	CONSTANT_BUFFER cb;
	cb.matWVP = XMMatrixTranspose(WorldMatrix * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
	cb.matW = XMMatrixTranspose(WorldMatrix); //ワールド行列を転置してコンスタントバッファに渡す

	//コンスタントバッファにデータを転送する
	D3D11_MAPPED_SUBRESOURCE pdata;
	Direct3D::pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのデータアクセスを止める
	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データを値を送る
	Direct3D::pContext->Unmap(pConstantBuffer_, 0);	//再開

	ID3D11SamplerState* pSampler = pTexture_->GetSampler();
	Direct3D::pContext->PSSetSamplers(0, 1, &pSampler);

	ID3D11ShaderResourceView* pSRV = pTexture_->GetSRV();
	Direct3D::pContext->PSSetShaderResources(0, 1, &pSRV);


	//頂点バッファ
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// インデックスバッファーをセット
	stride = sizeof(int);
	offset = 0;
	Direct3D::pContext->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	//コンスタントバッファ
	Direct3D::pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//頂点シェーダー用	
	Direct3D::pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//ピクセルシェーダー用
	
	Direct3D::pContext->DrawIndexed(36, 0, 0);
}

void Quad::Release()
{
	if (pVertexBuffer_) { pVertexBuffer_->Release(); pVertexBuffer_ = nullptr; }
	if (pIndexBuffer_) { pIndexBuffer_->Release(); pIndexBuffer_ = nullptr; }
	if (pConstantBuffer_) { pConstantBuffer_->Release(); pConstantBuffer_ = nullptr; }
}
