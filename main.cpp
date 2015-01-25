#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <mmsystem.h>
#include "ShaderDevise.h"
#include "Common.h"
#include "Vertex.h"
#include "Camera.h"
#include "Model.h"
#include "Plane.h"
#include "Input.h"
#include "DebugScene.h"
#include "SceneManager.h"
#include "WindowManager.h"
#include "ModelManager.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(855);
#endif
	MSG msg; HWND hWnd;
	WNDCLASSEX wcex ={sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance, NULL, LoadCursor(NULL , IDC_ARROW),
	(HBRUSH)(COLOR_WINDOW+1), NULL, "cls_name", NULL};

	if(!RegisterClassEx(&wcex))
		return 0;

	int w = (int)Common::window_width, h = (int)Common::window_height;
	RECT clientRect = {0, 0, w, h};
	AdjustWindowRect( &clientRect, WS_OVERLAPPEDWINDOW, FALSE );

	if(!(hWnd = CreateWindow("cls_name", "SP42", WS_OVERLAPPEDWINDOW, 0, 0,
		clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
		NULL, NULL, hInstance, NULL)))
		return 0;
	WindowManager& wm = WindowManager::inst();
	wm.setInst(hInstance);
	wm.setWnd(hWnd);

	ShaderDevise::init(w, h, hWnd);

	ShowWindow(hWnd, nCmdShow);
	Camera::init();
	Input::init(hInstance, hWnd);

	SceneManager::init();

	DWORD fps = 60;
	// ���b�Z�[�W ���[�v
	do{
		DWORD start_time, end_time, pass_time;
		start_time = timeGetTime();

		Input::update();

		if(InputKeyboard::isKey(DIK_R, Input::Press)) {
			fps = (fps != 60) ? fps + 1: fps;
		}
		if(InputKeyboard::isKey(DIK_T, Input::Press)) {
			fps = (fps != 0) ? fps - 1: fps;
		}

		if(fps) {
			SceneManager::update();
			SceneManager::draw();
			ShaderDevise::resetOffsetY();
		}

		end_time = timeGetTime();
		pass_time = end_time - start_time;

		DWORD _fps_ = (fps) ? fps : 60;
		DWORD wait_time = (1000 / _fps_ > pass_time) ? (1000 / _fps_ - pass_time) : 0;
		{
			DWORD start_time = timeGetTime();

			do {
				// ���b�Z�[�W����
				if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				if(wait_time > 0) Sleep(1); // ������Ƌx�e�iCPU�̐�L���������邽�߁j
			}while(timeGetTime() < wait_time + start_time); // wait_time �������
		}

	} while(msg.message != WM_QUIT);

	ModelManager::release();
	SceneManager::release();
	Input::release();
	ShaderDevise::release();

	return 0;
}