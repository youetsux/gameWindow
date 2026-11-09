#pragma once
//インクルード
#include <d3d11.h>
#define SAFE_DELETE(p) if(p != nullptr){ delete p; p = nullptr;}
#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}

//リンカ
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

enum SHADER_TYPE
{
	SHADER_3D,
	SHADER_2D,
	SHADER_MAX
};
//Shadebundleを作って、InitShader3Dに適用したところまで（11/5)
namespace Direct3D
{
	void Initialize(int width, int height, HWND hWnd);
	void BeginDraw();
	void EndDraw();
	void Release();
	//シェーダの初期化処理
	void InitShader();
	//各シェーダの初期化
	HRESULT InitShader3D();
	//HRESULT InitShader2D();
	void SetShader(SHADER_TYPE type);
	extern ID3D11Device* pDevice;	//デバイス
	extern ID3D11DeviceContext* pContext; //デバイスコンテキスト
}
