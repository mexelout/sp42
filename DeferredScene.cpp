#include "Common.h"
#include "ShaderDevise.h"
#include "Camera.h"
#include "DeferredScene.h"
#include "Model.h"
#include "Vertex.h"
#include "Grid.h"
#include "Input.h"
#include "Box.h"
#include "Sphere.h"
#include "TextureManager.h"

OwnTexture::OwnTexture() {
	tex = NULL;
	sur = NULL;
}
OwnTexture::~OwnTexture() {
	SAFE_RELEASE(tex);
	SAFE_RELEASE(sur);
}
void OwnTexture::createTexture(UINT width, UINT height, UINT levels,DWORD usage, D3DFORMAT format, D3DPOOL pool) {
	auto device = ShaderDevise::device();
	device->CreateTexture(width, height, levels, usage, format, pool, &tex, NULL);
	tex->GetSurfaceLevel(0, &sur);
}

DeferredScene::DeferredScene() {
	m = NULL;
	camera_rot = Common::vec3zero;
}
DeferredScene::~DeferredScene() {
}
DeferredScene* DeferredScene::init() {
	auto device = ShaderDevise::device();

	diffuse.createTexture((UINT)Common::window_width, (UINT)Common::window_height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT);
	normal.createTexture((UINT)Common::window_width, (UINT)Common::window_height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT);

	m = (new Sphere)->init();
	g = (new Grid)->init()->setSize(10);
	camera_rot = D3DXVECTOR3((float)M_PI_4/3, (float)D3DX_PI*0.7f, 6);

	device->GetRenderTarget(0, &bb);
	vb = Common::plane(D3DXVECTOR3(Common::window_width, Common::window_height, 1));

	deferred_vs.loadFunc(device, "deferred.hlsl", "vsDeferred", "vs_2_0");
	deferred_ps.loadFunc(device, "deferred.hlsl", "psDeferred", "ps_2_0");

	sphere_tex_number = TextureManager::loadTexture("textures/wall000.png", device);
	deferred_light_ps.loadFunc(device, "deferred.hlsl", "psDeferredLight", "ps_2_0");

	return this;
}
void DeferredScene::update() {
	if(InputMouse::btn(InputMouse::Left, Input::Press)) {
		D3DXVECTOR3 dir(Camera::dir()), up(Common::vec3zero), left(Common::vec3zero);
		{
			// create camera up direction
			float y = (dir.y >= 0) ? 1.0f : 1.0f;
			float xz_len = D3DXVec2Length(&D3DXVECTOR2(dir.x, dir.z));
			y *= xz_len;
			xz_len = dir.y*-1;
			up = D3DXVECTOR3(dir.x, 0, dir.z);
			D3DXVec3Normalize(&up, &up);
			up *= xz_len;
			up.y = y;
		}
		D3DXVec3Cross(&left, &dir, &up);
		D3DXMATRIX world(m->getWorld()), rot;
		D3DXMatrixRotationAxis(&rot, &up, -(InputMouse::move().x/300.0f));
		world *= rot;
		D3DXMatrixRotationAxis(&rot, &left, (InputMouse::move().y/300.0f));
		world *= rot;
		//m->setWorld(world); // for box
	}
	if(InputMouse::btn(InputMouse::Right, Input::Press)) {
		camera_rot.y += (InputMouse::move().x/300.0f);
		camera_rot.x -= (InputMouse::move().y/300.0f);
		if(camera_rot.x >= ((float)M_PI_2 - 0.017f)) camera_rot.x = ((float)M_PI_2 - 0.017f);
		else if(camera_rot.x <= -((float)M_PI_2 - 0.017f)) camera_rot.x = -((float)M_PI_2 - 0.017f);
	}
	camera_rot.z += InputMouse::wheel() / 300;

	float len = cosf(camera_rot.x), s = sinf(camera_rot.y) * len, c = cosf(camera_rot.y) * len;
	Camera::setEye(D3DXVECTOR3(c, sinf(camera_rot.x), s)*camera_rot.z);
	Camera::setAt(D3DXVECTOR3(0, 0, 0));

}
void DeferredScene::draw() {
	auto device = ShaderDevise::device();

	device->SetRenderTarget(0, diffuse.sur);
	device->SetRenderTarget(1, normal.sur);

	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff191970, 1.0f, 0);
	device->BeginScene();
	D3DXMATRIX world(Common::identity), view(Camera::view()), proj(Camera::projection());
	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	{
		world = m->getWorld();
		D3DXMATRIX wvp(world * view * proj);
		TextureManager::applyTexture(sphere_tex_number, 0, device);
		deferred_vs.constant_table->SetMatrix(device, "world_view_proj", &wvp);
		deferred_vs.constant_table->SetMatrix(device, "world_matrix", &world);
		deferred_ps.constant_table->SetVector(device, "diffuse", &D3DXVECTOR4(1, 1, 1, 1));
		deferred_ps.constant_table->SetVector(device, "specular", &D3DXVECTOR4(1, 1, 1, 1));
		device->SetVertexShader(deferred_vs.vs);
		device->SetPixelShader(deferred_ps.ps);
		m->draw();
		TextureManager::applyTexture(0, 0, device);
		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);
		g->draw();
	}

	device->EndScene();

	device->SetRenderTarget(0, bb);
	device->SetRenderTarget(1, NULL);

	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff191970, 1.0f, 0);
	device->BeginScene();
	world = Common::identity;
	view = Common::identity;
	proj = Camera::ortho();
	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
	device->SetRenderState(D3DRS_LIGHTING, false);

	world = Common::identity;
	device->SetTransform(D3DTS_WORLD, &world);
	device->SetFVF(D3DFVF_CUSTOMVERTEX);
	UINT tex_sampler =  deferred_light_ps.constant_table->GetSamplerIndex("tex_sampler");
	UINT tex_diffuse =  deferred_light_ps.constant_table->GetSamplerIndex("tex_diffuse");
	UINT tex_normal =  deferred_light_ps.constant_table->GetSamplerIndex("tex_normal");
	device->SetTexture(0, NULL);
	device->SetTexture(1, NULL);
	device->SetTexture(2, NULL);
	device->SetTexture(tex_diffuse, diffuse.tex);
	device->SetTexture(tex_normal, normal.tex);
	device->SetPixelShader(deferred_light_ps.ps);
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	device->SetPixelShader(NULL);
	device->SetTexture(tex_diffuse, NULL);
	device->SetTexture(tex_normal, NULL);

	D3DXMatrixScaling(&world, 0.25f, 0.25f, 1);

	world._41 = -Common::window_width*0.375f;
	world._42 = Common::window_height*0.375f;
	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTexture(0, diffuse.tex);
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	world._41 = -Common::window_width*0.375f;
	world._42 = Common::window_height*0.125f;
	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTexture(0, normal.tex);
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


	device->SetRenderState(D3DRS_LIGHTING, true);
	device->SetTexture(0, 0);
	device->SetTexture(1, 0);

	device->EndScene();

}
void DeferredScene::release() {
	SAFE_RELEASE(vb);
	SAFE_RELEASE(bb);
	SAFE_RELEASE_DELETE(g);
	SAFE_RELEASE_DELETE(m);
	deferred_vs.release();
	deferred_ps.release();
	deferred_light_ps.release();
}
// end file