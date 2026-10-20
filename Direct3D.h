#pragma once
//インクルード
#include <d3d11.h>

//リンカ
#pragma comment(lib, "d3d11.lib")

namespace Direct3D
{
	//Direct3D関係の変数
		// Direct3D 11 デバイスとスワップチェーンのポインタ
    ID3D11Device* pDevice;		//デバイス
    ID3D11DeviceContext* pContext;		//デバイスコンテキスト
    IDXGISwapChain* pSwapChain;		//スワップチェイン
    ID3D11RenderTargetView* pRenderTargetView;	//レンダーターゲットビュー
	//Direct3D関係の関数（プロトタイプ宣言）
	void Initialize(int width, int height, HWND hWnd);
	void BeginDraw();
	void EndDraw();
	void Release();
}

void Direct3D::Initialize(int width, int height, HWND hWnd)
{
 ///////////////////////////いろいろ準備するための設定///////////////////////////////
    //いろいろな設定項目をまとめた構造体
   DXGI_SWAP_CHAIN_DESC scDesc = {};

   //とりあえず全部0
   //ZeroMemory(&scDesc, sizeof(scDesc));

   //描画先のフォーマット
   scDesc.BufferDesc.Width = width;		//画面幅
   scDesc.BufferDesc.Height = height;	//画面高さ
   scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 何色使えるか

   //FPS（1/60秒に1回）
   scDesc.BufferDesc.RefreshRate.Numerator = 60;
   scDesc.BufferDesc.RefreshRate.Denominator = 1;

   //その他
   scDesc.Windowed = TRUE;			//ウィンドウモードかフルスクリーンか
   scDesc.OutputWindow = hWnd;		//ウィンドウハンドル
   scDesc.BufferCount = 1;			//バックバッファの枚数
   scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//バックバッファの使い道＝画面に描画するために
   scDesc.SampleDesc.Count = 1;		//MSAA（アンチエイリアス）の設定
   scDesc.SampleDesc.Quality = 0;		//　〃

   ////////////////上記設定をもとにデバイス、コンテキスト、スワップチェインを作成////////////////////////
   D3D_FEATURE_LEVEL level;
   D3D11CreateDeviceAndSwapChain(
       nullptr,				// どのビデオアダプタを使用するか？既定ならばnullptrで
       D3D_DRIVER_TYPE_HARDWARE,		// ドライバのタイプを渡す。ふつうはHARDWARE
       nullptr,				// 上記をD3D_DRIVER_TYPE_SOFTWAREに設定しないかぎりnullptr
       0,					// 何らかのフラグを指定する。（デバッグ時はD3D11_CREATE_DEVICE_DEBUG？）
       nullptr,				// デバイス、コンテキストのレベルを設定。nullptrにしとけばOK
       0,					// 上の引数でレベルを何個指定したか
       D3D11_SDK_VERSION,			// SDKのバージョン。必ずこの値
       &scDesc,				// 上でいろいろ設定した構造体
       &pSwapChain,				// 無事完成したSwapChainのアドレスが返ってくる
       &pDevice,				// 無事完成したDeviceアドレスが返ってくる
       &level,					// 無事完成したDevice、Contextのレベルが返ってくる
       &pContext);				// 無事完成したContextのアドレスが返ってくる

   ///////////////////////////レンダーターゲットビュー作成///////////////////////////////
   //スワップチェーンからバックバッファを取得（バックバッファ ＝ レンダーターゲット）
   ID3D11Texture2D* pBackBuffer;
   pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

   //レンダーターゲットビューを作成
   pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);

   //一時的にバックバッファを取得しただけなので解放
   pBackBuffer->Release();

   ///////////////////////////ビューポート（描画範囲）設定///////////////////////////////
   //レンダリング結果を表示する範囲
   D3D11_VIEWPORT vp;
   vp.Width = (float)width;	//幅
   vp.Height = (float)height;//高さ
   vp.MinDepth = 0.0f;	//手前
   vp.MaxDepth = 1.0f;	//奥
   vp.TopLeftX = 0;	//左
   vp.TopLeftY = 0;	//上

   //データを画面に描画するための一通りの設定（パイプライン）
   pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  // データの入力種類を指定
   pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);            // 描画先を設定
   pContext->RSSetViewports(1, &vp);
}

void Direct3D::BeginDraw()
{
	//画面をクリアする
	float clearColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f }; // RGBA
	pContext->ClearRenderTargetView(pRenderTargetView, clearColor);
}

void Direct3D::EndDraw()
{
	//スワップチェインのバッファを表示する
	pSwapChain->Present(1, 0); // VSync有効
}

void Direct3D::Release()
{
	//解放処理
	if (pRenderTargetView) pRenderTargetView->Release();
	if (pSwapChain) pSwapChain->Release();
	if (pContext) pContext->Release();
	if (pDevice) pDevice->Release();
}