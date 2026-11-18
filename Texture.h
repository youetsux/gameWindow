#pragma once
#include <d3d11.h>
#include <string>

class Texture
{
public:
	Texture();
	~Texture();
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	HRESULT Load(std::string fileName);
	HRESULT Load(const std::wstring& fileName);
	ID3D11SamplerState* GetSampler() { return pSampler_; }
	ID3D11ShaderResourceView* GetSRV() { return pSRV_; }
	void Release();
private:
	//本当は、スマポを使ったほういいよ
	ID3D11SamplerState* pSampler_ = nullptr;
	ID3D11ShaderResourceView* pSRV_ = nullptr;
};

