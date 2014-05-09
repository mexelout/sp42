#include <Windows.h>
#pragma once
class ShaderDevise {
public:
	ShaderDevise(void);
	~ShaderDevise(void);
	ShaderDevise* init(int w, int h, HWND hWnd);
	LPDIRECT3DDEVICE9 device();
	void release();
private:
	LPDIRECT3D9 _d3d;
	LPDIRECT3DDEVICE9 _device;
};

