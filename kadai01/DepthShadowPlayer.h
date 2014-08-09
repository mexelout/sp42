#pragma once
#include "Player.h"
class DepthShadowPlayer : public Player {
public:
	DepthShadowPlayer(void);
	~DepthShadowPlayer(void);
	DepthShadowPlayer* init();
	void update();
	void draw();
	void release();
	D3DXMATRIX getWorld();
private:
	
};

