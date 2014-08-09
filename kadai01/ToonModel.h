#pragma once

#include <array>
#include "Vertex.h"
#include "Player.h"

class ToonModel : public Player {
public:
	ToonModel(void);
	~ToonModel(void);
	ToonModel* init();
	void update();
	void draw();
	void release();
private:
	LPDIRECT3DTEXTURE9 toon;

};

