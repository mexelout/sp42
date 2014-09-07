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
	camera_rot = D3DXVECTOR3((float)M_PI_4, D3DX_PI*0.5f, 5);
}
NormalMapScene::~NormalMapScene(void) {
}

NormalMapScene* NormalMapScene::init() {
	box = (new Box)->init();
	grid = (new Grid)->init();
	return this;
}

void NormalMapScene::update() {

	if(InputMouse::btn(InputMouse::Right, Input::Press)) {
		camera_rot.y += (InputMouse::move().x/500.0f);
		camera_rot.x -= (InputMouse::move().y/500.0f);
		if(camera_rot.x >= ((float)M_PI_2 - 0.017f)) camera_rot.x = ((float)M_PI_2 - 0.017f);
		else if(camera_rot.x <= -((float)M_PI_2 - 0.017f)) camera_rot.x = -((float)M_PI_2 - 0.017f);
	}
	camera_rot.z += InputMouse::wheel() / 500;

	float len = cosf(camera_rot.x), s = sinf(camera_rot.y) * len, c = cosf(camera_rot.y) * len;
	Camera::setEye(D3DXVECTOR3(c, sinf(camera_rot.x), s)*camera_rot.z);
	Camera::setAt(D3DXVECTOR3(0, 1, 0));

	box->update();
	grid->update();
}

void NormalMapScene::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	const D3DXMATRIX& view = Camera::view(), proj = Camera::projection();

	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff191970, 1.0f, 0 );
	device->BeginScene();

	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	box->draw();
	//grid->draw();

	device->EndScene();

	device->Present( NULL, NULL, NULL, NULL );
}

void NormalMapScene::release() {
	SAFE_RELEASE_DELETE(box);
	SAFE_RELEASE_DELETE(grid);
}