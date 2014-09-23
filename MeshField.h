#pragma once
#include <vector>
#include <d3dx9.h>

class MeshField {
public:
	MeshField(void);
	~MeshField(void);
	virtual MeshField* init();
	virtual void update();
	virtual void draw();
	virtual void release();
	float getHeight(const D3DXVECTOR3* pos);
protected:
	void calcNor();
	LPDIRECT3DVERTEXBUFFER9 vtx;
	LPDIRECT3DINDEXBUFFER9 idx;
	LPDIRECT3DTEXTURE9 texture;

	int horizontal, vertical, primitive_num;
	float cell_size;
	D3DCOLOR color;
	std::vector<D3DXVECTOR3> nors;
	// ���_���b�V���̍����̐ݒ�
	void setMeshVertex(std::vector<float> vertices);
	// ���_���b�V���̍����̎擾
	std::vector<float> getMeshVertex();
};

