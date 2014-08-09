#include "ToonModel.h"
#include "Camera.h"
#include "ShaderDevise.h"
#include "Input.h"
#include "Common.h"

ToonModel::ToonModel(void) {
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

	for(int i = 0, len = numMaterials(); i < len; i++) {
		device->SetMaterial(&mat[i].MatD3D);
		device->SetTexture(0, tex[i]);
		this->mesh()->DrawSubset(i);
	}
	device->SetMaterial(&tmp_mat);
}
void ToonModel::release() {
	SAFE_RELEASE(toon);
	Player::release();
}
