#include "PointLightScene.h"
#include "Player.h"
#include "Input.h"
#include "Camera.h"
#include "Common.h"
#include "ShaderDevise.h"
#include "StaticPlane.h"

PointLightScene::PointLightScene(void) {
	player = NULL;
	plane = NULL;
	camera_rot = Common::vec3zero;
	for(int i = 0; i < (int)point_light_pos.size(); i++) point_light_pos[i] = Common::vec3zero;
}
PointLightScene::~PointLightScene(void) {
}
PointLightScene* PointLightScene::init() {
	auto device = ShaderDevise::device();

	player = (new Player)->init();
	plane = (new StaticPlane)->init();

	point_light_pos[0] = D3DXVECTOR3(4, 1, 0);
	point_light_pos[1] = D3DXVECTOR3(-4, 1, 0);
	camera_rot = D3DXVECTOR3((float)M_PI_4/2, D3DX_PI*0.5f, 30);

	vs_point_light.loadFunc(device, "point_light.hlsl", "vsPointLight", "vs_3_0");
	ps_point_light.loadFunc(device, "point_light.hlsl", "psPointLight", "ps_3_0");

	return this;
}
void PointLightScene::update() {
	if(InputMouse::btn(InputMouse::Right, Input::Press)) {
		camera_rot.y += (InputMouse::move().x/300.0f);
		camera_rot.x -= (InputMouse::move().y/300.0f);
		if(camera_rot.x >= ((float)M_PI_2 - 0.017f)) camera_rot.x = ((float)M_PI_2 - 0.017f);
		else if(camera_rot.x <= -((float)M_PI_2 - 0.017f)) camera_rot.x = -((float)M_PI_2 - 0.017f);
	}
	camera_rot.z += InputMouse::wheel() / 300;

	float len = cosf(camera_rot.x), s = sinf(camera_rot.y) * len, c = cosf(camera_rot.y) * len;
	Camera::setEye(D3DXVECTOR3(c, sinf(camera_rot.x), s)*camera_rot.z);
	Camera::setAt(D3DXVECTOR3(0, 0, 0));

	if(InputKeyboard::isKey(DIK_A, Input::Press)) {
		point_light_pos[0].x++;
	} else if(InputKeyboard::isKey(DIK_S, Input::Press)) {
		point_light_pos[0].z++;
	} else if(InputKeyboard::isKey(DIK_D, Input::Press)) {
		point_light_pos[0].x--;
	} else if(InputKeyboard::isKey(DIK_W, Input::Press)) {
		point_light_pos[0].z--;
	}

	if(InputKeyboard::isKey(DIK_LEFT, Input::Press)) {
		point_light_pos[1].x++;
	} else if(InputKeyboard::isKey(DIK_UP, Input::Press)) {
		point_light_pos[1].z--;
	} else if(InputKeyboard::isKey(DIK_RIGHT, Input::Press)) {
		point_light_pos[1].x--;
	} else if(InputKeyboard::isKey(DIK_DOWN, Input::Press)) {
		point_light_pos[1].z++;
	}

}
void PointLightScene::draw() {
	auto device = ShaderDevise::device();
	D3DXMATRIX world(Common::identity), view(Camera::view()), proj(Camera::projection());

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff191970, 1.0f, 0 );
	device->BeginScene();

	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	ShaderDevise::drawText("WASD: 左ライト移動");
	ShaderDevise::drawText("矢印: 右ライト移動");
	{
		device->SetVertexShader(vs_point_light.vs);
		device->SetPixelShader(ps_point_light.ps);
		D3DXMATRIX w(Common::identity), v(Camera::view()), p(Camera::projection()), wvp(w * v * p), inv_mat;
		D3DXMatrixInverse(&inv_mat, NULL, &w);

		D3DXVECTOR3 inv_light3;
		D3DXVECTOR4* inv_light4 = new D3DXVECTOR4[point_light_pos.size()];
		for(int i = 0; i < (int)point_light_pos.size(); i++) {
			D3DXVec3TransformNormal(&inv_light3, &point_light_pos[i], &inv_mat);
			inv_light4[i] = D3DXVECTOR4(inv_light3, 0);
		}
		ps_point_light.constant_table->SetVectorArray(device, "g_inv_light_direction", inv_light4, point_light_pos.size());
		SAFE_DELETE_ARRAY(inv_light4);

		vs_point_light.constant_table->SetMatrix(device, "g_world_view_projection", &wvp);

		plane->draw();

		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);
	}

	device->EndScene();
}
void PointLightScene::release() {
	SAFE_RELEASE_DELETE(player);
	SAFE_RELEASE_DELETE(plane);
	vs_point_light.release();
	ps_point_light.release();

}
