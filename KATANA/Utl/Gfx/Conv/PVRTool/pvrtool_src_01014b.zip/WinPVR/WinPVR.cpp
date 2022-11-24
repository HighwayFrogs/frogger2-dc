/*************************************************
 Windows version of the PVR tool


**************************************************/
#include <Windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <afxres.h>
#include "../VQCompressor.h"
#include "../Image.h"
#include "../Twiddle.h"
#include "../Util.h"
#include "../PVR.h"
#include "WinUtil.h"
#include "VQAnalysis.h"
#include "Browse.h"
#include "Log.h"
#include "VQFDialog.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////

//window caption
const char* szWindowTitle = "PVR Tool";

//handles
HINSTANCE g_hInstance = NULL;
HWND g_hWnd = NULL;
extern HWND g_hWndBrowser;

//images
CImage* g_pImage = NULL;
CVQImage* g_pVQImage = NULL;
CImage* g_pRefImage = NULL;

//VQ compressor object
CVQCompressor g_VQCompressor;

//display variables
int g_nZoom = 1;
POINT g_ptOrigin = { 0, 0 };
POINT g_ptTemp = { 0, 0 };
BOOL g_bStretchMipmaps = TRUE;
BOOL g_bMipmapImage = TRUE;
BOOL g_bMipmapVQImage = TRUE;
BOOL g_bMipmapRefImage = TRUE;

//full-screen variables
BOOL g_bMirrorFullScreen = FALSE;
HWND g_hWndFullScreen = NULL;
HBITMAP g_hBmFullScreen = NULL;

//misc. PVR stuff
unsigned long int g_nGlobalIndex = 0;
bool g_bEnableGlobalIndex = true;

//////////////////////////////////////////////////////////////////////
// Zoom level calculation
//////////////////////////////////////////////////////////////////////
float CalcZoom()
{
    float a = 1.0f, b = 1.0f;
    if( g_nZoom < 0.0f ) b = float(-g_nZoom); else a = float(g_nZoom);
    return a/b;
}


//////////////////////////////////////////////////////////////////////
// Displays the current zoom level on the status bar
//////////////////////////////////////////////////////////////////////
void UpdateZoomMessage()
{
    char szText[64];
    wsprintf( szText, "Zoom: %d%%", int(CalcZoom() * 100.0f) );
    SendDlgItemMessage( g_hWnd, IDC_STATUSBAR, SB_SETTEXT, 1, (LPARAM)szText );
}


//////////////////////////////////////////////////////////////////////
// Updates the global index on the status bar
//////////////////////////////////////////////////////////////////////
void UpdateGlobalIndex()
{
    if( g_bEnableGlobalIndex )
    {
        char szText[64];
        wsprintf( szText, "GBIX: %lu", g_nGlobalIndex );
        SendDlgItemMessage( g_hWnd, IDC_STATUSBAR, SB_SETTEXT, 2, (LPARAM)szText );
        if( g_nGlobalIndex > MAX_GBIX ) DisplayStatusMessage( "Warning: Global index > 0x%X - this may cause problems if you're using Ninja", MAX_GBIX );
    }
    else
    {
        SendDlgItemMessage( g_hWnd, IDC_STATUSBAR, SB_SETTEXT, 2, (LPARAM)"GBIX Disabled" );
    }
}


//////////////////////////////////////////////////////////////////////
// Updates the ranges of the mip map level selection slider
//////////////////////////////////////////////////////////////////////
void UpdateMipmapSlider()
{
    int nMipMaps = 0;
    if( g_pVQImage ) nMipMaps = g_pVQImage->GetNumMipMaps();
    if( g_pImage && g_pImage->GetNumMipMaps() > nMipMaps ) nMipMaps = g_pImage->GetNumMipMaps();

    HWND hWndSlider = GetDlgItem( GetDlgItem(g_hWnd,IDC_TOOLBAR), IDC_MIPMAPSLIDER );

    if( nMipMaps == 0 )
        EnableWindow( hWndSlider, FALSE );
    else
    {
        EnableWindow( hWndSlider, TRUE );
        SendMessage( hWndSlider, TBM_SETRANGE, TRUE, MAKELONG(0,nMipMaps-1) );
        SendMessage( hWndSlider, TBM_SETPOS, TRUE, 0 );
        if( g_pVQImage ) g_pVQImage->SetMipMapLevel( 0 );
        if( g_pImage ) g_pImage->SetMipMapLevel( 0 );
        if( g_pRefImage ) g_pRefImage->SetMipMapLevel( 0 );
    }

}

//////////////////////////////////////////////////////////////////////
// Updates the ranges of the scroll bars
//////////////////////////////////////////////////////////////////////
void UpdateScrollbars()
{
    RECT rc;
    GetClientRect( GetDlgItem(g_hWnd,IDC_CLIENTAREA), &rc );

    int nDocWidth = 10, nDocHeight = 0, i = 0;
    float fZoom = CalcZoom();
    if( g_pImage )      { g_pImage->SetDrawScaling(fZoom);   i++; nDocWidth += (g_pImage->GetScaledWidth()+10); nDocHeight = max(nDocHeight,(g_pImage->GetScaledHeight()+10)*2); };
    if( g_pVQImage )    { g_pVQImage->SetDrawScaling(fZoom); i++; nDocWidth += (g_pVQImage->GetScaledWidth()+10); nDocHeight = max(nDocHeight,(g_pVQImage->GetScaledHeight()+10)*2); };
    if( g_pRefImage )   { g_pRefImage->SetDrawScaling(fZoom);i++; nDocWidth += (g_pRefImage->GetScaledWidth()+10); nDocHeight = max(nDocHeight,(g_pRefImage->GetScaledHeight()+10)*2); };
    nDocHeight += 10;

    int nScaleWidth = i ? (nDocWidth / i) : 0;
    int nScaleHeight = nDocHeight / 2;

    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE|SIF_PAGE|SIF_POS;
    si.nPos = -g_ptOrigin.x;
    si.nMin = 0;
    si.nMax = nDocWidth-rc.right + nScaleWidth;
    si.nPage = nScaleWidth;
    SetScrollInfo( GetDlgItem(g_hWnd,IDC_CLIENTAREA), SB_HORZ, &si, TRUE );

    si.nPos = -g_ptOrigin.y;
    si.nMax = nDocHeight-rc.bottom + nScaleHeight;
    si.nPage = nScaleHeight;
    SetScrollInfo( GetDlgItem(g_hWnd,IDC_CLIENTAREA), SB_VERT, &si, TRUE );

    g_ptOrigin.x = -GetScrollPos( GetDlgItem(g_hWnd,IDC_CLIENTAREA), SB_HORZ );
    g_ptOrigin.y = -GetScrollPos( GetDlgItem(g_hWnd,IDC_CLIENTAREA), SB_VERT );
}

//////////////////////////////////////////////////////////////////////
// Moves the given bitmap to the clipboard (the hBitmap belongs to the system afterwards)
//////////////////////////////////////////////////////////////////////
void Copy( HBITMAP hBitmap )
{
    //put it on the clipboard
    if( !OpenClipboard( g_hWnd ) ) return;
    EmptyClipboard();
    SetClipboardData( CF_BITMAP, hBitmap );
    CloseClipboard();
}

