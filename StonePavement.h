#pragma once
#include "staticplane.h"
#include <map>

class StonePavement : public StaticPlane {
public:
	StonePavement(void);
	~StonePavement(void);
	StonePavement* init(void);
	void update(void);
	void draw(void);
	void release(void);

	std::map<std::string, LPDIRECT3DTEXTURE9> getTextures();

private:
	LPDIRECT3DTEXTURE9 texture;
	LPDIRECT3DTEXTURE9 normal;
	LPDIRECT3DTEXTURE9 height_map;

};

