#include "ToonModel.h"
#include "Camera.h"
#include "ShaderDevise.h"
#include "Input.h"
#include "Common.h"

ToonModel::ToonModel(void) {
	shader_pack = NULL;
}


ToonModel::~ToonModel(void) {
}

ToonModel* ToonModel::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	Model::init(device, "models/dosei.x");
	D3DXCreateTextureFromFile(device, "textures/toon.png", &toon);

	_pos = _speed = D3DXVECTOR3(0, 0, 0);
	is_grounded = false;
	ground_pos_y = 0;
	angle = 0;

	return this;
}
void ToonModel::update() {
	updateAction();
}
void ToonModel::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();

	LPD3DXBUFFER materials = this->materials();
	LPD3DXMATERIAL mat = (LPD3DXMATERIAL)this->materials()->GetBufferPointer();
	std::vector<LPDIRECT3DTEXTURE9> tex = this->textures();
	device->SetTexture(1, toon);

	D3DXMATRIX world = getWorld();
	device->SetTransform(D3DTS_WORLD, &world);

	D3DMATERIAL9 tmp_mat;
	device->GetMaterial(&tmp_mat);
	device->SetFVF(D3DFVF_CUSTOMMODEL);

	D3DXMATRIX v(Camera::view()), p(Camera::projection()), wvp = (world * v * p);

	HRESULT hr;
	if(shader_pack) {
		hr = device->SetVertexShader(shader_pack->vs);
		hr = shader_pack->constant_table->SetMatrix(device, "g_world_view_projection", &wvp);
		D3DXVECTOR3 lv(ShaderDevise::getLight(0).Direction);
		hr = shader_pack->constant_table->SetVector(device, "g_light_direction", &D3DXVECTOR4(lv.x, lv.y, lv.z, 1));
		hr = shader_pack->constant_table->SetMatrix(device, "g_world", &world);
	}

	for(int i = 0, len = numMaterials(); i < len; i++) {
		hr = device->SetMaterial(&mat[i].MatD3D);
		D3DXVECTOR4 diff(mat[i].MatD3D.Diffuse.r, mat[i].MatD3D.Diffuse.g, mat[i].MatD3D.Diffuse.b, mat[i].MatD3D.Diffuse.a);
		if(shader_pack) {
			hr = shader_pack->constant_table->SetVector(device, "g_material_diffuse", &diff);
		}
		hr = device->SetTexture(0, toon);
		this->mesh()->DrawSubset(i);
	}
	if(shader_pack) {
		device->SetVertexShader(NULL);
	}
	device->SetMaterial(&tmp_mat);
}
void ToonModel::release() {
	SAFE_RELEASE(toon);
	Player::release();
}

void ToonModel::setShaderPack(ShaderPack* shader_pack) {
	this->shader_pack = shader_pack;
}