//////////////////////////////////////////////////////////////////////
// Locates the popup menu with the given command identifier on the given menu
//////////////////////////////////////////////////////////////////////
HMENU FindOwnerMenu( HMENU hMenu, UINT uID )
{
    int iMax = GetMenuItemCount(hMenu);
    for( int i = 0; i < iMax; i++ )
    {
        HMENU hSubMenu = GetSubMenu( hMenu, i );
        if( hSubMenu )
        {
            HMENU hFound = FindOwnerMenu( hSubMenu, uID );
            if( hFound ) return hFound;
        }
        else
        {
            if( GetMenuItemID( hMenu, i ) == uID ) return hMenu;
        }
    }

    return NULL;
}

//////////////////////////////////////////////////////////////////////
// Displays the drop-down box for the given toolbar button
//////////////////////////////////////////////////////////////////////
void ShowTBDropdown( UINT uIDButton, HMENU hMenu )
{
    if( hMenu == NULL ) return;

    //get the location of the copy button on the toolbar
    int iPos = SendDlgItemMessage( g_hWnd, IDC_TOOLBAR, TB_COMMANDTOINDEX, uIDButton, 0 );
    RECT rc; SendDlgItemMessage( g_hWnd, IDC_TOOLBAR, TB_GETITEMRECT, iPos, (LPARAM)&rc );

    //define where we want the menu to appear
    POINT pt = { rc.left, rc.bottom };
    ClientToScreen( GetDlgItem(g_hWnd, IDC_TOOLBAR), &pt );

    //display the menu
    TrackPopupMenu( hMenu, TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON, pt.x, pt.y, 0, g_hWnd, NULL );
}

//////////////////////////////////////////////////////////////////////
// Displays the available clipbard-copy options
//////////////////////////////////////////////////////////////////////
void ShowCopyOptionMenu()
{
    ShowTBDropdown( ID_EDIT_COPY, FindOwnerMenu( GetMenu(g_hWnd), ID_EDIT_COPY_IMAGE ) );
}

//////////////////////////////////////////////////////////////////////
// Displays the available view-tiled options
//////////////////////////////////////////////////////////////////////
void ShowViewTilesOptionMenu()
{
    ShowTBDropdown( ID_VIEW_TILES, FindOwnerMenu( GetMenu(g_hWnd), ID_VIEW_TILES_IMAGE ) );
}

//////////////////////////////////////////////////////////////////////
// Displays the available mipmap display options
//////////////////////////////////////////////////////////////////////
void ShowMipMapOptionMenu()
{
    ShowTBDropdown( ID_VIEW_MIPMAP, FindOwnerMenu( GetMenu(g_hWnd), ID_VIEW_MIPMAP_IMAGE ) );
}

//////////////////////////////////////////////////////////////////////
// Displays the available VQ analysis display options
//////////////////////////////////////////////////////////////////////
void ShowVQAnalysisOptionMenu()
{
    ShowTBDropdown( ID_VQ_COMPRESSIONANALYSIS, FindOwnerMenu( GetMenu(g_hWnd), ID_VQ_COMPRESSIONANALYSIS_DIFFERENCE ) );
}



//////////////////////////////////////////////////////////////////////
// Loads the given file
//////////////////////////////////////////////////////////////////////
void LoadFile( const char* pszFilename )
{
    CImage* pImage = new CImage();
    if( pImage->Load( pszFilename ) )
    {
        delete g_pImage;
        if( g_bEnableGlobalIndex ) g_nGlobalIndex++; 
        UpdateGlobalIndex();
        g_pImage = pImage;
        delete g_pVQImage; g_pVQImage = NULL;
        delete g_pRefImage; g_pRefImage = NULL;
        SendMessage( g_hWnd, WM_COMMAND, MAKEWPARAM(ID_VIEW_ZOOMNORMAL,0), 0 );
        UpdateScrollbars();
        RedrawWindow( GetDlgItem(g_hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
    }
    else
    {
        delete pImage;
        ShowErrorMessage( "Could not open \"%s\"", GetFileNameNoPath(pszFilename) );
    }

    UpdateMipmapSlider();
}




//////////////////////////////////////////////////////////////////////
// Message processing function for the about box
//////////////////////////////////////////////////////////////////////
BOOL CALLBACK DlgProc_About( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
        { 
            //center the window over it's parent
            CenterWindow( hDlg, GetParent(hDlg) );

            //rip some info out of the VS_VERSION_INFO resource
            char szBuffer[MAX_PATH+1]; 
            GetVersionInfoString( szBuffer, "FileVersion" );     
#ifdef _DEBUG
            strcat( szBuffer, " (Debug build)" );
#else
            strcat( szBuffer, " (Release build)" );
#endif
            SetDlgItemText( hDlg, IDC_VERSION, szBuffer ); 


            GetVersionInfoString( szBuffer, "LegalCopyright" );  SetDlgItemText( hDlg, IDC_COPYRIGHT, szBuffer ); 

            //get the version number from the VQ dll
            VqGetVersionInfoString( szBuffer, "FileVersion" );   SetDlgItemText( hDlg, IDC_VQVERSION, szBuffer ); 

            //convert the e-mail button into a hyperlink
            MakeButtonIntoHyperlink( GetDlgItem(hDlg,IDC_MAIL) );
            MakeButtonIntoHyperlink( GetDlgItem(hDlg,IDC_PAINTLIBURL) );
            return TRUE;
        }

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDOK: EndDialog( hDlg, 0 ); return TRUE;
                case IDC_MAIL: ShellExecute( hDlg, NULL, "mailto:edts@soe.sega.co.uk", NULL, NULL, SW_SHOW ); return TRUE;
                case IDC_PAINTLIBURL: ShellExecute( hDlg, NULL, "http://www.paintlib.de", NULL, NULL, SW_SHOW ); return TRUE;
            }
            break;

        case WM_CLOSE:
            EndDialog( hDlg, 0 ); return TRUE;
            break;
    }

    return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Message processing function for the global index dialog
//////////////////////////////////////////////////////////////////////
BOOL CALLBACK DlgProc_GlobalIndex( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
            CenterWindow( hDlg, GetParent(hDlg) );
            SetDlgItemInt( hDlg, IDC_GLOBALINDEX, g_nGlobalIndex, FALSE );
            CheckDlgButton( hDlg, IDC_DISABLEGLOBALINDEX, g_bEnableGlobalIndex ? BST_UNCHECKED : BST_CHECKED );
            break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDOK:
                    g_bEnableGlobalIndex = (BST_UNCHECKED == IsDlgButtonChecked( hDlg, IDC_DISABLEGLOBALINDEX ));
                    g_nGlobalIndex = GetDlgItemInt( hDlg, IDC_GLOBALINDEX, NULL, FALSE );

                    UpdateGlobalIndex();
                    //drop through...

                case IDCANCEL:
                    EndDialog( hDlg, LOWORD(wParam) );
                    break;
            }
            break;

        case WM_CLOSE:
            EndDialog( hDlg, IDCANCEL );
            break;

        default:
            return FALSE;
    }

    return TRUE;

}




