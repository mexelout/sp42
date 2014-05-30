#pragma once

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <Windows.h>
#include <dinput.h>

// キーの数
const DWORD KEY_NUM = 256ul;

class Input {
public:
	typedef enum {
		Press,
		Trigger,
		Release
	}State;
	Input(void);
	~Input(void);
	static void init(HINSTANCE inst, HWND hwnd);
	static void update();
	static void release();
	static BOOL isKey(BYTE dik, State state);
protected:
	static LPDIRECTINPUT8 d_input;
	static LPDIRECTINPUTDEVICE8 d_device;

	// キーボードに必要な変数
	static BYTE press_key_state[KEY_NUM];
	static BYTE trigger_key_state[KEY_NUM];
	static BYTE release_key_state[KEY_NUM];
	static BYTE repeat_key_state[KEY_NUM];
};

