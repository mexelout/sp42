#include "TestScene.h"
#include "StonePavement.h"
#include "Common.h"
#include "Camera.h"
#include "Input.h"
#include "ShaderDevise.h"
#include "Player.h"
#include "Grid.h"
#include "Sphere.h"

TestScene::TestScene(void) {
	stone_pavement = NULL;
	camera_rot = Common::vec3zero;
	player = NULL;
	light_vec[0] = Common::vec3zero;
	light_vec[1] = Common::vec3zero;
}
TestScene::~TestScene(void) {
}

TestScene* TestScene::init() {
	auto device = ShaderDevise::device();

	camera_rot = D3DXVECTOR3((float)M_PI_4*0.5f, D3DX_PI*0.25f, 3);

	stone_pavement = (new StonePavement)->init();
	player = (new Player)->init();

	normal_vs.loadFunc(device, "normal_map.hlsl", "vsNormalMap", "vs_2_0");
	normal_ps.loadFunc(device, "normal_map.hlsl", "psNormalMap", "ps_2_0");

	grid = (new Grid)->init()->setSize(10);

	lambert_vs.loadFunc(device, "lambert.hlsl", "vsLambert", "vs_2_0");
	lambert_ps.loadFunc(device, "lambert.hlsl", "psLambert", "ps_2_0");

	ppl_vs.loadFunc(device, "per_pixel_lighting.hlsl", "vsPPL", "vs_2_0");
	ppl_ps.loadFunc(device, "per_pixel_lighting.hlsl", "psPPL", "ps_2_0");

	sphere = (new Sphere)->init();

	light_vec[0] = D3DXVECTOR3(-1.0f, -1.0f, -0.1f);
	D3DXVec3Normalize(&light_vec[0], &light_vec[0]);
	light_vec[1] = D3DXVECTOR3(1.0f, -1.0f, 0.6f);
	D3DXVec3Normalize(&light_vec[1], &light_vec[1]);

	return this;
}

