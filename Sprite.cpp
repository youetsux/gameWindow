#include "Sprite.h"
#include "Texture.h"


namespace
{
	std::wstring ToWstring(const std::string& utf8Str) {
		if (utf8Str.empty()) return std::wstring();

		// 必要なバッファサイズを取得
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), (int)utf8Str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);

		// 実際の変換
		MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), (int)utf8Str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}
	std::string ToString(const std::wstring& wstr) {
		if (wstr.empty()) return std::string();

		// 必要なバッファサイズを取得
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);

		// 実際の変換
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

}
Sprite::Sprite(const std::wstring& filename)
	:pVertexBuffer_(nullptr),
	pIndexBuffer_(nullptr),
	pConstantBuffer_(nullptr),
	pTexture_(nullptr)
{
	HRESULT hr = InitBuffers();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"バッファの作成に失敗", L"エラー", MB_OK);
		return;
	}
	LoadTexture(filename);
	
}

Sprite::~Sprite()
{}



HRESULT Sprite::InitBuffers()
{
	HRESULT hr;
	VERTEX vertices[4];
	vertices[0] = { XMVectorSet(-1.0f,  1.0f, 0.0f, 1.0f),
					XMVectorSet( 0.0f,  0.0f, 0.0f, 0.0f) };	// 四角形の頂点（左上）
	vertices[1] = { XMVectorSet( 1.0f,  1.0f, 0.0f, 1.0f),
					XMVectorSet( 1.0f,  0.0f, 0.0f, 0.0f) };	// 四角形の頂点（右上）
	vertices[2] = { XMVectorSet( 1.0f, -1.0f, 0.0f, 1.0f),
					XMVectorSet( 1.0f,  1.0f, 0.0f, 0.0f) };	// 四角形の頂点（右下）
	vertices[3] = { XMVectorSet(-1.0f, -1.0f, 0.0f, 1.0f),
					XMVectorSet( 0.0f,  1.0f, 0.0f, 0.0f) };	// 四角形の頂点（左下）
	
	// 頂点データ用バッファの設定
	D3D11_BUFFER_DESC bd_vertex = {};
	bd_vertex.ByteWidth = sizeof(vertices) ; // バッファのサイズ
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data_vertex = {};
	data_vertex.pSysMem = vertices;// バッファにコピーするデータのアドレス
	hr = Direct3D::pDevice->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"頂点バッファの作成に失敗", L"エラー", MB_OK);
		return hr;
	}


	//インデックス情報
	int index[] = { 0, 2, 3, 0, 1, 2 };
	// インデックスバッファを生成する
	D3D11_BUFFER_DESC   bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(index);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = index;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	hr = Direct3D::pDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"インデックスバッファの作成に失敗", L"エラー", MB_OK);
		return hr;
	}

	//コンスタントバッファ作成
	D3D11_BUFFER_DESC cb = {};
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// コンスタントバッファの作成
	hr = Direct3D::pDevice->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"コンスタントバッファの作成に失敗", L"エラー", MB_OK);
		return hr;
	}

	return S_OK;
}

void Sprite::LoadTexture(const std::wstring& filename)
{
	if (pTexture_ == nullptr)
	{
		pTexture_ = new Texture();
		std::string u8Filename = ToString(filename);
		pTexture_->Load(u8Filename);
	}
}

void Sprite::Draw(Transform& transform)
{
	XMMATRIX WorldMatrix = transform.GetWorldMatrix();

	Direct3D::SetShader(SHADER_2D);

	CONSTANT_BUFFER cb;
	cb.matW = XMMatrixTranspose(WorldMatrix); //ワールド行列を転置してコンスタントバッファに渡す

	//コンスタントバッファにデータを転送する
	D3D11_MAPPED_SUBRESOURCE pdata = {};
	Direct3D::pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのデータアクセスを止める
	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データを値を送る
	Direct3D::pContext->Unmap(pConstantBuffer_, 0);	//再開


	//頂点バッファ
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// インデックスバッファーをセット
	Direct3D::pContext->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	//コンスタントバッファ
	Direct3D::pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//頂点シェーダー用	
	Direct3D::pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//ピクセルシェーダー用

	if (pTexture_)
	{

		ID3D11SamplerState* pSampler = pTexture_->GetSampler();
		Direct3D::pContext->PSSetSamplers(0, 1, &pSampler);

		ID3D11ShaderResourceView* pSRV = pTexture_->GetSRV();
		Direct3D::pContext->PSSetShaderResources(0, 1, &pSRV);
	}
	else
	{
		MessageBox(nullptr, L"テクスチャがロードされていません", L"エラー", MB_OK);
		return;
	}

	Direct3D::pContext->DrawIndexed(6, 0, 0);
}
