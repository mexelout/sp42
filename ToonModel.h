#pragma once

#include <array>
#include "Vertex.h"
#include "Player.h"
#include "ShaderPack.h"

class ToonModel : public Player {
public:
	ToonModel(void);
	~ToonModel(void);
	ToonModel* init();
	void update();
	void draw();
	void release();

	void setShaderPack(ShaderPack* shader_pack);
private:
	LPDIRECT3DTEXTURE9 toon;

	ShaderPack* shader_pack;
};

