/*
 *  AVE-TCP 3.1 SDK source code.       
 *
 * 	at_pltfm.h
 *
 *	�ڐA�v���b�g�t�H�[���ˑ��w�b�_�t�@�C��
 *
 *	Copyright (C) 1998 ACCESS CO.,LTD.
 */

#ifdef __cplusplus	/* Support for C++ */
extern "C" {
#endif /* __cplusplus */

#ifndef AT_PLTFRM_H
#define AT_PLTFRM_H

/* �T���v���v���b�g�t�H�[�� */

/* �v���b�g�t�H�[����include�t�@�C�� */
/* #    include <memory.h>	/* memset(), memcpy() */

/* �萔�̒�` */
/* #define AT_LITTLE_ENDIAN	/* �o�C�g�I�[�_ : �R���p�C�����Ɏw�肷�邱�� */

/* 
 *  �f�[�^�^�̒�` 
 *  ��: �ȉ��́A
 *  �Evoid�^�������A
 *  �E�f�[�^�̃T�C�Y��
 *  char: 1byte, int: 2byte, long int: 4byte
 *  �̃A�[�L�e�N�`����z�肵�Ă���B
 */

/* ��{�^ */
typedef void AT_VOID;                 /* void�^ */

/* �����^ */
typedef unsigned char AT_UBYTE;       /* 8bit�����Ȃ����� */
typedef char AT_SBYTE;                /* 8bit���������� */
typedef unsigned short int AT_UINT16; /* 16bit�����Ȃ����� */
typedef short int AT_SINT16;          /* 16bit���������� */
typedef unsigned long int AT_UINT32;  /* 32bit�����Ȃ����� */
typedef long int AT_SINT32;           /* 32bit���������� */

/* �^�X�N�߂�l�̌^ */
typedef AT_VOID AT_TASK; 
#define AT_VOLATILE volatile

/* �f�o�b�O�o�� */
#define AT_DEBUG_LOG(title, fmt, arg) (title, fmt, arg) /* �]�������B*/

#endif /* AT_PLTFRM_H */
#ifdef __cplusplus
} /* End of extern "C" { */
#endif /* __cplusplus */
