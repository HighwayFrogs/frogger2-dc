/*************************************************
 Windows utility functions

   This file contains various misc. library
   functions for manipulating Windows GUI
   components. They can probably be added
   to any WinAPI program with little changes.
  

**************************************************/

#include <windows.h>
#include <commctrl.h>
#include <afxres.h>
#include "resource.h"
#include "WinUtil.h"
#include "../Util.h"

//////////////////////////////////////////////////////////////////////
// Registers and creates a new window with the given parameters
//////////////////////////////////////////////////////////////////////
HWND CreateRegisteredWindow( WNDPROC pfnWndProc, 
                             HINSTANCE hInstance, 
                             DWORD dwStyle, 
                             DWORD dwExStyle            /*0*/, 
                             const char* pszWindowTitle /*NULL*/, 
                             HMENU hMenu                /*NULL*/, 
                             HCURSOR hCursor            /*NULL*/, 
                             HICON hIcon                /*NULL*/, 
                             HWND hWndParent            /*NULL*/, 
                             int x                      /*def*/, 
                             int y                      /*def*/, 
                             int nWidth                 /*def*/, 
                             int nHeight                /*def*/, 
                             HBRUSH hbrBackground       /*NULL*/, 
                             DWORD dwClassStyle         /*CS_DBLCLKS*/, 
                             const char* pszClassName   /*NULL*/, 
                             LPVOID lpParam             /*NULL*/ )
{
    WNDCLASS wc;

    //if the class name isn't specified, create one from the WndProc
    char szClassName[16];
    if( pszClassName == NULL )
    {
        wsprintf( szClassName, "Class_%X", (DWORD)pfnWndProc );
        pszClassName = szClassName;
    }

    //see if the class already exists
    if( GetClassInfo( hInstance, pszClassName, &wc ) == FALSE )
    {
        //try to load the icon from anywhere possible
        HICON                   hIconLoad = LoadIcon( hInstance, MAKEINTRESOURCE((UINT)hIcon ) );
        if( hIconLoad == NULL ) hIconLoad = LoadIcon( NULL,      MAKEINTRESOURCE((UINT)hIcon ) );
        if( hIconLoad == NULL ) hIconLoad = LoadIcon( hInstance, (LPCTSTR)hIcon );
        if( hIconLoad == NULL ) hIconLoad = LoadIcon( NULL,      (LPCTSTR)hIcon );
        if( hIconLoad == NULL ) hIconLoad = hIcon;

        //do the same with the cursor
        HCURSOR                   hCursorLoad = LoadCursor( hInstance, MAKEINTRESOURCE((UINT)hCursor ) );
        if( hCursorLoad == NULL ) hCursorLoad = LoadCursor( NULL,      MAKEINTRESOURCE((UINT)hCursor ) );
        if( hCursorLoad == NULL ) hCursorLoad = LoadCursor( hInstance, (LPCTSTR)hCursor );
        if( hCursorLoad == NULL ) hCursorLoad = LoadCursor( NULL,      (LPCTSTR)hCursor );
        if( hCursorLoad == NULL ) hCursorLoad = hCursor;

        //initialise class info structure
        ZeroMemory( &wc, sizeof(wc) );
        wc.hbrBackground = hbrBackground;
        wc.hCursor = hCursorLoad;
        wc.hIcon = hIconLoad;
        wc.hInstance = hInstance;
        wc.lpfnWndProc = pfnWndProc;
        wc.style = dwClassStyle;
        wc.lpszClassName = pszClassName;

        //register the class
        if( !RegisterClass( &wc ) ) return NULL;
    }

    //create a window
    return CreateWindowEx( dwExStyle, pszClassName, pszWindowTitle, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam );
}