void TestScene::update() {
	if(InputMouse::btn(InputMouse::Right, Input::Press)) {
		camera_rot.y += (InputMouse::move().x/300.0f);
		camera_rot.x -= (InputMouse::move().y/300.0f);
		if(camera_rot.x >= ((float)M_PI_2 - 0.017f)) camera_rot.x = ((float)M_PI_2 - 0.017f);
		else if(camera_rot.x <= -((float)M_PI_2 - 0.017f)) camera_rot.x = -((float)M_PI_2 - 0.017f);
	}
	camera_rot.z += InputMouse::wheel() / 300;

	float len = cosf(camera_rot.x), s = sinf(camera_rot.y) * len, c = cosf(camera_rot.y) * len;
	D3DXVECTOR3 at((player)?player->pos():Common::vec3zero);

	Camera::setEye(D3DXVECTOR3(c, sinf(camera_rot.x), s)*camera_rot.z + at);
	Camera::setAt(at);

	player->update();

	Camera::setEye(D3DXVECTOR3(c, sinf(camera_rot.x), s)*camera_rot.z + player->pos());
	Camera::setAt(player->pos());

	sphere->update();

	static float light_angle = 0;
	light_angle += 0.05f;
	//light_vec[0] = D3DXVECTOR3(cosf(light_angle) * 10, -1.0f, sinf(light_angle) * 10);
	//D3DXVec3Normalize(&light_vec[0], &light_vec[0]);

}
void TestScene::draw() {
	auto device = ShaderDevise::device();
	D3DXMATRIX world(Common::identity), view = Camera::view(), proj = Camera::projection();

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff191970, 1.0f, 0 );
	device->BeginScene();

	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	grid->draw();

	if(0) {
		D3DXMATRIX w(sphere->getWorld()), v(Camera::view()), p(Camera::projection()), wvp(w*v*p), inv_mat;
		ppl_vs.constant_table->SetMatrix(device, "g_world_view_proj", &wvp);
		D3DXVECTOR3 inv_light3(Common::vec3zero);
		D3DXVECTOR4 inv_light4(Common::vec4zero);
		D3DXVec3TransformNormal(&inv_light3, &ShaderDevise::getLightVec(), &inv_mat);
		inv_light4 = D3DXVECTOR4(inv_light3, 0);
		ppl_vs.constant_table->SetVector(device, "g_inv_light_direction", &inv_light4);

		D3DXVECTOR4 mat_dif(0.2f, 1.0f, 0.2f, 1.0f);
		ppl_vs.constant_table->SetVector(device, "g_material_diffuse", &mat_dif);

		device->SetVertexShader(ppl_vs.vs);
		device->SetPixelShader(ppl_ps.ps);

		sphere->draw();

		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);
	}

	if(0) {
		device->SetVertexShader(lambert_vs.vs);
		device->SetPixelShader(lambert_ps.ps);

		D3DXMATRIX t(Common::identity), r(Common::identity);
		D3DXMATRIX w(sphere->getWorld()), v(Camera::view()), p(Camera::projection()), wvp(w*v*p), inv_mat;
		lambert_vs.constant_table->SetMatrix(device, "g_world_view_proj", &wvp);

		D3DXVECTOR3 inv_light3(Common::vec3zero);
		D3DXVECTOR4 inv_light4[2] = {(Common::vec4zero), Common::vec3zero};
		
		D3DXVec3TransformNormal(&inv_light3, &light_vec[0], &inv_mat);
		inv_light4[0] = D3DXVECTOR4(inv_light3, 0);

		D3DXVec3TransformNormal(&inv_light3, &light_vec[1], &inv_mat);
		inv_light4[1] = D3DXVECTOR4(inv_light3, 0);
		lambert_vs.constant_table->SetVectorArray(device, "g_inv_light_direction", inv_light4, 2);

		D3DXVECTOR4 mat_dif(1.0f, 0.3f, 0.2f, 1.0f);
		lambert_vs.constant_table->SetVector(device, "g_material_diffuse", &mat_dif);

		D3DXVECTOR4 inv_camera4;
		D3DXVec3Transform(&inv_camera4, &Camera::eye(), &inv_mat);
		lambert_vs.constant_table->SetVector(device, "g_inv_camera_position", &inv_camera4);

		sphere->draw();

		D3DXMatrixTranslation(&t, 5, 0, 0);
		w *= t;
		wvp = w*v*p;
		lambert_vs.constant_table->SetMatrix(device, "g_world_view_proj", &wvp);

		sphere->draw();

		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);
	}

	if(1) {
		device->SetVertexShader(normal_vs.vs);
		device->SetPixelShader(normal_ps.ps);

		D3DXMATRIX w(Common::identity), v(Camera::view()), p(Camera::projection()), inv_mat;
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

		normal_ps.constant_table->SetFloat(device, "g_material_ambient", 0.2f);
		normal_ps.constant_table->SetBool(device, "g_normal_flag", true);

		std::map<std::string, LPDIRECT3DTEXTURE9> textures = stone_pavement->getTextures();
		device->SetTexture(normal_ps.constant_table->GetSamplerIndex("texture_sampler"), textures["texture"]);
		device->SetTexture(normal_ps.constant_table->GetSamplerIndex("normal_sampler"), textures["normal"]);
		device->SetTexture(normal_ps.constant_table->GetSamplerIndex("height_sampler"), textures["height_map"]);

		stone_pavement->draw();

		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);
		device->SetTexture(normal_ps.constant_table->GetSamplerIndex("texture_sampler"), NULL);
		device->SetTexture(normal_ps.constant_table->GetSamplerIndex("normal_sampler"), NULL);
		device->SetTexture(normal_ps.constant_table->GetSamplerIndex("height_sampler"), NULL);
	}

	device->EndScene();

	device->Present( NULL, NULL, NULL, NULL );
}
void TestScene::release() {
	SAFE_RELEASE_DELETE(stone_pavement);
	SAFE_RELEASE_DELETE(player);
	SAFE_RELEASE_DELETE(grid);
	SAFE_RELEASE_DELETE(sphere);
	normal_vs.release();
	normal_ps.release();
	lambert_vs.release();
	lambert_ps.release();
	ppl_vs.release();
	ppl_ps.release();
}

