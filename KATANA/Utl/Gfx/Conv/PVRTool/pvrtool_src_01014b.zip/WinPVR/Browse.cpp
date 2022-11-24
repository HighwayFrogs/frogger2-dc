/*************************************************
 PVR Browser window

  To Do
  -----

  Enumerate the file system using COM rather
  than find all files and preset a better
  file browser, showing folders too ?

**************************************************/
#include <Windows.h>
#include <CommCtrl.h>
#include <ShlObj.h>
#include <afxres.h>
#include <stdio.h>
#include <io.h>
#include "Browse.h"
#include "WinUtil.h"
#include "../Image.h"
#include "../Util.h"
#include "../PVR.h"
#include "resource.h"


extern HINSTANCE g_hInstance;
extern HWND g_hWnd;

const char* szBrowseWindowTitle = "PVR Browser";

HWND g_hWndBrowser = NULL;

char g_pszCurrentFolder[MAX_PATH+1] = "";

extern void LoadFile( const char* pszFilename );

struct BrowseItem 
{
    CImage Image;
    bool bFailed;
    int nWidth, nHeight;
    char szFilename[MAX_PATH];
    BrowseItem* next;
};

BrowseItem* g_pBrowseItemList = NULL;
int g_nBrowseItems = 0;


HINSTANCE g_hInstShell = NULL;

extern void Copy( HBITMAP hBitmap );

//////////////////////////////////////////////////////////////////////
// Gets the current selection
//////////////////////////////////////////////////////////////////////
BrowseItem* GetCurrentBrowseItem()
{
    int iItem = SendDlgItemMessage( g_hWndBrowser, IDC_CLIENTAREA, LB_GETCURSEL, 0, 0 );
    if( iItem == LB_ERR ) return NULL;
    return (BrowseItem*)SendDlgItemMessage( g_hWndBrowser, IDC_CLIENTAREA, LB_GETITEMDATA, iItem, 0 );
}


//////////////////////////////////////////////////////////////////////
// Message processor for subclassed edit control on toolbar
//////////////////////////////////////////////////////////////////////
WNDPROC g_pfnDefEditProc = NULL;
LRESULT CALLBACK WndProc_EditSubclass( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( uMsg == WM_KEYUP && wParam == VK_RETURN ) PostMessage( g_hWndBrowser, WM_COMMAND, MAKEWPARAM(ID_BROWSE_GO,0), 0 );
    return CallWindowProc( g_pfnDefEditProc, hWnd, uMsg, wParam, lParam );
}


//////////////////////////////////////////////////////////////////////
// Deletes the current browse item list
//////////////////////////////////////////////////////////////////////
void DeleteBrowseList()
{
    while( g_pBrowseItemList )
    {
        BrowseItem* temp = g_pBrowseItemList;
        g_pBrowseItemList = g_pBrowseItemList->next;
        delete temp;
    }
    SendDlgItemMessage( g_hWndBrowser, IDC_CLIENTAREA, LB_RESETCONTENT, 0, 0 );
    g_nBrowseItems = 0;
}


