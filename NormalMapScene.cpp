#include "Common.h"
#include "NormalMapScene.h"
#include "Box.h"
#include "Camera.h"
#include "Input.h"
#include "ShaderDevise.h"
#include "Grid.h"

NormalMapScene::NormalMapScene(void) {
	box = NULL;
	grid = NULL;
	camera_rot = D3DXVECTOR3((float)M_PI_4/2, D3DX_PI*0.5f, 3);
}
NormalMapScene::~NormalMapScene(void) {
}

NormalMapScene* NormalMapScene::init() {
	auto device = ShaderDevise::device();

	box = (new Box)->init();
	grid = (new Grid)->init()->setSize(10);

	normal_vs.loadFunc(device, "normal_map.hlsl", "vsNormalMap", "vs_3_0");
	normal_ps.loadFunc(device, "normal_map.hlsl", "psNormalMap", "ps_3_0");
	return this;
}

void NormalMapScene::update() {
	if(InputMouse::btn(InputMouse::Left, Input::Press)) {
		D3DXVECTOR3 dir(Camera::dir()), up(Common::vec3zero), left(Common::vec3zero);
		{
			// create camera up direction
			float y = (dir.y >= 0) ? 1.0f : 1.0f;
			float xz_len = D3DXVec2Length(&D3DXVECTOR2(dir.x, dir.z));
			y *= xz_len;
			xz_len = dir.y*-1;
			up = D3DXVECTOR3(dir.x, 0, dir.z);
			D3DXVec3Normalize(&up, &up);
			up *= xz_len;
			up.y = y;
		}
		D3DXVec3Cross(&left, &dir, &up);
		D3DXMATRIX world(box->getWorld()), rot;
		D3DXMatrixRotationAxis(&rot, &up, -(InputMouse::move().x/300.0f));
		world *= rot;
		D3DXMatrixRotationAxis(&rot, &left, (InputMouse::move().y/300.0f));
		world *= rot;
		box->setWorld(world);
	}
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

	box->update();
	grid->update();
}

void NormalMapScene::draw() {
	auto device = ShaderDevise::device();
	D3DXMATRIX view = Camera::view(), proj = Camera::projection();

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff191970, 1.0f, 0 );
	device->BeginScene();

	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);


	{// いろいろな変数が作られるので、生存エリア限定させる
		device->SetVertexShader(normal_vs.vs);
		device->SetPixelShader(normal_ps.ps);

		D3DXMATRIX w(box->getWorld()), v(Camera::view()), p(Camera::projection()), inv_mat;
		normal_vs.constant_table->SetMatrix(device, "g_world_view_projection", &D3DXMATRIX(w * v * p));
		D3DXMatrixInverse(&inv_mat, NULL, &w);

		D3DXVECTOR3 inv_light3;
		D3DXVECTOR4 inv_light4;
		D3DXVec3TransformNormal(&inv_light3, &ShaderDevise::getLightVec(), &inv_mat);
		inv_light4 = D3DXVECTOR4(inv_light3, 0);
		normal_vs.constant_table->SetVector(device, "g_inv_light_direction", &inv_light4);

		D3DXVECTOR4 inv_camera4;
		D3DXVec3Transform(&inv_camera4, &Camera::eye(), &inv_mat);
		normal_vs.constant_table->SetVector(device, "g_inv_camera_position", &inv_camera4);

		static float ambient = 0.2f;
		if(InputKeyboard::isKey(DIK_1, Input::Trigger)) {
			ambient += 0.1f;
		}
		if(InputKeyboard::isKey(DIK_2, Input::Trigger)) {
			ambient -= 0.1f;
		}
		static bool normal_flag = false;
		if(InputKeyboard::isKey(DIK_N, Input::Trigger)) {
			normal_flag = !normal_flag;
		}
		ShaderDevise::drawText(Common::debug("ambient: %f 1:up 2:down", ambient).c_str());
		ShaderDevise::drawText(Common::debug("%s: n: normal or parallax change", (normal_flag) ? "normal" : "parallax", normal_flag).c_str());
		ShaderDevise::drawText("左クリックドラッグ: モデル回転");
		ShaderDevise::drawText("右クリックドラッグ: カメラat軸回転");
		normal_ps.constant_table->SetFloat(device, "g_material_ambient", ambient);
		normal_ps.constant_table->SetFloat(device, "g_normal_flag", normal_flag);

		std::map<std::string, LPDIRECT3DTEXTURE9> textures = box->getTextures();
		device->SetTexture(normal_ps.constant_table->GetSamplerIndex("texture_sampler"), textures["texture"]);
		device->SetTexture(normal_ps.constant_table->GetSamplerIndex("normal_sampler"), textures["normal"]);
		device->SetTexture(normal_ps.constant_table->GetSamplerIndex("height_sampler"), textures["height_map"]);

		box->draw();
		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);
		device->SetTexture(normal_ps.constant_table->GetSamplerIndex("texture_sampler"), NULL);
		device->SetTexture(normal_ps.constant_table->GetSamplerIndex("normal_sampler"), NULL);
	}

	grid->draw();

	device->EndScene();

}

void NormalMapScene::release() {
	SAFE_RELEASE_DELETE(box);
	SAFE_RELEASE_DELETE(grid);
	normal_vs.release();
	normal_ps.release();
}

