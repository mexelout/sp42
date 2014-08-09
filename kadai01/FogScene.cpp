#include "FogScene.h"
#include "ShaderDevise.h"
#include "GroundMesh.h"
#include "Camera.h"
#include "Common.h"
#include "Input.h"

FogScene::FogScene(void) {
}

FogScene::~FogScene(void) {
}

FogScene* FogScene::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	device->SetRenderState(D3DRS_FOGENABLE, true);
	device->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_XRGB(128, 128, 128));
	device->SetRenderState(D3DRS_FOGSTART,  Common::ftodw(75.0f));
	device->SetRenderState(D3DRS_FOGEND, Common::ftodw(100.0f));
	device->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
	device->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
	vs_linear.loadFunc(device, "fog.fx", "vertexShaderFogLinear", "vs_2_0");
	vs_length.loadFunc(device, "fog.fx", "vertexShaderFogLength", "vs_2_0");
	vs_height.loadFunc(device, "fog.fx", "vertexShaderFogHeight", "vs_2_0");
	ground_mesh = (new GroundMesh)->init();
	return this;
}
void FogScene::update() {
	static float angle = 0;
	angle+=0.01f;
	D3DXVECTOR3 eye(cosf(angle), 0.5f, sinf(angle));
	eye *= 10;
	Camera::setEye(eye);

	ShaderDevise::resetOffsetY();
}
void FogScene::draw() {
	static int shader_number = 0;
	const int shader_number_max = 3;

	if(InputKeyboard::isKey(DIK_RETURN, Input::Trigger)) {
		shader_number = (shader_number + 1 <= shader_number_max) ? shader_number + 1 : 0;
	}

	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff87ceeb, 1.0f, 0 );
	device->BeginScene();

	static D3DXVECTOR3 at(0, 0, 0);
	D3DXVECTOR3 dif(0, (shader_number==2)?2.0f:0.0f, 0);
	dif = dif - at;
	dif *= 0.03f;
	at += dif;
	Camera::setAt(at);

	D3DXMATRIX world(ground_mesh->getWorld()), view(Camera::view()), proj(Camera::projection()), world_view_proj(world*view*proj);
	D3DXVECTOR3 light_vec3(ShaderDevise::getLightVec()), eye3(Camera::eye());
	D3DXVECTOR4 light_vec4(light_vec3.x, light_vec3.y, light_vec3.z, 1), eye4(eye3.x, eye3.y, eye3.z, 1);

	static float fog_end = 100;

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
			vs_length.constant_table->SetFloat(device, "g_fog_start", 75);
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

	device->EndScene();
	device->Present( NULL, NULL, NULL, NULL );
}
void FogScene::release() {
	SAFE_RELEASE_DELETE(ground_mesh);
	vs_linear.release();
	vs_length.release();
	vs_height.release();
}




