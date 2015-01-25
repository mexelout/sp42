#include "Bokoboko.h"
#include "Common.h"
#include "ShaderDevise.h"
#include "Vertex.h"

Bokoboko::Bokoboko(void) {
	world = Common::identity;
}
Bokoboko::~Bokoboko(void) {
}
Bokoboko* Bokoboko::init() {
	auto device = ShaderDevise::device();

	Model::init(device, "models/bokoboko.x");

	return this;
}
void Bokoboko::update() {
}
void Bokoboko::draw() {
	auto device = ShaderDevise::device();

	D3DXMATERIAL* mat = (D3DXMATERIAL*)materials()->GetBufferPointer();
	int num_mat = numMaterials();
	device->SetFVF(D3DFVF_CUSTOMMODEL);
	LPD3DXMESH mesh = this->mesh();
	device->SetTransform(D3DTS_WORLD, &world);

	D3DMATERIAL9 tmp_mat;
	device->GetMaterial(&tmp_mat);

	for(int i = 0; i < num_mat; i++) {
		device->SetMaterial(&mat[i].MatD3D);
		mesh->DrawSubset(i);
	}

	device->SetMaterial(&tmp_mat);
}
void Bokoboko::release() {
	Model::release();
}
