
/*	Definition of Kamui Wait V-Sync Mode		*/
#define		USR_KAMUI_NOWAITVSYNC			//	Not wait V-Sync

/***
*			Include Files
***/

#include	<shinobi.h>
#include	<stdarg.h>
#include	<string.h>

#include	"usr.h"
#include	"DCK_System.h"

#define		_STRICT_UNION_		/* for kamui2.h */
#include	"kamui2.h"			/* for Kamui2 Call */
#include	"sg_chain.h"			/* for Kamui2 Call */
//#include	"dispstr.h"


/* Audio driver to use, select one */
//#define USE_MANATEE		// Uncomment to use the Manatee2 driver
#define USE_AUDIO64AC		// Uncomment to use the Audio64 driver, AC layer
//#define USE_AUDIO64AM		// Uncomment to use the Audio64 driver, AM layer

//#define USE_MIDIDA		// Uncomment to use the MidiDA driver (default is Audio64.drv)

/***
*			Video Functions
***/

///////////////////  Kamui2  ///////////////////

#define	USR_INT_PRI			(0x90)
#define	VERTEXBUFFERSIZE	(0x100000*2)	/* 2MB	*/

SYCHAIN				usrg_chid = (SYCHAIN)NULL;

KMINT32				UsrRenderId[2];

/*	Set V-sync interrupt callback function	*/
void UsrSetVsyncFunc(void (*func)(void))
{
	if (usrg_chid != (SYCHAIN)NULL) {
		syChainDeleteHandler(usrg_chid);
	}
	usrg_chid = syChainAddHandler(
		(SY_CHAIN_EVENT) SYD_CHAIN_EVENT_IML6_VBLANK,
		(void *)func, (Uint32)USR_INT_PRI, (void *)NULL);
}

/*	Print string for debug	*/
Sint32 UsrPrintf(Sint32 lx, Sint32 ly, char *fmt,...)
{
    va_list args;
    static char buf[256];
    Sint32 ret=0, pos;

	memset(buf, 0, sizeof(buf));
    va_start(args, fmt);
    ret = vsprintf(buf, fmt, args);
    va_end(args);
//ma	txt_SetCursor(lx, ly+1);
//ma	txt_DispString(buf);

    return ret;
}

/*	Start Drawing	*/
void UsrBeginDraw(void)
{
	kmBeginScene(&kmSystemConfig);			/* シーンの開始 */
	kmBeginPass(&vertexBufferDesc);			/* パスの開始 */
	pdExecPeripheralServer();
//ma	txt_ClearScreen();
}

/*	End Drawing	*/
void UsrEndDraw(void)
{
//ma	txt_DispScreen(&vertexBufferDesc);
	kmEndPass(&vertexBufferDesc);			/* パスの終了 */
	UsrRenderId[1] = UsrRenderId[0];
	UsrRenderId[0] = 
		kmRender(KM_RENDER_FLIP);			/* レンダリング */
	kmEndScene(&kmSystemConfig);				/* シーンの終了 */
}

/*	Wait V-Blank	*/
void UsrWaitVBlank(void)
{
#ifdef USR_KAMUI_NOWAITVSYNC
	KMSTATUS	stat;

	for (;;) {
		stat = kmGetRenderStatus(UsrRenderId[1]);
		if (stat == KMSTATUS_UNDER_RENDER) {
			kmWaitVBlank();
		} else {
			return;
		}
	}
#endif
}

/* Code for Audio64 AC layer.  See README.TXT for details */
#ifdef USE_AUDIO64AC

///////////////////  Audio64 (AC level)  ///////////////////

#include <ac.h>
#include <a64thunk.h>

#include "bpacsetup.h"

