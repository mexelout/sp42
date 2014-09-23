#pragma once

#include <string>
#include <vector>
#include <d3dx9.h>
#include "Singleton.h"

class TextureManager : public Singleton<TextureManager> {
private:
	friend Singleton; // ���F�B�`���F�B�`�Ղ���炟�I�I�I�I�I�I
public:
	/** �e�N�X�`���ǂݍ���
	* �ǂݍ��ݎ��s�ł�0���Ԃ��Ă���
	* @param filename �t�@�C����
	* @param device �`��f�o�C�X
	* @return �����ԍ�
	*/
	UINT loadTexture(std::string filename, LPDIRECT3DDEVICE9 device);
	/** �e�N�X�`���K�p
	* @param idx �����ԍ�
	* @param stage �X�e�[�W�ԍ�
	* @param device �`��f�o�C�X
	*/
	void applyTexture(UINT idx, DWORD stage, LPDIRECT3DDEVICE9 device);
	/** �e�N�X�`���̏����擾
	* @param idx �����ԍ�
	* @return �e�N�X�`���̏�񂪕Ԃ��Ă���(�ύX�s��)
	*/
	const D3DXIMAGE_INFO* getImageInfo(UINT idx);
	/** �e�N�X�`����S�J��&�X�^�b�N����
	*/
	void releaseAll();
	/** �e�N�X�`�����ēǂݍ���(�f�t�H���g�w��ɂ����)
	*/
	void resetLoad(LPDIRECT3DDEVICE9 device);
protected:
	/**
	* �R���X�g���N�^
	*/
	TextureManager();
	/**
	* �f�X�g���N�^
	*/
	~TextureManager();

	/** �����\����
	*/
	typedef struct TEXTURE_DATA{
		std::string filename;
		D3DXIMAGE_INFO info;
		LPDIRECT3DTEXTURE9 texture;
	}TEXTURE_DATA, *LPTEXTURE_DATA;



	/** datas */
	std::vector<TEXTURE_DATA> data;
	/** �K�p���摜�C���f�b�N�X*/
	UINT idx_apply;
};
