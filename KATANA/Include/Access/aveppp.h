/*
 *
 *    AVEPPP API related definitions
 *
 *    aveppp.h
 *
 *    Copyright (C) 1998-1999 ACCESS CO.,LTD.
 */

/* $Header: /NetFront/TCP/dm_sdk/ppp/aveppp.h 2     99/12/06 11:22 Hamamoto $ */

#ifdef __cplusplus /* Support for C++ */
extern "C" {
#endif /* __cplusplus */

#ifndef AVEPPP_H
#define AVEPPP_H

#ifndef N_LONG_DEFINED
#define N_LONG_DEFINED
typedef unsigned long n_long;
typedef unsigned short n_short;
#endif

/*
 * <ppp/pppapi.h>�ɂ�����̍\���̂���`����Ă���̂Œ���
 * ���҂͈�v���Ă���K�v������
 */

typedef struct AvepppSerialParam {
	short speed;			/* �{�[���[�g 0:2400, 1:4800, 2:9600, */
							/* 3:19200, 4:38400 5:57600 (,6:115200) */
	short stop;				/* �X�g�b�v�r�b�g 0:1bit, 1:2bit */
	short parity;			/* �p���e�B 0:even, 1:odd, 2:none */
	short databit;			/* �f�[�^�� 0:8bit, 1:7bit */
	short flow;				/* �n�[�h�t���[���� 0:non, 1:RTS, 2:DTR */
	long reserved_1;		/* �g���p�O�ɂ��� */
	long reserved_2;		/* �g���p�O�ɂ��� */
} AvepppSerialParam;

typedef struct AvepppTelephoneParam {
	short dialtype;			/* �_�C�A���^�C�v 0:TONE, 1:PULSE */
	short outside_line;		/* �O��(0���M) 0:�Ȃ� 1:���� */
	short timeout;			/* �ʐM���^�C���A�E�g(�b) */
	short dial_retry;		/* �_�C�A�����g���C�� 1�`99 */
	short dial_interval;	/*	�_�C�A�����g���C�Ԋu(�b) 30�ȏ�𐄏� */
	char* outside_number;	/* �O��(�O���M)�ԍ��̕����� */
	long reserved_1;		/* �g���p�O�ɂ��� */
	long reserved_2;		/* �g���p�O�ɂ��� */
} AvepppTelephoneParam;

typedef struct AvepppConnectionParam {
	n_long his_ip;			/* �ڑ���IP�A�h���X(���g�p) */
	n_long my_ip;			/* ������IP�A�h���X(���g�p) */
	short recognize;		/* ���[�U�F�� 0:PAP, 1:chat, 2:CHAP, */
							/*			  3:�ڑ���̎w��ɂ�� */
	short mru;				/* �ő��M�o�C�g�� 2048�ȉ�(������1500) */
	long magic_number;		/* �}�W�b�N�i���o�[ 16�i8�� */
	short acfcomp;			/* HDLC�w�b�_�̈��k 0:enable, 1:disable */
	short protocomp;		/* PPP�w�b�_�̈��k 0:enable, 1:disable */
	short vjcomp;			/* IPCP��P-TCP�w�b�_�̈��k 0:enable, 1:disable */
	long reserved_1;		/* �g���p�O�ɂ��� */
	long reserved_2;		/* �g���p�O�ɂ��� */
} AvepppConnectionParam;

typedef struct AvepppOpenParam {
	struct AvepppSerialParam* serial;
	struct AvepppTelephoneParam* telephone;
	struct AvepppConnectionParam* pppconection;
	char* login_script1;	/* ���O�C���X�N���v�g1(���f���������p) */
	char* login_script2;	/* ���O�C���X�N���v�g2(�v���o�C�_�p) */
	char* telephone_number; /* �����d�b�ԍ� */
	char* login;			/* ���O�C���� */
	char* password;			/* �p�X���[�h */
	char* signup;			/* �T�C���A�b�v */
	long reserved_1;		/* �g���p�O�ɂ��� */
	long reserved_2;		/* �g���p�O�ɂ��� */
} AvepppOpenParam;

enum {
	AvepppConnectUnused,			/* ���I�[�v�� */
	AvepppConnectOpenStandby,		/* �I�[�v�����N�G�X�g����s�҂� */
	AvepppConnectDialing,			/* �_�C�A�����O */
	AvepppConnectAuthen,			/* �F�؏����� */
	AvepppConnectEstablished,		/* �I�[�v������ */
	AvepppConnectDisconnected,		/* ���肩��ؒf���ꂽ */
	AvepppConnectCloseStandby,		/* �ؒf������ */
	AvepppConnectOnhooking,			/* ����ؒf������ */
	AvepppConnectFail				/* �ڑ����s */
};

enum {
	AvepppErrorNon,
	AvepppErrorModem,
	AvepppErrorBusy,
	AvepppErrorNoDialTone,
	AvepppErrorScript,
	AvepppErrorLcp,
	AvepppErrorAuth,
	AvepppErrorIpcp,
	AvepppErrorTcp
};

typedef struct AvepppStatus {
	n_long	his_ip;			/* �ڑ���h�o */
	n_long	my_ip;			/* �����̂h�o */
	short	phase;			/* ����t�F�[�Y */
	long baud_rate;
	short port_inuse;		/* ���f���g�p�� */
	short last_error;
} AvepppStatus;

short AvepppInitialize(void);
short AvepppInitialize2(char *TxBuf, char *RxBuf,
						int TxBufSize, int RxBufSize); /*hamamoto 1999.11.16*/
void AvepppTerminate(void);
void AvepppSetDisconnectScript(char* p);
#ifndef DNSGET	/* horiuchi 1997041600 */
#else
void AvepppSetDns(
	long myip, long hisip,
	long mydns1, long mydns2, long hisdns1, long hisdns2
);
void AvepppGetDns(
	long* myip, long* hisip,
	long* mydns1, long* mydns2, long* hisdns1, long* hisdns2
);
#endif
short AvepppOpen(const AvepppOpenParam*);
short AvepppClose(short);
short AvetcpAbort(short nh);
short AvepppGetStatus(AvepppStatus*);

#endif

#ifdef __cplusplus
} /* End of extern "C" { */
#endif /* __cplusplus */
