#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include "TextureManager.h"
#include "safe.h"
#include "main.h"

/* �R���X�g���N�^*/
TextureManager::TextureManager() {
	idx_apply = 0;
	TEXTURE_DATA d;
	d.filename = "";
	memset(&d.info, 0, sizeof(D3DXIMAGE_INFO));
	d.texture = NULL;
	data.push_back(d);
}
/* �f�X�g���N�^*/
TextureManager::~TextureManager() {
	releaseAll();
}
/** �e�N�X�`���ǂݍ���\n
* �X�^�b�N�ɓ��肫��Ȃ�������0���Ԃ��Ă���\m
* �ǂݍ��ݎ��s�ł�0���Ԃ��Ă���
* @param pFilename �t�@�C����
* @param pDevice �`��f�o�C�X
* @return �����ԍ�
*/
UINT TextureManager::loadTexture(std::string filename, LPDIRECT3DDEVICE9 device) {
	TextureManager& t = TextureManager::inst();

	// �������̓ǂݍ��݋֎~
	for(UINT i = 0, len = t.data.size(); i < len; ++i) {
		if(filename == t.data[i].filename) {
			return i;
		}
	}

	HRESULT hr;
	TEXTURE_DATA d;
	hr = D3DXGetImageInfoFromFile(filename.c_str(), &d.info);

	// �t�@�C���̗L��
	if(SUCCEEDED(hr)) {
		// �L��Ƃ킩������F�X�ۑ�
		d.filename = filename;

		// �e�N�X�`���o�b�t�@�ǂݍ���
		D3DXCreateTextureFromFileEx(device, filename.c_str(), d.info.Width, d.info.Height, 1, 0, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0x00000000, NULL, NULL, &d.texture);
		t.data.push_back(d);

		return t.data.size() - 1;
	} else {
		// �J���˂��I�H
		MessageBox(NULL, "�摜���ǂݍ��߂˂���I", "�G���[", MB_ERROR);
		return 0;
	}
}
/** �e�N�X�`���K�p
* @param nIdxNum �����ԍ�
* @param dwStage �X�e�[�W�ԍ�
* @param pDevice �`��f�o�C�X
*/
void TextureManager::applyTexture(UINT idx, DWORD stage, LPDIRECT3DDEVICE9 device) {
	TextureManager& t = TextureManager::inst();

	// �ő�ȏ�Ȃ炳�����Ɣ�����
	if(idx >= t.data.size()) return;

	// ���ɓK�p�ς݂Ȃ炳�����Ɣ�����
	if(t.idx_apply == idx) return;

	// �e�N�X�`���K�p
	device->SetTexture(stage, t.data[idx].texture);

	// ����ݒ肵���ԍ���ۑ�(����K�p���Ȃ���)
	t.idx_apply = idx;
}
/** �e�N�X�`���̏����擾\n
* �ςȐ��l�������ɂ����NULL���Ԃ��Ă���
* @param nIdxNum �����ԍ�
* @return �e�N�X�`���̏�񂪕Ԃ��Ă���(�ύX�s��)
*/
const D3DXIMAGE_INFO* TextureManager::getImageInfo(UINT idx) {
	TextureManager& t = TextureManager::inst();
	if(idx >= t.data.size()) return NULL;
	return &t.data[idx].info;
}
/** �e�N�X�`����S�J��&�X�^�b�N����
*/
void TextureManager::releaseAll() {
	TextureManager& t = TextureManager::inst();
	for each(TEXTURE_DATA d in t.data) {
		SAFE_RELEASE(d.texture);
	}
	t.data.clear();
	t.idx_apply = 0;
}
/** �e�N�X�`�����ēǂݍ���(�f�t�H���g�w��ɂ����)
*/
void TextureManager::resetLoad(LPDIRECT3DDEVICE9 device) {
	TextureManager& t = TextureManager::inst();
}

// end file