//////////////////////////////////////////////////////////////////////
// Message processing function for the main window
//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc_MainWnd( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_CREATE:
            DragAcceptFiles( hWnd, TRUE );
            break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case ID_FILE_NEW:
                    g_pImage->CreateDefault();
                    if( g_bEnableGlobalIndex ) g_nGlobalIndex++; 
                    UpdateGlobalIndex();
                    delete g_pVQImage; g_pVQImage = NULL;
                    delete g_pRefImage; g_pRefImage = NULL;
                    UpdateMipmapSlider();
                    UpdateScrollbars();
                    RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    break;

                case ID_FILE_OPEN:
                {
                    CImage* pImage = new CImage();
                    if( pImage->PromptAndLoad() )
                    {
                        delete g_pImage;
                        if( g_bEnableGlobalIndex ) g_nGlobalIndex++; 
                        UpdateGlobalIndex();
                        g_pImage = pImage;
                        delete g_pVQImage; g_pVQImage = NULL;
                        delete g_pRefImage; g_pRefImage = NULL;
                        SendMessage( hWnd, WM_COMMAND, MAKEWPARAM(ID_VIEW_ZOOMNORMAL,0), 0 );
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    }
                    else
                        delete pImage;

                    UpdateMipmapSlider();
                    break;
                }

                case ID_FILE_IMPORTALPHA:
                {
                    if( g_pImage )
                        if( g_pImage->PromptAndLoad( TRUE ) )
                            RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    break;

                }

                case ID_FILE_SAVE_AS:
                    if( g_pImage ) g_pImage->PromptAndSave();
                    break;

                case ID_FILE_EXPORT:
                    if( g_pVQImage ) g_pVQImage->ExportFile();
                    break;

                case ID_FILE_BROWSE:
                    Browse();
                    break;

                case ID_VIEW_LOG:
                    ShowLogWindow();
                    break;

                case ID_VIEW_ZOOMIN:
                {
                    NMHDR nmh;
                    nmh.hwndFrom = GetDlgItem(GetDlgItem( hWnd, IDC_TOOLBAR), IDC_ZOOMSLIDER);
                    nmh.idFrom = IDC_ZOOMSLIDER;
                    SendDlgItemMessage( GetDlgItem( hWnd, IDC_TOOLBAR), IDC_ZOOMSLIDER, TBM_SETPOS, TRUE, SendDlgItemMessage( GetDlgItem( hWnd, IDC_TOOLBAR), IDC_ZOOMSLIDER, TBM_GETPOS, 0, 0 ) + 1 );
                    SendMessage( hWnd, WM_NOTIFY, IDC_ZOOMSLIDER, (LPARAM)&nmh );
                    break;
                }

                case ID_VIEW_ZOOMOUT:
                {
                    NMHDR nmh;
                    nmh.hwndFrom = GetDlgItem(GetDlgItem( hWnd, IDC_TOOLBAR), IDC_ZOOMSLIDER);
                    nmh.idFrom = IDC_ZOOMSLIDER;
                    SendDlgItemMessage( GetDlgItem( hWnd, IDC_TOOLBAR), IDC_ZOOMSLIDER, TBM_SETPOS, TRUE, SendDlgItemMessage( GetDlgItem( hWnd, IDC_TOOLBAR), IDC_ZOOMSLIDER, TBM_GETPOS, 0, 0 ) - 1 );
                    SendMessage( hWnd, WM_NOTIFY, IDC_ZOOMSLIDER, (LPARAM)&nmh );
                    break;
                }

                case ID_VIEW_ZOOMNORMAL:
                {
                    NMHDR nmh;
                    nmh.hwndFrom = GetDlgItem(GetDlgItem( hWnd, IDC_TOOLBAR), IDC_ZOOMSLIDER);
                    nmh.idFrom = IDC_ZOOMSLIDER;
                    SendDlgItemMessage( GetDlgItem( hWnd, IDC_TOOLBAR), IDC_ZOOMSLIDER, TBM_SETPOS, TRUE, 11 );
                    SendMessage( hWnd, WM_NOTIFY, IDC_ZOOMSLIDER, (LPARAM)&nmh );
                    break;
                }

                case ID_VIEW_TILES_IMAGE:
                    if( g_pImage )
                    {
                        g_hBmFullScreen = g_pImage->GetImageBitmap();
                        ShowWindow( g_hWndFullScreen, SW_SHOW );
                        UpdateWindow( g_hWndFullScreen );
                    }
                    break;

                case ID_VIEW_TILES_VQ:
                    if( g_pVQImage )
                    {
                        g_hBmFullScreen = g_pVQImage->GetImageBitmap();
                        ShowWindow( g_hWndFullScreen, SW_SHOW );
                        UpdateWindow( g_hWndFullScreen );
                    }
                    break;

                case ID_VIEW_TILES_MIRROR:
                    g_bMirrorFullScreen = !g_bMirrorFullScreen;
                    break;

                case ID_TOOLS_GENERATEVQF:
                    if( g_pImage ) 
                    {
                        //display VQ settings dialog
                        if( DialogBox( g_hInstance, MAKEINTRESOURCE(IDD_VQFSETTINGS), hWnd, DlgProc_VQFDialog ) == FALSE ) break;

                        //generate the VQ file
                        delete g_pVQImage; g_pVQImage = NULL;
                        delete g_pRefImage; g_pRefImage = NULL;
                        DisplayStatusMessage( "VQ compressing image..." );
                        g_pVQImage = g_VQCompressor.GenerateVQ( g_pImage );
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    }
                    UpdateMipmapSlider();
                    break;

                case ID_TOOLS_DELETEVQ:
                    delete g_pVQImage; g_pVQImage = NULL;
                    delete g_pRefImage; g_pRefImage = NULL; 
                    UpdateScrollbars();
                    RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    UpdateMipmapSlider();
                    break;

                case ID_TOOLS_DELETEALPHA:
                    if( g_pImage ) g_pImage->DeleteAlpha();
                    UpdateScrollbars();
                    RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    break;

                case ID_TOOLS_REVERSEALPHA:
                    g_nOpaqueAlpha = ~g_nOpaqueAlpha;
                    break;

                case ID_TOOLS_SETGLOBALINDEX:
                    DialogBox( g_hInstance, MAKEINTRESOURCE(IDD_SETGLOBALINDEX), hWnd, DlgProc_GlobalIndex );
                    break;

                case ID_TOOLS_REGENERATEMIPMAPS:
                    if( g_pImage )
                    {
                        g_pImage->GenerateMipMaps();
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                        UpdateMipmapSlider();
                    }
                    break;

                case ID_TOOLS_DELETEMIPMAPS:
                    if( g_pImage )
                    {
                        g_pImage->DeleteMipMaps();
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                        UpdateMipmapSlider();
                    }
                    break;

                case ID_TOOLS_PAGETOMIPMAP:
                    if( g_pImage )
                    {
                        g_pImage->PageToMipmaps();
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                        UpdateMipmapSlider();
                    }
                    break;

                case ID_TOOLS_MIPMAPSTOPAGE:
                    if( g_pImage )
                    {
                        g_pImage->MipmapsToPage();
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                        UpdateMipmapSlider();
                    }
                    break;

                case ID_TOOLS_ENLARGETOPOW2:
                    if( g_pImage )
                    {
                        g_pImage->EnlargeToPow2();
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                        UpdateMipmapSlider();
                    }
                    break;

                case ID_TOOLS_VFLIP:
                    if( g_pImage )
                    {
                        g_pImage->Flip( FALSE, TRUE );
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                        UpdateMipmapSlider();
                    }
                    break;

                case ID_TOOLS_HFLIP:
                    if( g_pImage )
                    {
                        g_pImage->Flip( TRUE, FALSE );
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                        UpdateMipmapSlider();
                    }
                    break;

                case ID_TOOLS_SCALEHALF:
                    if( g_pImage )
                    {
                        g_pImage->ScaleHalfSize();
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                        UpdateMipmapSlider();
                    }
                    break;

                case ID_TOOLS_MAKESQUARE:
                    if( g_pImage )
                    {
                        g_pImage->MakeSquare();
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                        UpdateMipmapSlider();
                    }
                    break;


                case ID_VIEW_MIPMAP_IMAGE:
                    g_bMipmapImage = !g_bMipmapImage;
                    if( g_pImage )
                    {
                        if( g_bMipmapImage )
                            g_pImage->SetMipMapLevel( SendDlgItemMessage( GetDlgItem(hWnd,IDC_TOOLBAR), IDC_MIPMAPSLIDER, TBM_GETPOS, 0, 0 ));
                        else
                            g_pImage->SetMipMapLevel(0);
                    }
                    UpdateScrollbars();
                    RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    break;

                case ID_VIEW_MIPMAP_VQIMAGE: 
                    g_bMipmapVQImage = !g_bMipmapVQImage;
                    if( g_pVQImage )
                    {
                        if( g_bMipmapVQImage )
                            g_pVQImage->SetMipMapLevel( SendDlgItemMessage( GetDlgItem(hWnd,IDC_TOOLBAR), IDC_MIPMAPSLIDER, TBM_GETPOS, 0, 0 ));
                        else
                            g_pVQImage->SetMipMapLevel(0);
                    }
                    UpdateScrollbars();
                    RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    break;

                case ID_VIEW_MIPMAP_REFIMAGE:
                    g_bMipmapRefImage = !g_bMipmapRefImage;
                    if( g_pRefImage )
                    {
                        if( g_bMipmapRefImage )
                            g_pRefImage->SetMipMapLevel( SendDlgItemMessage( GetDlgItem(hWnd,IDC_TOOLBAR), IDC_MIPMAPSLIDER, TBM_GETPOS, 0, 0 ));
                        else
                            g_pRefImage->SetMipMapLevel(0);
                    }
                    UpdateScrollbars();
                    RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    break;

                case ID_VIEW_STRETCHMIPMAPS:
                    g_bStretchMipmaps = !g_bStretchMipmaps;
                    UpdateScrollbars();
                    RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    break;

                case ID_VQ_COMPRESSIONANALYSIS_DIFFERENCE:
                    delete g_pRefImage; g_pRefImage = NULL;
                    if( g_pVQImage ) g_pRefImage = GenerateDifferenceImage( g_pImage, g_pVQImage, 1 );
                    UpdateScrollbars();
                    RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    break;

                case ID_VQ_COMPRESSIONANALYSIS_DIFFERENCEX4:
                    delete g_pRefImage; g_pRefImage = NULL;
                    if( g_pVQImage ) g_pRefImage = GenerateDifferenceImage( g_pImage, g_pVQImage, 4 );
                    UpdateScrollbars();
                    RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    break;

                case ID_VQ_COMPRESSIONANALYSIS_CODEBOOKINDICES:
                    delete g_pRefImage; g_pRefImage = NULL;
                    if( g_pVQImage ) g_pRefImage = GenerateCodebookIndicesImage( g_pVQImage );
                    UpdateScrollbars();
                    RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    break;

                case ID_VQ_COMPRESSIONANALYSIS_DELETEREFERENCEIMAGE:
                    delete g_pRefImage; g_pRefImage = NULL;
                    UpdateScrollbars();
                    RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    break;

                case ID_EDIT_COPY:
                    ShowCopyOptionMenu();
                    break;

                case ID_VIEW_TILES:
                    ShowViewTilesOptionMenu();
                    break;

                case ID_VIEW_MIPMAP:
                    ShowMipMapOptionMenu();
                    break;

                case ID_VQ_COMPRESSIONANALYSIS:
                    ShowVQAnalysisOptionMenu();
                    break;

                case ID_EDIT_COPY_IMAGE:
                    if( g_pImage ) Copy( g_pImage->GetImageBitmap() );
                    break;

                case ID_EDIT_COPY_ALPHA:
                    if( g_pImage ) Copy( g_pImage->GetAlphaBitmap() );
                    break;

                case ID_EDIT_COPY_VQFIMAGE:
                    if( g_pVQImage ) Copy( g_pVQImage->GetImageBitmap() );
                    break;

                case ID_EDIT_COPY_VQFALPHA:
                    if( g_pVQImage ) Copy( g_pVQImage->GetAlphaBitmap() );
                    break;

                case ID_EDIT_COPY_REFERENCEIMAGE:
                    if( g_pRefImage ) Copy( g_pRefImage->GetImageBitmap() );
                    break;

                case ID_EDIT_COPY_REFERENCEALPHA:
                    if( g_pRefImage ) Copy( g_pRefImage->GetAlphaBitmap() );
                    break;

                case ID_FILE_PROPERTIES:
                    if( g_pImage )
                    {
                        char szBuffer[8192];
                        wsprintf( szBuffer, "%d x %d\n\n%s\n%s\n%s\n%s\n\nSource Image:\n%s", g_pImage->GetWidth(), g_pImage->GetHeight(), (g_pVQImage&&g_pVQImage->GetNumMipMaps() > 0 ? "Mipmaps" : "No Mipmaps" ), (g_pImage->HasAlpha() ? "Alpha Channel" : "No Alpha Channel" ), ( g_pVQImage ? "VQ Generated" : "VQ Not Generated" ), ( g_pImage->GetMMRGBA()->bPalette ? ( ((g_pImage->GetMMRGBA()->nPaletteDepth == 4) ? "4bpp Palette" : "8bpp Palette") ) : "No Palette") ,g_pImage->GetMMRGBA()->szDescription );
                        MessageBox( hWnd, szBuffer, szWindowTitle, MB_ICONINFORMATION );
                    }
                    else
                        MessageBox( hWnd, "There is no image", szWindowTitle, MB_ICONINFORMATION );
                        break;

                case ID_FILE_EXIT:
                    SendMessage( hWnd, WM_CLOSE, 0, 0 );
                    break;

                case ID_HELP_ABOUT:
                    DialogBox( g_hInstance, MAKEINTRESOURCE(IDD_ABOUT), hWnd, DlgProc_About );
                    break;

            }
            return 0;

        case WM_CLOSE:
            if( IsWindowVisible( g_hWndBrowser ) )
            {
                Browse();
                return TRUE;
            }
            else
            {
                if( g_pVQImage && g_pVQImage->m_bChanged )
                {
                    int nResult = MessageBox( hWnd, "Image has changed - do you want to save changes?", szWindowTitle, MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON2 );
                    switch( nResult )
                    {
                        case IDCANCEL: return 0;
                        case IDYES: if( g_pVQImage && g_pVQImage->ExportFile() == FALSE ) return 0;
                    }               
                }
            }
            break;

        case WM_SYSCOLORCHANGE:
            SendDlgItemMessage( hWnd, IDC_TOOLBAR,   WM_SYSCOLORCHANGE, 0, 0 );
            SendDlgItemMessage( hWnd, IDC_STATUSBAR, WM_SYSCOLORCHANGE, 0, 0 );
            break;

        case WM_INITMENU:
            CheckMenuItem( (HMENU)wParam, ID_TOOLS_REVERSEALPHA, MF_BYCOMMAND|(g_nOpaqueAlpha == 0x00) ? MF_CHECKED : MF_UNCHECKED );
            CheckMenuItem( (HMENU)wParam, ID_VIEW_TILES_MIRROR, MF_BYCOMMAND|(g_bMirrorFullScreen ? MF_CHECKED : MF_UNCHECKED ) );
            
            CheckMenuItem( (HMENU)wParam, ID_VIEW_MIPMAP_IMAGE, MF_BYCOMMAND|(g_bMipmapImage ? MF_CHECKED : MF_UNCHECKED ) );
            CheckMenuItem( (HMENU)wParam, ID_VIEW_MIPMAP_VQIMAGE, MF_BYCOMMAND|(g_bMipmapVQImage ? MF_CHECKED : MF_UNCHECKED ) );
            CheckMenuItem( (HMENU)wParam, ID_VIEW_MIPMAP_REFIMAGE, MF_BYCOMMAND|(g_bMipmapRefImage ? MF_CHECKED : MF_UNCHECKED ) );
            CheckMenuItem( (HMENU)wParam, ID_VIEW_STRETCHMIPMAPS, MF_BYCOMMAND|(g_bStretchMipmaps ? MF_CHECKED : MF_UNCHECKED ) );
            break;

        case WM_MENUSELECT:
            //display the prompt for the menu item
            if( HIWORD(wParam) == 0xFFFF && lParam == 0 )
                SendDlgItemMessage( hWnd, IDC_STATUSBAR, SB_SETTEXT, 0, (LPARAM)"" );
            else
            {
                //load string associated with the menu item
                char szBuffer[128] = "";
                if( LoadString( g_hInstance, wParam, szBuffer, 127 ) )
                {
                    char *lpszEOL = strchr( szBuffer, '\n' ); 
                    if( lpszEOL ) *lpszEOL = '\0';
                }
                SendDlgItemMessage( hWnd, IDC_STATUSBAR, SB_SETTEXT, 0, (LPARAM)szBuffer );
            }
            break;


        case WM_NOTIFY:
            switch( ((LPNMHDR)lParam)->idFrom )
            {
                case IDC_MIPMAPSLIDER:
                    if( g_pImage )
                    {
                        int nMipMapLevel = SendMessage( ((LPNMHDR)lParam)->hwndFrom, TBM_GETPOS, 0, 0 );
                        if( g_pImage && g_bMipmapImage ) g_pImage->SetMipMapLevel( nMipMapLevel );
                        if( g_pVQImage && g_bMipmapVQImage ) g_pVQImage->SetMipMapLevel( nMipMapLevel );
                        if( g_pRefImage && g_bMipmapRefImage ) g_pRefImage->SetMipMapLevel( nMipMapLevel );
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );
                    }
                    break;

                case IDC_ZOOMSLIDER:
                {
                    int iPos = SendMessage( ((LPNMHDR)lParam)->hwndFrom, TBM_GETPOS, 0, 0 ) - 10; 
                    if( iPos <= 0 ) iPos -= 2;
                    if( iPos != g_nZoom )
                    {
                        g_nZoom = iPos;
                        SetCursor( LoadCursor(NULL,IDC_WAIT) );
                        if( g_pImage ) g_pImage->SetDrawScaling( CalcZoom() );
                        if( g_pVQImage ) g_pVQImage->SetDrawScaling( CalcZoom() );
                        if( g_pRefImage ) g_pRefImage->SetDrawScaling( CalcZoom() );
                        SetCursor( LoadCursor(NULL,IDC_ARROW) );
                        g_ptOrigin.x = g_ptOrigin.y = 0;
                        UpdateZoomMessage();
                        UpdateScrollbars();
                        RedrawWindow( GetDlgItem(hWnd,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE );
                    }
                    break;
                }

                case IDC_TOOLBAR:
                    switch( ((LPNMHDR)lParam)->code )
                    {
                        case TBN_DROPDOWN:
                        {
                            switch( ((LPNMTOOLBAR)lParam)->iItem )
                            {
                                case ID_EDIT_COPY:  ShowCopyOptionMenu(); break;
                                case ID_VIEW_TILES: ShowViewTilesOptionMenu(); break;
                                case ID_VIEW_MIPMAP:ShowMipMapOptionMenu(); break;
                                case ID_VQ_COMPRESSIONANALYSIS: ShowVQAnalysisOptionMenu(); break;
                            }
                            return TBDDRET_DEFAULT;
                        }
                    }
                    break;

                case IDC_STATUSBAR:
                    switch( ((LPNMHDR)lParam)->code )
                    {
                        case NM_DBLCLK:
                            switch( ((LPNMMOUSE)lParam)->dwItemSpec )
                            {
                                case 1: //zoom value panel
                                    if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
                                        SendMessage( hWnd, WM_COMMAND, MAKEWPARAM(ID_VIEW_ZOOMNORMAL,0), 0);
                                    else
                                        SendMessage( hWnd, WM_COMMAND, MAKEWPARAM(ID_VIEW_ZOOMIN,0), 0);
                                    break;

                                case 2: //global index panel
                                    SendMessage( hWnd, WM_COMMAND, MAKEWPARAM(ID_TOOLS_SETGLOBALINDEX,0), 0);
                                    break;

                            }
                            break;

                        case NM_RDBLCLK:
                            if( ((LPNMMOUSE)lParam)->dwItemSpec == 1 )  //zoom value panel
                                SendMessage( hWnd, WM_COMMAND, MAKEWPARAM(ID_VIEW_ZOOMOUT,0), 0);
                            break;
                    }
                    break;

                default:
                    switch( ((LPNMHDR)lParam)->code )
                    {
                        case TTN_GETDISPINFO: //a tooltip control needs some text
                        {
                            LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
                            char szBuffer[128] = "";

                            //load string associated with the id
                            if( LoadString( g_hInstance, lpttt->hdr.idFrom, szBuffer, 127 ) )
                            {
                                //get a pointer to the second line of the string
                                char *lpszEOL = strchr( szBuffer, '\n' );
                                if( lpszEOL ) 
                                    strcpy( lpttt->szText, ++lpszEOL );
                                else
                                    strncpy( lpttt->szText, szBuffer, 80 );
                            }
                            else strcpy( lpttt->szText, "" ); //no tip available
                            return TRUE;
                            break;
                        }

                
                        case TTN_POP: //a tooltip control is about to be hidden - clear status bar
                            SendDlgItemMessage( hWnd, IDC_STATUSBAR, SB_SETTEXT, 0, (LPARAM)"" );
                            return TRUE;
                            break;

                        //a tooltip control is being shown
                        case TTN_SHOW:
                            //change the status text to it's long description
                            SendMessage( hWnd, WM_MENUSELECT, (WPARAM)((LPNMHDR)lParam)->idFrom, 0 );
                            return TRUE;
                            break;
                    }
                    break;

            }
            break;

        case WM_SIZE:
        {
            RECT rc; GetClientRect( hWnd, &rc );

            //resize toolbar and status bar
            SendDlgItemMessage( hWnd, IDC_TOOLBAR, uMsg, wParam, lParam );
            SendDlgItemMessage( hWnd, IDC_STATUSBAR, uMsg, wParam, lParam );

            //adjust client rect
            RECT rcBar;
            GetClientRect( GetDlgItem(hWnd,IDC_TOOLBAR), &rcBar );   rc.top += rcBar.bottom+2;
            GetClientRect( GetDlgItem(hWnd,IDC_STATUSBAR), &rcBar ); rc.bottom -= rcBar.bottom;
            MoveWindow( GetDlgItem(hWnd, IDC_CLIENTAREA), rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE );

            //prepare status bar sections
            INT nParts[] = { rc.right-200, rc.right-100, -1 };
            SendDlgItemMessage( hWnd, IDC_STATUSBAR, SB_SETPARTS, 3, (LPARAM)nParts );

            //adjust slider controls
            RECT rc2;   GetClientRect( GetDlgItem(hWnd,IDC_TOOLBAR), &rc2 );
            RECT rcB;   SendDlgItemMessage( hWnd, IDC_TOOLBAR, TB_GETITEMRECT, SendDlgItemMessage(hWnd,IDC_TOOLBAR,TB_BUTTONCOUNT,0,0)-1, (LPARAM)&rcB );

            rc2.top = 5;
            rc2.bottom -= 1;
            rc2.left = rcB.right + 4;
            rc2.right = rc2.left + 40;
            MoveWindow( GetDlgItem( GetDlgItem(hWnd,IDC_TOOLBAR), IDC_INFOTEXT), rc2.left, rc2.top, rc2.right-rc2.left, rc2.bottom-rc2.top, TRUE );

            rc2.top = 1;
            rc2.left = rc2.right + 4;
            rc2.right = rc2.left + 100;
            MoveWindow( GetDlgItem( GetDlgItem(hWnd,IDC_TOOLBAR), IDC_MIPMAPSLIDER ), rc2.left, rc2.top, rc2.right-rc2.left, rc2.bottom-rc2.top, TRUE );

            rc2.top = 5;
            rc2.left = rc2.right + 8;
            rc2.right = rc2.left + 40;
            MoveWindow( GetDlgItem( GetDlgItem(hWnd,IDC_TOOLBAR), IDC_ZOOMLABEL), rc2.left, rc2.top, rc2.right-rc2.left, rc2.bottom-rc2.top, TRUE );

            rc2.top = 1;
            rc2.left = rc2.right + 4;
            rc2.right = rc2.left + 100;
            MoveWindow( GetDlgItem( GetDlgItem(hWnd,IDC_TOOLBAR), IDC_ZOOMSLIDER ), rc2.left, rc2.top, rc2.right-rc2.left, rc2.bottom-rc2.top, TRUE );

            UpdateScrollbars();
            break;
        }

        case WM_DROPFILES:
        {
            char szFilename[MAX_PATH+1] = "";
            UINT uVal = DragQueryFile( (HDROP)wParam, 0, szFilename, MAX_PATH );

            if( GetFileAttributes( szFilename ) & FILE_ATTRIBUTE_DIRECTORY )
                Browse( szFilename );
            else
                LoadFile( szFilename );
            DragFinish( (HDROP)wParam );
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////
// Message processing function for the client area window
//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc_ClientArea( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_ERASEBKGND:
            return FALSE;

        case WM_PAINT:
        {
            //get the client rectangle size, allowing for child controls            
            RECT rc; GetClientRect( hWnd, &rc );

            //get the DC
            HDC hDC = GetDC(hWnd);
            POINT ptOldOrg = { 0, 0 };
            SetViewportOrgEx( hDC, g_ptOrigin.x, g_ptOrigin.y, &ptOldOrg );
            RECT rcDoc = { 0, 0, 10, 0 };
            HBRUSH hBrBorder = GetSysColorBrush(COLOR_3DFACE);

            //declare image array
            CImage* ImgArray[3] = { g_pImage, g_pVQImage, g_pRefImage };

            //get maxiumum height
            int nMaxHeight = 0, i;
            for( i = 0; i < 3; i++ ) if( ImgArray[i] && ImgArray[i]->GetScaledHeight() > nMaxHeight ) nMaxHeight = ImgArray[i]->GetScaledHeight();

            //draw all images
            float fZoom = CalcZoom();
            for( i = 0; i < 3; i++ )
            {
                CImage* pImage = ImgArray[i];
                if( pImage )
                {
                    //prepare drawing rect
                    pImage->SetDrawScaling( fZoom );
                    RECT rcImage = { 0, 0, pImage->GetScaledWidth(), pImage->GetScaledHeight() };

                    //draw right, middle, bottom & right borders
                    RECT rcBord;
                    int nMaxBottom = max( rcImage.bottom, nMaxHeight );
                    SetRect( &rcBord, rcImage.right, 0, rcImage.right+10, ((nMaxBottom+10)*2)+10 );
                    OffsetRect( &rcBord, rcDoc.right, 0 );
                    FillRect( hDC, &rcBord, hBrBorder );

                    SetRect( &rcBord, 0, rcImage.bottom+10, rcImage.right, rcImage.bottom+20 );
                    OffsetRect( &rcBord, rcDoc.right, 0 );
                    FillRect( hDC, &rcBord, hBrBorder );

                    SetRect( &rcBord, 0, (rcImage.bottom+10)*2, rcImage.right, ((nMaxBottom+10)*2)+10 );
                    OffsetRect( &rcBord, rcDoc.right, 0 );
                    FillRect( hDC, &rcBord, hBrBorder );

                    //draw image & alpha
                    pImage->Draw( hDC, rcDoc.right, 10 );
                    pImage->DrawAlpha( hDC, rcDoc.right, rcImage.bottom+20 );

                    //adjust the document rect
                    rcDoc.right += (rcImage.right + 10);
                    rcDoc.bottom = max( rcDoc.bottom, (rcImage.bottom + 10) * 2 );
                }
            }
            rcDoc.bottom += 10;

            //draw the right-hand border
            RECT rcBord1 = { 0, 0, rcDoc.right, 10 };
            RECT rcBord2 = { 0, 10, 10, rcDoc.bottom };
            FillRect( hDC, &rcBord1, hBrBorder );
            FillRect( hDC, &rcBord2, hBrBorder );

            //erase background
            RECT rcBack1 = { rcDoc.right, 0, rc.right, rcDoc.bottom };
            RECT rcBack2 = { 0, rcDoc.bottom, rc.right, rc.bottom };
            if( rcBack1.left < rcBack1.right ) FillRect( hDC, &rcBack1, GetSysColorBrush(COLOR_APPWORKSPACE) );
            if( rcBack2.top < rcBack2.bottom ) FillRect( hDC, &rcBack2, GetSysColorBrush(COLOR_APPWORKSPACE) );

            SetViewportOrgEx( hDC, ptOldOrg.x, ptOldOrg.y, NULL );
            ReleaseDC( hWnd, hDC );
            ValidateRect( hWnd, &rc );
            break;
        }

        case WM_LBUTTONDOWN:
            GetCursorPos( &g_ptTemp );
            ScreenToClient( hWnd, &g_ptTemp );
            SetCapture( hWnd );
            break;

        case WM_MOUSEMOVE:
            if( GetCapture() == hWnd )
            {
                //get the current mouse position
                POINT ptCur; GetCursorPos( &ptCur ); ScreenToClient( hWnd, &ptCur );

                //calculate how far it has moved since last time
                int dx = ( ptCur.x - g_ptTemp.x );
                int dy = ( ptCur.y - g_ptTemp.y );
                if( GetAsyncKeyState(VK_SHIFT) & 0x8000 ) { dx *= 3; dy *= 3; }
                g_ptTemp = ptCur;

                //update the origin
                g_ptOrigin.x += dx;
                g_ptOrigin.y += dy;

                //tell the scroll bars to update and read the constrained value back
                SetScrollPos( hWnd, SB_HORZ, -g_ptOrigin.x, TRUE );
                SetScrollPos( hWnd, SB_VERT, -g_ptOrigin.y, TRUE );
                g_ptOrigin.x = -GetScrollPos( hWnd, SB_HORZ );
                g_ptOrigin.y = -GetScrollPos( hWnd, SB_VERT );

                //redraw the window
                RedrawWindow( hWnd, NULL, NULL, RDW_INVALIDATE );
            }
            break;

        case WM_VSCROLL:
        case WM_HSCROLL:
        {
            RECT rc; GetClientRect( hWnd, &rc );
            int nBar = (uMsg == WM_HSCROLL) ? SB_HORZ : SB_VERT;
            int nMax = (uMsg == WM_HSCROLL) ? rc.right : rc.bottom;
            
            int iMin, iMax;
            GetScrollRange( hWnd, nBar, &iMin, &iMax);

            int iPos = GetScrollPos( hWnd, nBar );

            int dn = 0;
            switch( LOWORD(wParam) )
            {
                case SB_LINEDOWN:   dn =  nMax / 16 + 1;  break;
                case SB_LINEUP:     dn = -nMax / 16 + 1;  break;
                case SB_PAGEDOWN:   dn =  nMax / 2  + 1;  break;
                case SB_PAGEUP:     dn = -nMax / 2  + 1;  break;
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK: dn = HIWORD(wParam)-iPos; break;
            }

            iPos += dn;
            SetScrollPos( hWnd, nBar, iPos, TRUE);
            
            g_ptOrigin.x = -GetScrollPos( hWnd, SB_HORZ );
            g_ptOrigin.y = -GetScrollPos( hWnd, SB_VERT );
            RedrawWindow( hWnd, NULL, NULL,  RDW_INVALIDATE );
            break;
        }

        case WM_LBUTTONUP:
            if( GetCapture() == hWnd ) ReleaseCapture();
            break;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////
// Message processing function for the full screen window
//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc_FullScreen( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_CREATE:
        case WM_DISPLAYCHANGE:
            MoveWindow( hWnd, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), FALSE );           
            break;

        case WM_ERASEBKGND:
            return FALSE;

        case WM_PAINT:
            if( g_hBmFullScreen )
            {
                //show the wait cursor
                SetCursor( LoadCursor(NULL,IDC_WAIT) );

                //get the drawing info
                RECT rc; GetClientRect( hWnd, &rc );
                HDC hDC = GetDC(hWnd);

                //get the bitmap's dimensions
                BITMAP bm;
                GetObject( g_hBmFullScreen, sizeof(bm), &bm );

                //create off-screen device to draw the tiles with
                HDC hDCOff = CreateCompatibleDC( hDC );
                HBITMAP hBmOff = (HBITMAP)SelectObject( hDCOff, g_hBmFullScreen );

                SetStretchBltMode( hDC, COLORONCOLOR );

                //draw tiles
                if( g_bMirrorFullScreen )
                {
                    int nHeight = bm.bmHeight, nY = 0;
                    for( int y = 0; y < rc.bottom; y += bm.bmHeight )
                    {
                        int nWidth = bm.bmWidth, nX = 0;
                        for( int x = 0; x < rc.right; x += bm.bmWidth )
                        {
                            StretchBlt( hDC, x, y, bm.bmWidth, bm.bmHeight, hDCOff, nX, nY, nWidth, nHeight, SRCCOPY );
                            nWidth = -nWidth;
                            nX = nX ^ (bm.bmWidth - 1);
                        }
                        nHeight = -nHeight;
                        nY = nY ^ (bm.bmHeight - 1);
                    }
                }
                else
                {
                    for( int x = 0; x < rc.right; x += bm.bmWidth )
                        for( int y = 0; y < rc.bottom; y += bm.bmHeight )
                            BitBlt( hDC, x, y, bm.bmWidth, bm.bmHeight, hDCOff, 0, 0, SRCCOPY );
                }

                //clean up
                SelectObject( hDCOff, hBmOff );
                DeleteDC( hDCOff );
                ReleaseDC( hWnd, hDC );
                ValidateRect( hWnd, &rc );

                //hide the wait cursor
                SetCursor( LoadCursor(NULL,IDC_ARROW) );
                break;
            }
            //else drop through...
        case WM_KEYDOWN:
        case WM_LBUTTONDOWN:
        case WM_KILLFOCUS:
            if( g_hBmFullScreen ) { DeleteObject( g_hBmFullScreen ); g_hBmFullScreen = NULL; }
            ShowWindow( hWnd, SW_HIDE );
            SetFocus( g_hWnd );
            break;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


