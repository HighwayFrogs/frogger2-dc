/*************************************************
 VQF settings dialog
 

**************************************************/
#include <Windows.h>
#include <CommCtrl.h>
#include <stdio.h>
#include "VQFDialog.h"
#include "WinUtil.h"
#include "../VQCompressor.h"
#include "../Image.h"
#include "../Util.h"
#include "resource.h"

extern CVQCompressor g_VQCompressor;


//////////////////////////////////////////////////////////////////////
// VQ Settings Dialog Box Message Processor
//////////////////////////////////////////////////////////////////////
BOOL CALLBACK DlgProc_VQFDialog( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
        {
            //center the window over it's parent
            CenterWindow( hDlg, GetParent(hDlg) );

            //build INI file name
            char szINIFile[MAX_PATH+1];
            GetModuleFileName( NULL, szINIFile, MAX_PATH );
            ChangeFileExtension( szINIFile, "INI" );

            //initialise controls
            SendDlgItemMessage( hDlg, IDC_COLOURFORMAT, CB_ADDSTRING, 0, (LPARAM)"Smart" );
            SendDlgItemMessage( hDlg, IDC_COLOURFORMAT, CB_ADDSTRING, 0, (LPARAM)"565" );
            SendDlgItemMessage( hDlg, IDC_COLOURFORMAT, CB_ADDSTRING, 0, (LPARAM)"555" );
            SendDlgItemMessage( hDlg, IDC_COLOURFORMAT, CB_ADDSTRING, 0, (LPARAM)"1555" );
            SendDlgItemMessage( hDlg, IDC_COLOURFORMAT, CB_ADDSTRING, 0, (LPARAM)"4444" );
            SendDlgItemMessage( hDlg, IDC_COLOURFORMAT, CB_ADDSTRING, 0, (LPARAM)"Smart YUV" );
            SendDlgItemMessage( hDlg, IDC_COLOURFORMAT, CB_ADDSTRING, 0, (LPARAM)"YUV" );

            SendDlgItemMessage( hDlg, IDC_DITHER, TBM_SETRANGE, 0, MAKELONG(0,2) );


            //set initial values
            int iSel = GetPrivateProfileInt( "VQF", "ImageColourFormat", 0, szINIFile );
            SendDlgItemMessage( hDlg, IDC_COLOURFORMAT, CB_SETCURSEL, iSel, 0 );
            CheckDlgButton( hDlg, IDC_MIPMAP, GetPrivateProfileInt( "VQF", "MipMap", 0, szINIFile ) ? BST_CHECKED : BST_UNCHECKED );
            CheckDlgButton( hDlg, IDC_HIGHFREQTOL, GetPrivateProfileInt( "VQF", "HighFreqTol", 0, szINIFile ) ? BST_CHECKED : BST_UNCHECKED );
            SendDlgItemMessage( hDlg, IDC_DITHER, TBM_SETPOS, TRUE, GetPrivateProfileInt( "VQF", "Dither", 1, szINIFile ) );

            switch( GetPrivateProfileInt( "VQF", "WeightValue", VQMetricWeighted, szINIFile ) )
            {
                default:
                case VQMetricWeighted: CheckDlgButton( hDlg, IDC_COLOURMETRIC_EYE, BST_CHECKED ); break;
                case VQMetricEqual:      CheckDlgButton( hDlg, IDC_COLOURMETRIC_RGBA, BST_CHECKED ); break;
            }

            //move ourselves to a sensible position on the screen
            RECT rc; GetClientRect( hDlg, &rc ); 
            RECT rcParent; GetClientRect( GetParent(hDlg), &rcParent );
            SetWindowPos( hDlg, NULL, rcParent.right/2 - rc.right/2, rcParent.bottom/2 - rc.bottom/2, 0, 0, SWP_NOSIZE|SWP_NOZORDER );
            return TRUE;
        }

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDOK:
                {
                    //build INI file name
                    char szINIFile[MAX_PATH+1];
                    GetModuleFileName( NULL, szINIFile, MAX_PATH );
                    ChangeFileExtension( szINIFile, "INI" );

                    int iSel = SendDlgItemMessage( hDlg, IDC_COLOURFORMAT, CB_GETCURSEL, 0, 0 );
                    WritePrivateProfileInt( "VQF", "ImageColourFormat", iSel, szINIFile );
                    switch( iSel )
                    {
                        default:
                        case 0: g_VQCompressor.m_icf = ICF_SMART; break;
                        case 1: g_VQCompressor.m_icf = ICF_565; break;
                        case 2: g_VQCompressor.m_icf = ICF_555; break;
                        case 3: g_VQCompressor.m_icf = ICF_1555; break;
                        case 4: g_VQCompressor.m_icf = ICF_4444; break;
                        case 5: g_VQCompressor.m_icf = ICF_SMARTYUV; break;
                        case 6: g_VQCompressor.m_icf = ICF_YUV422; break;
                    }

                    iSel = SendDlgItemMessage( hDlg, IDC_DITHER, TBM_GETPOS, 0, 0 );
                    WritePrivateProfileInt( "VQF", "Dither", iSel, szINIFile );
                    switch( iSel )
                    {
                        case 0: g_VQCompressor.m_Dither = VQNoDither; break;
                        default:
                        case 1: g_VQCompressor.m_Dither = VQSubtleDither; break;
                        case 2: g_VQCompressor.m_Dither = VQFullDither; break;
                    }

                    g_VQCompressor.m_bMipmap = ( IsDlgButtonChecked( hDlg, IDC_MIPMAP ) == BST_CHECKED );
                    WritePrivateProfileInt( "VQF", "MipMap", int(g_VQCompressor.m_bMipmap), szINIFile );

                    g_VQCompressor.m_bTolerateHigherFrequency = ( IsDlgButtonChecked( hDlg, IDC_HIGHFREQTOL ) == BST_CHECKED );
                    WritePrivateProfileInt( "VQF", "HighFreqTol", int(g_VQCompressor.m_bTolerateHigherFrequency), szINIFile );

                    if( IsDlgButtonChecked( hDlg, IDC_COLOURMETRIC_RGBA ) == BST_CHECKED ) g_VQCompressor.m_Metric = VQMetricEqual;
                    else if( IsDlgButtonChecked( hDlg, IDC_COLOURMETRIC_EYE ) == BST_CHECKED ) g_VQCompressor.m_Metric = VQMetricWeighted;
                    WritePrivateProfileInt( "VQF", "WeightValue", int(g_VQCompressor.m_Metric), szINIFile );

                    //...phew!!

                    //close the dialog
                    EndDialog( hDlg, TRUE );
                    break;
                }

                case IDCANCEL:
                    //close the dialog
                    EndDialog( hDlg, FALSE );
                    break;
            }
            break;

        case WM_CLOSE:
            //close the dialog
            EndDialog( hDlg, FALSE );
            break;
    }

    return FALSE;
}
