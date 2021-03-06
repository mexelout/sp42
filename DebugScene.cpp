#include <array>
#include <vector>
#include "Common.h"
#include "ShaderDevise.h"
#include "Input.h"
#include "SceneManager.h"
#include "WindowManager.h"
#include "DebugScene.h"
#include "FogScene.h"
#include "NormalMapScene.h"
#include "BlurScene.h"
#include "MotionBlurScene.h"
#include "PointLightScene.h"
#include "TestScene.h"
#include "DeferredScene.h"
#include "SSAOScene.h"
#include "MSAAScene.h"
#include "Kadai08Scene.h"

DebugScene::DebugScene(void) {
	current = 0;
}

DebugScene::~DebugScene(void) {
	std::vector<std::string>().swap(menu);
}

DebugScene* DebugScene::init() {
	menu.push_back("Fog");
	menu.push_back("NormalMap");
	menu.push_back("DepthBlur");
	menu.push_back("MotionBlur");
	menu.push_back("PointLight");
	menu.push_back("Deferred");
	menu.push_back("SSAO");
	menu.push_back("MSAA");
	menu.push_back("Kadai08");
	menu.push_back("Test");
	menu.push_back("Exit");

	return this;
}
void DebugScene::update() {
	if(InputKeyboard::isKey(DIK_UP, Input::Trigger)) {
		if(current > 0) {
			current -= 1;
		} else {
			current = (int)menu.size()-1;
		}
	}
	if(InputKeyboard::isKey(DIK_DOWN, Input::Trigger)) {
		if(current < (int)menu.size()-1) {
			current += 1;
		} else {
			current = 0;
		}
	}
	const int exit_num = menu.size()-1;
	if(InputKeyboard::isKey(DIK_RETURN, Input::Trigger)) {
		switch(current) {
			case 0: SceneManager::setScene((new FogScene)->init()); break;
			case 1: SceneManager::setScene((new NormalMapScene)->init()); break;
			case 2: SceneManager::setScene((new BlurScene)->init()); break;
			case 3: SceneManager::setScene((new MotionBlurScene)->init()); break;
			case 4: SceneManager::setScene((new PointLightScene)->init()); break;
			case 5: SceneManager::setScene((new DeferredScene)->init()); break;
			case 6: SceneManager::setScene((new SSAOScene)->init()); break;
			case 7: SceneManager::setScene((new MSAAScene)->init()); break;
			case 8: SceneManager::setScene((new Kadai08Scene)->init()); break;
			case 9: SceneManager::setScene((new TestScene)->init()); break;
			default: DestroyWindow(WindowManager::inst().getWnd()); break;
		}
	}
}
void DebugScene::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0 );
	device->BeginScene();

	for(int i = 0, len = menu.size(); i < len; i++) {
		ShaderDevise::drawText((menu[i] + ((i == current) ? "*" : "")).c_str());
	}

	device->EndScene();
}
void DebugScene::release() {
	DebugScene::~DebugScene();
}




