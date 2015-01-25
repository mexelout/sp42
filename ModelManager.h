#pragma once
#include "singleton.h"
#include "MutableArray.hpp"
#include "Model.h"
#include <string>

class ModelManager : public Singleton<ModelManager> {
public:
	ModelManager();
	~ModelManager();
	static int load(std::string filename); // ���s����-1���Ԃ��Ă���B����ȊO�͗v�f�ԍ�
	static Model* get(int i);
	static void release();
private:
	typedef struct ModelInfo{
		ModelInfo();
		Model* model;
		std::string filename;
	}ModelInfo;
	ModelInfo operator[] (int i);
	MutableArray<ModelInfo> models;
};

