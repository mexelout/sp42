#include <Windows.h>
#include <stdarg.h>
#include "Common.h"

const float Common::screen_width = (float)GetSystemMetrics(SM_CXSCREEN);
const float Common::screen_height = (float)GetSystemMetrics(SM_CYSCREEN );
const float Common::window_width = 1200;
const float Common::window_height = 900;

Common::Common() {

}
Common::~Common() {
}

std::string Common::debug(const char* fmt, ...) {
	va_list ap;
	std::string rs;

	va_start(ap, fmt);

	char check = 0;

	for(int i = 0, len = strlen(fmt); i < len; ++i) {
		check = fmt[i];
		if(check == '%') {
			++i;
			check = fmt[i];
			switch(check) {
				case 'c': {
					char c = va_arg(ap, int);
					rs += c;
					break;
				}
				case 'd': {
					int _ = va_arg(ap, int);
					rs += std::to_string((_Longlong)_);
					break;
				}
				case 'f': {
					rs += std::to_string((long double)va_arg(ap, double));
					break;
				}
				case 's': {
					rs += va_arg(ap, char*);
					break;
				}
				case 'b': {
					rs += (va_arg(ap, bool)) ? "true" : "false";
					break;
				}
			}
		} else {
			rs += check;
		}
	}
	rs += "\n";

	va_end(ap);
	OutputDebugString(rs.c_str());

	return rs;
}

DWORD Common::ftodw(float f) {
	return *((DWORD*)&f);
}

float Common::random(float min, float max) {
	return min + (rand()*(max-min+1.0f)/(1.0f+RAND_MAX));
}

int Common::random(int min, int max) {
	return (int)(random((float)min, (float)max));
}

// end line