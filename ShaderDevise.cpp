
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "Common.h"
#include "ShaderDevise.h"
#include "Cylinder.h"
#include "Vertex.h"

#define self this

LPDIRECT3D9 ShaderDevise::_d3d;
LPDIRECT3DDEVICE9 ShaderDevise::_device;

LPD3DXCONSTANTTABLE ShaderDevise::vs_constant_table;
LPD3DXCONSTANTTABLE ShaderDevise::ps_constant_table;
LPDIRECT3DVERTEXSHADER9 ShaderDevise::vertex_shader;
LPDIRECT3DPIXELSHADER9 ShaderDevise::pixel_shader;
D3DLIGHT9 ShaderDevise::light;
LPD3DXFONT ShaderDevise::font;
int ShaderDevise::offset_text_y;

D3DXVECTOR3 ShaderDevise::light_vec;


ShaderDevise::ShaderDevise(void) {
}

ShaderDevise::~ShaderDevise(void) {
}

ShaderDevise* ShaderDevise::init(int w, int h, HWND hWnd) {
	if(!(_d3d = Direct3DCreate9(D3D_SDK_VERSION))) return 0;

	D3DPRESENT_PARAMETERS d3dpp = {w, h, D3DFMT_UNKNOWN, 0, D3DMULTISAMPLE_NONE, 0,
	D3DSWAPEFFECT_DISCARD,NULL,TRUE,TRUE,D3DFMT_D24S8,0,0}; 

	if( FAILED(_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &_device))) {
		if( FAILED(_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &_device))) {
			if( FAILED(_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &_device))) {
				if( FAILED(_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &_device))) {
					_d3d->Release();
					_d3d = NULL;
					_device = NULL;
					return 0;
				}
			}
		}
	}

	_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	_device->SetRenderState(D3DRS_LIGHTING, TRUE);
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);  
 	_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	ZeroMemory( &light, sizeof(D3DLIGHT9) );
	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = light.Diffuse.g = light.Diffuse.b = light.Diffuse.a = 1.0f;
	light.Specular.r = light.Specular.g = light.Specular.b = light.Specular.a = 0.6f;
	light.Ambient.r = light.Ambient.g = light.Ambient.b = light.Ambient.a = 0.3f;
	D3DXVECTOR3 vecDir;
	vecDir = D3DXVECTOR3(-1.0f, -1.0f, -0.1f);
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
	_device->SetLight( 0, &light );
	_device->LightEnable( 0, TRUE );
	light_vec = light.Direction;
	_device->SetRenderState(D3DRS_SPECULARENABLE, true);

	D3DMATERIAL9 mat = {
		1, 1, 1, 1,
		0.5f, 0.5f, 0.5f, 1.0f,
		0.8f, 0.8f, 0.8f, 1.0f,
		0, 0, 0, 0,
		5
	};

	_device->SetMaterial(&mat);

	// フォントの生成
	int fontsize = 24;
	D3DXFONT_DESC lf = {fontsize, 0, 0, 1, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
	PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "ＭＳ ゴシック"};
	if(FAILED(D3DXCreateFontIndirect(_device, &lf, &font))){
		return 0;
	}

	return self;
}

LPDIRECT3DDEVICE9 ShaderDevise::device() {
	return _device;
}

void ShaderDevise::release() {
	SAFE_RELEASE(font);
	SAFE_RELEASE(vertex_shader);
	SAFE_RELEASE(pixel_shader);

	SAFE_RELEASE(vs_constant_table);
	SAFE_RELEASE(ps_constant_table);

	SAFE_RELEASE(_device);
	SAFE_RELEASE(_d3d);
}

