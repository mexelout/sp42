#include "Common.h"
#include "Box.h"
#include "Model.h"
#include "ShaderDevise.h"
#include "Vertex.h"

Box::Box() {
	model = NULL;
	world = Common::identity;
	texture = NULL;
}
Box::~Box() {
}

Box* Box::init() {
	auto device = ShaderDevise::device();
	model = (new Model)->init(device, "models/box.x");
	D3DXCreateTextureFromFile(device, "textures/Rock_Normal.bmp", &texture);
	return this;
}

void Box::update() {
}

void Box::draw() {
	auto device = ShaderDevise::device();
	D3DXMATERIAL* mat = (D3DXMATERIAL*)model->materials()->GetBufferPointer();
	int num_mat = model->numMaterials();
	device->SetFVF(D3DFVF_CUSTOMMODEL);
	LPD3DXMESH mesh = model->mesh();
	device->SetTransform(D3DTS_WORLD, &D3DXMATRIX(Common::identity));
	device->SetTexture(0, texture);

	D3DMATERIAL9 tmp_mat;
	device->GetMaterial(&tmp_mat);

	device->SetRenderState(D3DRS_LIGHTING, true);
	for(int i = 0; i < num_mat; i++) {
		mesh->DrawSubset(i);
	}

	device->SetMaterial(&tmp_mat);
}

void Box::release() {
	SAFE_RELEASE(texture);
	SAFE_RELEASE_DELETE(model);
}
