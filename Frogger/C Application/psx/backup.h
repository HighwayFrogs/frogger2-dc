/********************************************************************
 *  Shinobi Library Sample
 *  Copyright (c) 1998 SEGA
 *
 *  Library : Backup Library
 *  Module  : Library Sample Header
 *  File    : backup.h
 *  Date    : 1999-01-14
 *  Version : 1.00
 *
 ********************************************************************/

#ifndef _BACKUP_H_
#define _BACKUP_H_

#include <sg_bup.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DRIVE		0

#define S_NOT_READY 0
#define S_READY     1
#define S_SAVE      2
#define S_LOAD      3
#define S_DELETE    4
#define S_COMPLETE  5

/******** メモリカード情報構造体 *******************************************/
/******** Memory card information structure ********************************/
typedef struct {
	Uint16 Ready;               /* メモリカードが接続されているか          */
	                            /* Is memory card connected ?              */
	Uint16 IsFormat;            /* フォーマットされているかどうか          */
	                            /* Is memory card formatted ?              */
	Uint32 LastError;           /* 最後に発生したエラーコード              */
	                            /* Error code which occured last.          */
	Uint32 ProgressCount;       /* 処理経過カウント                        */
	                            /* Operation progress count.               */
	Uint32 ProgressMax;         /* 処理経過カウント最大                    */
	                            /* Operattion progress count maximum.      */
	Uint32 Operation;           /* 処理中のオペレーションコード            */
	                            /* Operation code in handling now.         */
	BUS_DISKINFO DiskInfo;      /* ドライブ情報                            */
	                            /* Drive information.                      */
} BACKUPINFO;


void BupInit(void);
void BupExit(void);
const BACKUPINFO* BupGetInfo(Sint32 drive);
const char* BupGetErrorString(Sint32 err);
const char* BupGetOperationString(Sint32 op);
Sint32 BupLoad(Sint32 drive, const char* fname, void* buf);
Sint32 BupSave(Sint32 drive, const char* fname, void* buf, Sint32 nblock);
Sint32 BupDelete(Sint32 drive, const char* fname);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BACKUP_H_ */

/******************************* end of file *******************************/
