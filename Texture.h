#pragma once
#include <d3d11.h>
#include <string>

class Texture
{
public:
	Texture();
	~Texture();
	HRESULT Load(std::string fileName);
	ID3D11SamplerState* GetSampler() { return pSampler_; }
	ID3D11ShaderResourceView* GetSRV() { return pSRV_; }
	void Release();
private:
	//本当は、スマポを使ったほういいよ
	ID3D11SamplerState* pSampler_;
	ID3D11ShaderResourceView* pSRV_;
};

