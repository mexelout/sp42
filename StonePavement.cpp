#include "StonePavement.h"
#include "ShaderDevise.h"
#include "Common.h"
#include "Vertex.h"

StonePavement::StonePavement(void) {
}
StonePavement::~StonePavement(void) {
}



StonePavement* StonePavement::init(void) {
	auto device = ShaderDevise::device();

	StaticPlane::init();

	CUSTOMVERTEX* data;
	vtx->Lock(0, 0, (void**)&data, 0);

	float size = 100;
	data[0].pos = D3DXVECTOR3(-size, 0,  size);
	data[1].pos = D3DXVECTOR3( size, 0,  size);
	data[2].pos = D3DXVECTOR3(-size, 0, -size);
	data[3].pos = D3DXVECTOR3( size, 0, -size);

	data[0].tex = D3DXVECTOR2(  0,   0);
	data[1].tex = D3DXVECTOR2(100,   0);
	data[2].tex = D3DXVECTOR2(  0, 100);
	data[3].tex = D3DXVECTOR2(100, 100);

	vtx->Unlock();

	D3DXCreateTextureFromFile(device, "textures/wall000.png", &texture);
	D3DXCreateTextureFromFile(device, "textures/wall000b.png", &normal);
	D3DXCreateTextureFromFile(device, "textures/wall000h.png", &height_map);

	return this;
}
void StonePavement::update(void) {
}
void StonePavement::draw(void) {
	auto device = ShaderDevise::device();

	device->SetTexture(0, texture);
	StaticPlane::draw();
}
void StonePavement::release(void) {
	SAFE_RELEASE(texture);
	SAFE_RELEASE(normal);
	SAFE_RELEASE(height_map);
}

std::map<std::string, LPDIRECT3DTEXTURE9> StonePavement::getTextures() {
	std::map<std::string, LPDIRECT3DTEXTURE9> textures;
	textures["texture"] = texture;
	textures["normal"] = normal;
	textures["height_map"] = height_map;
	return textures;
}