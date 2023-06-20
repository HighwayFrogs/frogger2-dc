#ifndef _MEMCARD_H_INCLUDE
#define _MEMCARD_H_INCLUDE


// *ASL* 14/08/2000 - Moved these from memcard.c to here
enum {
	SAVEMENU_CHECK,
	SAVEMENU_FULL,
	SAVEMENU_NOCARD,
	SAVEMENU_SAVE,
	SAVEMENU_COMPLETE,
	SAVEMENU_FAIL,
	SAVEMENU_FORMATYN,
	SAVEMENU_FORMAT,
	SAVEMENU_FORMATFAIL,
	SAVEMENU_NEEDFORMAT,
	SAVEMENU_LOAD,
	SAVEMENU_LOADERROR,
	SAVEMENU_LOADCOMPLETE,
	SAVEMENU_SAVEYN,
	SAVEMENU_CHANGED,
	SAVEMENU_OVERWRITE,
	SAVEMENU_FORMATCOMPLETE,
	SAVEMENU_CORRUPT,
};

typedef struct
{
	int saveStage;
	int saveFrame;
	int saveChosen;
	int load;
}SAVE_INFO;

extern SAVE_INFO saveInfo;

int gameSaveGetCardStatus();
void SaveGame(void);
void LoadGame(void);

int CheckVMUs( );

void ShowLCDLogo( );

//int FontInSpaceScaled(int x, int y, char *str, int w, int l, unsigned char r, unsigned char g, unsigned char b, int scale,psFont *fontPtr, int centre);

#endif