/** SAFE�`�̃}�N���S
* @author HAL�����Q�[���w��3�N 25 ���R���a
*/
#ifndef _SAFE_H_
#define _SAFE_H_

#define SAFE_NEW(p, type) {if(p==NULL){p=new type;}}		// �������m��
#define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}	// �C���X�^���X�J��
#define SAFE_DELETE(p) {if(p){delete p;p=NULL;}}			// �������J��
#define SAFE_DELETE_ARRAY(p) {if(p){delete[]p;p=NULL;}}		// �z�񃁃����̈�J��
#define SAFE_UNINIT(p) {if(p){p->Uninit();SAFE_DELETE(p);}}	// �㏈���チ�����J��
#define SAFE_UNINIT_RELEASE(p) {if(p){p->Uninit();p->Release();p = NULL;}}

#endif // _SAFE_H_