#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")


#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "Common.h"
#include "ShaderDevise.h"
#include "Vertex.h"
#include "Camera.h"
#include "Model.h"
#include "Plane.h"
#include "Input.h"
#include "DepthShadowScene.h"
#include "BlurScene.h"
#include "MultiTexScene.h"
#include "ToonScene.h"
#include "MotionBlurScene.h"
#include "FogScene.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_CREATE:
			return 0;
		case WM_KEYDOWN:
			if(wParam == VK_ESCAPE)
				DestroyWindow(hWnd);
			return 0;
		case WM_MOUSEMOVE:
			return 0;
		case WM_DESTROY:                                    // ウィンドウが破棄されたときの処理
			PostQuitMessage(0); // 終了メッセージ
			return 0;

		default:                                            // デフォルト処理
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(138);
#endif
	srand((unsigned)time(0));

	// アプリケーションの初期化
	MSG msg; HWND hWnd;
	WNDCLASSEX wcex ={sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance, NULL, LoadCursor(NULL , IDC_ARROW),
	(HBRUSH)(COLOR_WINDOW+1), NULL, "cls_name", NULL};

	if(!RegisterClassEx(&wcex))
		return 0;

	int w = (int)Common::window_width, h = (int)Common::window_height;
	RECT clientRect = {0, 0, w, h};
	AdjustWindowRect( &clientRect, WS_OVERLAPPEDWINDOW, FALSE );

	if(!(hWnd = CreateWindow("cls_name", "SP42", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
		clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
		NULL, NULL, hInstance, NULL)))
		return 0;

	// Direct3Dの初期化
	ShaderDevise* sd = (new ShaderDevise())->init(w, h, hWnd);

	ShowWindow(hWnd, nCmdShow);
	Camera::init();
	Input::init(hInstance, hWnd);

	Scene* s = (new FogScene)->init();

	if(s)
		// メッセージ ループ
		do{
			if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			} else{
				Input::update();

				s->update();
				s->draw();
			}
		} while(msg.message != WM_QUIT);

	SAFE_RELEASE_DELETE(s);
	Input::release();
	SAFE_RELEASE_DELETE(sd);

	return 0;
}