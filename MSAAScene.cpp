#include "Common.h"
#include "ShaderDevise.h"
#include "Camera.h"
#include "MSAAScene.h"
#include "Input.h"
#include "Vertex.h"

MSAAScene::MSAAScene() {
	sphere = NULL;
	camera_rot = Common::vec3zero;
	vb = NULL;
	bb = NULL;
}
MSAAScene::~MSAAScene() {
}
MSAAScene* MSAAScene::init() {
	auto device = ShaderDevise::device();

	sphere = (new Sphere)->init();
	camera_rot = D3DXVECTOR3((float)M_PI_4*0.75f, (float)D3DX_PI*0.7f, 10);

	// ２倍サイズ
	z.createTexture(
		(UINT)Common::window_width*2,
		(UINT)Common::window_height*2,
		0,
		D3DUSAGE_RENDERTARGET
		,D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT);

	device->CreateDepthStencilSurface(
		(UINT)Common::window_width*2,
		(UINT)Common::window_height*2,
		D3DFMT_D16, D3DMULTISAMPLE_NONE,
		0, TRUE, &dep, NULL);

	// オリジナルサイズ
	origin_z.createTexture(
		(UINT)Common::window_width,
		(UINT)Common::window_height,
		0,
		D3DUSAGE_RENDERTARGET
		,D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT);

	device->CreateDepthStencilSurface(
		(UINT)Common::window_width,
		(UINT)Common::window_height,
		D3DFMT_D16, D3DMULTISAMPLE_NONE,
		0, TRUE, &origin_dep, NULL);

	msaa_vs_z.loadFunc(device, "msaa.hlsl", "vsWithDepth", "vs_2_0");
	msaa_ps_z.loadFunc(device, "msaa.hlsl", "psWithDepth", "ps_2_0");

	msaa_vs.loadFunc(device, "msaa.hlsl", "vsAntiAliasing", "vs_2_0");
	msaa_ps.loadFunc(device, "msaa.hlsl", "psAntiAliasing", "ps_3_0");

	{
		D3DXVECTOR3 scl(Common::window_width, Common::window_height, 1), uv_pos(0, 0, 0), uv_scl(1, 1, 0);
		LPDIRECT3DVERTEXBUFFER9 ___vtx;
		device->CreateVertexBuffer(
			sizeof(CUSTOMVERTEX)*4,
			D3DUSAGE_WRITEONLY,
			D3DFVF_CUSTOMVERTEX,
			D3DPOOL_MANAGED,
			&___vtx,
			NULL
		);

		CUSTOMVERTEX2D *data;
		___vtx->Lock(0, 0, (void**)&data, 0);
		data[0] = CUSTOMVERTEX2D(0, 0, 0, 1.0f, 0xffffffff, uv_pos.x, uv_pos.y);
		data[1] = CUSTOMVERTEX2D(scl.x, 0, 0, 1.0f, 0xffffffff, uv_pos.x + uv_scl.x, uv_pos.y);
		data[2] = CUSTOMVERTEX2D(0, scl.y, 0, 1.0f, 0xffffffff, uv_pos.x, uv_pos.y + uv_scl.y);
		data[3] = CUSTOMVERTEX2D(scl.x, scl.y, 0, 1.0f, 0xffffffff, uv_pos.x + uv_scl.x, uv_pos.y + uv_scl.y);
		___vtx->Unlock();

		vb = ___vtx;
	}
	device->GetRenderTarget(0, &bb);
	device->GetDepthStencilSurface(&bdb);

	return this;
}
void MSAAScene::update() {
	if(InputMouse::btn(InputMouse::Right, Input::Press)) {
		camera_rot.y += (InputMouse::move().x/300.0f);
		camera_rot.x -= (InputMouse::move().y/300.0f);
		if(camera_rot.x >= ((float)M_PI_2 - 0.017f)) camera_rot.x = ((float)M_PI_2 - 0.017f);
		else if(camera_rot.x <= -((float)M_PI_2 - 0.017f)) camera_rot.x = -((float)M_PI_2 - 0.017f);
	}
	camera_rot.z += InputMouse::wheel() / 300;

	float len = cosf(camera_rot.x), s = sinf(camera_rot.y) * len, c = cosf(camera_rot.y) * len;
	Camera::setEye(D3DXVECTOR3(c, sinf(camera_rot.x), s)*camera_rot.z);
	Camera::setAt(D3DXVECTOR3(0, 1, 0));

}
void MSAAScene::draw() {
	auto device = ShaderDevise::device();
	static bool is_antialias = false;
	static bool is_first = false;
	if(is_first == false) {
		is_first = true;
		is_antialias = false;
	}

	if(InputKeyboard::isKey(DIK_RETURN, Input::Trigger)) {
		is_antialias = !is_antialias;
	}

	if(is_antialias) {
		D3DVIEWPORT9 vp;
		device->GetViewport(&vp);
		device->SetRenderTarget(0, z.sur);
		device->SetDepthStencilSurface(dep);
		vp.X = (DWORD)0;
		vp.Y = (DWORD)0;
		vp.Width = (DWORD)Common::window_width*2;
		vp.Height = (DWORD)Common::window_height*2;
		device->SetViewport(&vp);
		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		D3DXMATRIX world(Common::identity), view(Camera::view()), proj(Camera::projection()), wvp(Common::identity);
		device->BeginScene();
		device->SetTransform(D3DTS_WORLD, &world);
		device->SetTransform(D3DTS_VIEW, &view);
		device->SetTransform(D3DTS_PROJECTION, &proj);
		wvp = world*view*proj;
		msaa_vs_z.constant_table->SetMatrix(device, "world_view_proj", &wvp);
		msaa_vs_z.constant_table->SetMatrix(device, "world_matrix", &world);
		device->SetVertexShader(msaa_vs_z.vs);
		device->SetPixelShader(msaa_ps_z.ps);
		if(sphere) sphere->draw();
		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);
		device->EndScene();

		device->SetRenderTarget(0, bb);
		device->SetDepthStencilSurface(bdb);
		vp.X = 0;
		vp.Y = 0;
		vp.Width = (DWORD)Common::window_width;
		vp.Height = (DWORD)Common::window_height;
		device->SetViewport(&vp);
		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff191970, 1.0f, 0);
		device->BeginScene();

		device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX2D));

		device->SetFVF(D3DFVF_CUSTOMVERTEX);
		device->SetTexture(0, z.tex);

		msaa_vs.constant_table->SetMatrix(device, "world_view_proj", &wvp);

		device->SetVertexShader(msaa_vs.vs);
		device->SetPixelShader(msaa_ps.ps);
		if(sphere) sphere->draw();
		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);
		device->SetTexture(0, NULL);

		device->EndScene();
	} else {
		D3DVIEWPORT9 vp;
		device->GetViewport(&vp);
		device->SetRenderTarget(0, origin_z.sur);
		device->SetDepthStencilSurface(origin_dep);
		vp.X = (DWORD)0;
		vp.Y = (DWORD)0;
		vp.Width = (DWORD)Common::window_width;
		vp.Height = (DWORD)Common::window_height;
		device->SetViewport(&vp);
		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		D3DXMATRIX world(Common::identity), view(Camera::view()), proj(Camera::projection()), wvp(Common::identity);
		device->BeginScene();
		device->SetTransform(D3DTS_WORLD, &world);
		device->SetTransform(D3DTS_VIEW, &view);
		device->SetTransform(D3DTS_PROJECTION, &proj);
		wvp = world*view*proj;
		msaa_vs_z.constant_table->SetMatrix(device, "world_view_proj", &wvp);
		msaa_vs_z.constant_table->SetMatrix(device, "world_matrix", &world);
		device->SetVertexShader(msaa_vs_z.vs);
		device->SetPixelShader(msaa_ps_z.ps);
		if(sphere) sphere->draw();
		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);
		device->EndScene();

		device->SetRenderTarget(0, bb);
		device->SetDepthStencilSurface(bdb);
		vp.X = 0;
		vp.Y = 0;
		vp.Width = (DWORD)Common::window_width;
		vp.Height = (DWORD)Common::window_height;
		device->SetViewport(&vp);
		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff191970, 1.0f, 0);
		device->BeginScene();
		world = Common::identity;
		view = Common::identity;
		proj = Camera::ortho();
		device->SetTransform(D3DTS_WORLD, &world);
		device->SetTransform(D3DTS_VIEW, &view);
		device->SetTransform(D3DTS_PROJECTION, &proj);

		device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX2D));

		device->SetFVF(D3DFVF_CUSTOMVERTEX2D);
		device->SetTexture(0, origin_z.tex);

		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		device->SetTexture(0, NULL);

		device->EndScene();
	}
}
void MSAAScene::release() {
	msaa_vs_z.release();
	msaa_ps_z.release();
	msaa_vs.release();
	msaa_ps.release();
	SAFE_RELEASE_DELETE(sphere);
	SAFE_RELEASE(vb);
	SAFE_RELEASE(dep);
	SAFE_RELEASE(origin_dep);
	SAFE_RELEASE(bb);
	SAFE_RELEASE(bdb);
}
// end file