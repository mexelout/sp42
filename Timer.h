#pragma once

class Timer {
public:
	Timer();
	~Timer();

	static void init();
	static void update();
	static unsigned int elapse();
private:
	static unsigned int t;
};