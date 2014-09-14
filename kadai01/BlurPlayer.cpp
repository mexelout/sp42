#include "BlurPlayer.h"
#include "Camera.h"
#include "ShaderDevise.h"
#include "Input.h"
#include "Common.h"

BlurPlayer::BlurPlayer(void) {
	_pos = oldpos = D3DXVECTOR3(0, 0, 0);
	blur_texture = NULL;
	for each(D3DXMATRIX& m in old_worlds) {
		m = Common::identity;
	}
}

BlurPlayer::~BlurPlayer(void) {
}

BlurPlayer* BlurPlayer::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	Model::init(device, "models/dosei.x");
	_pos = oldpos = D3DXVECTOR3(18.01f, -48.6f, 0.01f);
	D3DXMATRIX w(getWorld());
	for each(D3DXMATRIX& m in old_worlds) m = w;

	return this;
}
void BlurPlayer::update() {

	for(int i = 9; i >= 1; i--) {
		old_worlds[i] = old_worlds[i-1];
	}

	Player::update();

	old_worlds[0] = getWorld();
}
void BlurPlayer::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();

	D3DXMATRIX world, rot, translate;
	world = getWorld();

	device->SetTransform(D3DTS_WORLD, &world);

	device->SetFVF(D3DFVF_CUSTOMMODEL);

	D3DMATERIAL9 tmp_mat;
	device->GetMaterial(&tmp_mat);

	D3DXMATERIAL* mat = (D3DXMATERIAL*)materials()->GetBufferPointer();
	std::vector<LPDIRECT3DTEXTURE9> tex = textures();

	if(0) {
		for(int j = 0; j < 9; j++) {
			device->SetTransform(D3DTS_WORLD, &old_worlds[j]);
			for(int i = 0, len = numMaterials(); i < len; i++) {
				D3DMATERIAL9 material = mat[i].MatD3D;
				float alpha = (10 - j) / 10.0f;
				material.Diffuse.a = material.Ambient.a = material.Specular.a = alpha;
				device->SetMaterial(&material);
				if(blur_texture) {
					device->SetTexture(0, blur_texture);
				} else {
					device->SetTexture(0, tex[i]);
				}
				mesh()->DrawSubset(i);
			}
		}
	}
	for(int i = 0, len = numMaterials(); i < len; i++) {
		D3DMATERIAL9 material = mat[i].MatD3D;
		device->SetMaterial(&material);
		if(blur_texture) {
			device->SetTexture(0, blur_texture);
		} else {
			device->SetTexture(0, tex[i]);
		}
		mesh()->DrawSubset(i);
	}

	device->SetTexture(0, NULL);
	device->SetMaterial(&tmp_mat);
}
void BlurPlayer::release() {

	Player::release();
}

D3DXVECTOR3 BlurPlayer::getOldPos() {
	return oldpos;
}

D3DXMATRIX BlurPlayer::getOldWorld(int i) {
	return old_worlds[i];
}