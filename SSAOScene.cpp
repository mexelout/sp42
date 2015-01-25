#include "Common.h"
#include "ShaderDevise.h"
#include "Camera.h"
#include "SSAOScene.h"
#include "TextureManager.h"
#include "Input.h"
#include "Vertex.h"
#include "Bokoboko.h"
#include "StaticPlane.h"
#include "Grid.h"

SSAOScene::SSAOScene() {
	camera_rot = Common::vec3zero;
	bkbk = NULL;
	sp = NULL;
	bb = NULL;
	vb = NULL;
	grid = NULL;
}
SSAOScene::~SSAOScene() {
}
SSAOScene* SSAOScene::init() {
	auto device = ShaderDevise::device();

	camera_rot = D3DXVECTOR3((float)M_PI_4*0.75f, (float)D3DX_PI*0.7f, 10);

	bkbk = (new Bokoboko)->init();
	sp = (new StaticPlane)->init();
	grid = (new Grid)->init()->setSize(50);

	//vb = Common::plane(D3DXVECTOR3(Common::window_width, Common::window_height, 1));

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

	diffuse.createTexture((UINT)Common::window_width, (UINT)Common::window_height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16, D3DPOOL_DEFAULT);
	depth.createTexture((UINT)Common::window_width, (UINT)Common::window_height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16, D3DPOOL_DEFAULT);

	ssao_vs.loadFunc(device, "ssao.hlsl", "vsWithDepth", "vs_2_0");
	SAFE_NULL_RETURN(ssao_vs.constant_table);
	ssao_ps.loadFunc(device, "ssao.hlsl", "psWithDepth", "ps_2_0");
	SAFE_NULL_RETURN(ssao_ps.constant_table);

	ssao_light_ps.loadFunc(device, "ssao.hlsl", "psSSAO", "ps_3_0");
	SAFE_NULL_RETURN(ssao_light_ps.constant_table);

	for(int i = 0; i < SAMPLE_NUM; i++) {
		float th, r;
		th = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * D3DX_PI;
		r = (float)rand() / RAND_MAX;
		rand_vec2[i] = D3DXVECTOR2(cosf(th), sinf(th)) * r;
	}

	return this;
}
void SSAOScene::update() {
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
void SSAOScene::draw() {
	auto device(ShaderDevise::device());

	device->SetRenderTarget(0, diffuse.sur);
	device->SetRenderTarget(1, depth.sur);
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	device->BeginScene();
	D3DXMATRIX world(Common::identity), view(Camera::view()), proj(Camera::projection()), wvp(Common::identity);
	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	wvp = world*view*proj;
	ssao_vs.constant_table->SetMatrix(device, "world_view_proj", &wvp);
	ssao_vs.constant_table->SetMatrix(device, "world_matrix", &world);
	ssao_vs.constant_table->SetVector(device, "light_direction", &D3DXVECTOR4(ShaderDevise::getLight(0).Direction, 0));
	{
		float depth_max(0);
		LPD3DXMESH mesh(bkbk->mesh());
		CUSTOMVERTEXMODEL* data(NULL);
		mesh->LockVertexBuffer(0, (void**)&data);
		D3DXVECTOR3 eye(Camera::eye());
		for(int i = 0, len = (int)mesh->GetNumVertices(); i < len; i++) {
			D3DXVECTOR4 tmp(0, 0, 0, 0);
			D3DXVec3Transform(&tmp, &data[i].pos, &wvp);
			if(tmp.z > depth_max) depth_max = tmp.z;
		}
		mesh->UnlockVertexBuffer();
		ssao_ps.constant_table->SetFloat(device, "depth_max", depth_max);
	}
	ssao_ps.constant_table->SetVector(device, "diffuse", &D3DXVECTOR4(1, 1, 1, 1));
	UINT tex_sampler =  ssao_ps.constant_table->GetSamplerIndex("tex_sampler");
	

	device->SetVertexShader(ssao_vs.vs);
	device->SetPixelShader(ssao_ps.ps);

	device->SetTexture(tex_sampler, NULL);

	if(bkbk) bkbk->draw();

	device->SetTexture(tex_sampler, NULL);
	device->SetVertexShader(NULL);
	device->SetPixelShader(NULL);

	if(grid) grid->draw();

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

	device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX2D));
	device->SetRenderState(D3DRS_LIGHTING, false);

	device->SetFVF(D3DFVF_CUSTOMVERTEX2D);

	device->SetPixelShader(ssao_light_ps.ps);

	// set texture
	UINT tex_diffuse =  ssao_light_ps.constant_table->GetSamplerIndex("tex_diffuse");
	UINT tex_depth =  ssao_light_ps.constant_table->GetSamplerIndex("tex_depth");
	device->SetTexture(tex_diffuse, diffuse.tex);
	device->SetTexture(tex_depth, depth.tex);

	ssao_light_ps.constant_table->SetFloatArray(device, "rand2", (float*)rand_vec2, SAMPLE_NUM*2);
	ssao_light_ps.constant_table->SetFloat(device, "occlusion", 1);
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	device->SetTexture(tex_diffuse, NULL);
	device->SetTexture(tex_depth, NULL);
	device->SetPixelShader(NULL);

	//D3DXMatrixScaling(&world, 0.25f, 0.25f, 1);
	//
	//world._41 = -Common::window_width*0.375f;
	//world._42 = Common::window_height*0.375f;
	//device->SetTransform(D3DTS_WORLD, &world);
	//device->SetTexture(0, diffuse.tex);
	//device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	//
	//world._41 = -Common::window_width*0.375f;
	//world._42 = Common::window_height*0.125f;
	//device->SetTransform(D3DTS_WORLD, &world);
	//device->SetTexture(0, depth.tex);
	//device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	//
	//device->SetRenderState(D3DRS_LIGHTING, true);
	//device->SetTexture(0, 0);

	device->EndScene();
}
void SSAOScene::release() {
	ssao_vs.release();
	ssao_ps.release();
	ssao_light_ps.release();
	SAFE_RELEASE(bb);
	SAFE_RELEASE(vb);
	SAFE_RELEASE_DELETE(bkbk);
	SAFE_RELEASE_DELETE(sp);
	SAFE_RELEASE_DELETE(grid);
}
// end file