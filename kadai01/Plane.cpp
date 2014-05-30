
#include <string>
#include "Plane.h"
#include "Camera.h"
#include "ShaderDevise.h"

Plane::Plane(void) {
	vbuf = NULL;
	D3DXMatrixIdentity(&world);
}

Plane::~Plane(void) {
}

Plane* Plane::init(LPDIRECT3DDEVICE9 device) {
	device->CreateVertexBuffer(
		sizeof(CUSTOMVERTEX)*4,
		D3DUSAGE_WRITEONLY,
		D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED,
		&vbuf,
		NULL
		);

	CUSTOMVERTEX vtx[] = {
		{D3DXVECTOR3(-10, 0,  10), D3DXVECTOR3(0, 1, 0), 0xffff0000, D3DXVECTOR2(0, 0)},
		{D3DXVECTOR3( 10, 4,  10), D3DXVECTOR3(0, 1, 0), 0xff00ff00, D3DXVECTOR2(1, 0)},
		{D3DXVECTOR3(-10, 4, -10), D3DXVECTOR3(0, 1, 0), 0xff0000ff, D3DXVECTOR2(0, 1)},
		{D3DXVECTOR3( 10, 0, -10), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(1, 1)}
	};

	D3DXVec3Cross(&vtx[0].nor, &(vtx[1].pos - vtx[0].pos), &(vtx[2].pos - vtx[0].pos));
	D3DXVec3Cross(&vtx[3].nor, &(vtx[2].pos - vtx[3].pos), &(vtx[1].pos - vtx[3].pos));
	D3DXVec3Normalize(&vtx[0].nor, &vtx[0].nor);
	D3DXVec3Normalize(&vtx[3].nor, &vtx[3].nor);

	for(int i = 0; i < 4; i++) 
		_vertices[i] = vtx[i];

	void *data;
	vbuf->Lock(0, sizeof(CUSTOMVERTEX)*4, &data, 0);
	memcpy(data, vtx, sizeof(CUSTOMVERTEX)*4);
	vbuf->Unlock();

	LPD3DXBUFFER code;
	LPD3DXBUFFER error;

	D3DXCompileShaderFromFile("shader.fx", NULL, NULL,
								"vertexShaderTexture", "vs_2_0", 0,
								&code, &error, &_vs_constant_table);

	if(error) {
		OutputDebugString((LPCSTR)error->GetBufferPointer());
		vbuf->Release();
		error->Release();
		return 0;
	}

	device->CreateVertexShader((DWORD*)code->GetBufferPointer(), &_vs);
	code->Release();

	D3DXCompileShaderFromFile("shader.fx", NULL, NULL,
								"vertexShaderShadow", "vs_2_0", 0,
								&code, &error, &_vs_shadow_constant_table);

	if(error) {
		OutputDebugString((LPCSTR)error->GetBufferPointer());
		vbuf->Release();
		error->Release();
		return 0;
	}

	device->CreateVertexShader((DWORD*)code->GetBufferPointer(), &_vs_shadow);
	code->Release();

	D3DXCompileShaderFromFile("shader.fx", NULL, NULL,
								"pixelShaderTexture", "ps_2_0", 0,
								&code, &error, &_ps_constant_table);

	if(error) {
		OutputDebugString((LPCSTR)error->GetBufferPointer());
		vbuf->Release();
		_vs_constant_table->Release();
		_vs->Release();
		error->Release();
		return 0;
	}

	device->CreatePixelShader((DWORD*)code->GetBufferPointer(), &_ps);
	code->Release();

	texture = NULL;
	white = NULL;

	D3DXCreateTextureFromFile(device, "textures/shadow.png", &texture);
	D3DXCreateTextureFromFile(device, "textures/white.png", &white);

	angle = (D3DX_PI / 180.0f * 2);
	//D3DXMATRIX rot;
	//D3DXMatrixRotationAxis(&rot, &D3DXVECTOR3(1, 0, 0), (D3DX_PI / 180.0f * -90));
	//world *= rot;

/*
	D3DXCreateTexture(device,
				640,
				480,
				0,
				D3DUSAGE_RENDERTARGET, 
                D3DFMT_A8R8G8B8, 
                D3DPOOL_DEFAULT, 
                &texture);

	device->CreateDepthStencilSurface( 
                 640,
                 480,
                 D3DFMT_D16, 
                 D3DMULTISAMPLE_NONE, 
                 0, 
                 TRUE, 
                 &z_surface, 
                 NULL);

	texture->GetSurfaceLevel(0, &back_surface);
*/

	return this;
}

void Plane::update() {
}

void Plane::draw(LPDIRECT3DDEVICE9 device) {
	device->SetVertexShader(_vs);
	device->SetPixelShader(_ps);
	device->SetFVF(D3DFVF_CUSTOMVERTEX);
	const D3DXMATRIX& view = Camera::view(), projection = Camera::projection();
	D3DXMATRIX rot;
	D3DXMatrixRotationAxis(&rot, &D3DXVECTOR3(0, 1, 0), angle);
	//world *= rot;
	D3DXMATRIX world_view_projection = world * view * projection;
	_vs_constant_table->SetMatrix(device, "g_world_view_projection", &world_view_projection);
	D3DXVECTOR4 light_dir(0, 0.4f, -1, 0);
	_vs_constant_table->SetVector(device, "g_light_direction", &light_dir);
	_vs_constant_table->SetMatrix(device, "g_world", &world);
	D3DXVECTOR4 mat_dif(1, 1, 1, 1);
	_vs_constant_table->SetVector(device, "g_material_diffuse", &mat_dif);

	device->SetTexture(0, white);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	device->SetStreamSource(0, vbuf, 0, sizeof(CUSTOMVERTEX));
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	device->SetTexture(0, texture);
	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	device->SetVertexShader(_vs_shadow);
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	
	device->SetTexture(0, NULL);
	device->SetVertexShader(NULL);
	device->SetPixelShader(NULL);
}

void Plane::release() {
	vbuf->Release();
	_vs->Release();
	_vs_constant_table->Release();
	_vs_shadow_constant_table->Release();
	_vs_shadow->Release();
	_ps->Release();
	_ps_constant_table->Release();
	if(texture) texture->Release();
	if(white) white->Release();
	delete this;
}

void Plane::setPlayerPosition(D3DXVECTOR3 pos) {
	_vs_shadow_constant_table->SetVector(ShaderDevise::device(), "g_player_position", &D3DXVECTOR4(pos, 0));
}

std::array<CUSTOMVERTEX, 4> Plane::vertices() {
	return _vertices;
}