//////////////////////////////////////////////////////////////////////
// Sets which folder to browse in and loads all images
//////////////////////////////////////////////////////////////////////
BOOL SetFolder( const char* pszFolder )
{
    //reset escape key check
    GetAsyncKeyState( VK_ESCAPE );

    //store the folder and update the edit control
    strcpy( g_pszCurrentFolder, pszFolder );
    SetDlgItemText( GetDlgItem(g_hWndBrowser,IDC_TOOLBAR), IDC_BROWSEPATH, g_pszCurrentFolder );
    SendDlgItemMessage( GetDlgItem(g_hWndBrowser,IDC_TOOLBAR), IDC_BROWSEPATH, EM_SETSEL, 0, -1 );

    //remove current list
    DeleteBrowseList();

    //make sure it's a valid directory
    DWORD dwAttributes = GetFileAttributes( pszFolder );
    if( dwAttributes == 0xFFFFFFFF || !( dwAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
    {
        MessageBeep(0xFFFFFFFF);
        SendDlgItemMessage( g_hWndBrowser, IDC_STATUSBAR, SB_SETTEXT, 0, (LPARAM)"Invalid path" );
        return FALSE;
    }

    //build file spec
    char szNicePath[MAX_PATH+1];
    strcpy( szNicePath, g_pszCurrentFolder );
    if( szNicePath[ strlen(szNicePath)-1 ] != '/' && szNicePath[ strlen(szNicePath)-1 ] != '\\' ) strcat( szNicePath, "\\" );

    char szFileSpec[MAX_PATH+1];
    strcpy( szFileSpec, szNicePath );
    strcat( szFileSpec, "*.pvr" );

    //find all files
    SendDlgItemMessage( g_hWndBrowser, IDC_STATUSBAR, SB_SETTEXT, 0, (LPARAM)"Loading files... (Press Escape to cancel)" );
    SendDlgItemMessage( g_hWndBrowser, IDC_STATUSBAR, SB_SETTEXT, 2, (LPARAM)"" );
    IndicateLongOperation( true );

    //disable drawing in the list box
    SendDlgItemMessage( g_hWndBrowser, IDC_CLIENTAREA, WM_SETREDRAW, FALSE, 0 );

    //do animation
    Animate_Play( GetDlgItem(g_hWndBrowser,IDC_ANIM), 0, -1, -1 );
    ShowWindow( GetDlgItem(g_hWndBrowser,IDC_ANIM), SW_SHOW );
  
    _finddata_t finddata;
    long hFind = _findfirst( szFileSpec, &finddata );
    if( hFind != -1 )
    {
        do 
        {
            //create a new item for it
            BrowseItem* pNew = new BrowseItem;
            pNew->next = g_pBrowseItemList;
            strcpy( pNew->szFilename, szNicePath );
            strcat( pNew->szFilename, finddata.name );


            //load the image file
            if( !pNew->Image.Load( pNew->szFilename ) )
            {
                pNew->bFailed = true;
                pNew->nWidth = 32;
                pNew->nHeight = 32;
            }
            else
            {
                //store it
                pNew->bFailed = false;
                pNew->nWidth = pNew->Image.GetWidth();
                pNew->nHeight = pNew->Image.GetHeight();

                //resample the image until it's small enough to fit in the list item
                //NOTE: cheap hack
                while( pNew->Image.GetHeight() > 128 )
                    pNew->Image.ScaleHalfSize();

                //delete everything except the aligned image            
                pNew->Image.DeleteMipMaps();
                pNew->Image.GetMMRGBA()->DeleteRGB();
                pNew->Image.GetMMRGBA()->DeleteAlpha();
            }

            if( pNew )
            {
                //add it to the list and increment the counter
                g_pBrowseItemList = pNew;
                g_nBrowseItems++;

                //add it to the listbox
                int iPos = SendDlgItemMessage( g_hWndBrowser, IDC_CLIENTAREA, LB_ADDSTRING, 0, (LPARAM)finddata.name );
                SendDlgItemMessage( g_hWndBrowser, IDC_CLIENTAREA, LB_SETITEMDATA, iPos, (LPARAM)pNew );

                //update the counter on the status bar
                char szTmp[10]; sprintf( szTmp, "%d items", g_nBrowseItems );
                SendDlgItemMessage( g_hWndBrowser, IDC_STATUSBAR, SB_SETTEXT, 2, (LPARAM)szTmp );
            }

            //check escape key
            if( GetAsyncKeyState( VK_ESCAPE ) & 1 ) break;

            //keep searching
        } while( _findnext( hFind, &finddata ) != -1 );

        SendDlgItemMessage( g_hWndBrowser, IDC_STATUSBAR, SB_SETTEXT, 0, (LPARAM)"" );
    }
    else
    {
        SendDlgItemMessage( g_hWndBrowser, IDC_STATUSBAR, SB_SETTEXT, 0, (LPARAM)"No files found." );
    }

    //update the status display etc.
    IndicateLongOperation( false );

    //stop animation
    Animate_Stop( GetDlgItem(g_hWndBrowser,IDC_ANIM) );
    ShowWindow( GetDlgItem(g_hWndBrowser,IDC_ANIM), SW_HIDE );

    //redraw the window
    SendDlgItemMessage( g_hWndBrowser, IDC_CLIENTAREA, WM_SETREDRAW, TRUE, 0 );
    RedrawWindow( GetDlgItem(g_hWndBrowser,IDC_CLIENTAREA), NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW );

    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Converts the given pidl into a file system path and browses in it
//////////////////////////////////////////////////////////////////////
void BrowseInPIDL( LPITEMIDLIST pidl )
{
    //get the file system path for the selected folder
    char szFolder[MAX_PATH+1];
    if( SHGetPathFromIDList( pidl, szFolder ) ) SetFolder( szFolder ); else MessageBox( g_hWndBrowser, "Browse failed", szBrowseWindowTitle, MB_ICONERROR );

    //free the shell item
    LPMALLOC pMalloc = NULL;
    if( SUCCEEDED(SHGetMalloc(&pMalloc)) )
    {
        pMalloc->Free(pidl);
        pMalloc->Release();
    }
}



//////////////////////////////////////////////////////////////////////
// PVR file information dialog box message processing function
//////////////////////////////////////////////////////////////////////
BOOL CALLBACK DlgProc_PVRInfo( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
        {
            //center the window over it's parent
            CenterWindow( hDlg, GetParent(hDlg) );

            //open the file
            const char* pszFilename = (const char*)lParam;
            FILE* file = fopen( pszFilename, "rb" );
            if( file == NULL ) { EndDialog( hDlg, FALSE ); return TRUE; }
            fseek( file, 0, SEEK_END ); int nFileLength = ftell( file ); fseek( file, 0, SEEK_SET );          

            //load the file into a buffer
            unsigned char* pBuffer = (unsigned char*)malloc( nFileLength );
            fread( pBuffer, nFileLength, 1, file );
            fclose(file);

            //extract headers
            unsigned char* pPtr = pBuffer;
            GlobalIndexHeader* pGBIX = NULL;

            if( memcmp( pPtr, "GBIX", 4 ) == 0 )
            {
                pGBIX = (GlobalIndexHeader*)pPtr;
                pPtr += sizeof(GlobalIndexHeader) + (pGBIX->nByteOffsetToNextTag-4);
            }

            PVRHeader* pHeader = NULL;
            if( memcmp( pPtr, "PVRT", 4 ) == 0 )
            {
                pHeader = (PVRHeader*)pPtr;
            }
            else
            {
                MessageBox( g_hWndBrowser, "This doesn't appear to be a valid PVR file", szBrowseWindowTitle, MB_ICONERROR );
                EndDialog( hDlg, FALSE );
                free( pBuffer );
                return TRUE;
            }

            //put the info into the controls
            SetDlgItemText( hDlg, IDC_FILENAME, GetFileNameNoPath(pszFilename) );
            if( pGBIX ) SetDlgItemInt( hDlg, IDC_GLOBALINDEX, pGBIX->nGlobalIndex, FALSE ); else SetDlgItemText( hDlg, IDC_GLOBALINDEX, "N/A" );

            char szText[1024];

            sprintf( szText, "%dKB (%d bytes)", nFileLength/1024, nFileLength );
            SetDlgItemText( hDlg, IDC_FILESIZE, szText );


            sprintf( szText, "%d x %d", pHeader->nWidth, pHeader->nHeight );
            SetDlgItemText( hDlg, IDC_IMAGESIZE, szText );

            char szTextureDesc[512] = "";
            BOOL bHasPixelFormat = TRUE;

            switch( pHeader->nTextureType & 0xFF00 )
            {
                case KM_TEXTURE_TWIDDLED:	        strcat( szTextureDesc, "KM_TEXTURE_TWIDDLED" );           break;
                case KM_TEXTURE_TWIDDLED_MM:	    strcat( szTextureDesc, "KM_TEXTURE_TWIDDLED_MM" );        break;
                case KM_TEXTURE_VQ:	                strcat( szTextureDesc, "KM_TEXTURE_VQ" );                 break;
                case KM_TEXTURE_VQ_MM:	            strcat( szTextureDesc, "KM_TEXTURE_VQ_MM" );              break;
                case KM_TEXTURE_PALETTIZE4:	        strcat( szTextureDesc, "KM_TEXTURE_PALETTIZE4" );         bHasPixelFormat = FALSE; break;
                case KM_TEXTURE_PALETTIZE4_MM:	    strcat( szTextureDesc, "KM_TEXTURE_PALETTIZE4_MM" );      bHasPixelFormat = FALSE; break;
                case KM_TEXTURE_PALETTIZE8:	        strcat( szTextureDesc, "KM_TEXTURE_PALETTIZE8" );         bHasPixelFormat = FALSE; break;
                case KM_TEXTURE_PALETTIZE8_MM:	    strcat( szTextureDesc, "KM_TEXTURE_PALETTIZE8_MM" );      bHasPixelFormat = FALSE; break;
                case KM_TEXTURE_RECTANGLE:	        strcat( szTextureDesc, "KM_TEXTURE_RECTANGLE" );          break;
                case KM_TEXTURE_RECTANGLE_MM:	    strcat( szTextureDesc, "KM_TEXTURE_RECTANGLE_MM" );       SetDlgItemText( hDlg, IDC_CATEGORY_DESC, "WARNING: reserved format. May not behave as expected." ); break;
                case KM_TEXTURE_STRIDE:	            strcat( szTextureDesc, "KM_TEXTURE_STRIDE" );             break;
                case KM_TEXTURE_STRIDE_MM:	        strcat( szTextureDesc, "KM_TEXTURE_STRIDE_MM" );          SetDlgItemText( hDlg, IDC_CATEGORY_DESC, "WARNING: reserved. May not behave as expected." ); break;
                case KM_TEXTURE_TWIDDLED_RECTANGLE: strcat( szTextureDesc, "KM_TEXTURE_TWIDDLED_RECTANGLE" ); break;
                case KM_TEXTURE_BMP:                strcat( szTextureDesc, "KM_TEXTURE_BMP" );                bHasPixelFormat = FALSE; SetDlgItemText( hDlg, IDC_CATEGORY_DESC, "WARNING: converted to twiddled. May not behave as expected." ); break;
                case KM_TEXTURE_BMP_MM:             strcat( szTextureDesc, "KM_TEXTURE_BMP_MM" );             bHasPixelFormat = FALSE; SetDlgItemText( hDlg, IDC_CATEGORY_DESC, "WARNING: converted to mipmap-twiddled. May not behave as expected." ); break;
                case KM_TEXTURE_SMALLVQ:            strcat( szTextureDesc, "KM_TEXTURE_SMALLVQ" );            break;
                case KM_TEXTURE_SMALLVQ_MM:         strcat( szTextureDesc, "KM_TEXTURE_SMALLVQ_MM" );         break;
                default:                            strcat( szTextureDesc, "(unknown image type)" );break;
            }

            if( bHasPixelFormat )
            {
                strcat( szTextureDesc, "\r\n" );
                switch( pHeader->nTextureType & 0xFF )
                {
                    case KM_TEXTURE_ARGB1555: strcat( szTextureDesc, "KM_TEXTURE_ARGB1555" );  break;
                    case KM_TEXTURE_RGB565:   strcat( szTextureDesc, "KM_TEXTURE_RGB565" );    break;
                    case KM_TEXTURE_ARGB4444: strcat( szTextureDesc, "KM_TEXTURE_ARGB4444" );  break;
                    case KM_TEXTURE_YUV422:   strcat( szTextureDesc, "KM_TEXTURE_ARGYUV422" ); break;
                    case KM_TEXTURE_BUMP:     strcat( szTextureDesc, "KM_TEXTURE_BUMP" );      break;
                    case KM_TEXTURE_RGB555:   strcat( szTextureDesc, "KM_TEXTURE_555 (PCX compatibilty only)" );      break;
                    case KM_TEXTURE_YUV420:   strcat( szTextureDesc, "KM_TEXTURE_YUV420 (YUV converter only)" );      break;
                    default:                  strcat( szTextureDesc, "(unknown texture type)");break;
                }
            }

            SetDlgItemText( hDlg, IDC_KAMUIFORMAT, szTextureDesc );

            //clean up
            free( pBuffer );
            return TRUE;
        }

        case WM_COMMAND:
            if( LOWORD(wParam) == IDOK ) { EndDialog( hDlg, TRUE ); return TRUE; }
            break;

        case WM_CLOSE:
            EndDialog( hDlg, TRUE );
            return TRUE;
    }

    return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Message processing function for the main window
//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc_Browser( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_CREATE:
            DragAcceptFiles( hWnd, TRUE );
            return 0;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case ID_FILE_OPEN:
                case ID_BROWSE_NEWFOLDER:
                {
                    //prepare browse information structure
                    char szDisplayName[MAX_PATH+1];
                    BROWSEINFO bi; ZeroMemory( &bi, sizeof(bi) );
                    bi.hwndOwner = hWnd;
                    bi.pszDisplayName = szDisplayName;
                    bi.lpszTitle = "Select the folder to browse for textures";
                    bi.ulFlags = BIF_RETURNONLYFSDIRS;

                    //browse for a new folder
                    LPITEMIDLIST pidl = SHBrowseForFolder( &bi );
                    if( pidl ) BrowseInPIDL( pidl );
                    return 0;
                }

                case ID_BROWSE_OPEN:
                {
                    BrowseItem* pbi = GetCurrentBrowseItem();
                    if( pbi )
                    {
                        if( IsIconic(g_hWnd) ) ShowWindow( g_hWnd, SW_RESTORE );
                        BringWindowToTop( g_hWnd );
                        LoadFile( pbi->szFilename );
                    }
                    return 0;
                }

                case ID_BROWSE_SWITCHWINDOW:
                    if( IsIconic(g_hWnd) ) ShowWindow( g_hWnd, SW_RESTORE );
                    BringWindowToTop( g_hWnd );
                    return 0;

                case ID_BROWSE_REFRESH:
                    SetFolder( g_pszCurrentFolder );
                    return 0;

                case ID_BROWSE_UPONELEVEL:
                {
                    //store the current directory
                    char szCurrentDirectory[MAX_PATH+1];
                    GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

                    //change into the parent directory
                    SetCurrentDirectory( g_pszCurrentFolder );
                    SetCurrentDirectory( ".." );

                    //store the parent directory
                    char szNewDirectory[MAX_PATH+1];
                    GetCurrentDirectory( MAX_PATH, szNewDirectory );

                    //restore directory and update
                    SetCurrentDirectory( szCurrentDirectory );
                    SetFolder( szNewDirectory );
                    return 0;
                }

                case IDC_CLIENTAREA:
                    if( HIWORD(wParam) == LBN_DBLCLK )
                    {
                        SendMessage( hWnd, WM_COMMAND, MAKEWPARAM(ID_BROWSE_OPEN,0), 0 );
                    }
                    return 0;

                case ID_BROWSE_PROPERTIES:
                {
                    BrowseItem* pbi = GetCurrentBrowseItem();
                    if( pbi )
                    {
                        DialogBoxParam( g_hInstance, MAKEINTRESOURCE(IDD_PVRFILEINFO), hWnd, DlgProc_PVRInfo, (LPARAM)pbi->szFilename );
                    }
                    return 0;
                }

                case ID_BROWSE_EXPLORER:
                    //try to explore the folder first
                    if( int(ShellExecute( hWnd, "Explore", g_pszCurrentFolder, NULL, NULL, SW_SHOW )) < 32 )
                        //that failed - do whatever the default action is
                        ShellExecute( hWnd, NULL, g_pszCurrentFolder, NULL, NULL, SW_SHOW );
                    return 0;

                case ID_BROWSE_GO:
                {
                    char szPath[MAX_PATH+1];
                    GetDlgItemText( GetDlgItem(hWnd,IDC_TOOLBAR), IDC_BROWSEPATH, szPath, MAX_PATH );
                    SetFolder( szPath );
                }
                return 0;

                case ID_EDIT_COPY_IMAGE: //the Ctrl+C accelerator overrides the edit control's Ctrl+C handling so put it back!
                    SendDlgItemMessage( GetDlgItem(g_hWndBrowser,IDC_TOOLBAR), IDC_BROWSEPATH, WM_COPY, 0, 0 );
                    return 0;

                case ID_EDIT_COPY:
                {
                    BrowseItem* pbi = GetCurrentBrowseItem();
                    if( pbi )
                    {
                        CImage image;
                        if( image.Load( pbi->szFilename ) ) Copy( image.GetImageBitmap() );
                    }
                    return 0;
                }

                case ID_EDIT_COPY_ALPHA:
                {
                    BrowseItem* pbi = GetCurrentBrowseItem();
                    if( pbi )
                    {
                        CImage image;
                        if( image.Load( pbi->szFilename ) ) Copy( image.GetAlphaBitmap() );
                    }
                    return 0;
                }

            }
            break;

        case WM_CLOSE:
            //if we're told to close, just hide rather than closing
            IndicateLongOperation(true);
            ShowWindow( hWnd, SW_HIDE );
            CleanupBrowser();
            IndicateLongOperation(false);
            return 0;

        case WM_CONTEXTMENU:
        {
            //get the cursor position
            POINT pt; GetCursorPos( &pt );

            //select the item under the cursor
            POINT ptC = pt;
            ScreenToClient( GetDlgItem(hWnd,IDC_CLIENTAREA), &ptC );
            DWORD dwItem = SendDlgItemMessage( hWnd, IDC_CLIENTAREA, LB_ITEMFROMPOINT, 0, MAKELPARAM(ptC.x,ptC.y) );
            SendDlgItemMessage( hWnd, IDC_CLIENTAREA, LB_SETCURSEL, LOWORD(dwItem), 0 );

            //create and build the popup menu depending on whether there's any items or not
            HMENU hMenu = CreatePopupMenu();
            if( SendDlgItemMessage( hWnd, IDC_CLIENTAREA, LB_GETCURSEL, 0, 0 ) != LB_ERR )
            {
                AppendMenu( hMenu, MF_ENABLED|MF_STRING, ID_BROWSE_OPEN, "&Open" );
                AppendMenu( hMenu, MF_ENABLED|MF_STRING, ID_EDIT_COPY, "&Copy" );
                AppendMenu( hMenu, MF_ENABLED|MF_STRING, ID_EDIT_COPY_ALPHA, "Copy &Alpha" );
                AppendMenu( hMenu, MF_SEPARATOR, 0, NULL );
                AppendMenu( hMenu, MF_ENABLED|MF_STRING, ID_BROWSE_PROPERTIES, "P&roperties" );
                SetMenuDefaultItem( hMenu, ID_BROWSE_OPEN, FALSE );
            }
            else
            {
                AppendMenu( hMenu, MF_GRAYED|MF_DISABLED|MF_STRING, 0, "(no selection)" );
            }

            //display the popup menu and destroy it when it returns
            TrackPopupMenu( hMenu, TPM_RIGHTBUTTON|TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL );
            DestroyMenu( hMenu );
            return 0;
        }

        case WM_DRAWITEM:
            switch( wParam )
            {
                case IDC_CLIENTAREA:
                {
                    BrowseItem* pbi = (BrowseItem*)((LPDRAWITEMSTRUCT)lParam)->itemData;
                    if( pbi )
                    {
                        //extract items from the draw item structure
                        HDC hDC = ((LPDRAWITEMSTRUCT)lParam)->hDC;
                        RECT rcItem = ((LPDRAWITEMSTRUCT)lParam)->rcItem;
                        UINT uState = ((LPDRAWITEMSTRUCT)lParam)->itemState;

                        SetBkMode( hDC, TRANSPARENT );

                        //draw the image
                        if( ((LPDRAWITEMSTRUCT)lParam)->itemAction == ODA_DRAWENTIRE )
                        {
                            if( pbi->bFailed )
                            {
                                //display the PVR file icon instead
                                DrawIcon( hDC, rcItem.left+8, rcItem.top+8, LoadIcon( g_hInstance, MAKEINTRESOURCE(IDI_PVRFILE) ) );
                            }
                            else
                            {
                                //draw image
                                pbi->Image.Draw( hDC, rcItem.left+8, rcItem.top+8 );
                            }
                        }
                        
                        //set image info
                        char szExtraInfo[128] = "";
                        if( pbi->bFailed )
                        {
                            strcpy( szExtraInfo, "Image format invalid or not supported" );
                            rcItem.left += 48;
                        }
                        else
                        {
                            sprintf( szExtraInfo, "%d x %d", pbi->nWidth, pbi->nHeight );
                            rcItem.left += pbi->Image.GetWidth() + 16;
                        }

                        //draw state images
                        if( uState & ODS_SELECTED )
                        {
                            FillRect( hDC, &rcItem, GetSysColorBrush(COLOR_HIGHLIGHT) );
                            SetTextColor( hDC, GetSysColor(COLOR_HIGHLIGHTTEXT) );
                        }
                        else
                        {
                            FillRect( hDC, &rcItem, GetSysColorBrush(COLOR_WINDOW) );
                            SetTextColor( hDC, GetSysColor(COLOR_WINDOWTEXT) );
                        }
                        if( uState & ODS_FOCUS )
                        {
                            DrawFocusRect( hDC, &rcItem );
                        }

                        //display filename too
                        const char* pszFilename = GetFileNameNoPath( pbi->szFilename );
                        TextOut( hDC, rcItem.left + 8, rcItem.top + 4, pszFilename, strlen(pszFilename) );
                        TextOut( hDC, rcItem.left + 8, rcItem.top + 16, szExtraInfo, strlen(szExtraInfo) );
                    }
                    return TRUE;
                }
            }
            break;

        case WM_MEASUREITEM:
            switch( ((LPMEASUREITEMSTRUCT)lParam)->CtlID )
            {
                case IDC_CLIENTAREA:
                    ((LPMEASUREITEMSTRUCT)lParam)->itemHeight = 144;
                    return TRUE;
            }
            break;

        case WM_SYSCOLORCHANGE:
            SendDlgItemMessage( hWnd, IDC_TOOLBAR,   WM_SYSCOLORCHANGE, 0, 0 );
            SendDlgItemMessage( hWnd, IDC_STATUSBAR, WM_SYSCOLORCHANGE, 0, 0 );
            return 0;

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
            return 0;

        case WM_NOTIFY:
            /*switch( ((LPNMHDR)lParam)->idFrom )
            {
                default:*/
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

            /*}
            break;*/

        case WM_SIZE:
        {
            RECT rc; GetClientRect( hWnd, &rc );

            //resize toolbar and status bar
            SendDlgItemMessage( hWnd, IDC_TOOLBAR, uMsg, wParam, lParam );
            SendDlgItemMessage( hWnd, IDC_STATUSBAR, uMsg, wParam, lParam );

            //adjust client rect
            RECT rcBar;
            GetClientRect( GetDlgItem(hWnd,IDC_TOOLBAR), &rcBar );   rc.top += rcBar.bottom;
            GetClientRect( GetDlgItem(hWnd,IDC_STATUSBAR), &rcBar ); rc.bottom -= rcBar.bottom;
            MoveWindow( GetDlgItem(hWnd, IDC_CLIENTAREA), rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE );

            MoveWindow( GetDlgItem(hWnd, IDC_ANIM), rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE );

            //prepare status bar sections
            INT nParts[] = { rc.right-200, rc.right-100, -1 };
            SendDlgItemMessage( hWnd, IDC_STATUSBAR, SB_SETPARTS, 3, (LPARAM)nParts );
            break;
        }

        case WM_CTLCOLORSTATIC:
            //use a window-coloured background for the animation control
            if( (HWND)lParam == GetDlgItem( hWnd, IDC_ANIM ) ) return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
            break;
    
        case WM_DROPFILES:
        {
            char szFilename[MAX_PATH+1] = "";
            UINT uVal = DragQueryFile( (HDROP)wParam, 0, szFilename, MAX_PATH );
            
            if( GetFileAttributes( szFilename ) & FILE_ATTRIBUTE_DIRECTORY )
                SetFolder( szFilename );
            else
            {
                if( IsIconic(g_hWnd) ) ShowWindow( g_hWnd, SW_RESTORE );
                BringWindowToTop( g_hWnd );
                LoadFile( szFilename );
            }
            
            DragFinish( (HDROP)wParam );
            return 0;
        }
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}





//////////////////////////////////////////////////////////////////////
// Initialises the browser window
//////////////////////////////////////////////////////////////////////
BOOL InitBrowser()
{
    //create the browse window
    g_hWndBrowser = CreateRegisteredWindow( WndProc_Browser, g_hInstance, WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 0, szBrowseWindowTitle, NULL, (HCURSOR)IDC_ARROW, (HICON)IDI_BROWSEICON );

    //create the list box
    HWND hWndClient = CreateWindowEx( WS_EX_CLIENTEDGE, "LISTBOX", NULL, WS_TABSTOP|WS_CHILD|WS_VISIBLE|WS_VSCROLL|LBS_NOTIFY|LBS_OWNERDRAWFIXED|LBS_SORT|LBS_HASSTRINGS|LBS_NOINTEGRALHEIGHT, 0, 0, 0, 0, g_hWndBrowser, (HMENU)IDC_CLIENTAREA, g_hInstance, NULL );
    SendMessage( hWndClient, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0 );
    
    //create a toolbar
    HWND hWndToolbar = CreateToolbarFromResource( g_hWndBrowser, IDR_BROWSETOOLBAR, IDC_TOOLBAR, WS_TABSTOP|WS_CHILD|WS_VISIBLE|TBSTYLE_FLAT|TBSTYLE_TOOLTIPS|CCS_NODIVIDER );
    SendMessage( hWndToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS );

    //create edit control
    HWND hWndEdit = CreateWindowEx( WS_EX_CLIENTEDGE, "EDIT", "", WS_TABSTOP|WS_CHILD|WS_VISIBLE|ES_LEFT, 0, 0, 0, 0, g_hWndBrowser, (HMENU)IDC_BROWSEPATH, g_hInstance, NULL );

    //place the edit control on the toolbar
    int iButton = SendMessage( hWndToolbar, TB_BUTTONCOUNT, 0, 0 ) - 1;
    RECT rcButton; SendMessage( hWndToolbar, TB_GETITEMRECT, iButton, (LPARAM)&rcButton );
    SetParent( hWndEdit, hWndToolbar );
    MoveWindow( hWndEdit, rcButton.right + 2, rcButton.top + 1, 320, rcButton.bottom-rcButton.top-2, TRUE );
    SendMessage( hWndEdit, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), MAKELPARAM(TRUE,0) );

    //subclass the edit control so we can catch the enter key being pressed
    g_pfnDefEditProc = (WNDPROC)SetWindowLong( hWndEdit, GWL_WNDPROC, (LONG)WndProc_EditSubclass );

    //create a status bar
    HWND hWndStatusBar = CreateStatusWindow( SBARS_SIZEGRIP|WS_CHILD|WS_VISIBLE, "", g_hWndBrowser, IDC_STATUSBAR );

    //create the animation control
    g_hInstShell = LoadLibrary("SHELL32.DLL");
    HWND hWndAnim = CreateWindowEx( WS_EX_CLIENTEDGE, ANIMATE_CLASS, NULL, WS_CHILD|ACS_CENTER|ACS_TRANSPARENT, 0, 0, 0, 0, g_hWndBrowser, (HMENU)IDC_ANIM, g_hInstShell, NULL );
    Animate_Open( hWndAnim, MAKEINTRESOURCE(150) ); //150=TORCH, 151=FIND FILES...

    //prepare status bar sections
    INT nParts[] = { 100, 120, -1 };
    SendMessage( hWndStatusBar, SB_SETPARTS, 3, (LPARAM)nParts );

    //don't show it yet though
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Shutdown Browser
//////////////////////////////////////////////////////////////////////
void CleanupBrowser()
{
    //build INI file name
    char szINIFile[MAX_PATH+1];
    GetModuleFileName( NULL, szINIFile, MAX_PATH );
    ChangeFileExtension( szINIFile, "INI" );

    //get the last used path
    if( *g_pszCurrentFolder )
    {
        DWORD dwAttributes = GetFileAttributes( g_pszCurrentFolder );
        if( dwAttributes != 0xFFFFFFFF && dwAttributes & FILE_ATTRIBUTE_DIRECTORY )
            WritePrivateProfileString( "Browse", "LastPath", g_pszCurrentFolder, szINIFile );
    }

    //free the browse list
    DeleteBrowseList();

    //unload the shell library
    FreeLibrary( g_hInstShell );
}


//////////////////////////////////////////////////////////////////////
// Browses all .PVR files in the given directory
//////////////////////////////////////////////////////////////////////
BOOL Browse( const char* pszFolder /*NULL*/ )
{
    //show/restore the window
    BOOL bNeedRefresh = FALSE;
    if( IsWindowVisible(g_hWndBrowser) )
    {
        if( IsIconic(g_hWndBrowser) ) ShowWindow( g_hWndBrowser, SW_RESTORE );
        BringWindowToTop( g_hWndBrowser );
    }
    else
    {
        //move the windows into a useful place if they're being shown

        //get the rectangles for the desktop area and the main window rect
        RECT rcMain;     GetWindowRect( g_hWnd, &rcMain );
        RECT rcWorkArea; SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWorkArea, 0 );

        //if the main window is maximised, restore it
        if( IsZoomed( g_hWnd ) ) ShowWindow( g_hWnd, SW_RESTORE );

        //prepare the browse window's rectangle
        RECT rcBrowse;
        int nWidth = max( ( rcMain.right / 3 ), 400 );
        rcBrowse.left = min( rcMain.right, rcWorkArea.right - nWidth );
        rcBrowse.right = rcBrowse.left + nWidth + GetSystemMetrics(SM_CXSIZEFRAME);
        rcBrowse.top = rcMain.top;
        rcBrowse.bottom = rcMain.bottom;

        //adjust the main window's rectangle
        rcMain.right = min( rcBrowse.left, rcMain.right );

        //move the windows into their new position
        MoveWindow( g_hWndBrowser, rcBrowse.left, rcBrowse.top, rcBrowse.right-rcBrowse.left, rcBrowse.bottom-rcBrowse.top, FALSE );
        MoveWindow( g_hWnd, rcMain.left, rcMain.top, rcMain.right-rcMain.left, rcMain.bottom-rcMain.top, TRUE );

        //show the window and refresh
        ShowWindow(g_hWndBrowser, SW_SHOW );
        bNeedRefresh = TRUE;
    }
    
    //update the window
    UpdateWindow( g_hWndBrowser );

    //select which folder to browse in
    char szFolder[MAX_PATH+1];
    if( pszFolder == NULL )
    {
        char szCurFolder[MAX_PATH+1];
        GetCurrentDirectory( MAX_PATH, szCurFolder ); 

        //build INI file name
        char szINIFile[MAX_PATH+1];
        GetModuleFileName( NULL, szINIFile, MAX_PATH );
        ChangeFileExtension( szINIFile, "INI" );

        //get the last used path
        GetPrivateProfileString( "Browse", "LastPath", szCurFolder, szFolder, MAX_PATH, szINIFile );
        if( strlen(szFolder) == 0 ) strcpy( szFolder, szCurFolder );
    }
    else strcpy( szFolder, pszFolder );

    //refresh only if we need to
    if( ( bNeedRefresh || stricmp( szFolder, g_pszCurrentFolder ) != 0 ) ) 
        SetFolder( szFolder );

    return TRUE;
}
