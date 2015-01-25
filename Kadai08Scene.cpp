#include "Common.h"
#include "ShaderDevise.h"
#include "Camera.h"
#include "Kadai08Scene.h"
#include "Input.h"
#include "GroundMesh.h"
#include "Wood.h"
#include "Player.h"
#include "TextureManager.h"
#include "ModelManager.h"

Kadai08Scene::Kadai08Scene() {
	ground_mesh = NULL;
	camera_rot = Common::vec3zero;
	player = NULL;
}
Kadai08Scene::~Kadai08Scene() {
}
Kadai08Scene* Kadai08Scene::init() {
	auto device = ShaderDevise::device();

	player = (new Player)->init();
	ground_mesh = (new GroundMesh)->init()->inputData("kadai08ground");

	{
		int wood_num = ModelManager::load("models/wood.x");
		Model* wood_model = ModelManager::get(wood_num);
		const D3DVERTEXELEMENT9 vertex_decl[] = {
			{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
			{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	  0},
			{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
			{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
			{0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
			D3DDECL_END()
		};
		wood_model->cloneMesh(device, vertex_decl);
		D3DXComputeTangent(wood_model->mesh(), 0, 0, 0, 0, NULL);
	}

	for(int i = 0; i < 300; i++) {
		woods[i] = ((new Wood)->init());
		auto world = woods[i]->world();
		float radius = (float)(rand() % 10000)/10;
		float angle = (float)(rand() % 314)/100.0f;
		world._41 = cosf(angle)*radius;
		world._43 = sinf(angle)*radius;
		D3DXVECTOR3 wood_pos(world._41, 0, world._43);
		world._42 = ground_mesh->getHeight(&wood_pos)-2;
		woods[i]->world(world);
	}

	camera_rot = D3DXVECTOR3((float)M_PI_4/2, D3DX_PI*0.5f, 30);
	InputMouse::hideCursor();
	InputMouse::fixed(true);

	stoplight_parallax_vs.loadFunc(device, "spotlight+parallax.hlsl", "vsNormalMapWithSpotLight", "vs_3_0");
	stoplight_parallax_ps.loadFunc(device, "spotlight+parallax.hlsl", "psNormalMapWithSpotLight", "ps_3_0");
	stoplight_vs.loadFunc(device, "spotlight+parallax.hlsl", "vsSpotLight", "vs_3_0");
	stoplight_ps.loadFunc(device, "spotlight+parallax.hlsl", "psSpotLight", "ps_3_0");

	return this;
}
void Kadai08Scene::update() {
	player->update();
	D3DXVECTOR3 pos(player->pos());
	{
		float height = ground_mesh->getHeight(&pos);
		player->setGroundHeight(height + 1.00f);
	}
	// 一人称視点用カメラ
	camera_rot.y -= (InputMouse::move().x/300.0f);
	camera_rot.x -= (InputMouse::move().y/300.0f);
	if(camera_rot.x >= ((float)M_PI_2 - 0.017f)) camera_rot.x = ((float)M_PI_2 - 0.017f);
	else if(camera_rot.x <= -((float)M_PI_2 - 0.017f)) camera_rot.x = -((float)M_PI_2 - 0.017f);
	float len = cosf(camera_rot.x), s = sinf(camera_rot.y) * len, c = cosf(camera_rot.y) * len;
	Camera::setEye(pos);
	Camera::setAt(D3DXVECTOR3(c, sinf(camera_rot.x), s) + pos);

}
void Kadai08Scene::draw() {
	auto device = ShaderDevise::device();
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff191970, 1.0f, 0);
	device->BeginScene();
	D3DXMATRIX world(Common::identity), view(Camera::view()), proj(Camera::projection());
	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	D3DXVECTOR3 light_pos(player->pos());
	{
		device->SetVertexShader(stoplight_parallax_vs.vs);
		device->SetPixelShader(stoplight_parallax_ps.ps);
		D3DXMATRIX inv_mat;

		D3DXMatrixInverse(&inv_mat, NULL, &world);

		D3DXVECTOR3 light_vec(Camera::dir());
		D3DXVec3Normalize(&light_vec, &light_vec);
		D3DXVECTOR3 inv_light3;
		D3DXVECTOR4 inv_light4;
		D3DXVec3TransformNormal(&inv_light3, &light_vec, &inv_mat);
		inv_light4 = D3DXVECTOR4(inv_light3, 0);
		stoplight_parallax_vs.constant_table->SetVector(device, "g_inv_light_direction", &inv_light4);

		D3DXVECTOR4 inv_camera4;
		D3DXVec3Transform(&inv_camera4, &Camera::eye(), &inv_mat);
		stoplight_parallax_vs.constant_table->SetVector(device, "g_inv_camera_position", &inv_camera4);

		static float ambient = 0.125f;

		stoplight_parallax_ps.constant_table->SetFloat(device, "g_material_ambient", ambient);

		D3DXVECTOR4 inv_light_position;


		UINT si[3] = {
			stoplight_parallax_ps.constant_table->GetSamplerIndex("texture_sampler"),
			stoplight_parallax_ps.constant_table->GetSamplerIndex("normal_sampler"),
			stoplight_parallax_ps.constant_table->GetSamplerIndex("height_sampler")
		};

		for(int i = 0, len = woods.size(); i < len; i++) {
			world = woods[i]->world();
			stoplight_parallax_vs.constant_table->SetMatrix(device, "g_world_view_projection", &D3DXMATRIX(world * view * proj));
			stoplight_parallax_vs.constant_table->SetMatrix(device, "g_world", &world);
			D3DXMatrixInverse(&inv_mat, NULL, &world);
			D3DXVec3TransformNormal(&inv_light3, &light_vec, &inv_mat);
			inv_light4 = D3DXVECTOR4(inv_light3, 0);
			stoplight_parallax_vs.constant_table->SetVector(device, "g_inv_light_direction", &inv_light4);
			D3DXVec3Transform(&inv_camera4, &Camera::eye(), &inv_mat);
			stoplight_parallax_vs.constant_table->SetVector(device, "g_inv_camera_position", &inv_camera4);

			D3DXVECTOR3 tmpLight;
			D3DXVec3TransformNormal(&tmpLight, &light_pos, &inv_mat);
			inv_light_position = D3DXVECTOR4(tmpLight, 0);
			stoplight_parallax_ps.constant_table->SetVector(device, "g_inv_light_position", &inv_light_position);

			woods[i]->setSamplerIndices(si);
			woods[i]->draw();
		}

		device->SetTexture(si[0], NULL);
		device->SetTexture(si[1], NULL);
		device->SetTexture(si[2], NULL);

		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);

	}
	{
		device->SetVertexShader(stoplight_vs.vs);
		device->SetPixelShader(stoplight_ps.ps);
		D3DXMATRIX w(Common::identity), v(Camera::view()), p(Camera::projection()), wvp(w * v * p), inv_mat;
		D3DXMatrixInverse(&inv_mat, NULL, &w);
		D3DXVECTOR3 inv_light3;
		D3DXVECTOR4 inv_light4;
		D3DXVec3TransformNormal(&inv_light3, &light_pos, &inv_mat);
		inv_light4 = D3DXVECTOR4(inv_light3, 0);
		stoplight_ps.constant_table->SetVector(device, "g_inv_light_position", &inv_light4);

		D3DXVECTOR3 light_vec(Camera::dir());
		D3DXVec3Normalize(&light_vec, &light_vec);
		D3DXVec3TransformNormal(&inv_light3, &light_vec, &inv_mat);
		inv_light4 = D3DXVECTOR4(inv_light3, 0);

		stoplight_ps.constant_table->SetVector(device, "g_inv_light_direction", &inv_light4);
		stoplight_vs.constant_table->SetMatrix(device, "g_world_view_projection", &wvp);

		ground_mesh->draw();

		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);
	}

	device->EndScene();
}
void Kadai08Scene::release() {
	for(int i = 0, len = woods.size(); i < len; i++) {
		if(woods[i]) {
			woods[i]->release();
			delete woods[i];
			woods[i] = NULL;
		}
	}
	SAFE_RELEASE_DELETE(player);
	SAFE_RELEASE_DELETE(ground_mesh);
	stoplight_parallax_vs.release();
	stoplight_parallax_ps.release();
	stoplight_vs.release();
	stoplight_ps.release();
	InputMouse::showCursor();
	InputMouse::fixed(false);
}
// end file