//////////////////////////////////////////////////////////////////////
// Creates a toolbar control from the given toolbar resource
//////////////////////////////////////////////////////////////////////
HWND CreateToolbarFromResource( HWND hWndParent, UINT uIDResource, UINT uID, DWORD dwStyle )
{
    HINSTANCE hInstance = (HINSTANCE)GetWindowLong( hWndParent, GWL_HINSTANCE );

    //define toolbar resource format and initialise the returned window handle
    struct ToolBarData { WORD wVersion, wWidth, wHeight, wItemCount, uID[1]; };
    HWND hWnd = NULL;

    //load in toolbar data
    HRSRC hRsrc = NULL;
    HGLOBAL hGlobal = NULL;
    if( (hRsrc = FindResource( hInstance, MAKEINTRESOURCE(uIDResource), RT_TOOLBAR )) && (hGlobal = LoadResource( hInstance, hRsrc )) )
    {
        //get pointer to toolbar initialisation data
	    ToolBarData* pData = (ToolBarData*)LockResource(hGlobal);
        if( pData != NULL )
        {
            //allocate toolbar button array
	        TBBUTTON* tbButtons = new TBBUTTON[pData->wItemCount];
            memset( tbButtons, 0, sizeof(TBBUTTON) * pData->wItemCount );

            //prepare toolbar button structure
            int iImage = 0;
	        for( int i = 0; i < pData->wItemCount; i++ )
            {
                //default to enabled
                tbButtons[i].fsState = TBSTATE_ENABLED;
                if( (tbButtons[i].idCommand = pData->uID[i] ) == 0 )
                {
                    //set values for a separator
                    tbButtons[i].fsStyle = TBSTYLE_SEP;
                    tbButtons[i].iBitmap = ( dwStyle & TBSTYLE_FLAT ) ? 6 : 8;
                }
                else
                {
                    //set values for a normal button
                    tbButtons[i].fsStyle = TBSTYLE_BUTTON;
                    tbButtons[i].iBitmap = iImage++;
                }
            }

            //create an empty toolbar
            hWnd = CreateToolbarEx( hWndParent, WS_CHILD|dwStyle, uID, pData->wItemCount, hInstance, uIDResource, tbButtons, pData->wItemCount, pData->wWidth, pData->wHeight, pData->wWidth+5, pData->wHeight+5, sizeof(TBBUTTON) );
	        delete[] tbButtons;

            //set initial toolbar values
            SendMessage( hWnd, TB_SETINDENT, ( dwStyle & TBSTYLE_FLAT ) ? 6 : 8, 0 );
        }
    }

    //release resource and return
	UnlockResource(hGlobal);
	FreeResource(hGlobal);
    return hWnd;
}


//////////////////////////////////////////////////////////////////////
// Turns the given toolbar button into a toggle button
//////////////////////////////////////////////////////////////////////
void SetToolbarButtonToggle( UINT uIDButton, HWND hWndToolbar, BOOL bInitialValue )
{
    TBBUTTONINFO tbbi;
    tbbi.cbSize = sizeof(TBBUTTONINFO);
    tbbi.dwMask = TBIF_STYLE|(bInitialValue?TBIF_STATE:0);
    tbbi.fsStyle = TBSTYLE_CHECK;
    tbbi.fsState = TBSTATE_CHECKED|TBSTATE_ENABLED;
    SendMessage( hWndToolbar, TB_SETBUTTONINFO, uIDButton, (LPARAM)&tbbi );
}


//////////////////////////////////////////////////////////////////////
// Opens the version resource and extracts the requested key
//////////////////////////////////////////////////////////////////////
void GetVersionInfoString( char* pszVersionInfoString, const char* pszKey )
{
    //get the filename
    char szFilename[MAX_PATH+1]; 
    GetModuleFileName( NULL, szFilename, MAX_PATH );

    //load the version info into a block
    DWORD dw, dwSize = GetFileVersionInfoSize( szFilename, &dw );
    LPVOID pVoid = GlobalAlloc( GPTR, dwSize );
    GetFileVersionInfo( szFilename, 0, dwSize, pVoid );

    //read the translation value
    WORD* pwLang = NULL;
    UINT uLen;
    if( VerQueryValue( pVoid, "\\VarFileInfo\\Translation", (void**)&pwLang, &uLen ) )
    {
        //build query key
        char szKey[128];
        wsprintf( szKey, "\\StringFileInfo\\%08X\\%s", (pwLang[0] << 16 | pwLang[1]), pszKey );

        //read fileversion and copy it into the buffer
        char* pszFileVersion = "";
        if( VerQueryValue( pVoid, szKey, (void**)&pszFileVersion, &uLen ) ) strcpy( pszVersionInfoString, pszFileVersion );
    }
    GlobalFree( pVoid );
}