//////////////////////////////////////////////////////////////////////
// Application initialisation function
//////////////////////////////////////////////////////////////////////
BOOL InitApplication( HINSTANCE hInstance )
{
    g_hInstance = hInstance;

    //initialise common control library
    InitCommonControls();

    //create the main window
    g_hWnd = CreateRegisteredWindow( WndProc_MainWnd, hInstance, WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 0, szWindowTitle, LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MAINMENU)), (HCURSOR)IDC_ARROW, (HICON)IDI_APPICON );

    //create the client area
    HWND hWndClient = CreateRegisteredWindow( WndProc_ClientArea, hInstance, WS_HSCROLL|WS_VSCROLL|WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, WS_EX_CLIENTEDGE, "", (HMENU)IDC_CLIENTAREA, (HCURSOR)IDC_PAN, NULL, g_hWnd,0,0,0,0,NULL,CS_OWNDC|CS_DBLCLKS );
    
    //create the full screen window
    g_hWndFullScreen = CreateRegisteredWindow( WndProc_FullScreen, hInstance, WS_POPUP, WS_EX_TOOLWINDOW|WS_EX_TOPMOST, "", NULL, (HCURSOR)IDC_ARROW, NULL, g_hWnd,0,0,0,0, NULL, CS_OWNDC|CS_DBLCLKS );

    //create a toolbar
    HWND hWndToolbar = CreateToolbarFromResource( g_hWnd, IDR_TOOLBAR, IDC_TOOLBAR, WS_CHILD|WS_VISIBLE|TBSTYLE_FLAT|TBSTYLE_TOOLTIPS );
    SendMessage( hWndToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS );

    //create a slider control on the toolbar
    HWND hWndInfoText = CreateWindowEx( 0, "STATIC", "Mipmap:", WS_CHILD|WS_VISIBLE|SS_CENTER, 0, 0, 0, 0, hWndToolbar, (HMENU)IDC_INFOTEXT, hInstance, NULL );
    SendMessage( hWndInfoText, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0 );
    HWND hWndMipmapSlider = CreateWindowEx( 0, TRACKBAR_CLASS, NULL, WS_TABSTOP|WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_BOTH|TBS_NOTICKS|TBS_TOOLTIPS, 0, 0, 0, 0, hWndToolbar, (HMENU)IDC_MIPMAPSLIDER, hInstance, NULL );

    //create a slider control on the toolbar
    HWND hWndZoomLabel = CreateWindowEx( 0, "STATIC", "Zoom:", WS_CHILD|WS_VISIBLE|SS_CENTER, 0, 0, 0, 0, hWndToolbar, (HMENU)IDC_ZOOMLABEL, hInstance, NULL );
    SendMessage( hWndZoomLabel, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0 );
    HWND hWndZoomSlider = CreateWindowEx( 0, TRACKBAR_CLASS, NULL, WS_TABSTOP|WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_BOTH|TBS_NOTICKS, 0, 0, 0, 0, hWndToolbar, (HMENU)IDC_ZOOMSLIDER, hInstance, NULL );

    SendMessage( hWndZoomSlider, TBM_SETRANGE, FALSE, MAKELONG(2,20) );
    SendMessage( hWndZoomSlider, TBM_SETPOS, FALSE, 11 ); g_nZoom = 1; //SetRange calls WM_NOTIFY >:-(

    //add a dropdown arrow on the copy button
    TBBUTTONINFO tbbi; ZeroMemory( &tbbi, sizeof(tbbi) ); 
    tbbi.cbSize = sizeof(tbbi);
    tbbi.dwMask = TBIF_STYLE;
    tbbi.fsStyle = TBSTYLE_DROPDOWN|TBSTYLE_BUTTON;
    SendMessage( hWndToolbar, TB_SETBUTTONINFO, ID_EDIT_COPY, (LPARAM)&tbbi );  
    SendMessage( hWndToolbar, TB_SETBUTTONINFO, ID_VIEW_TILES, (LPARAM)&tbbi );  
    SendMessage( hWndToolbar, TB_SETBUTTONINFO, ID_VIEW_MIPMAP, (LPARAM)&tbbi );  
    SendMessage( hWndToolbar, TB_SETBUTTONINFO, ID_VQ_COMPRESSIONANALYSIS, (LPARAM)&tbbi );  


    //create a status bar
    HWND hWndStatusBar = CreateStatusWindow( SBARS_SIZEGRIP|WS_CHILD|WS_VISIBLE, "", g_hWnd, IDC_STATUSBAR );

    //prepare status bar sections
    INT nParts[] = { 100, 120, -1 };
    SendMessage( hWndStatusBar, SB_SETPARTS, 3, (LPARAM)nParts );
    UpdateZoomMessage();
    UpdateGlobalIndex();

    //initialise browser
    if( !InitBrowser() ) return FALSE;

    //initialise the log
    if( !InitLogWindow() ) return FALSE;

    //load default image
    g_pImage = new CImage();
    g_pImage->CreateDefault();

    //show the window
    ShowWindow( g_hWnd, SW_SHOW );
    UpdateWindow( g_hWnd );

    return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Program entry point
