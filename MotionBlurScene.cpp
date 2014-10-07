#include "MotionBlurScene.h"
#include "ShaderDevise.h"
#include "Common.h"
#include "Camera.h"
#include "Input.h"
#include "BlurPlayer.h"
#include "MeshField.h"
#include "GroundMesh.h"

MotionBlurScene::MotionBlurScene(void) {
	player = NULL;
	mesh_field = NULL;
	texture = NULL;
	back_surface = NULL;
	z_surface = NULL;
	back_buffer = NULL;
	z_buffer = NULL;
	vtx = NULL;
	blur_frame_num = 0;
}

MotionBlurScene::~MotionBlurScene(void) {
}

MotionBlurScene* MotionBlurScene::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();

	player = (new BlurPlayer)->init();
	mesh_field = (new GroundMesh)->init();
	D3DXVECTOR3 ppos = player->pos();
	ppos.y = mesh_field->getHeight(&ppos);
	player->setGroundHeight(ppos.y);


	device->GetRenderTarget(0, &back_buffer);
	device->GetDepthStencilSurface(&z_buffer);

	device->CreateVertexBuffer(
		sizeof(CUSTOMVERTEX)*4,
		D3DUSAGE_WRITEONLY,
		D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED,
		&vtx,
		NULL
	);

	float harf_width = Common::window_width/2, harf_height = Common::window_height/2;
	CUSTOMVERTEX vertex[4] = {
		CUSTOMVERTEX(D3DXVECTOR3(-harf_width,  harf_height, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(0, 0)),
		CUSTOMVERTEX(D3DXVECTOR3( harf_width,  harf_height, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(1, 0)),
		CUSTOMVERTEX(D3DXVECTOR3(-harf_width, -harf_height, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(0, 1)),
		CUSTOMVERTEX(D3DXVECTOR3( harf_width, -harf_height, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(1, 1)),
	};

	void *data;
	vtx->Lock(0, sizeof(CUSTOMVERTEX)*4, &data, 0);
	memcpy(data, vertex, sizeof(CUSTOMVERTEX)*4);
	vtx->Unlock();

	D3DXCreateTexture(device,
				(UINT)Common::window_width,
				(UINT)Common::window_height,
				1,
				D3DUSAGE_RENDERTARGET,
				D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT,
				&texture);

	device->CreateDepthStencilSurface((UINT)Common::window_width, (UINT)Common::window_height, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, FALSE, &z_surface, NULL);

	texture->GetSurfaceLevel(0, &back_surface);

	InputMouse::hideCursor();
	InputMouse::fixed(true);


	D3DMATERIAL9 mat = {0};
	mat.Power = 5;
	mat.Diffuse.r = mat.Diffuse.g = mat.Diffuse.b = mat.Diffuse.a = 1;
	mat.Ambient.r = mat.Ambient.g = mat.Ambient.b = mat.Ambient.a = 0.7f;
	mat.Emissive.r = mat.Emissive.g = mat.Emissive.b = mat.Emissive.a = 0.1f;
	mat.Specular.r = mat.Specular.g = mat.Specular.b = mat.Specular.a = 0.2f;
	device->SetMaterial(&mat);

	vertex_shader.loadFunc(device, "motion_blur.hlsl", "vertexShaderMotionBlur", "vs_2_0");
	pixel_shader.loadFunc(device, "motion_blur.hlsl", "pixelShaderMotionBlur", "ps_2_0");

	Camera::setEye(D3DXVECTOR3(30, -43, 0));
	Camera::setAt(D3DXVECTOR3(0, -50, 0));

	// ２の方がブラーってる(え？
	blur_frame_num = 2;

	return this;
}
void MotionBlurScene::update() {
	player->update();
	D3DXVECTOR3 ppos = player->pos();
	ppos.y = mesh_field->getHeight(&ppos);
	player->setGroundHeight(ppos.y);

	for(int dik = DIK_1; dik <= DIK_9; dik++) {
		if(InputKeyboard::isKey(dik, Input::Trigger)) {
			blur_frame_num = dik - DIK_1 + 1;
		}
	}
	if(InputKeyboard::isKey(DIK_0, Input::Trigger)) {
		blur_frame_num = 0;
	}
}
void MotionBlurScene::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	D3DXMATRIX view(Camera::view()), proj(Camera::projection());
	D3DXMATRIX world(Common::identity);

	// texture へ描画
	device->SetRenderTarget(0, back_surface);
	device->SetDepthStencilSurface(z_surface);

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff191970, 1.0f, 0 );
	device->BeginScene();

	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	mesh_field->draw();
	player->draw();

	device->EndScene();

	// バックバッファへ描画
	device->SetRenderTarget(0, back_buffer);
	device->SetDepthStencilSurface(z_buffer);

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0 );
	device->BeginScene();

	view = Common::identity;

	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &Camera::ortho());

	device->SetFVF(D3DFVF_CUSTOMVERTEX);
	device->SetStreamSource(0, vtx, 0, sizeof(CUSTOMVERTEX));
	device->SetRenderState(D3DRS_LIGHTING, false);
	device->SetTexture(0, texture);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	device->SetRenderState(D3DRS_LIGHTING, true);
	device->SetStreamSource(0, NULL, 0, 0);
	device->SetIndices(NULL);
	device->SetTexture(0, NULL);

	device->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0xff191970, 1.0f, 0 );

	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	D3DXMATRIX wvp, owvp;
	view = Camera::view();
	world = player->getWorld();
	wvp = world * view * proj;
	world = player->getOldWorld(blur_frame_num);
	owvp = world * view * proj;
	HRESULT hr = 0;
	hr = vertex_shader.constant_table->SetMatrix(device, "g_wvp", &wvp);
	hr = vertex_shader.constant_table->SetMatrix(device, "g_owvp", &owvp);
	hr = pixel_shader.constant_table->SetInt(device, "blur_frame_num", blur_frame_num);

	device->SetVertexShader(vertex_shader.vs);
	device->SetPixelShader(pixel_shader.ps);
	player->blur_texture = texture;
	player->draw();
	player->blur_texture = NULL;
	device->SetVertexShader(NULL);
	device->SetPixelShader(NULL);

	ShaderDevise::drawText(Common::debug("blur num(0～9): %d", blur_frame_num).c_str());
	ShaderDevise::drawText(Common::debug("WASD 移動").c_str());
	ShaderDevise::drawText(Common::debug("SPACE ジャンプ").c_str());
	ShaderDevise::drawText(Common::debug("ESC アプリケーション終了").c_str());

	device->EndScene();
	device->Present( NULL, NULL, NULL, NULL );
}
void MotionBlurScene::release() {
	vertex_shader.release();
	pixel_shader.release();
	SAFE_RELEASE_DELETE(player);
	SAFE_RELEASE_DELETE(mesh_field);

	SAFE_RELEASE(vtx);

	SAFE_RELEASE(texture);
	SAFE_RELEASE(back_surface);
	SAFE_RELEASE(z_surface);

	SAFE_RELEASE(back_buffer);
	SAFE_RELEASE(z_buffer);
}
