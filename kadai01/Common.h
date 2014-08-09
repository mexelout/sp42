#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>

class Common {
public:
	Common();
	~Common();
	static std::string debug(const char* fmt, ...);
	static DWORD ftodw(float f);
	static const float screen_width;
	static const float screen_height;
	static const float window_width;
	static const float window_height;
	static float random(float min, float max);
	static int random(int min, int max);
private:
};

#define SAFE_RELEASE(p) {if(p){p->Release();p=NULL;}}
#define SAFE_RELEASE_DELETE(p) {if(p){p->release();delete p;p=NULL;}}
#define SAFE_DELETE(p) {if(p) {delete p; p=NULL;}}
#define SAFE_DELETE_ARRAY(p) {if(p){delete[] p;p=NULL;}}
