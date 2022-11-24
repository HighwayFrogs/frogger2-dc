/*
 *
 *    AVETCP API related definitions
 *
 *    at_api.h
 *
 *    Copyright (C) 1998 ACCESS CO.,LTD.
 */

#ifdef __cplusplus /* Support for C++ */
extern "C" {
#endif /* __cplusplus */

#ifndef HEAD_OF_AVETCPAP
#define HEAD_OF_AVETCPAP

#include "at_types.h"

/* Function Code Spec */

#define	GETCONFIG       0x4101
#define	SETASR          0x4102
#define	IFCONFIG        0x4105
#define	IFDOWN          0x4106

#define	TCP_OPEN        0x4110
#define	TCP_POPEN       0x4111
#define	TCP_ACCEPT      0x4112
#define	TCP_CLOSE       0x4113
#define	TCP_SHUTDOWN    0x4114
#define	TCP_ABORT       0x4115
#define	TCP_GETADDR     0x4116
#define	TCP_STAT        0x4117
#define	TCP_SEND        0x4118
#define	TCP_RECEIVE     0x4119
#define	TCP_CANCEL      0x411a
#define	TCP_SEND_F      0x411b

#define	UDP_OPEN        0x4120
#define	UDP_SEND        0x4121
#define	UDP_CLOSE       0x4113

#define	ROUTE_ADD       0x4130
#define	ROUTE_DEL       0x4131
#define	ROUTE_LIST      0x4132

/* Error Code Spec */

#define SUCCESS                 (  0)
#define API_PENDING             ( -1) /* 0xffff */
#define API_PORTFULL            ( -2) /* 0xfffe */
#define API_DUPSOCK             ( -3) /* 0xfffd */
#define API_INVALIDHANDLE       ( -4) /* 0xfffc */
#define API_SENDPENDING         ( -5) /* 0xfffb */
#define API_CONNRESET           ( -6) /* 0xfffa */
#define API_UNREACHABLE         ( -7) /* 0xfff9 */
#define API_RECVFIN             ( -8) /* 0xfff8 */
#define API_RECVPENDING         ( -9) /* 0xfff7 */
#define API_PORTCLOSED          (-10) /* 0xfff6 */
#define	API_TIMEOUT             (-11) /* 0xfff5 */
#define API_NOBUF               (-12) /* 0xfff4 */
#define API_BADOPT              (-13) /* 0xfff3 */
#define API_GENERAL             (-16) /* 0xfff0 */
#define API_UNSUPPORTEDFUNC     (-16) /* 0xfff0 */

/* ASR event code */

/* for TCP module */

#define ASR_ESTABLISHED        1
#define ASR_FIN                2
#define ASR_DISCONNECT         4
#define ASR_RST                10
#define ASR_TIMEOUT            11
#define ASR_HOST               12
#define ASR_PORT               13

/* for UDP module */

#define ASR_ADDR_UNREACH       128
#define ASR_ADDR_PORT_UNREACH  129 

/* for common */

#define ASR_LAST               255

/* AVE-TCP API Function */

/* API Call */
extern AT_SINT16 AT_apiCall(AT_UINT16 code, AT_VOID *inp, AT_VOID *outp);
/* Initialize  */
extern AT_SINT16 AT_init(AT_VOID *initArg);
/* Dispose */
extern AT_SINT16 AT_disp(AT_VOID *dispArg);

/* Parameter Structures */

/* general/common command */

typedef struct _AT_config {
    AT_NINT32 ip_addr;    /* IP �A�h���X�i�l�b�g���[�N�I�[�_�j*/
    AT_NINT32 netmask;    /* �T�u�l�b�g�}�X�N */
    AT_NINT32 broadcast;  /* �u���[�h�L���X�g�A�h���X */
}AT_config;

typedef struct _AT_configptr{
    AT_VOID *config_ptr;  /* �R���t�B�O���[�V�����u���b�N�̃A�h���X */
}AT_configptr;

typedef struct _AT_setasr {
    AT_SINT16 nh;         /* �l�b�g�n���h�� */
    AT_VOID (* asraddr)(AT_SINT16, AT_SINT16);
                          /* ASR ���[�`���̃A�h���X */
}AT_setasr;

typedef struct _AT_ifconfigparam {
    AT_SINT16 if_num;     /* �C���^�t�F�[�X�ԍ��i�O�̂݃T�|�[�g�j */
    AT_NINT32 ip_addr;    /* IP �A�h���X */
    AT_NINT32 netmask;    /* �T�u�l�b�g�}�X�N */
    AT_NINT32 broadcast;  /* �u���[�h�L���X�g�A�h���X */
}AT_ifconfigupparam;

typedef struct _AT_ifconfigdownparam {
    AT_SINT16 if_num;     /* �C���^�t�F�[�X�ԍ��i�O�̂݃T�|�[�g�j */
}AT_ifconfigdownparam;

/* TCP command */

typedef struct _AT_openparam {
    AT_NINT32 youraddr;   /* ����� IP �A�h���X */
    AT_NINT16 yourport;   /* ����̃|�[�g�ԍ� */
    AT_NINT16 myport;     /* �����̃|�[�g�ԍ� */
    AT_UBYTE reserve1;    /* ���g�p�̈� */
    AT_UBYTE ttl;         /* Time to live */
    AT_UBYTE svctype;     /* Service type */
    AT_UBYTE df_flag;     /* Don't Fragment flag */
    AT_SINT16 reserve2;   /* ���g�p�̈� */
    AT_VOID *reserve3;     /* ���g�p�̈� */
    AT_VOID (* asraddr)(AT_SINT16, AT_SINT16);
                          /* ASR ���[�`���̃A�h���X */
}AT_openparam;

typedef struct _AT_popenparam {
    AT_NINT32 youraddr;   /* ����� IP �A�h���X */
    AT_NINT16 yourport;   /* ����̃|�[�g�ԍ� */
    AT_NINT16 myport;     /* �����̃|�[�g�ԍ� */
    AT_UBYTE reserve1;    /* ���g�p�̈� */
    AT_UBYTE ttl;         /* Time to live */
    AT_UBYTE svctype;     /* Service type */
    AT_UBYTE df_flag;     /* Don't Fragment flag */
    AT_SINT16 reserve2;   /* ���g�p�̈� */
    AT_VOID *reserve3;     /* ���g�p�̈� */
}AT_popenparam;

typedef struct _AT_acceptparam {
    AT_SINT16 nh;         /* �l�b�g�n���h�� */
    AT_VOID (* notifyaddr)(AT_SINT16, AT_SINT16, AT_UINT32, AT_UINT16, AT_UINT16);
                          /* notify ���[�`���̃A�h���X */
    AT_VOID (* asraddr)(AT_SINT16, AT_SINT16);
                          /* ASR ���[�`���̃A�h���X */
}AT_acceptparam;

typedef struct _AT_nhparam {
    AT_SINT16 nh;         /* �l�b�g�n���h�� */
}AT_nhparam;

typedef struct _AT_getaddr_r{
    AT_NINT32 ipaddr;     /* ����̂h�o�A�h���X */
    AT_NINT16 myport;     /* �����̃|�[�g�ԍ� */
    AT_NINT16 yourport;   /* ����̃|�[�g�ԍ� */
}AT_getaddr_r;

typedef struct _AT_stat_r{
    AT_UINT16 stat;       /* �R�l�N�V�����̏�� */
    AT_UINT16 reserve;    /* ���g�p�̈� */
    AT_UINT16 sendwin;    /* ���M�E�C���h�E�̎c�� */
    AT_UINT16 recvwin;    /* �����ςݎ�M�f�[�^�� */
}AT_stat_r;

typedef struct _AT_sendparam {
    AT_SINT16 nh;         /* �l�b�g�n���h�� */
    AT_VOID (* notifyaddr)(AT_SINT16, AT_SINT16);
                          /* notify ���[�`���̃A�h���X */
    AT_UBYTE reserve;     /* ���g�p�̈� */
    AT_UBYTE bufnum;      /* �g�p����o�b�t�@�� */
    struct {
        AT_SINT16 len;    /* �f�[�^�� */
        AT_UBYTE  *buf;   /* �f�[�^�A�h���X */
    }buffers[1];          /* Variable length array 1�`4 */
}AT_sendparam;

typedef struct _AT_rcvparam {
    AT_SINT16 nh;         /* �l�b�g�n���h�� */
    AT_SINT16 len;        /* �o�b�t�@�� */
    AT_UBYTE  *buf;       /* �o�b�t�@�A�h���X */
    AT_VOID (* notifyaddr)(AT_SINT16, AT_SINT16, AT_SINT16);
                          /* notify ���[�`���̃A�h���X */
}AT_rcvparam;

typedef struct _AT_cancelparam {
    AT_SINT16 nh;         /* �l�b�g�n���h�� */
    AT_SINT16 what;       /* �L�����Z���Ώ� */
}AT_cancelparam ;

typedef struct _AT_sendfparam {
    AT_SINT16 nh;         /* �l�b�g�n���h�� */
    AT_VOID (* notifyaddr)(AT_SINT16, AT_SINT16, AT_UBYTE *);
                          /* notify ���[�`���̃A�h���X */
    AT_UBYTE reserve;     /* ���g�p�̈� */
    AT_SINT16 len;        /* �f�[�^�� */
    AT_UBYTE  *buf;       /* �f�[�^�A�h���X */
}AT_sendfparam;

/* UDP command */

typedef struct _AT_UDPPRM {
    AT_SINT16 handle;     /* �l�b�g�n���h�� */
    AT_VOID *buf;         /* �f�[�^�A�h���X */
    AT_SINT16 len;        /* �f�[�^�� */
    AT_NINT32 srcaddr;    /* ���M���� IP �A�h���X */
    AT_NINT16 srcport;    /* ���M���̃|�[�g�ԍ� */
    AT_UBYTE *usrbuff;    /* �o�b�t�@�A�h���X */
} AT_UDPPRM;

typedef struct _AT_uopenparam {
    AT_NINT32 youraddr;   /* ����� IP address */
    AT_NINT16 yourport;   /* ����̃|�[�g�ԍ� */
    AT_NINT16 myport;     /* �����̃|�[�g�ԍ� */
    AT_UBYTE reserve1;    /* ���g�p�̈� */
    AT_UBYTE ttl;         /* Time to live */
    AT_UBYTE svctype;     /* Service type */
    AT_UBYTE df_flag;     /* Don't Fragment flag */
    AT_SINT16 reserve2;   /* ���g�p�̈� */
    AT_VOID  *reserve3;   /* ���g�p�̈� */
    AT_VOID (* asraddr)(AT_SINT16, AT_SINT16);
                          /* ASR ���[�`���̃A�h���X */
    AT_SINT16 (* receiver)(AT_SINT16, AT_UDPPRM *);
                          /* ���V�[�o���[�`���̃A�h���X */
}AT_uopenparam;

typedef struct _AT_usendparam {
    AT_SINT16 nh;        /* �l�b�g�n���h�� */
    AT_NINT32 youraddr;  /* ����� IP �A�h���X */
    AT_NINT16 yourport;  /* ����̃|�[�g�ԍ� */
    AT_UBYTE bufnum;     /* �g�p����o�b�t�@�� */
    struct {
        AT_SINT16 len;   /* �f�[�^�� */
        AT_UBYTE *buf;   /* �f�[�^�A�h���X */
    }buffers[1];         /* Variable length array 1�`4 */
}AT_usendparam;

typedef struct _AT_addrtparam {
    AT_NINT32 dstaddr;   /* ����� IP �A�h���X */
    AT_NINT32 gtwayaddr; /* �Q�[�g�E�F�C�A�h���X */
    AT_SINT16 reserve;   /* ���g�p�̈� */
}AT_addrtparam;

typedef struct _AT_delrtparam {
    AT_NINT32 dstaddr;   /* ����� IP �A�h���X */
    AT_NINT32 gtwayaddr; /* �Q�[�g�E�F�C�A�h���X */
}AT_delrtparam;

typedef struct _AT_RouteEntry {
    AT_NINT32 dstaddr;   /* ����� IP �A�h���X */
    AT_NINT32 gtwayaddr; /* �Q�[�g�E�F�C�A�h���X */
    AT_UINT16 reserve1;  /* ���g�p�̈� */
    AT_UINT16 reserve2;  /* ���g�p�̈� */
    AT_UINT16 reserve3;  /* ���g�p�̈� */
} AT_RouteEntry;

typedef struct _AT_lstrtparam {
    AT_SINT16 len;       /* �f�[�^�� */
    AT_RouteEntry *buf;     /* �f�[�^�A�h���X */
}AT_lstrtparam;

#endif /* HEAD_OF_AVETCPAP */
#ifdef __cplusplus
} /* End of extern "C" { */
#endif /* __cplusplus */
