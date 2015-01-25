#include "SceneManager.h"
#include "Scene.h"
#include "Common.h"
#include "DebugScene.h"
#include "Input.h"
#include "NormalMapScene.h"
#include "WindowManager.h"
#include "BlurScene.h"
#include "MotionBlurScene.h"
#include "ShaderDevise.h"
#include "PointLightScene.h"
#include "Kadai08Scene.h"

Scene* SceneManager::scene;

SceneManager::SceneManager(void) {
	scene = NULL;
}

SceneManager::~SceneManager(void) {
}

void SceneManager::init() {
#ifdef _DEBUG
	scene = (new DebugScene)->init();
#else
	scene = (new Kadai08Scene)->init();
#endif
}
void SceneManager::update() {
	if(InputKeyboard::isKey(DIK_ESCAPE, Input::Trigger)) {
#ifdef _DEBUG
		setScene((new DebugScene)->init());
#else
		DestroyWindow(WindowManager::inst().getWnd());
#endif
	}
	scene->update();
}
void SceneManager::draw() {
	scene->draw();

	auto device = ShaderDevise::device();
	device->Present(NULL, NULL, NULL, NULL);
}
void SceneManager::release() {
	SAFE_RELEASE_DELETE(scene);
}
void SceneManager::setScene(Scene* scene) {
	if(scene) {
		SAFE_RELEASE_DELETE(SceneManager::scene);
		SceneManager::scene = scene;
	}
}

