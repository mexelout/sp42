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
	D3DXMATRIX getOldWorld(int i);
private:
	D3DXVECTOR3 oldpos;
	D3DXMATRIX old_worlds[10];
};

