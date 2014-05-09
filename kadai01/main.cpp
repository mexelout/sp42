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
#include "Model.h"
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
		case WM_DESTROY:                                    // �E�B���h�E���j�����ꂽ�Ƃ��̏���
			PostQuitMessage(0); // �I�����b�Z�[�W
			return 0;

		default:                                            // �f�t�H���g����
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	// �A�v���P�[�V�����̏�����
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

	// Direct3D�̏�����
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

	// �t�H���g�̐���
	int fontsize = 24;
	D3DXFONT_DESC lf = {fontsize, 0, 0, 1, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
	PROOF_QUALITY, FIXED_PITCH | FF_MODERN, _T("�l�r �S�V�b�N")};

	// ID3DXFont�R���|�[�l���g����
	LPD3DXFONT pD3DFont;
	if(FAILED(D3DXCreateFontIndirect(sd->device(), &lf, &pD3DFont))){
		sd->release();
		return 0;
	}

	sd->device()->SetRenderState(D3DRS_CULLMODE , D3DCULL_CCW);
	sd->device()->SetRenderState(D3DRS_LIGHTING , FALSE);
	sd->device()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	sd->device()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	sd->device()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	sd->device()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	sd->device()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );  
 
	sd->device()->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
 
	sd->device()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	sd->device()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	sd->device()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

	sd->device()->SetFVF(D3DFVF_CUSTOMVERTEX);

	LPDIRECT3DVERTEXSHADER9 vertex_shader;
	sd->device()->CreateVertexShader((DWORD*)code->GetBufferPointer(), &vertex_shader);

	FLOAT Ang = 0.0f;   // ��]�p�x

	D3DXVECTOR4 light_dir(0, 0.4f, -1, 0);
	D3DXVec4Normalize(&light_dir, &light_dir);

	D3DXVECTOR4 ambient_color(0.8f, 0.8f, 0.8f, 1);

	Model* model = (new Model())->init(sd->device(), "models/sword001.x");

	ShowWindow(hWnd, nCmdShow);

	Plane* plane = (new Plane())->init(sd->device());
	
	Camera::init();

	// ���b�Z�[�W ���[�v
	do{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		} else{

			plane->update();

			Ang += 0.03f;
			// Direct3D�̏���
			sd->device()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
			sd->device()->BeginScene();

			// �`��
			RECT r={0, 0, 0, 0};
			pD3DFont->DrawText(NULL, _T("Hello World !"), -1, &r, DT_LEFT | DT_SINGLELINE | DT_NOCLIP, 0xffffffff);

			D3DXMATRIX world;
			const D3DXMATRIX& view = Camera::view();
			const D3DXMATRIX& projection = Camera::projection();
			D3DXMATRIX worldViewProjection;

			D3DXMatrixIdentity(&world);
			D3DXMATRIX translate, rot;
			D3DXMatrixRotationX(&rot, Ang);
			world *= rot;
			D3DXMatrixRotationY(&rot, Ang);
			world *= rot;
			D3DXMatrixTranslation(&translate, 0, 2, 0);
			world *= translate;

			worldViewProjection = world * view * projection;
			vs_constant_table->SetMatrix(sd->device(), "g_world_view_projection", &worldViewProjection);
			vs_constant_table->SetVector(sd->device(), "g_light_direction", &light_dir);
			vs_constant_table->SetMatrix(sd->device(), "g_world", &world);
			vs_constant_table->SetVector(sd->device(), "g_ambient", &ambient_color);
			
			sd->device()->SetVertexShader(vertex_shader);

			model->mesh()->DrawSubset(0);

			sd->device()->SetVertexShader(NULL);

			plane->draw(sd->device());

			sd->device()->EndScene();
			sd->device()->Present( NULL, NULL, NULL, NULL );
		}
	} while(msg.message != WM_QUIT);

	vertex_shader->Release();

	code->Release();
	vs_constant_table->Release();
	model->release();
	plane->release();
	pD3DFont->Release();
	sd->release();

	return 0;
}