void ShaderDevise::stencilShadow(Cylinder* model) {

	// ----------------------------------------------------------
	// パス1:影ボリュームの描画
	// ----------------------------------------------------------
	// 深度バッファに書き込みはしない
	_device->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
	// レンダリングターゲットに書き込みはしない
	_device->SetRenderState( D3DRS_COLORWRITEENABLE,  FALSE );
	// フラットシェーディングする
	_device->SetRenderState( D3DRS_SHADEMODE,   D3DSHADE_FLAT );
	// 両面描く
	_device->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
 
	// 両面ステンシルを使用する
	_device->SetRenderState( D3DRS_STENCILENABLE, TRUE );
	_device->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, TRUE );
 
	// ステンシルテストは常に合格する（＝テストしない）
	_device->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
	_device->SetRenderState( D3DRS_CCW_STENCILFUNC,  D3DCMP_ALWAYS );
	// ステンシルバッファの増減を1に設定する
	_device->SetRenderState( D3DRS_STENCILREF,    0x1 );
	_device->SetRenderState( D3DRS_STENCILMASK,      0xffffffff );
	_device->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );
	// 表面は深度テストに合格したらステンシルバッファの内容を+1する
	_device->SetRenderState( D3DRS_STENCILPASS,  D3DSTENCILOP_INCR );
	_device->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
	_device->SetRenderState( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );
	// 裏面は深度テストに合格したらステンシルバッファの内容を-1する
	_device->SetRenderState( D3DRS_CCW_STENCILPASS, D3DSTENCILOP_DECR );
	_device->SetRenderState( D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP );
	_device->SetRenderState( D3DRS_CCW_STENCILFAIL,  D3DSTENCILOP_KEEP );

	model->draw();

	// 状態を元に戻す
	_device->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	_device->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
	_device->SetRenderState( D3DRS_ZWRITEENABLE,    TRUE );
	_device->SetRenderState( D3DRS_COLORWRITEENABLE,  0xf );
	_device->SetRenderState( D3DRS_STENCILENABLE,  FALSE );
	_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	_device->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, FALSE );
 
	// ----------------------------------------------------------
	// パス2:影の描画
	// ----------------------------------------------------------
	// 深度テストはしない
	_device->SetRenderState( D3DRS_ZENABLE,          FALSE );
	// ステンシルテストはする
	_device->SetRenderState( D3DRS_STENCILENABLE,  TRUE );
	// アルファブレンディングは線形に掛ける
	_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	_device->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	// ポリゴンを描画するときには、テクスチャと頂点色の両方を見る
	_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
 
	// ステンシルバッファの値が１以上のときに書き込む
	_device->SetRenderState( D3DRS_STENCILREF,  0x1 );
	_device->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL );
	_device->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );

	float harf_width = Common::window_width/2, harf_height = Common::window_height/2;
	// 全画面黒描画
	CUSTOMVERTEX vtx[4] = {
		CUSTOMVERTEX(D3DXVECTOR3(-harf_width, harf_height, 0), D3DXVECTOR3(0, 0, -1), 0x80000000, D3DXVECTOR2(0, 0)),
		CUSTOMVERTEX(D3DXVECTOR3( harf_width, harf_height, 0), D3DXVECTOR3(0, 0, -1), 0x80000000, D3DXVECTOR2(0, 0)),
		CUSTOMVERTEX(D3DXVECTOR3(-harf_width, -harf_height, 0), D3DXVECTOR3(0, 0, -1), 0x80000000, D3DXVECTOR2(0, 0)),
		CUSTOMVERTEX(D3DXVECTOR3( harf_width, -harf_height, 0), D3DXVECTOR3(0, 0, -1), 0x80000000, D3DXVECTOR2(1, 1)),
	};
	_device->SetFVF(D3DFVF_CUSTOMVERTEX);
	D3DXMATRIX world, view, ortho;
	D3DXMATRIX worldViewProjection;

	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&view);
	D3DXMatrixOrthoLH(&ortho, Common::window_width, Common::window_height, -1, 1);

	worldViewProjection = world * view * ortho;

	vs_constant_table->SetMatrix(_device, "g_world_view_projection", &worldViewProjection);
	vs_constant_table->SetMatrix(_device, "g_world", &world);

	_device->SetVertexShader(vertex_shader);
	_device->SetPixelShader(pixel_shader);

	_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vtx, sizeof(CUSTOMVERTEX));
	_device->SetVertexShader(NULL);
	_device->SetPixelShader(NULL);

	// 状態を元に戻す
	_device->SetRenderState( D3DRS_ZENABLE,          TRUE );
	_device->SetRenderState( D3DRS_STENCILENABLE,  FALSE );
	_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}

void ShaderDevise::beginDepthShadow() {

}

void ShaderDevise::endDepthShadow() {
}

D3DXVECTOR3& ShaderDevise::getLightVec() {
	return light_vec;
}

void ShaderDevise::drawText(const char* text) {
	// 描画
	RECT r = {0, offset_text_y, 0, 0};
	font->DrawText(NULL, text, -1, &r, DT_LEFT | DT_SINGLELINE | DT_NOCLIP, 0xffffffff);
	offset_text_y += 24;
}

void ShaderDevise::resetOffsetY() {
	offset_text_y = 0;
}