/*	Initialize sound library and load sound driver	*/
void UsrInitSound(void)
{
#define	MW_MAX_CH		(8)
#define	MW_SNDRAM_SIZE	(0x4040*MW_MAX_CH)

	KTU32		port[8], i;
	volatile KTU32	*sndram=KTNULL;
#ifdef USE_MIDIDA
	if ( bpAcSetup(AC_DRIVER_MIDI, KTFALSE, KTNULL)==KTFALSE )
#else
	if ( bpAcSetup(AC_DRIVER_DA, KTFALSE, KTNULL)==KTFALSE )
#endif
		for (;;);
	// The Middleware libraries require ports and memory in order to operate.
	// You can use any contiguous block of AICA (sound) RAM and any ports you want.
	// The memory (of size MW_SNDRAM_SIZE) and the ports are returned to you when the
	// Middleware library is properly shut down.
	// This example uses the first free available memory slot and the first eight ports.
	sndram = acSystemGetFirstFreeSoundMemory();
	for (i = 0; i < MW_MAX_CH; i++)
		port[i] = i;	// Define the port numbers
	mwSetupAudio64(sndram, MW_SNDRAM_SIZE, port, MW_MAX_CH);	// Pass the information to Middleware
	acSetTransferMode(AC_TRANSFER_DMA);
}

#endif

/* Code for Audio64 AM layer.  See README.TXT for details */
#ifdef USE_AUDIO64AM

///////////////////  Audio64 (AM level)  ///////////////////

#include <ac.h>
#include <a64thunk.h>
#include <am.h>

#include "bpamsetup.h"

#define	MW_MAX_CH		(8)
#define	MW_SNDRAM_SIZE	(0x4040*MW_MAX_CH)

/*	Initialize sound library and load sound driver	*/
void UsrInitSound(void)
{
	KTBOOL ret;
	AM_SOUND	amsnd[8];
	KTU32		port[8], i;

	volatile KTU32	*sndram=KTNULL;

#ifdef USE_MIDIDA
	if ( bpAmSetup(AC_DRIVER_MIDI, KTFALSE, KTNULL)==KTFALSE )
#else
	if ( bpAmSetup(AC_DRIVER_DA, KTFALSE, KTNULL)==KTFALSE )
#endif
		for (;;);
	// As above in the AC layer, the Middleware library requires a block of sound RAM and
	// ports.  However, some AM level commands are used to accomplish this.
	for (i=0; i<MW_MAX_CH; i++) {
		ret = amSoundAllocateVoiceChannel(&amsnd[i]);
		if (ret == KTFALSE) {
			for (;;);
		}
		port[i] = amsnd[i].voiceChannel;
	}
	ret = amHeapAlloc(&sndram, MW_SNDRAM_SIZE, 32, AM_FIXED_MEMORY, NULL);
	if (ret == KTFALSE) {
		for (;;);
	}
	mwSetupAudio64(sndram, MW_SNDRAM_SIZE, port, MW_MAX_CH);
	acSetTransferMode(AC_TRANSFER_DMA);
}

#endif

/***
*			Utility functions
***/

/*	Check Door Status	*/
void UsrCheckDoorOpen(void){
	Sint32 dstat;

	/*	ドアオープンチェック	*/
	/*	Check door open			*/
	dstat = gdFsGetDrvStat();
	if (dstat == GDD_DRVSTAT_OPEN || dstat == GDD_DRVSTAT_BUSY) {
		gdFsReqDrvStat();
	}
}

/*	Load data to memory	*/
void *UsrLoadFile(char *fname, Sint32 *len)
{
	GDFS gdfs;
	Sint32 fsize, nsct;
	void *dat=NULL;

	if ( (gdfs=gdFsOpen(fname, NULL)) == NULL ) {
		for (;;) {
			UsrBeginDraw();
			UsrPrintf(3, 5, "can't open %s", fname);
			UsrEndDraw();
		}
	}
	gdFsGetFileSize(gdfs, &fsize);
	nsct = (fsize+2047)/2048;
	if ( (dat=syMalloc(nsct*2048)) != NULL ) {
		gdFsRead(gdfs, nsct, dat);
	}
	gdFsClose(gdfs);
	if ( len != NULL )
		*len = fsize;

	return dat;
}

/*	Convert from sampling frequency and counter to time(hour,min,sec,frame)	*/
void UsrSfcnt2time(Sint32 sf, Sint32 ncnt, Sint32 *hh, Sint32 *mm, Sint32 *ss, Sint32 *ff)
{
	register Sint32 nf = (Sint32)((Float)ncnt/(Float)sf*100.0);

	*hh = nf/(60*60*100);
	*mm = nf/(60*100) - *hh*60;
	*ss = nf/100 - *hh*60*60 - *mm*60;
	*ff = nf%100;
}
