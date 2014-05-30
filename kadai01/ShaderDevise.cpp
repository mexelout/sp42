
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "ShaderDevise.h"
#include "Model.h"

#define self this

LPDIRECT3D9 ShaderDevise::_d3d;
LPDIRECT3DDEVICE9 ShaderDevise::_device;

ShaderDevise::ShaderDevise(void) {
}

ShaderDevise::~ShaderDevise(void) {
}

ShaderDevise* ShaderDevise::init(int w, int h, HWND hWnd) {
	if(!(_d3d = Direct3DCreate9(D3D_SDK_VERSION))) return 0;

	D3DPRESENT_PARAMETERS d3dpp = {w, h, D3DFMT_UNKNOWN, 0, D3DMULTISAMPLE_NONE, 0,
	D3DSWAPEFFECT_DISCARD,NULL,TRUE,TRUE,D3DFMT_D24S8,0,0}; 

	if( FAILED(_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &_device)))
		if( FAILED(_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &_device)))
			if( FAILED(_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &_device)))
				if( FAILED(_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &_device))) {
					_d3d->Release();
					_d3d = NULL;
					_device = NULL;
					return 0;
				}

	_device->SetRenderState(D3DRS_CULLMODE , D3DCULL_CCW);
	_device->SetRenderState(D3DRS_LIGHTING , FALSE);
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );  
	_device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
			
	return self;
}

LPDIRECT3DDEVICE9 ShaderDevise::device() {
	return _device;
}

void ShaderDevise::release() {
	_device->Release();
	_d3d->Release();
}

void ShaderDevise::stencilShadow(const Model* model) {
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
	_device->SetRenderState(D3DRS_STENCILENABLE, false);
	_device->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP); // Ž¸”sF‚»‚Ì‚Ü‚Ü
	_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR); // ¬Œ÷F+1
	_device->SetRenderState(D3DRS_STENCILREF, 1);
	_device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	_device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	// ‰~’Œ•`‰æ
	_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
	_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	// ‰~’Œ•`‰æ
	_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	_device->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0f);
	_device->SetRenderState(D3DRS_ZENABLE, false);
	_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL); // ref‚Æ“¯‚¶‚Æ‚±‚ë‚¾‚¯•`‰æ
	_device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP); // Ž¸”sF‚»‚Ì‚Ü‚Ü
	_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP); // ¬Œ÷F‚»‚Ì‚Ü‚Ü
	// ‘S‰æ–Ê••`‰æ
	_device->SetRenderState(D3DRS_ZENABLE, true);
	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
	_device->SetRenderState(D3DRS_STENCILENABLE, false);

}

