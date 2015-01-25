#include "OwnTexture.h"
#include "Common.h"
#include "ShaderDevise.h"

OwnTexture::OwnTexture() {
	tex = NULL;
	sur = NULL;
}
OwnTexture::~OwnTexture() {
	SAFE_RELEASE(tex);
	SAFE_RELEASE(sur);
}
void OwnTexture::createTexture(UINT width, UINT height, UINT levels,DWORD usage, D3DFORMAT format, D3DPOOL pool) {
	auto device = ShaderDevise::device();
	device->CreateTexture(width, height, levels, usage, format, pool, &tex, NULL);
	tex->GetSurfaceLevel(0, &sur);
}
