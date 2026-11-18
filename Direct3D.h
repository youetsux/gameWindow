#pragma once
//インクルード
#include <d3d11.h>
#include <DirectXMath.h>
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
    // Simple3D.hlsl と共有する頂点と定数バッファの配置。
    struct Vertex3D
    {
        DirectX::XMVECTOR pos;
        DirectX::XMVECTOR uv;
        DirectX::XMVECTOR normal;
        DirectX::XMVECTOR color = DirectX::XMVectorSplatOne();
    };

    struct Constants3D
    {
        DirectX::XMMATRIX matWVP;
        DirectX::XMMATRIX matW;
        DirectX::XMFLOAT4 diffuse = { 1, 1, 1, 1 };
        UINT useLighting = 1;
        UINT useTexture = 1;
        UINT padding[2] = {};
    };

	HRESULT Initialize(int width, int height, HWND hWnd);
	void BeginDraw();
	void EndDraw();
	void Release();
	//各シェーダの初期化
	HRESULT InitShader3D();
	HRESULT InitShader2D();
	void UseDepthBuffer(bool enabled);
	void SetShader(SHADER_TYPE type);
	extern ID3D11Device* pDevice;	//デバイス
	extern ID3D11DeviceContext* pContext; //デバイスコンテキスト
}
