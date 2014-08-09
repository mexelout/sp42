#include "BlurPlayer.h"
#include "Camera.h"
#include "ShaderDevise.h"
#include "Input.h"
#include "Common.h"

BlurPlayer::BlurPlayer(void) {
	oldpos = D3DXVECTOR3(0, 0, 0);
	blur_texture = NULL;
}

BlurPlayer::~BlurPlayer(void) {
}

BlurPlayer* BlurPlayer::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	Model::init(device, "models/dosei.x");

	return this;
}
void BlurPlayer::update() {
	static int count = 0;

	count++;
	if(count > 0) {
		oldpos = _pos;
		count = 0;
	}

	Player::update();
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

	for(int i = 0, len = numMaterials(); i < len; i++) {
		device->SetMaterial(&mat[i].MatD3D);
		if(blur_texture) {
			device->SetTexture(0, blur_texture);
		} else {
			device->SetTexture(0, tex[i]);
		}
		mesh()->DrawSubset(i);
	}

	device->SetMaterial(&tmp_mat);
}
void BlurPlayer::release() {

	Player::release();
}

D3DXVECTOR3 BlurPlayer::getOldPos() {
	return oldpos;
}
