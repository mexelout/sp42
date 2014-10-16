#include "ToonScene.h"
#include "ShaderDevise.h"
#include "ToonModel.h"
#include "Common.h"
#include "Camera.h"
#include "Input.h"

ToonScene::ToonScene(void) {
}

ToonScene::~ToonScene(void) {
}

ToonScene* ToonScene::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();

	toon_cell.loadFunc(device, "shader.hlsl", "vertexShaderCell", "vs_2_0");
	toon_line.loadFunc(device, "shader.hlsl", "vertexShaderLine", "vs_2_0");

	D3DXHANDLE g_world_view_projection = toon_cell.constant_table->GetConstantByName(NULL, "g_world_view_projection");
	D3DXHANDLE g_light_direction = toon_cell.constant_table->GetConstantByName(NULL, "g_light_direction");
	D3DXHANDLE g_world = toon_cell.constant_table->GetConstantByName(NULL, "g_world");
	D3DXHANDLE g_ambient = toon_cell.constant_table->GetConstantByName(NULL, "g_ambient");
	D3DXHANDLE g_material_diffuse = toon_cell.constant_table->GetConstantByName(NULL, "g_material_diffuse");
	D3DXHANDLE g_light_matrix = toon_cell.constant_table->GetConstantByName(NULL, "g_light_matrix");
	D3DXHANDLE g_player_position = toon_cell.constant_table->GetConstantByName(NULL, "g_player_position");

	toon_model = (new ToonModel)->init();

	return this;
}

void ToonScene::update() {
	static float angle = D3DX_PI*0.5f;
	static float angle_x = (float)M_PI_4;
	static float length = 5;

	if(InputMouse::btn(InputMouse::Right, Input::Press)) {
		angle+=(InputMouse::move().x/100.0f);
		angle_x-=(InputMouse::move().y/100.0f);
		if(angle_x >= ((float)M_PI_2 - 0.017f)) angle_x = ((float)M_PI_2 - 0.017f);
		else if(angle_x <= -((float)M_PI_2 - 0.017f)) angle_x = -((float)M_PI_2 - 0.017f);
	}
	length += InputMouse::wheel() / 100;

	float len = cosf(angle_x), s = sinf(angle) * len, c = cosf(angle) * len;
	Camera::setEye(D3DXVECTOR3(c, sinf(angle_x), s)*length + toon_model->pos());
	Camera::setAt(toon_model->pos() + D3DXVECTOR3(0, 1, 0));

	toon_model->update();

	Camera::setEye(D3DXVECTOR3(c, sinf(angle_x), s)*length + toon_model->pos());
	Camera::setAt(toon_model->pos() + D3DXVECTOR3(0, 1, 0));
}

void ToonScene::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	const D3DXMATRIX& view = Camera::view(), proj = Camera::projection();

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff191970, 1.0f, 0 );
	device->BeginScene();

	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);
	toon_model->draw();

	device->EndScene();

}

void ToonScene::release() {
	SAFE_RELEASE_DELETE(toon_model);
	toon_cell.release();
	toon_line.release();
}

