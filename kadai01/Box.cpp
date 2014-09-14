#include "Common.h"
#include "Box.h"
#include "Model.h"
#include "ShaderDevise.h"
#include "Vertex.h"

Box::Box() {
	model = NULL;
	world = Common::identity;
	texture = NULL;
	normal = NULL;
	height_map = NULL;
}
Box::~Box() {
}

Box* Box::init() {
	auto device = ShaderDevise::device();
	model = (new Model)->init(device, "models/box.x");
	const D3DVERTEXELEMENT9 vertex_decl[] = {
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	  0},
		{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
		{0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
		D3DDECL_END()
	};
	model->cloneMesh(device, vertex_decl);
	D3DXComputeTangent(model->mesh(), 0, 0, 0, 0, NULL);

	D3DXCreateTextureFromFile(device, "textures/Base.bmp", &texture);
	D3DXCreateTextureFromFile(device, "textures/Rock_Normal.bmp", &normal);
	D3DXCreateTextureFromFile(device, "textures/Rock_Displacement.bmp", &height_map);

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
	device->SetTransform(D3DTS_WORLD, &world);

	D3DMATERIAL9 tmp_mat;
	device->GetMaterial(&tmp_mat);

	for(int i = 0; i < num_mat; i++) {
		//device->SetMaterial(&mat[i].MatD3D);
		mesh->DrawSubset(i);
	}

	device->SetMaterial(&tmp_mat);
}

void Box::release() {
	SAFE_RELEASE(texture);
	SAFE_RELEASE(normal);
	SAFE_RELEASE(height_map);
	SAFE_RELEASE_DELETE(model);
}

D3DXMATRIX& Box::getWorld() {
	return world;
}

void Box::setWorld(D3DXMATRIX& world) {
	this->world = world;
}

std::map<std::string, LPDIRECT3DTEXTURE9> Box::getTextures() {
	std::map<std::string, LPDIRECT3DTEXTURE9> textures;
	textures["texture"] = texture;
	textures["normal"] = normal;
	textures["height_map"] = height_map;
	return textures;
}

