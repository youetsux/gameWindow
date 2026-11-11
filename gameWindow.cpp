// gameWindow.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "gameWindow.h"

#define MAX_LOADSTRING 100

#include <d3d11.h>
#include <wrl/client.h>

#include "Direct3D.h"
#include "Camera.h"
#include "Fbx.h"

#include "Transform.h"

//#pragma comment(lib, "d3d11.lib")

namespace {
  
    // グローバル変数:
    HINSTANCE hInst;                                // 現在のインターフェイス
    WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
    WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名
	const int WINDOW_WIDTH = 800;                          // ウィンドウの幅
	const int WINDOW_HEIGHT = 600;                          // ウィンドウの高さ
	RECT WIN_RECT = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // ウィンドウの矩形


}

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR lpCmdLine,
                     _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAMEWINDOW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    Fbx model;
    HRESULT hr = model.Load("Assets/FBXTest/TexturedCube/textured_cube.fbx");
    if (FAILED(hr))
    {
        MessageBoxW(nullptr, L"FBX読み込み失敗。出力ウィンドウを確認してください。",
            L"FBX", MB_OK);
        model.Release();
        Direct3D::Release();
        return 1;
    }

    Transform modelTransform;
    modelTransform.scale_ = XMFLOAT3(100.0f, 100.0f, 100.0f);
    modelTransform.rotate_ = XMFLOAT3(20.0f, 30.0f, 0.0f);

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Camera::SetPosition(XMVectorSet(0, 3, -10, 0));
            Camera::SetTarget(XMVectorSet(0, 0, 0, 0));
            Camera::Update();
            modelTransform.rotate_.y += 0.5f;
            modelTransform.Calculation();

            Direct3D::BeginDraw();
            model.Draw(modelTransform);
            Direct3D::EndDraw();
        }
    }
    // GPUのバッファを先に解放してからDirect3Dを終了する。
    model.Release();
    Direct3D::Release();
    return static_cast<int>(msg.wParam);
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMEWINDOW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_CROSS);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する
   AdjustWindowRect(&WIN_RECT, WS_OVERLAPPEDWINDOW, FALSE); // ウィンドウの矩形を調整する
   int windowWidth = WIN_RECT.right - WIN_RECT.left; // 調整後のウィンドウの幅を計算する
   int windowHeight = WIN_RECT.bottom - WIN_RECT.top; // 調整後のウィンドウの高さを計算する
   
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   Direct3D::Initialize(windowWidth, windowHeight, hWnd); // Direct3D 11 の初期化
   // この初期化はHRESULTを確認し、失敗時には描画を開始しない。
   if (FAILED(Direct3D::InitShaderModel()))
   {
       MessageBoxW(hWnd, L"SimpleModel.hlslの初期化に失敗しました。", L"Shader", MB_OK);
       Direct3D::Release();
       return FALSE;
   }
   Camera::Initialize(); // カメラの初期化

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
