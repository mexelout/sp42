#include "Cylinder.h"
#include "Camera.h"
#include "ShaderDevise.h"

Cylinder::Cylinder(void) {
}


Cylinder::~Cylinder(void) {
}

Cylinder* Cylinder::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	Model::init(device, "models/cylinder.x");

	LPD3DXBUFFER code;
	LPD3DXBUFFER error;
	D3DXCompileShaderFromFile("shader.hlsl", NULL, NULL,
								"vertexShaderTexture", "vs_2_0", 0,
								&code, &error, &vs_constant_table);

	if(error) {
		OutputDebugString((LPCSTR)error->GetBufferPointer());
		this->release();
		MessageBox(NULL, "shader error.\ncheck output string message.", "error", MB_OK);
		return 0;
	}

	device->CreateVertexShader((DWORD*)code->GetBufferPointer(), &vertex_shader);

	code->Release();

	D3DXCompileShaderFromFile("shader.hlsl", NULL, NULL,
								"pixelShaderTexture", "ps_2_0", 0,
								&code, &error, &ps_constant_table);

	if(error) {
		OutputDebugString((LPCSTR)error->GetBufferPointer());
		this->release();
		MessageBox(NULL, "shader error.\ncheck output string message.", "error", MB_OK);
		return 0;
	}

	device->CreatePixelShader((DWORD*)code->GetBufferPointer(), &pixel_shader);

	code->Release();

	D3DXCreateTextureFromFile(device, "textures/white.png", &texture);

	_pos = D3DXVECTOR3(0, 0, 0);

	return this;
}

void Cylinder::update() {
}

void Cylinder::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();

	const D3DXMATRIX& view = Camera::view();
	const D3DXMATRIX& projection = Camera::projection();
	D3DXMATRIX worldViewProjection;

	D3DXMatrixIdentity(&world);
	D3DXMATRIX translate, rot;

	D3DXMatrixTranslation(&translate, _pos.x, _pos.y, _pos.z);
	D3DXMatrixRotationAxis(&rot, &D3DXVECTOR3(0, 0, 1), 0.5f);
	world *= rot;
	world *= translate;

	worldViewProjection = world * view * projection;

	vs_constant_table->SetMatrix(device, "g_world_view_projection", &worldViewProjection);
	vs_constant_table->SetMatrix(device, "g_world", &world);


	device->SetVertexShader(vertex_shader);
	device->SetPixelShader(pixel_shader);


	LPD3DXBUFFER materials = this->materials();
	device->SetTexture(0, texture);
	for(int i = 0, len = numMaterials(); i < len; i++) {
		D3DXMATERIAL mat = ((D3DXMATERIAL*)(materials->GetBufferPointer()))[i];
		D3DXVECTOR4 mat_dif(mat.MatD3D.Diffuse.r, mat.MatD3D.Diffuse.g, mat.MatD3D.Diffuse.b, mat.MatD3D.Diffuse.a);
		vs_constant_table->SetVector(device, "g_material_diffuse", &mat_dif);
		this->mesh()->DrawSubset(i);
	}

	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	D3DXVECTOR4 mat_dif(1, 1, 1, 1);
	vs_constant_table->SetVector(device, "g_material_diffuse", &mat_dif);

	device->SetVertexShader(NULL);
	device->SetPixelShader(NULL);

}

void Cylinder::release() {
	vertex_shader->Release();
	pixel_shader->Release();

	texture->Release();
	vs_constant_table->Release();
	ps_constant_table->Release();
	Model::release();

}

D3DXVECTOR3 Cylinder::pos() {
	return _pos;
}

void Cylinder::setPos(const D3DXVECTOR3& pos) {
	_pos = pos;
}