#include "WinApp.h"
#define IDI_ICON1 101
WinApp* WinApp::instance = nullptr;

#ifdef _DEBUG
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // DEBUG
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifdef _DEBUG

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
#endif // DEBUG
	//　メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		// ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);

}

WinApp* WinApp::GetInstance()
{
	if (instance == nullptr) {
		instance = new WinApp;
	}
	return instance;
}

void WinApp::Initialize()
{
	HRESULT result = CoInitializeEx(0, COINIT_MULTITHREADED);
	D3DResourceLeakChecker leakCheck;

	if (FAILED(result)) {
		// エラー処理
		MessageBox(nullptr, L"COMライブラリの初期化に失敗しました", L"エラー", MB_OK);
		return;
	}

	// ウィンドウプロージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	wc.lpszClassName = L"CG2WindowClass";
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	// アイコン
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));			/// メインアイコン
	// ウィンドウクラスを登録する
	RegisterClass(&wc);

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth , kClientHeight };
	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//--------------- ウィンドウの生成 ---------------//
	hwnd = CreateWindow(
		wc.lpszClassName,			  // 利用するクラス名
		L"3107_七転び八起キリン",	  // タイトルバー
		WS_OVERLAPPEDWINDOW,		  // よく見るウィンドウスタイル
		CW_USEDEFAULT,				  // 表示X座標 (Windowsに任せる)
		CW_USEDEFAULT,				  // 表示Y座標 (WindowsOsに任せる)
		wrc.right - wrc.left,		  // ウィンドウ横幅
		wrc.bottom - wrc.top,		  // ウィンドウ縦幅
		nullptr,					  // 親ウィンドウハンドル
		nullptr,					  // メニューハンドル
		wc.hInstance,				  // インスタンスハンドル
		nullptr						  // オプション
	);

	// ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);
	// システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	SendMessage(GetConsoleWindow(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
}


void WinApp::Finalize()
{
	CloseWindow(hwnd);
	CoUninitialize();
	delete instance;
	instance = nullptr;
}

bool WinApp::ProcessMessage()
{
	MSG msg{};

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return true;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return false;
}
