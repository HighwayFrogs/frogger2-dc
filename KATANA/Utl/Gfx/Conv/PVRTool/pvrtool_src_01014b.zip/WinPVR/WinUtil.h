#ifndef _WINUTIL_H_
#define _WINUTIL_H_

HWND CreateRegisteredWindow( WNDPROC pfnWndProc, 
                             HINSTANCE hInstance,
                             DWORD dwStyle, 
                             DWORD dwExStyle            = 0, 
                             const char* pszWindowTitle = NULL, 
                             HMENU hMenu                = NULL, 
                             HCURSOR hCursor            = NULL, 
                             HICON hIcon                = NULL, 
                             HWND hWndParent            = NULL, 
                             int x                      = CW_USEDEFAULT, 
                             int y                      = CW_USEDEFAULT, 
                             int nWidth                 = CW_USEDEFAULT, 
                             int nHeight                = CW_USEDEFAULT, 
                             HBRUSH hbrBackground       = NULL, 
                             DWORD dwClassStyle         = CS_DBLCLKS, 
                             const char* pszClassName   = NULL, 
                             LPVOID lpParam             = NULL );

HWND CreateToolbarFromResource( HWND hWndParent, UINT uIDResource, UINT uID, DWORD dwStyle );
void SetToolbarButtonToggle( UINT uIDButton, HWND hWndToolbar, BOOL bInitialValue );
void GetVersionInfoString( char* pszVersionInfoString, const char* pszKey );
BOOL MakeButtonIntoHyperlink( HWND hWnd );
void CenterWindow( HWND hWndToCenter, HWND hWndOver );

#define MAX_MRU 8

#endif //_WINUTIL_H_
