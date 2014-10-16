#include "MultiTexScene.h"
#include "ShaderDevise.h"
#include "Common.h"
#include "Camera.h"
#include "Input.h"

MultiTexScene::MultiTexScene(void) {
	texture1 = NULL;
	texture2 = NULL;
	vtx = NULL;
	ps_ct = NULL;
	ps = NULL;
}

MultiTexScene::~MultiTexScene(void) {
}

MultiTexScene* MultiTexScene::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	D3DXCreateTextureFromFile(device, "textures/texture.png", &texture1);
	D3DXCreateTextureFromFile(device, "textures/eye2R.png", &texture2);
	float harf_width = Common::window_width/2, harf_height = Common::window_height/2;

	device->CreateVertexBuffer(
		sizeof(CUSTOMVERTEX)*4,
		D3DUSAGE_WRITEONLY,
		D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED,
		&vtx,
		NULL
		);

	CUSTOMVERTEX vtx[] = {
		CUSTOMVERTEX(D3DXVECTOR3(-harf_width, harf_height,  0), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(0, 0)),
		CUSTOMVERTEX(D3DXVECTOR3( harf_width, harf_height,  0), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(1, 0)),
		CUSTOMVERTEX(D3DXVECTOR3(-harf_width, -harf_height,  0), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(0, 1)),
		CUSTOMVERTEX(D3DXVECTOR3( harf_width, -harf_height,  0), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(1, 1))
	};

	void *data;
	this->vtx->Lock(0, sizeof(CUSTOMVERTEX)*4, &data, 0);
	memcpy(data, vtx, sizeof(CUSTOMVERTEX)*4);
	this->vtx->Unlock();

	LPD3DXBUFFER code = NULL;
	LPD3DXBUFFER error = NULL;

	D3DXCompileShaderFromFile("multi_tex.hlsl", NULL, NULL,
							"pixelShaderMulthTex", "ps_2_0", 0,
							&code, &error, &ps_ct);

	if(error) {
		OutputDebugString((LPCSTR)error->GetBufferPointer());
		error->Release();
		release();
		return 0;
	}

	device->CreatePixelShader((DWORD*)code->GetBufferPointer(), &ps);
	code->Release();

	return this;
}

void MultiTexScene::update() {
}

void MultiTexScene::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0 );
	device->BeginScene();

	D3DXMATRIX world, view, proj;
	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&view);
	D3DXMatrixOrthoLH(&proj, Common::window_width, Common::window_height, -1, 1);

	device->SetFVF(D3DFVF_CUSTOMVERTEX);
	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	device->SetStreamSource(0, vtx, 0, sizeof(CUSTOMVERTEX));
	device->SetRenderState(D3DRS_LIGHTING, false);

	device->SetPixelShader(ps);
	device->SetTexture(ps_ct->GetSamplerIndex("g_tex_sampler1"), texture1);
	device->SetTexture(ps_ct->GetSamplerIndex("g_tex_sampler2"), texture2);

	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	device->EndScene();

}

void MultiTexScene::release() {
	SAFE_RELEASE(texture1);
	SAFE_RELEASE(texture2);
	SAFE_RELEASE(vtx);
	SAFE_RELEASE(ps_ct);
	SAFE_RELEASE(ps);
}


