#pragma once
#include "Model.h"

class Bokoboko : public Model {
public:
	Bokoboko(void);
	~Bokoboko(void);
	Bokoboko* init();
	void update();
	void draw();
	void release();

private:
	D3DXMATRIX world;
};

