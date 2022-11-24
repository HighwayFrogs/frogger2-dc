/*
** sbinit.c Ver.0.60  1998/8/28
** Copyright (C) 1998 SEGA Enterprises Co.,Ltd
** All Rights Reserved
*/

#include <shinobi.h>      /* �E�w�b�_�t�@�C��                         */
#include <sg_syhw.h>      /* �n�[�h�E�F�A���������C�u����             */

#define P1AREA   0x80000000

extern Uint8* _BSG_END;   /* BSG/BSG32�Z�N�V�����̏I���A�h���X        */

/* ���[�NRAM�̏I���A�h���X                                            */
#define WORK_END (((Uint32)_BSG_END) & 0xe0000000 | 0x0d000000)




/* �����̋L�q�́A���C�u�����g�p�҂̃V�X�e���ɍ��킹��                 */
/* �ύX���邱�Ƃ��ł��܂��B                                           */
/* �ύX�̍ۂ͊֘A�h�L�������g���Q�Ƃ��A�e���ڂ�K�؂ɏC�����Ă������� */

/***** GD�t�@�C���V�X�e�����g�����ǂ��� *******************************/
#define USE_GDFS 1        /* 0...�g��Ȃ�                             */
                          /* 1...�g��(DEFALUT)                        */

/***** B�Z�N�V�����̏I�����q�[�v�̐擪�Ƃ��邩 **********************/
#define USE_B_END 1       /* 0...���Ȃ�                               */
                          /* 1...����(DEFALUT)                        */
/* B�Z�N�V�����̏I�����q�[�v�̐擪�Ƃ��Ȃ��ꍇ�A                    */
/* HEAP_SIZE �Ƀq�[�v�e�ʂ��`���Ă��������B                         */


/* GD:�����ɊJ�����Ƃ̂ł���ő�t�@�C���� */
#define FILES 8

/* GD:�J�����g�f�B���N�g���o�b�t�@ */
#define BUFFERS 1024

/* syMalloc()�Ŋm�ۂł��鍇�v�e��(��4MB)                              */
/* (B�Z�N�V�����̏I�����q�[�v�̐擪�Ƃ��Ȃ��ꍇ�̂ݗL��)            */
#if !USE_B_END
#define HEAP_SIZE 0x00400000
#endif









/* �O���[�o�����[�N�̐錾 */

Uint8 gMapleRecvBuf[1024 * 24 * 2 + 32];
Uint8 gMapleSendBuf[1024 * 24 * 2 + 32];

#if USE_GDFS
Uint8 gdfswork[GDFS_WORK_SIZE(FILES) + 32];
Uint8 gdfscurdir[GDFS_DIRREC_SIZE(BUFFERS) + 32];
#endif

/* syMalloc()�̊Ǘ����ɒu���q�[�v�̐擪�A�h���X */
#if USE_B_END
#define HEAP_AREA ((void*)((((Uint32)_BSG_END | P1AREA) & 0xffffffe0) + 0x20))
#define HEAP_SIZE (WORK_END - (Uint32)HEAP_AREA)
#else
#define HEAP_AREA ((void*)((Uint32)WORK_END - (Uint32)HEAP_SIZE))
#endif

/*
** �A�v���P�[�V�����̏������֐�
** ������njInitSystem()�݊�
*/
void sbInitSystem(Int mode, Int frame, Int count)
{
	/* �W���I�ȏ������������s���܂��B                             */
	/* ���̐ݒ�ŁA�����̃A�v���P�[�V�������ő�̃p�t�H�[�}���X�� */
	/* ���邱�Ƃ��ł��܂��B                                       */
	/* ���ɗ��R�̂Ȃ�����A�����̋L�q���C�����Ȃ��ł��������B     */
	/* �C������ꍇ�́A�e���C�u�����̎d�l���\������������A       */
	/* ���C�u�����g�p�҂̐ӔC�ɂ����ďC�����Ă��������B           */

	/* ���荞�݋֎~ */
	set_imask(15);

	/* �n�[�h�E�F�A�̏����� */
	syHwInit();

	/* �������Ǘ��̏����� */
	syMallocInit(HEAP_AREA, HEAP_SIZE);

	/* Ninja/Kamui�̏����� */
	njInitSystem(mode, frame, count);

	/* �n�[�h�E�F�A�̏��������̂Q */
	syHwInit2();

	/* �R���g���[�����C�u�����̏����� */
	pdInitPeripheral(PDD_PLOGIC_ACTIVE, gMapleRecvBuf, gMapleSendBuf);

	/* RTC���C�u�����̏����� */
	syRtcInit();

	/* ���荞�݋��� */
	set_imask(0);

#if USE_GDFS
	/* GD�t�@�C���V�X�e���̏�����                               */
	/* �����ł́AgdFsInit()����������܂Ŗ����Ƀ��g���C���܂��B */
	/* ���ۂ̃A�v���P�[�V�����ł́A�g���C�I�[�v�����̃`�F�b�N�� */
	/* �K���s���Ă��������B                                     */
	{
		Uint8* wk;
		Uint8* dir;
		Sint32 err;

		wk  = (Uint8*)(((Uint32)gdfswork & 0xffffffe0) + 0x20);
		dir = (Uint8*)(((Uint32)gdfscurdir & 0xffffffe0) + 0x20);

		do {
			err = gdFsInit(FILES, wk, BUFFERS, dir);
		} while (err != GDD_ERR_OK);
	}
#endif

	/* ���̑��̏�����                                           */
	/* ��ɂȂ����̑��̃��C�u�����̏������A�����               */
	/* ���[�U�[�ōs���ׂ�������������ꍇ�A�����ɋL�q�ł��܂��B */

	;

}


/*
** �A�v���P�[�V�����̏I���֐�
*/

void sbExitSystem(void)
{

#if USE_GDFS
	/* GD�t�@�C���V�X�e���̏I������ */
	gdFsFinish();
#endif

	/* RTC���C�u�����̏I������ */
	syRtcFinish();

	/* �R���g���[�����C�u�����̏I������ */
	pdExitPeripheral();

	/* Ninja/Kamui�̏I������ */
	njExitSystem();

	/* �������Ǘ��̏I������ */
	syMallocFinish();

	/* �n�[�h�E�F�A�̏I������ */
	syHwFinish();

	/* ���荞�݋֎~ */
	set_imask(15);
}


/******************************* end of file *******************************/

