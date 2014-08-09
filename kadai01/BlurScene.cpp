#include "BlurScene.h"
#include "Camera.h"
#include "ShaderDevise.h"
#include "Input.h"
#include "Common.h"
#include "Timer.h"
#include "MeshField.h"

BlurScene::BlurScene(void) {
	player = NULL;
	plane = NULL;
	for(int i = 0; i < 4; i++) vtx[i] = NULL;
	vb = NULL;
	for(int i = 0; i < 4; i++) {
		texture[0]= NULL;
		z_surface[0] = NULL;
		back_surface[0] =  NULL;
	}
	back_buffer = NULL;
	z_buffer = NULL;
}

BlurScene::~BlurScene(void) {
}

BlurScene* BlurScene::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	player = (new BlurPlayer)->init();
	//plane = (new StaticPlane)->init();
	mesh_field = (new MeshField)->init();

	float harf_width = Common::window_width/2, harf_height = Common::window_height/2;
	for(int i = 0; i < 4; i++) {
		device->CreateVertexBuffer(
			sizeof(CUSTOMVERTEX)*4,
			D3DUSAGE_WRITEONLY,
			D3DFVF_CUSTOMVERTEX,
			D3DPOOL_MANAGED,
			&vtx[i],
			NULL
		);

		float x_offset = (i % 2) * harf_width, y_offset = (i / 2) * harf_height;
		CUSTOMVERTEX vertex[4] = {
			CUSTOMVERTEX(D3DXVECTOR3(-harf_width + x_offset,  harf_height - y_offset, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(0, 0)),
			CUSTOMVERTEX(D3DXVECTOR3(   0 + x_offset,  harf_height - y_offset, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(1, 0)),
			CUSTOMVERTEX(D3DXVECTOR3(-harf_width + x_offset,    0.0f - y_offset, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(0, 1)),
			CUSTOMVERTEX(D3DXVECTOR3(   0 + x_offset,    0.0f - y_offset, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(1, 1)),
		};

		void *data;
		vtx[i]->Lock(0, sizeof(CUSTOMVERTEX)*4, &data, 0);
		memcpy(data, vertex, sizeof(CUSTOMVERTEX)*4);
		vtx[i]->Unlock();
	}

	device->CreateVertexBuffer(
		sizeof(CUSTOMVERTEX)*4,
		D3DUSAGE_WRITEONLY,
		D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED,
		&vb,
		NULL
	);

	CUSTOMVERTEX vertex[4] = {
		CUSTOMVERTEX(D3DXVECTOR3(-harf_width,  harf_height, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(0, 0)),
		CUSTOMVERTEX(D3DXVECTOR3( harf_width,  harf_height, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(1, 0)),
		CUSTOMVERTEX(D3DXVECTOR3(-harf_width, -harf_height, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(0, 1)),
		CUSTOMVERTEX(D3DXVECTOR3( harf_width, -harf_height, 0), D3DXVECTOR3(0, 0, -1), 0xffffffff, D3DXVECTOR2(1, 1)),
	};

	void *data;
	vb->Lock(0, sizeof(CUSTOMVERTEX)*4, &data, 0);
	memcpy(data, vertex, sizeof(CUSTOMVERTEX)*4);
	vb->Unlock();

	for(int i = 0; i < 4; i++) {
		texture[i] = NULL;
		z_surface[i] = NULL;
		back_surface[i] = NULL;

		D3DXCreateTexture(device,
					(UINT)Common::window_width,
					(UINT)Common::window_height,
					1,
					D3DUSAGE_RENDERTARGET,
					D3DFMT_A8R8G8B8,
					D3DPOOL_DEFAULT,
					&texture[i]);

		device->CreateDepthStencilSurface((UINT)Common::window_width, (UINT)Common::window_height, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, FALSE, &z_surface[i], NULL);

		texture[i]->GetSurfaceLevel(0, &back_surface[i]);
	}

	device->GetRenderTarget(0, &back_buffer);
	device->GetDepthStencilSurface(&z_buffer);

	blur_vertical.loadFunc(device, "blur.fx", "pixelShaderBlurX", "ps_2_0");
	blur_horizontal.loadFunc(device, "blur.fx", "pixelShaderBlurY", "ps_2_0");
	depth_vertex.loadFunc(device, "depth_buffer_shadow.fx", "vertexShadowDepth", "vs_2_0");
	depth_pixel.loadFunc(device, "depth_buffer_shadow.fx", "pixelShaderDepth", "ps_2_0");
	multi_tex.loadFunc(device, "multi_tex.fx", "pixelShaderFocus", "ps_2_0");

	expf(-5);

	return this;
}
void BlurScene::update() {
	static float angle = D3DX_PI*0.5f;
	static float angle_x = (float)M_PI_4;
	static float length = 5;

	if(InputMouse::btn(InputMouse::Right, Input::Press)) {
		angle+=(InputMouse::move().x/100.0f);
		angle_x-=(InputMouse::move().y/100.0f);
		if(angle_x >= ((float)M_PI_2 - 0.017f)) angle_x = ((float)M_PI_2 - 0.017f);
		else if(angle_x <= -((float)M_PI_2 - 0.017f)) angle_x = -((float)M_PI_2 - 0.017f);
	}
	length += InputMouse::wheel() / 100;

	float len = cosf(angle_x), s = sinf(angle) * len, c = cosf(angle) * len;

	player->update();
	//plane->update();
	mesh_field->update();

	Camera::setEye(D3DXVECTOR3(c, sinf(angle_x), s)*length + player->pos());
	Camera::setAt(player->pos() + D3DXVECTOR3(0, 1, 0));

	Timer::update();

}

void BlurScene::draw() {
	static int slide = 0;

	if(InputKeyboard::isKey(DIK_RETURN, Input::Trigger)) {
		slide = (slide+1)%5;
	}

	LPDIRECT3DDEVICE9 device = ShaderDevise::device();

	// original
	device->SetRenderTarget(0, back_surface[0]);
	device->SetDepthStencilSurface(z_surface[0]);

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 );

	device->BeginScene();

	player->draw();
	//plane->draw();
	mesh_field->draw();

	device->EndScene();

	// vertical
	device->SetRenderTarget(0, back_surface[1]);
	device->SetDepthStencilSurface(z_surface[1]);

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 );

	device->BeginScene();

	D3DXMATRIX world, view, proj;
	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&view);
	D3DXMatrixOrthoLH(&proj, Common::window_width, Common::window_height, -1, 1);

	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
	device->SetTexture(0, texture[0]);
	device->SetPixelShader(blur_vertical.ps);
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	device->EndScene();

	// horizontal
	device->SetRenderTarget(0, back_surface[2]);
	device->SetDepthStencilSurface(z_surface[2]);

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 );

	device->BeginScene();

	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
	device->SetTexture(0, texture[0]);
	device->SetPixelShader(blur_horizontal.ps);
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	device->EndScene();


	// depth
	device->SetRenderTarget(0, back_surface[3]);
	device->SetDepthStencilSurface(z_surface[3]);

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 );

	device->BeginScene();

	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
	device->SetTexture(0, NULL);
	device->SetVertexShader(depth_vertex.vs);
	device->SetPixelShader(depth_pixel.ps);

	depth_vertex.constant_table->SetMatrix(device, "g_world", &player->getWorld());
	depth_vertex.constant_table->SetMatrix(device, "g_view", &Camera::view());
	depth_vertex.constant_table->SetMatrix(device, "g_proj", &Camera::projection());
	player->draw();

	depth_vertex.constant_table->SetMatrix(device, "g_world", &plane->getWorld());
	//plane->draw();
	mesh_field->draw();

	device->EndScene();


	// back buffer
	device->SetRenderTarget(0, back_buffer);
	device->SetDepthStencilSurface(z_buffer);

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 30, 67), 1.0f, 0 );
	device->BeginScene();

	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&view);
	D3DXMatrixOrthoLH(&proj, Common::window_width, Common::window_height, -1, 1);

	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);


	device->SetFVF(D3DFVF_CUSTOMVERTEX);
	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	switch(slide) {
		case 0:
			device->SetVertexShader(NULL);
			device->SetPixelShader(NULL);

			device->SetTexture(0, texture[0]);
			device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
			device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			break;
		case 1:
			device->SetVertexShader(NULL);
			device->SetPixelShader(NULL);

			device->SetTexture(0, texture[1]);
			device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
			device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			break;
		case 2:
			device->SetVertexShader(NULL);
			device->SetPixelShader(NULL);

			device->SetTexture(0, texture[2]);
			device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
			device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			break;
		case 3:
			device->SetVertexShader(NULL);
			device->SetPixelShader(NULL);

			device->SetTexture(0, texture[3]);
			device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
			device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			break;


		case 4:
			device->SetPixelShader(multi_tex.ps);
			device->SetVertexShader(NULL);

			device->SetTexture(multi_tex.constant_table->GetSamplerIndex("g_tex_sampler1"), texture[0]);
			device->SetTexture(multi_tex.constant_table->GetSamplerIndex("g_tex_sampler2"), texture[2]);
			device->SetTexture(multi_tex.constant_table->GetSamplerIndex("g_tex_sampler3"), texture[3]);
			device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
			device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			device->SetPixelShader(NULL);
			device->SetTexture(multi_tex.constant_table->GetSamplerIndex("g_tex_sampler1"), NULL);
			device->SetTexture(multi_tex.constant_table->GetSamplerIndex("g_tex_sampler2"), NULL);
			device->SetTexture(multi_tex.constant_table->GetSamplerIndex("g_tex_sampler3"), NULL);

			break;
		default:
			break;
	}



	device->SetRenderState(D3DRS_LIGHTING, TRUE);

	device->EndScene();

	device->Present( NULL, NULL, NULL, NULL );
}
void BlurScene::release() {
	SAFE_RELEASE_DELETE(player);
	SAFE_RELEASE_DELETE(plane);
	SAFE_RELEASE_DELETE(mesh_field);

	for(int i = 0; i < 4; i++) SAFE_RELEASE(vtx[i]);
	SAFE_RELEASE(vb);

	for(int i = 0; i < 4; i++) {
		SAFE_RELEASE(texture[i]);
		SAFE_RELEASE(back_surface[i]);
		SAFE_RELEASE(z_surface[i]);
	}
	SAFE_RELEASE(back_buffer);
	SAFE_RELEASE(z_buffer);

	blur_vertical.release();
	blur_horizontal.release();
	depth_vertex.release();
	depth_pixel.release();
	multi_tex.release();
}
