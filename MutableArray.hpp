#pragma once

#ifndef null
#define null 0
#endif
#ifndef delete_array
#define delete_array(p) {if(p){delete[] p;p=null;}}
#endif

template<typename T>
class MutableArray {
public:
	MutableArray(void) {
		substance = null;
		_count = 0;
	}
	~MutableArray(void) {
		release();
	}
	void add(T t, bool prepend = false) { // prepend‚ªtrue‚È‚çæ“ª‚É’Ç‰Á
		_count++;
		T* tmp = new T[_count];
		for(int i = (prepend) ? 1 : 0, len = _count - 1; i < len; i++) {
			tmp[i] = substance[i];
		}
		delete_array(substance);
		tmp[(prepend) ? 0 : _count-1] = t;
		substance = tmp;
	}
	void sub(int idx) {
		if(idx > _count) return;
		_count--;
		T* tmp = new T[_count];
		for(int i = 0, len = _count + 1; i < len; i++) {
			if(i != idx) tmp[i] = substance[i];
		}
		delete_array(substance);
		substance = tmp;
	}
	T get(int idx) {
		if(idx > _count) return substance[0];
		return (T)substance[idx];
	}
	void set(T t, int idx) {
		if(idx > _count) return;
		substance[idx] = t;
	}
	T operator [] (int idx) {
		return get(idx);
	}
	void operator << (T& t) {
		add(t);
	}
	int count() {
		return _count;
	}
	void release() {
		delete_array(substance);
		_count = 0;
	}
private:
	T* substance;
	int _count;
};

#undef null
#undef delete_array