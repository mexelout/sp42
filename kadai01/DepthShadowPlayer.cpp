#include "DepthShadowPlayer.h"
#include "Camera.h"
#include "ShaderDevise.h"
#include "Input.h"
#include "Common.h"

DepthShadowPlayer::DepthShadowPlayer(void) {
}

DepthShadowPlayer::~DepthShadowPlayer(void) {
}

DepthShadowPlayer* DepthShadowPlayer::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	Model::init(device, "models/Advance Frame _A.x");

	return this;
}
void DepthShadowPlayer::update() {
	Player::update();
}
void DepthShadowPlayer::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();

	D3DXMATRIX world, rot, translate;
	D3DXMatrixIdentity(&world);
	D3DXMatrixRotationAxis(&rot, &D3DXVECTOR3(0, 1, 0), -angle - (float)M_PI_2);
	world *= rot;
	D3DXMatrixTranslation(&translate, _pos.x, _pos.y, _pos.z);
	world *= translate;

	device->SetTransform(D3DTS_WORLD, &world);
	device->SetFVF(D3DFVF_CUSTOMMODEL);
	D3DXMATERIAL* mat = (D3DXMATERIAL*)materials()->GetBufferPointer();
	std::vector<LPDIRECT3DTEXTURE9> tex = textures();

	for(int i = 0, len = numMaterials(); i < len; i++) {
		device->SetMaterial(&mat[i].MatD3D);
		device->SetTexture(0, tex[i]);
		mesh()->DrawSubset(i);
	}

}
void DepthShadowPlayer::release() {
	Player::release();
}

D3DXMATRIX DepthShadowPlayer::getWorld() {
	D3DXMATRIX world, rot, translate;
	D3DXMatrixIdentity(&world);
	D3DXMatrixRotationAxis(&rot, &D3DXVECTOR3(0, 1, 0), -angle - (float)M_PI_2);
	world *= rot;
	D3DXMatrixTranslation(&translate, _pos.x, _pos.y, _pos.z);
	world *= translate;
	return world;
}
