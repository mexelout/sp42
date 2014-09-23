#pragma once

template<typename T>class Singleton {
public:
	/** �C���X�^���X�擾
	* @return T& ������
	*/
	static T& inst() {
		static T i;
		return i;
	}
protected:
	/**
	* �R���X�g���N�^
	*/
	Singleton() {}
	/**
	* �f�X�g���N�^
	*/
	~Singleton() {}
private:
	/** �R�s�[�R���X�g���N�^
	* �Ăяo���֎~
	*/
	Singleton(const Singleton& s) {(*this) = s}

	/** ������Z�q
	* �Ăяo���֎~
	*/
	Singleton& operator = (const Singleton &s) {(*this) = s}
};
