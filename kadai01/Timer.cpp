#include "Timer.h"

unsigned int Timer::t;

Timer::Timer() {
	t = 0;
}
Timer::~Timer() {
	t = 0;
}
void Timer::init() {
}
void Timer::update() {
	++t;
}
unsigned int Timer::elapse() {
	return t;
}
