#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "ShaderDevise.h"
#include "Vertex.h"
#include "Camera.h"
#include "ToonModel.h"
#include "Plane.h"

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
	// アプリケーションの初期化
	MSG msg; HWND hWnd;
	WNDCLASSEX wcex ={sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance, NULL, LoadCursor(NULL , IDC_ARROW),
	(HBRUSH)(COLOR_WINDOW+1), NULL, "cls_name", NULL};

	if(!RegisterClassEx(&wcex))
		return 0;

	int w = 640, h = 480;
	RECT clientRect = {0, 0, w, h};
	AdjustWindowRect( &clientRect, WS_OVERLAPPEDWINDOW, FALSE );

	if(!(hWnd = CreateWindow("cls_name", "shader program", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
		clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
		NULL, NULL, hInstance, NULL)))
		return 0;

	// Direct3Dの初期化
	ShaderDevise* sd = (new ShaderDevise())->init(w, h, hWnd);

	// フォントの生成
	int fontsize = 24;
	D3DXFONT_DESC lf = {fontsize, 0, 0, 1, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
	PROOF_QUALITY, FIXED_PITCH | FF_MODERN, _T("ＭＳ ゴシック")};

	// ID3DXFontコンポーネント生成
	LPD3DXFONT pD3DFont;
	if(FAILED(D3DXCreateFontIndirect(sd->device(), &lf, &pD3DFont))){
		sd->release();
		return 0;
	}

	sd->device()->SetFVF(D3DFVF_CUSTOMVERTEX);

	FLOAT Ang = 0.0f;   // 回転角度

	D3DXVECTOR4 light_dir(0, 0.4f, -1, 0);
	D3DXVec4Normalize(&light_dir, &light_dir);

	D3DXVECTOR4 ambient_color(0.1f, 0.1f, 0.1f, 1);


	ShowWindow(hWnd, nCmdShow);

	Plane* plane = (new Plane())->init(sd->device());
	
	Camera::init();

	ToonModel* toon_model = (new ToonModel())->init();

	// メッセージ ループ
	do{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		} else{

			plane->update();
			toon_model->update();

			toon_model->calcGround(plane->vertices());
			D3DXVECTOR3 pos = toon_model->pos();

			pos = toon_model->pos();
			plane->setPlayerPosition(pos - D3DXVECTOR3(-1, 0, -1));
			Camera::setAt(pos);
			Camera::setEye(pos + D3DXVECTOR3(5, 5, 5));

			//Ang += 0.03f;
			// Direct3Dの処理
			sd->device()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
			sd->device()->BeginScene();

			// 描画
			//RECT r={0, 0, 0, 0};
			//pD3DFont->DrawText(NULL, _T("Hello World !"), -1, &r, DT_LEFT | DT_SINGLELINE | DT_NOCLIP, 0xffffffff);

			plane->draw(sd->device());
			toon_model->draw();

			sd->device()->EndScene();
			sd->device()->Present( NULL, NULL, NULL, NULL );
		}
	} while(msg.message != WM_QUIT);

	toon_model->release();
	plane->release();
	pD3DFont->Release();
	sd->release();

	return 0;
}