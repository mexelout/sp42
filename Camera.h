#pragma once

#include <d3dx9.h>

class Camera {
public:
	Camera(void);
	~Camera(void);
	static void init();
	static const D3DXMATRIX& view();
	static const D3DXMATRIX& projection();
	static const D3DXMATRIX ortho();
	static void setEye(D3DXVECTOR3 eye);
	static void setAt(D3DXVECTOR3 at);
	static D3DXVECTOR3 eye();
	static D3DXVECTOR3 at();
	static D3DXVECTOR3 dir();
	static void setProjMatrix(D3DXMATRIX& proj);
private:
	static D3DXVECTOR3 _eye, _at, _up;
	static D3DXMATRIX _view, _projection, _ortho;
};

