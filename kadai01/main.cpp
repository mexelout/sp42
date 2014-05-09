#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "ShaderDevise.h"
#include "Vertex.h"
#include "Model.h"

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

	if(!(hWnd = CreateWindow("cls_name", "kadai01", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
		clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
		NULL, NULL, hInstance, NULL)))
		return 0;

	// Direct3Dの初期化
	ShaderDevise* sd = (new ShaderDevise())->init(w, h, hWnd);

	LPD3DXBUFFER code;
	LPD3DXBUFFER error;
	LPD3DXCONSTANTTABLE vs_constant_table;

	D3DXCompileShaderFromFile("shader.fx", NULL, NULL,
								"vertexShader3D", "vs_2_0", 0,
								&code, &error, &vs_constant_table);

	if(error) {
		OutputDebugString((LPCSTR)error->GetBufferPointer());
		sd->release();
		MessageBox(hWnd, "shader error.\ncheck output string message.", "error", MB_OK);
		return 0;
	}

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

	sd->device()->SetRenderState(D3DRS_CULLMODE , D3DCULL_CCW);
	sd->device()->SetRenderState(D3DRS_LIGHTING , FALSE);
	sd->device()->SetFVF(D3DFVF_CUSTOMVERTEX);

	LPDIRECT3DVERTEXSHADER9 vertex_shader;
	sd->device()->CreateVertexShader((DWORD*)code->GetBufferPointer(), &vertex_shader);

	FLOAT Ang = 0.0f;   // 回転角度

	D3DXVECTOR4 light_dir(0, 0.4f, -1, 0);
	D3DXVec4Normalize(&light_dir, &light_dir);

	D3DXVECTOR4 ambient_color(0.8f, 0.8f, 0.8f, 1);

	Model* model = (new Model())->init(sd->device(), "models/sword001.x");

	ShowWindow(hWnd, nCmdShow);

	// メッセージ ループ
	do{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		} else{

			Ang += 0.03f;
			// Direct3Dの処理
			sd->device()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
			sd->device()->BeginScene();

			// 描画
			RECT r={0, 0, 0, 0};
			pD3DFont->DrawText(NULL, _T("Hello World !"), -1, &r, DT_LEFT | DT_SINGLELINE | DT_NOCLIP, 0xffffffff);

			D3DXMATRIX world, view, projection;
			D3DXMATRIX worldViewProjection;

			D3DXMatrixIdentity(&world);
			D3DXMATRIX rot;
			D3DXMatrixRotationX(&rot, Ang);
			D3DXMatrixMultiply(&world, &world, &rot);
			D3DXMatrixRotationY(&rot, Ang);
			D3DXMatrixMultiply(&world, &world, &rot);

			// ビュー変換
			// 視点は原点固定ですが、カメラの位置は適当です
			D3DXMatrixLookAtLH(
				&view,
				&D3DXVECTOR3(3, 3, 3),
				&D3DXVECTOR3(0, 0, 0),
				&D3DXVECTOR3(0, 1, 0)
			);

			// 射影変換
			D3DXMatrixPerspectiveFovLH(&projection, D3DXToRadian(45), 640.0f/480.0f, 1.0f, 10000.0f);

			worldViewProjection = world * view * projection;
			vs_constant_table->SetMatrix(sd->device(), "g_world_view_projection", &worldViewProjection);
			vs_constant_table->SetVector(sd->device(), "g_light_direction", &light_dir);
			vs_constant_table->SetMatrix(sd->device(), "g_world", &world);
			vs_constant_table->SetVector(sd->device(), "g_ambient", &ambient_color);
			
			sd->device()->SetVertexShader(vertex_shader);

			model->mesh()->DrawSubset(0);

			sd->device()->SetVertexShader(NULL);
			sd->device()->EndScene();
			sd->device()->Present( NULL, NULL, NULL, NULL );
		}
	} while(msg.message != WM_QUIT);

	vertex_shader->Release();

	code->Release();
	vs_constant_table->Release();
	model->release();
	pD3DFont->Release();
	sd->release();

	return 0;
}