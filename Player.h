#pragma once
#include <array>
#include "Model.h"
#include "Vertex.h"

class Player : public Model {
public:
	Player(void);
	~Player(void);
	virtual Player* init();
	virtual void update();
	virtual void draw();
	virtual void release();
	D3DXVECTOR3 pos();
	void updateAction();
	void updateGravity();
	void calcGround(std::array<CUSTOMVERTEX, 4> vtx);
	// �|�W�V�����ݒ�
	void setPos(D3DXVECTOR3 pos);
	D3DXMATRIX getWorld();
	// ������̒n�ʂ̍�����ݒ�
	void setGroundHeight(float ground_height);
protected:
	D3DXVECTOR3 _pos;
	D3DXVECTOR3 _speed;
	BOOL is_grounded;
	float ground_pos_y;
	float angle;
};

