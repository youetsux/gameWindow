#include "Texture.h"
#include <DirectXTex.h>
#include "Direct3D.h"

// DirectXTexのライブラリをリンク(非推奨だが簡単)
#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;

namespace {
	wchar_t* StringToWChar(const std::string& str) {
		if (str.empty()) return nullptr;

		// 必要なバッファサイズを取得（終端null含む）
		int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);

		std::vector<wchar_t> wstrTo(size_needed + 1);
		MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		wstrTo[size_needed] = L'\0';

		// 注意: new またはヒープで確保したポインターを返す場合は、呼び出し側で delete[] が必要
		// または std::vector<wchar_t> や std::wstring で管理することを推奨します。
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
	hr = LoadFromWICFile(StringToWChar(fileName), 
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
}

void Texture::Release()
{
	SAFE_RELEASE(pSampler_);
	SAFE_RELEASE(pSRV_);
}
