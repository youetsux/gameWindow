#pragma once
//インクルード
#include <d3d11.h>

//リンカ
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace Direct3D
{
	void Initialize(int width, int height, HWND hWnd);
	void BeginDraw();
	void EndDraw();
	void Release();
	//シェーダの初期化処理
	void InitShader();
	extern ID3D11Device* pDevice;	//デバイス
	extern ID3D11DeviceContext* pContext; //デバイスコンテキスト
}
