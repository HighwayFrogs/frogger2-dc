
#include <Windows.h>
#include "resource.h"
#include "../Util.h"
#include "WinUtil.h"
#include "Log.h"

#define MAX_LOG_LENGTH 50 //max no. lines in log window

HWND g_hWndLogWindow;
extern HINSTANCE g_hInstance;
extern HWND g_hWnd;

void Log( char* pszText )
{
    //sanity check
    if( pszText == NULL ) return;

    //disable painting
    SendDlgItemMessage( g_hWndLogWindow, IDC_LOG, WM_SETREDRAW, FALSE, 0 );

    int iSel = -1;
   
    //add all lines of text
    while( *pszText )
    {
        //get a pointer to the next chunk
        char* pszEnd = strchr( pszText, '\n' );
        if( pszEnd )
        {
            //temporarily terminate the string
            *pszEnd = '\0';

            //add it to the list box           
            if( strlen( pszText ) )
                iSel = SendDlgItemMessage( g_hWndLogWindow, IDC_LOG, LB_ADDSTRING, 0, (LPARAM)pszText );

            //replace carriage return and continue
            *pszEnd = '\n';
            pszText = ++pszEnd;
        }
        else
        {
            //no more lines - just add it and exit the while
            if( strlen( pszText ) )
                iSel = SendDlgItemMessage( g_hWndLogWindow, IDC_LOG, LB_ADDSTRING, 0, (LPARAM)pszText );
            break;
        }
    }

    //select last added entry
    SendDlgItemMessage( g_hWndLogWindow, IDC_LOG, LB_SETCURSEL, iSel, 0 );

    //cap the number of lines in the listbox
    int nMax = SendDlgItemMessage( g_hWndLogWindow, IDC_LOG, LB_GETCOUNT, 0, 0 );
    while( nMax-- > MAX_LOG_LENGTH ) SendDlgItemMessage( g_hWndLogWindow, IDC_LOG, LB_DELETESTRING, 0, 0 );

    //enable painting
    SendDlgItemMessage( g_hWndLogWindow, IDC_LOG, WM_SETREDRAW, TRUE, 0 );
    InvalidateRect( g_hWndLogWindow, NULL, TRUE );
}

LRESULT CALLBACK WndProc_LogWindow( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_SIZE:
        {
            RECT rc; GetClientRect( hWnd, &rc );
            MoveWindow( GetDlgItem( hWnd, IDC_LOG), 0, 0, rc.right, rc.bottom, TRUE );
            return 0;
        }

        case WM_CLOSE:
            ShowLogWindow( FALSE );
            return 0;

    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

BOOL InitLogWindow()
{
    g_hWndLogWindow = CreateRegisteredWindow( WndProc_LogWindow, g_hInstance, WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME, WS_EX_TOOLWINDOW, "Log", (HMENU)0, (HCURSOR)IDC_ARROW, (HICON)NULL, g_hWnd, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300 );
    HWND hWndLog = CreateWindowEx( WS_EX_CLIENTEDGE, "LISTBOX", NULL, WS_CHILD|WS_VISIBLE|WS_VSCROLL|LBS_NOINTEGRALHEIGHT, 0, 0, 0, 0, g_hWndLogWindow, (HMENU)IDC_LOG, g_hInstance, NULL );
    SendMessage( hWndLog, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), MAKELPARAM(FALSE,0) );
    return ( g_hWndLogWindow != NULL );
}

void CleanupLogWindow()
{
}

void PositionLogWindow()
{
    RECT rc; GetWindowRect( g_hWndLogWindow, &rc ); ScreenToClient( g_hWndLogWindow, (LPPOINT)&rc.left ); ScreenToClient( g_hWndLogWindow, (LPPOINT)&rc.right );
    RECT rcMain; GetWindowRect( GetDlgItem(g_hWnd,IDC_CLIENTAREA), &rcMain );
    MoveWindow( g_hWndLogWindow, rcMain.right-rc.right, rcMain.bottom-rc.bottom, rc.right, rc.bottom, TRUE );
}

void ShowLogWindow( BOOL bShow /*TRUE*/ )
{
    if( bShow )
    {
        PositionLogWindow();
        ShowWindow( g_hWndLogWindow, SW_SHOW );
        UpdateWindow( g_hWndLogWindow );
    }
    else
    {
        ShowWindow( g_hWndLogWindow, SW_HIDE );
    }
}
