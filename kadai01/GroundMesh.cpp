#include "GroundMesh.h"
#include "ShaderDevise.h"
#include "Common.h"

GroundMesh::GroundMesh(void) {
}

GroundMesh::~GroundMesh(void) {
}

GroundMesh* GroundMesh::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	cell_size = 10;
	MeshField::init();

	std::vector<float> vertices;
	for(int i = 0; i < vertical; i++) {
		for(int j = 0; j < horizontal; j++) {
			vertices.push_back((rand()%100)/25.0f-sinf((float)M_PI * ((float)i / (float)vertical))*50);
		}
	}
	setMeshVertex(vertices);
	calcNor();

	D3DXCreateTextureFromFile(device, "textures/ground.png", &texture);
	return this;
}
void GroundMesh::update() {
	MeshField::update();
}
void GroundMesh::draw() {
	MeshField::draw();
}
void GroundMesh::release() {
	MeshField::release();
}



