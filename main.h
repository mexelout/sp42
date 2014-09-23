//=============================================================================
//
// �T�v      :�S�̂ɋ��ʂ��鍀�ڂɊւ���R�[�h
// �t�@�C����:[main.h]
// �쐬��    :AT13J295 25 ths00212 ���R���a
//
//=============================================================================
#pragma once

#include <windows.h>

// NULL�����邽�߂̃}�N��
#define NULL_VECTOR3(v) {v.x=v.y=v.z=0;}
#define NULL_COLORVALUE(c) {c.r=c.g=0c.b=c.a=0;}
#define SET_COLORVALUE(c, red, grn, ble, alp) {c.r=red;c.g=grn;c.b=ble;c.a=alp;}
#define NULL_LIGHT(l) {l.Type=D3DLIGHTTYPE(0);NULL_COLORVALUE(l.Diffuse);NULL_COLORVALUE(l.Specular);NULL_COLORVALUE(l.Ambient);NULL_VECTOR3(l.Position);NULL_VECTOR3(l.Direction);l.Range=0;l.Falloff=0;l.Attenuation0=0;l.Attenuation1=0;l.Attenuation2=0;l.Theta=0;l.Phi=0;}

// �������x(1/60�̑��x)
#define GRAVITY(t) (0.16f * t)

// �G���[�p
#define MB_ERROR (MB_OK | MB_ICONWARNING)

// �F(����ǉ�)
enum {
	COLOR_WHITE = 0xffffffff,
	COLOR_RED = 0xffff0000,
	COLOR_GREEN = 0xff00ff00,
	COLOR_BLUE = 0xff0000ff,
	COLOR_BLACK = 0xff000000
};

// �F�̍ő�l(byte)
#define COLOR_MAX_BYTE (255)

// �F�̍ő�l(float)
#define COLOR_MAX_FLOAT (1.0f)

//=====================================
// �v���g�^�C�v�錾

// ���̃A�v���̃C���X�^���X���擾
HINSTANCE GetInst();
// ���̃A�v���̃E�B���h�E�n���h�����擾
HWND GetWnd();

// end file