//////////////////////////////////////////////////////////////////////
void CheckColourDepth()
{
    //get the display's BPP
    HDC hDC = CreateDC("DISPLAY",NULL,NULL,NULL);
    int nBPP = GetDeviceCaps( hDC, BITSPIXEL );
    DeleteDC(hDC);

    //display a warning if it's too low
    if( nBPP < 16 )
        MessageBox( NULL, "Your display has 256 colours or less. The preview images will not be displayed correctly.", szWindowTitle, MB_ICONINFORMATION );
}


//////////////////////////////////////////////////////////////////////
// Program entry point
//////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
#ifdef _DEBUG
    _CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF );
#endif

    //do display check
    CheckColourDepth();

    //initialise the application
    if( !InitApplication( hInstance ) ) return FALSE;
    BuildTwiddleTable();

    //see if there's a command line
    if( strlen(lpCmdLine) )
    {
        //strip enclosing quotes, if any.
        if( *lpCmdLine == '\"' )
        {
            lpCmdLine++;
            char* pszTmp = lpCmdLine; while( *pszTmp ) { if(pszTmp[0] == '\"' ) *pszTmp = '\0'; pszTmp++; }
        }

        DWORD dwAttributes = GetFileAttributes( lpCmdLine );
        if( dwAttributes != 0xFFFFFFFF )
        {
            if( dwAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
                //it's a folder - browse it
                SetActiveWindow( g_hWndBrowser );
                ShowWindow( g_hWnd, SW_HIDE );
                ShowWindow( g_hWnd, SW_MINIMIZE );
                ShowWindow( g_hWnd, SW_SHOWNA );
                Browse( lpCmdLine );
            }
            else //it's a file - load it
                LoadFile( lpCmdLine );
        }
        else
            DisplayStatusMessage( "%s - file error", lpCmdLine );
    }

    RedrawWindow( g_hWnd, NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN );
    UpdateMipmapSlider();

    //load accelerator table
    HACCEL hAccel = LoadAccelerators( hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR) );

    //pump messages
    MSG msg;
    while( GetMessage( &msg, NULL, 0, 0 ) )
    {
        if( !TranslateAccelerator( GetActiveWindow(), hAccel, &msg ) ) TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    //clean up and exit
    CleanupBrowser();
    CleanupLogWindow();
    delete g_pImage;
    delete g_pVQImage;

    return msg.wParam;
}
