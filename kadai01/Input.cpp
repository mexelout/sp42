#include "Input.h"
#include <string>

LPDIRECTINPUT8 Input::d_input = NULL;
LPDIRECTINPUTDEVICE8 Input::d_device;

BYTE Input::press_key_state[KEY_NUM];
BYTE Input::trigger_key_state[KEY_NUM];
BYTE Input::release_key_state[KEY_NUM];
BYTE Input::repeat_key_state[KEY_NUM];

Input::Input(void) {
	memset(press_key_state, 0, sizeof press_key_state);
	memset(trigger_key_state, 0, sizeof trigger_key_state);
	memset(release_key_state, 0, sizeof release_key_state);
	memset(repeat_key_state, 0, sizeof repeat_key_state);
}

Input::~Input(void) {
}

void Input::init(HINSTANCE inst, HWND hwnd) {
	if(!d_input) DirectInput8Create(inst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&d_input, NULL);

	// ここからキーボードの設定
	d_input->CreateDevice(GUID_SysKeyboard, &d_device, NULL);
	d_device->SetDataFormat(&c_dfDIKeyboard);
	d_device->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	d_device->Acquire();
}

void Input::update() {
	HRESULT hr;
	BYTE prev_key_state[256];
	memcpy(prev_key_state, press_key_state, sizeof press_key_state);
	hr = d_device->GetDeviceState(KEY_NUM, press_key_state);
	if(SUCCEEDED(hr)) {
		for(int i = 0; i < KEY_NUM; i++) {
			press_key_state[i] = (press_key_state[i] & 0x80) == 0x80;
			trigger_key_state[i] = (!prev_key_state[i] == press_key_state[i]);
			release_key_state[i] = (prev_key_state[i] == !press_key_state[i]);
		
		}
	} else {
		d_device->Acquire();
	}
}

void Input::release() {
	d_device->Unacquire();
	d_device->Release();
	d_input->Release();
}

BOOL Input::isKey(BYTE dik, State state) {
	switch(state) {
		case State::Press:
			return (press_key_state[dik]);
		case State::Trigger:
			return (trigger_key_state[dik]);
		case State::Release:
			return (release_key_state[dik]);
	};
}

