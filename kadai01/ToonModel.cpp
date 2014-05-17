#include "ToonModel.h"
#include "Camera.h"
#include "ShaderDevise.h"

ToonModel::ToonModel(void) {
}


ToonModel::~ToonModel(void) {
}

ToonModel* ToonModel::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	Model::init(device, "models/dosei.x");

	LPD3DXBUFFER code;
	LPD3DXBUFFER error;
	D3DXCompileShaderFromFile("shader.fx", NULL, NULL,
								"vertexShaderCell", "vs_2_0", 0,
								&code, &error, &vs_constant_table);

	if(error) {
		OutputDebugString((LPCSTR)error->GetBufferPointer());
		this->release();
		MessageBox(NULL, "shader error.\ncheck output string message.", "error", MB_OK);
		return 0;
	}

	LPDIRECT3DVERTEXSHADER9 vertex_shader;
	device->CreateVertexShader((DWORD*)code->GetBufferPointer(), &vertex_shader);

	code->Release();

	D3DXCompileShaderFromFile("shader.fx", NULL, NULL,
								"vertexShaderLine", "vs_2_0", 0,
								&code, &error, &vs_line_constant_table);

	if(error) {
		OutputDebugString((LPCSTR)error->GetBufferPointer());
		this->release();
		MessageBox(NULL, "shader error.\ncheck output string message.", "error", MB_OK);
		return 0;
	}

	LPDIRECT3DVERTEXSHADER9 vertex_line_shader;
	device->CreateVertexShader((DWORD*)code->GetBufferPointer(), &vertex_line_shader);

	code->Release();

	D3DXCompileShaderFromFile("shader.fx", NULL, NULL,
								"pixelShaderTexture", "ps_2_0", 0,
								&code, &error, &ps_constant_table);

	if(error) {
		OutputDebugString((LPCSTR)error->GetBufferPointer());
		this->release();
		MessageBox(NULL, "shader error.\ncheck output string message.", "error", MB_OK);
		return 0;
	}

	LPDIRECT3DPIXELSHADER9 pixel_shader;
	device->CreatePixelShader((DWORD*)code->GetBufferPointer(), &pixel_shader);

	code->Release();

	LPDIRECT3DTEXTURE9 toon;
	D3DXCreateTextureFromFile(device, "textures/toon.png", &toon);
	return this;
}
void ToonModel::update() {
}
void ToonModel::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	static float Ang = 0;
	Ang += 0.03f;

	const D3DXMATRIX& view = Camera::view();
	const D3DXMATRIX& projection = Camera::projection();
	D3DXMATRIX worldViewProjection;

	D3DXMatrixIdentity(&world);
	D3DXMATRIX translate, rot;

	D3DXMatrixRotationY(&rot, Ang);
	world *= rot;
	D3DXMatrixTranslation(&translate, 0, 1, 0);
	world *= translate;

	worldViewProjection = world * view * projection;
	D3DXVECTOR4 light_dir(0, 0.4f, -1, 0);
	D3DXVec4Normalize(&light_dir, &light_dir);

	vs_constant_table->SetMatrix(device, "g_world_view_projection", &worldViewProjection);
	vs_constant_table->SetVector(device, "g_light_direction", &light_dir);
	vs_constant_table->SetMatrix(device, "g_world", &world);


	device->SetVertexShader(vertex_shader);
	device->SetPixelShader(pixel_shader);


	LPD3DXBUFFER materials = this->materials();
	device->SetTexture(0, toon);
	for(int i = 0, len = numMaterials(); i < len; i++) {
		D3DXMATERIAL mat = ((D3DXMATERIAL*)(materials->GetBufferPointer()))[i];
		D3DXVECTOR4 mat_dif(mat.MatD3D.Diffuse.r, mat.MatD3D.Diffuse.g, mat.MatD3D.Diffuse.b, mat.MatD3D.Diffuse.a);
		vs_constant_table->SetVector(device, "g_material_diffuse", &mat_dif);
		this->mesh()->DrawSubset(i);
	}

	// line
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	vs_line_constant_table->SetMatrix(device, "g_world", &world);
	vs_line_constant_table->SetMatrix(device, "g_world_view_projection", &worldViewProjection);
	device->SetVertexShader(vertex_line_shader);
	for(int i = 0, len = this->numMaterials(); i < len; i++) {
		D3DXMATERIAL mat = ((D3DXMATERIAL*)(materials->GetBufferPointer()))[i];
		D3DXVECTOR4 mat_dif(0, 0, 0, 1);
		vs_line_constant_table->SetVector(device, "g_material_diffuse", &mat_dif);
		this->mesh()->DrawSubset(i);
	}
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	D3DXVECTOR4 mat_dif(1, 1, 1, 1);
	vs_constant_table->SetVector(device, "g_material_diffuse", &mat_dif);

	device->SetVertexShader(NULL);
	device->SetPixelShader(NULL);

}
void ToonModel::release() {
	vertex_shader->Release();
	vertex_line_shader->Release();
	pixel_shader->Release();

	toon->Release();
	vs_constant_table->Release();
	vs_line_constant_table->Release();
	ps_constant_table->Release();
	Model::release();
}