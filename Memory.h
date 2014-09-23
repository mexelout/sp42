#pragma once
#include <vector>

class Memory {
public:
	static void init();
	template<class T> static void writeArray(char* key, std::vector<T> vec);
	template<class T> static std::vector<T> readArray(char* key);
private:
	Memory(void);
	~Memory(void);

	
};