//////////////////////////////////////////////////////////////////////
// Subclassed message procedure for simple hyperlink button window
//////////////////////////////////////////////////////////////////////
WNDPROC g_lpfnButtonWndProc = NULL;
LRESULT CALLBACK WndProc_Hyperlink( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_LBUTTONUP:  SetWindowLong( hWnd, GWL_USERDATA, 2 | GetWindowLong( hWnd, GWL_USERDATA ) ); break;
        case WM_SETCURSOR:  SetCursor( LoadCursor((HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE ),MAKEINTRESOURCE(IDC_POINT))); return TRUE;
        case WM_MOUSEMOVE:  SetTimer( hWnd, 0x01, 10, NULL ); return TRUE;
        case WM_ERASEBKGND: return TRUE;
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            RedrawWindow( hWnd, NULL, NULL, RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW );
            break;
        case WM_PAINT:
        {
            //prepare painting
            PAINTSTRUCT ps; BeginPaint( hWnd, &ps );
            LONG lUserData = GetWindowLong( hWnd, GWL_USERDATA);

            //use and underlined font if the mouse is over (bit 0 is set)
            HFONT hFont = NULL, hFontOld = NULL;
            if( lUserData & 1 || GetFocus() == hWnd )
            {
                LOGFONT lf; GetObject( GetStockObject(ANSI_VAR_FONT), sizeof(lf), &lf );
                lf.lfUnderline = TRUE;
                hFont = CreateFontIndirect( &lf );
                hFontOld = (HFONT)SelectObject( ps.hdc, hFont );
            }
            else
            {
                hFontOld = (HFONT)SelectObject( ps.hdc, GetStockObject(ANSI_VAR_FONT) );
            }

            //set the colour - if we've been clicked on (bit 1 set) then use visited colour
            SetTextColor( ps.hdc, (lUserData & 2) ? RGB(128,0,128) : RGB(0,0,255) );
            SetBkMode( ps.hdc, TRANSPARENT );

            //draw the text in the client area
            RECT rc; GetClientRect( hWnd, &rc );
            char szText[MAX_PATH+1]; GetWindowText( hWnd, szText, MAX_PATH );
            FillRect( ps.hdc, &rc, GetSysColorBrush(COLOR_3DFACE));
            DrawText( ps.hdc, szText, -1, &rc, DT_NOPREFIX|DT_LEFT|DT_SINGLELINE|DT_VCENTER );

            //clean up
            SelectObject( ps.hdc, hFontOld );
            if( hFont ) DeleteObject( hFont );
            EndPaint( hWnd, &ps );
            return TRUE;
        }
        case WM_TIMER:
        {
            //see if the mouse is over the window
            POINT pt; GetCursorPos( &pt );  RECT rc; GetWindowRect( hWnd, &rc );
            BOOL bMouseOver = PtInRect( &rc, pt );
            if( !bMouseOver ) KillTimer( hWnd, 0x01 );

            //update the state and repaint if the state has changed
            LONG lUserData = GetWindowLong( hWnd, GWL_USERDATA);
            SetWindowLong( hWnd, GWL_USERDATA, bMouseOver ? (lUserData|1) : (lUserData&~1) );
            if( bMouseOver != ((lUserData & 1) == 1) ) RedrawWindow( hWnd, NULL, NULL, RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW );
            break;
        }
    }
    return CallWindowProc( g_lpfnButtonWndProc, hWnd, uMsg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////
// Makes the given button window a hyperlink
//////////////////////////////////////////////////////////////////////
BOOL MakeButtonIntoHyperlink( HWND hWnd )
{
    //make sure we're being asked to subclass a button
    char szClass[10]; GetClassName( hWnd, szClass, 8 );
    if( stricmp( szClass, "BUTTON" ) != 0 ) return FALSE;

    //change it's style so it's owner drawn
    SetWindowLong( hWnd, GWL_STYLE, BS_OWNERDRAW | GetWindowLong( hWnd, GWL_STYLE ) );

    //get the button message proc
    if( g_lpfnButtonWndProc == NULL ) g_lpfnButtonWndProc = (WNDPROC)GetWindowLong( hWnd, GWL_WNDPROC );

    //subclass it and return
    SetWindowLong( hWnd, GWL_WNDPROC, (LONG)WndProc_Hyperlink );
    return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Centers the given window over another
//////////////////////////////////////////////////////////////////////
void CenterWindow( HWND hWndToCenter, HWND hWndOver )
{
    RECT rc; GetClientRect( hWndToCenter, &rc );
    RECT rcOver; GetWindowRect( hWndOver, &rcOver );
    POINT ptCenter = { rcOver.left + ((rcOver.right-rcOver.left)/2), rcOver.top + ((rcOver.bottom-rcOver.top)/2) };
    SetWindowPos( hWndToCenter, NULL, ptCenter.x - (rc.right/2), ptCenter.y - (rc.bottom/2), 0, 0, SWP_NOSIZE|SWP_NOZORDER );
}


