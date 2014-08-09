#pragma once
#include "Player.h"

class BlurPlayer : public Player {
public:
	BlurPlayer(void);
	~BlurPlayer(void);
	BlurPlayer* init();
	void update();
	void draw();
	void release();
	D3DXVECTOR3 getOldPos();
	LPDIRECT3DTEXTURE9 blur_texture;
private:
	D3DXVECTOR3 oldpos;
};

