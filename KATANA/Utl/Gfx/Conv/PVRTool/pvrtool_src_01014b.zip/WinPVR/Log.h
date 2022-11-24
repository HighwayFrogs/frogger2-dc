#ifndef _LOG_H_
#define _LOG_H_

extern BOOL InitLogWindow();
extern void Log( char* pszText );
extern void CleanupLogWindow();
extern void ShowLogWindow( BOOL bShow = TRUE );

#endif //_LOG_H_
