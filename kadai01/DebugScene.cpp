#include <array>
#include "DebugScene.h"
#include "Common.h"
#include "ShaderDevise.h"
#include "Input.h"
#include "SceneManager.h"

DebugScene::DebugScene(void) {
	current = 0;
}

DebugScene::~DebugScene(void) {
}

DebugScene* DebugScene::init() {
	return this;
}
void DebugScene::update() {
	current -= (InputKeyboard::isKey(DIK_UP, Input::Trigger) && current > 0) ? 1 : 0;
	current += (InputKeyboard::isKey(DIK_DOWN, Input::Trigger) && current < 2) ? 1 : 0;
	if(InputKeyboard::isKey(DIK_RETURN, Input::Trigger)) {
		switch(current) {
			default:
				break;
		}
	}
}
void DebugScene::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0 );
	device->BeginScene();

	std::array<std::string, 3> menu = {
		"dammy"
	};
	menu[current] += "��";
	for(int i = 0, len = menu.size(); i < len; i++) {
		ShaderDevise::drawText(menu[i].c_str());
	}

	device->EndScene();
	device->Present( NULL, NULL, NULL, NULL );
}
void DebugScene::release() {
}




