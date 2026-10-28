#include "Texture.h"
#include <DirectXTex.h>
#include "Direct3D.h"

// DirectXTexのライブラリをリンク(非推奨だが簡単)
#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;

namespace {
	// std::wstringを返却するように変更（メモリ安全）
	std::wstring StringToWString(const std::string& str) {
		if (str.empty()) return L"";

		// 必要なバッファサイズを取得
		int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);

		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

		return wstrTo;
	}
}

Texture::Texture()
{
	
}

Texture::~Texture()
{}

HRESULT Texture::Load(std::string fileName)
{
	TexMetadata metadata; //画像の付属情報
	ScratchImage image;   //画像本体
	HRESULT hr;

	//実際に読んでゆくぅ　　　　　 
	hr = LoadFromWICFile(StringToWString(fileName).data(),
		                 WIC_FLAGS::WIC_FLAGS_NONE,
		                 &metadata, image);
	if (FAILED(hr))
	{
		return S_FALSE;
	}
	D3D11_SAMPLER_DESC  SamDesc = {};
	//ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = Direct3D::pDevice->CreateSamplerState(&SamDesc, &pSampler_);//グローバル！
	if (FAILED(hr))
	{
		return S_FALSE;
	}
	//シェーダーリソースビュー

	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;
	hr = CreateShaderResourceView(Direct3D::pDevice,
		image.GetImages(), image.GetImageCount(), metadata, &pSRV_);

	if (FAILED(hr))
	{
		return S_FALSE;
	}
	return S_OK;
}

void Texture::Release()
{
	SAFE_RELEASE(pSampler_);
	SAFE_RELEASE(pSRV_);
}
