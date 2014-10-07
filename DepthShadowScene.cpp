#include "DepthShadowScene.h"
#include "Camera.h"
#include "ShaderDevise.h"
#include "Input.h"
#include "Common.h"

DepthShadowScene::DepthShadowScene(void) {
	texture = NULL;
	back_surface = NULL;
	z_surface = NULL;
	ZeroMemory(&viewport, sizeof(viewport));
	show_depth_shadow_texture = true;
}


DepthShadowScene::~DepthShadowScene(void) {
}

DepthShadowScene* DepthShadowScene::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	player = (new DepthShadowPlayer)->init();
	plane = (new Plane)->init();

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
		CUSTOMVERTEX(D3DXVECTOR3(harf_width - harf_height, harf_height, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(0, 0)),
		CUSTOMVERTEX(D3DXVECTOR3(harf_width              , harf_height, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(1, 0)),
		CUSTOMVERTEX(D3DXVECTOR3(harf_width - harf_height, - 0, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(0, 1)),
		CUSTOMVERTEX(D3DXVECTOR3(harf_width              , - 0, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(1, 1)),
	};
	
	void *data;
	vtx->Lock(0, sizeof(CUSTOMVERTEX)*4, &data, 0);
	memcpy(data, vertex, sizeof(CUSTOMVERTEX)*4);
	vtx->Unlock();



	texture = NULL;
	z_surface = NULL;
	back_surface = NULL;
	D3DXCreateTexture(device,
				1024,
				1024,
				1,
				D3DUSAGE_RENDERTARGET,
                D3DFMT_A8R8G8B8,
                D3DPOOL_DEFAULT,
                &texture);

	device->CreateDepthStencilSurface( 1024, 1024, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, FALSE, &z_surface, NULL);

	texture->GetSurfaceLevel(0, &back_surface);

	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = 1024;
	viewport.Height = 1024;
	viewport.MinZ = 0;
	viewport.MaxZ = 1;

	device->GetRenderTarget(0, &back_buffer);
	device->GetDepthStencilSurface(&z_buffer);

	InputMouse::hideCursor();

	depth_vs.loadFunc(device, "depth_buffer_shadow.hlsl", "vertexShadowDepth", "vs_2_0");
	depth_ps.loadFunc(device, "depth_buffer_shadow.hlsl", "pixelShaderDepth", "ps_2_0");
	depth_shadow_vs.loadFunc(device, "depth_buffer_shadow.hlsl", "vertexShaderDepthShadow", "vs_2_0");
	depth_shadow_vs.loadFunc(device, "depth_buffer_shadow.hlsl", "pixelShaderDepthShadow", "ps_2_0");

	D3DXHANDLE g_light_view = depth_vs.constant_table->GetConstantByName(NULL, "g_light_view");
	D3DXHANDLE g_world = depth_vs.constant_table->GetConstantByName(NULL, "g_world");
	D3DXHANDLE g_view = depth_vs.constant_table->GetConstantByName(NULL, "g_view");
	D3DXHANDLE g_proj = depth_vs.constant_table->GetConstantByName(NULL, "g_proj");

	return this;
}
void DepthShadowScene::update() {
	static float y_axis_angle = D3DX_PI*0.5f, x_axis_angle = D3DX_PI*0.25f;
	static float length = 5;
	if(InputMouse::btn(InputMouse::Right, Input::Press)) {
		D3DXVECTOR2 move(InputMouse::move());
		y_axis_angle += D3DXToRadian(move.x);
		x_axis_angle -= D3DXToRadian(move.y);
		if(x_axis_angle >= (float)(M_PI_2-0.017f)) x_axis_angle = (float)(M_PI_2-0.017f);
		if(x_axis_angle <= -(float)(M_PI_2-0.017f)) x_axis_angle = -(float)(M_PI_2-0.017f);
	}

	player->update();
	plane->update();
	player->calcGround(plane->vertices());

	float hori_len = cosf(x_axis_angle);
	float s = sinf(y_axis_angle), c = cosf(y_axis_angle);
	length += (InputMouse::wheel() / 100);
	Camera::setEye(D3DXVECTOR3(c*hori_len, sinf(x_axis_angle), s*hori_len)*length + player->pos());
	Camera::setAt(player->pos() + D3DXVECTOR3(0, 1, 0));

	if(InputKeyboard::isKey(DIK_1, Input::Trigger)) show_depth_shadow_texture = !show_depth_shadow_texture;
}
void DepthShadowScene::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();

	{
		HRESULT hr = 0;
		Common::debug("==== start ====");
		D3DVIEWPORT9 old_viewport;
		device->GetViewport(&old_viewport);
		device->SetViewport(&viewport);

		device->SetRenderTarget(0, back_surface);
		device->SetDepthStencilSurface(z_surface);
		device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0 );
		device->BeginScene();

		D3DXMATRIX world, view, proj, light_view;
		D3DXMatrixIdentity(&world);

		D3DXMatrixLookAtLH(
			&view,
			&D3DXVECTOR3(ShaderDevise::getLightVec()*-20),
			&D3DXVECTOR3(0, 0, 0),
			&D3DXVECTOR3(0, 1, 0)
		);
		light_view = view;
		D3DXMatrixPerspectiveFovLH(&proj, D3DXToRadian(45), Common::window_width/Common::window_height, 1.0f, 100.0f);

		device->SetVertexShader(depth_vs.vs);
		device->SetPixelShader(depth_ps.ps);

		device->SetTransform(D3DTS_WORLD, &world);
		device->SetTransform(D3DTS_VIEW, &view);
		device->SetTransform(D3DTS_PROJECTION, &proj);

		device->SetTexture(0, NULL);
		depth_vs.constant_table->SetMatrix(device, "g_world", &world);
		depth_vs.constant_table->SetMatrix(device, "g_view", &view);
		depth_vs.constant_table->SetMatrix(device, "g_proj", &proj);

		hr = depth_vs.constant_table->SetMatrix(device, "g_light_view", &light_view);
		Common::debug("%b", FAILED(hr));

		plane->draw();

		depth_vs.constant_table->SetMatrix(device, "g_world", &player->getWorld());
		player->draw();

		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);

		device->EndScene();

		device->SetRenderTarget(0, back_buffer);
		device->SetDepthStencilSurface(z_buffer);
		device->SetViewport(&old_viewport);
		Common::debug("==== end ====");
	}

	{
		device->SetTexture(0, texture);

		// Direct3D‚Ìˆ—
		device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
		device->BeginScene();

		D3DXMATRIX view;
		view = Camera::view();
		device->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&D3DXMATRIX()));
		device->SetTransform(D3DTS_VIEW, &view);
		device->SetRenderState(D3DRS_LIGHTING, true);

		depth_vs.constant_table->SetMatrix(device, "g_world", D3DXMatrixIdentity(&D3DXMATRIX()));
		depth_vs.constant_table->SetMatrix(device, "g_view", &view);

		device->SetVertexShader(depth_shadow_vs.vs);
		device->SetPixelShader(depth_shadow_ps.ps);
		plane->draw();
		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);
		player->draw();
	}

	if(show_depth_shadow_texture) {
		device->SetStreamSource(0, vtx, 0, sizeof(CUSTOMVERTEX));
		device->SetFVF(D3DFVF_CUSTOMVERTEX);
		D3DXMATRIX world, view, ortho;
		device->SetRenderState(D3DRS_LIGHTING, false);
		D3DXMatrixIdentity(&world);
		D3DXMatrixIdentity(&view);
		D3DXMatrixOrthoLH(&ortho, Common::window_width, Common::window_height, -1, 1);
		device->SetTexture(0, texture);
		device->SetTransform(D3DTS_WORLD, &world);
		device->SetTransform(D3DTS_VIEW, &view);
		device->SetTransform(D3DTS_PROJECTION, &ortho);

		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		device->SetRenderState(D3DRS_LIGHTING, true);
	}

	device->EndScene();
	device->Present( NULL, NULL, NULL, NULL );


}
void DepthShadowScene::release() {
	depth_vs.release();
	depth_ps.release();
	depth_shadow_vs.release();
	depth_shadow_ps.release();

	SAFE_RELEASE_DELETE(player);
	SAFE_RELEASE_DELETE(plane);

	SAFE_RELEASE(back_buffer);
	SAFE_RELEASE(z_buffer);

	SAFE_RELEASE(texture);
	SAFE_RELEASE(z_surface);
	SAFE_RELEASE(back_surface);
	SAFE_RELEASE(vtx);

}


