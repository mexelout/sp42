#include "FogScene.h"
#include "ShaderDevise.h"
#include "GroundMesh.h"
#include "Camera.h"
#include "Common.h"
#include "Input.h"
#include "ToonModel.h"

FogScene::FogScene(void) {
}

FogScene::~FogScene(void) {
}

FogScene* FogScene::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();

	// fog
	device->SetRenderState(D3DRS_FOGENABLE, true);
	device->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_XRGB(128, 128, 128));
	device->SetRenderState(D3DRS_FOGSTART,  Common::ftodw(100.0f));
	device->SetRenderState(D3DRS_FOGEND, Common::ftodw(800.0f));
	device->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
	device->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
	vs_linear.loadFunc(device, "fog.fx", "vertexShaderFogLinear", "vs_2_0");
	vs_length.loadFunc(device, "fog.fx", "vertexShaderFogLength", "vs_2_0");
	vs_height.loadFunc(device, "fog.fx", "vertexShaderFogHeight", "vs_2_0");
	ground_mesh = (new GroundMesh)->init();

	// toon
	toon_model = (new ToonModel)->init();
	toon_cell.loadFunc(device, "shader.fx", "vertexShaderCell", "vs_2_0");
	toon_line.loadFunc(device, "shader.fx", "vertexShaderLine", "vs_2_0");

	return this;
}
void FogScene::update() {
	//static float angle = 0;
	//angle+=0.01f;
	//D3DXVECTOR3 eye(cosf(angle), 0.1f, sinf(angle));
	//eye *= 10;
	//Camera::setEye(eye);

	static float angle = D3DX_PI*0.5f;
	static float angle_x = (float)M_PI_4;
	static float length = 5;

	if(InputMouse::btn(InputMouse::Right, Input::Press)) {
		angle+=(InputMouse::move().x/500.0f);
		angle_x-=(InputMouse::move().y/500.0f);
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

	ShaderDevise::resetOffsetY();
}
void FogScene::draw() {
	static int shader_number = 1;
	const int shader_number_max = 3;

	if(InputKeyboard::isKey(DIK_RETURN, Input::Trigger)) {
		//shader_number = (shader_number + 1 <= shader_number_max) ? shader_number + 1 : 0;
	}

	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff87ceeb, 1.0f, 0 );
	device->BeginScene();

	static D3DXVECTOR3 at(0, 0, 0);
	D3DXVECTOR3 dif(0, (shader_number==2)?2.0f:0.0f, 0);
	dif = dif - at;
	dif *= 0.03f;
	//at += dif;
	//Camera::setAt(at);

	D3DXMATRIX world(Common::identity), view(Camera::view()), proj(Camera::projection()), world_view_proj(world*view*proj);
	D3DXVECTOR3 light_vec3(ShaderDevise::getLightVec()), eye3(Camera::eye());
	D3DXVECTOR4 light_vec4(light_vec3.x, light_vec3.y, light_vec3.z, 1), eye4(eye3.x, eye3.y, eye3.z, 1);

	static float fog_end = 300;

	switch(shader_number) {
		case 0:
			ShaderDevise::drawText("linear");
			vs_linear.constant_table->SetMatrix(device, "g_world_view_projection", &world_view_proj);
			vs_linear.constant_table->SetVector(device, "g_light_direction", &light_vec4);
			vs_linear.constant_table->SetMatrix(device, "g_world", &world);
			vs_linear.constant_table->SetFloat(device, "g_fog_end", fog_end);
			vs_linear.constant_table->SetFloat(device, "g_fog_start", 75);
			device->SetVertexShader(vs_linear.vs);
			break;
		case 1:
			ShaderDevise::drawText("length");
			vs_length.constant_table->SetMatrix(device, "g_world_view_projection", &world_view_proj);
			vs_length.constant_table->SetVector(device, "g_light_direction", &light_vec4);
			vs_length.constant_table->SetMatrix(device, "g_world", &world);
			vs_length.constant_table->SetFloat(device, "g_fog_end", fog_end);
			vs_length.constant_table->SetFloat(device, "g_fog_start", 100);
			vs_length.constant_table->SetVector(device, "g_camera_position", &eye4);
			device->SetVertexShader(vs_length.vs);
			break;
		case 2:
			ShaderDevise::drawText("height");
			vs_height.constant_table->SetMatrix(device, "g_world_view_projection", &world_view_proj);
			vs_height.constant_table->SetVector(device, "g_light_direction", &light_vec4);
			vs_height.constant_table->SetMatrix(device, "g_world", &world);
			device->SetVertexShader(vs_height.vs);
			break;
		default:
			ShaderDevise::drawText("static");
			break;
	}

	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	ground_mesh->draw();
	device->SetVertexShader(NULL);

	device->SetRenderState(D3DRS_FOGENABLE, false);

	toon_model->setShaderPack(&toon_cell);
	toon_model->draw();
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	toon_model->setShaderPack(&toon_line);
	toon_model->draw();
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	device->SetRenderState(D3DRS_FOGENABLE, true);

	device->EndScene();
	device->Present( NULL, NULL, NULL, NULL );
}
void FogScene::release() {
	SAFE_RELEASE_DELETE(ground_mesh);
	SAFE_RELEASE_DELETE(toon_model);
	toon_cell.release();
	toon_line.release();
	vs_linear.release();
	vs_length.release();
	vs_height.release();
}




