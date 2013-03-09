//=============================================================================
// Copyright © 2004 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with Point Grey Research, Inc. (PGR).
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: MainFrm.cpp,v 1.41 2007/08/28 21:07:50 demos Exp $
//=============================================================================

//=============================================================================
// Includes
//=============================================================================
#include "stdafx.h"
#include "FlyCap.h"
#include "FlyCapDoc.h"
#include "FlyCapView.h"
#include "PGRFlyCapture.h"
#include "pgrflycapturegui.h"


#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Uncomment to make the application start in full screen mode.
//
//#define START_FULL_SCREEN_MODE


IMPLEMENT_DYNCREATE( CMainFrame, CFrameWnd )

BEGIN_MESSAGE_MAP( CMainFrame, CFrameWnd )
//{{AFX_MSG_MAP(CMainFrame)
ON_WM_DESTROY()
ON_WM_SHOWWINDOW()
ON_WM_CREATE()
ON_WM_TIMER()
ON_COMMAND(ID_FILE_FULLSCREENMODE, OnFileFullScreenMode)
ON_WM_GETMINMAXINFO()
ON_COMMAND(ID_HIDE_TOOLBAR, OnHideToolbar)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


static UINT indicators[] =
{
   ID_INDICATOR_FRAME_RATE,         // position for frame rate display.
   ID_INDICATOR_IMAGEINFO,          // position for image info display.
   ID_INDICATOR_BUSEVENT,           // position for bus event display.
};


CMainFrame::CMainFrame()
{
   m_bFullScreenViewingMode = false;

   m_sizeMax.cx = 0;
   m_sizeMax.cy = 0;
   m_hTimer = NULL;
}


CMainFrame::~CMainFrame()
{
   m_ilToolBarHigh.DeleteImageList();
}


LRESULT
CMainFrame::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{   
   //
   // This function should handle the following messages:
   //
   // FLYCAPTURE_WINDOWS_MESSAGE_BUS_RESET
   // FLYCAPTURE_WINDOWS_MESSAGE_DEVICE_ARRIVAL
   // FLYCAPTURE_WINDOWS_MESSAGE_DEVICE_REMOVAL
   // FLYCAPTURE_WINDOWS_MESSAGE_RECORDING
   // FLYCAPTURE_WINDOWS_MESSAGE_SAVING
   // FLYCAPTURE_WINDOWS_MESSAGE_STOPPED_RECORDING
   //
   if( message == FLYCAPTURE_WINDOWS_MESSAGE_BUS_RESET )
   {
      updateBusEvent( "Bus Reset" );
      return 0;
   }
   else if( message == FLYCAPTURE_WINDOWS_MESSAGE_DEVICE_ARRIVAL )
   {
      updateBusEvent( "Device Arrival" );
      return 0;
   }
   else if( message == FLYCAPTURE_WINDOWS_MESSAGE_DEVICE_REMOVAL )
   {
      updateBusEvent( "Device Removal" );
      return 0;
   }
   else if( message == FLYCAPTURE_WINDOWS_MESSAGE_RECORDING )
   {
      updateBusEvent( "Recording ..." );
      return 0;
   }
   else if( message == FLYCAPTURE_WINDOWS_MESSAGE_SAVING )
   {
      updateBusEvent( "Saving ..." );
      return 0;
   }
    else if( message == FLYCAPTURE_WINDOWS_MESSAGE_STOPPED_RECORDING )
   {
      updateBusEvent( "" );
      return 0;
   }
   else
   {
      return CFrameWnd::DefWindowProc( message, wParam, lParam );
   }
}


void
CMainFrame::OnTimer(UINT_PTR nIDEvent) 
{      
   const CFlyCapDoc* pDoc = (CFlyCapDoc*)GetActiveDocument();      
   if( pDoc != NULL )
   {            
      updateFrameRate();
      updateImageInfo();
   }
   CFrameWnd::OnTimer(nIDEvent);
}


void
CMainFrame::OnDestroy()
{
   if( m_hTimer != NULL )
   {
      ::KillTimer( m_hWnd, m_hTimer );
   }
   CFrameWnd::OnDestroy();
}


void
CMainFrame::OnShowWindow( BOOL bShow, UINT nStatus )
{
   if( bShow )
   {
      // 
      // Initialized the View menu's title bar selection
      //
      CFlyCapDoc* pDoc = (CFlyCapDoc*) GetActiveDocument();
      if( pDoc != NULL )
      {
         UINT nID = 0;
         switch( pDoc->m_CameraInfoMode )
         {
         case PROCESSED_FPS:
            nID = ID_VIEW_TB_PROCESSEDFPS;
            break;
         case DISPLAYED_FPS:
            nID = ID_VIEW_TB_DISPLAYEDFPS;
            break;
         case REQUESTED_FPS:
            nID = ID_VIEW_TB_REQUESTEDFPS;
            break;
         case MODEL_AND_SERIAL:
            nID = ID_VIEW_TB_MODELANDSERIAL;
            break;
         }
         GetMenu()->CheckMenuRadioItem( 
            ID_VIEW_TB_REQUESTEDFPS, 
            ID_VIEW_TB_MODELANDSERIAL,
            nID,
            MF_BYCOMMAND );            
      }
   }
   CFrameWnd::OnShowWindow( bShow, nStatus );
}


int 
CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
   if( CFrameWnd::OnCreate( lpCreateStruct ) == -1 )
   {
      return -1;
   }

   if( !m_wndToolBar.CreateEx(
      this, 
      TBSTYLE_FLAT, 
      WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | 
         CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC ) ||
      !m_wndToolBar.LoadToolBar( IDR_MAINFRAME ) )
   {
      TRACE0("Failed to create toolbar\n");
      return -1;      // fail to create
   }
   

   //
   // This chunk of code handles the toolbar transparency.
   // We manually replace the transparent colour with the system
   // button colour since Windows doesn't seem to provide a method
   // to do so with 24-bit colour.
   //
   HGDIOBJ hBitmap = (HGDIOBJ) LoadImage(
      AfxFindResourceHandle( MAKEINTRESOURCE( IDB_TOOLBARHI ), RT_BITMAP),
      MAKEINTRESOURCE( IDB_TOOLBARHI ),
      IMAGE_BITMAP,
      0,
      0,
      LR_CREATEDIBSECTION );
   CBitmap bmpToolBarHigh;
   bmpToolBarHigh.Attach( hBitmap );
   BITMAP bmpInfo;
   bmpToolBarHigh.GetBitmap( &bmpInfo );      
   UINT uiNumPix = bmpInfo.bmHeight * bmpInfo.bmWidth;
   DIBSECTION ds;
   bmpToolBarHigh.GetObject( sizeof (DIBSECTION), &ds );   
   RGBTRIPLE* pRGB = (RGBTRIPLE*)( ds.dsBm.bmBits );   
   COLORREF fill = GetSysColor( COLOR_BTNFACE );
   COLORREF target = RGB( 236, 233, 216 );   
   for( UINT i = 0; i < uiNumPix; i++ )
   {      
      COLORREF curr = RGB( pRGB[ i ].rgbtRed, pRGB[ i ].rgbtGreen, pRGB[ i ].rgbtBlue );
      if( curr == target )
      {
         pRGB[ i ].rgbtRed = GetRValue( fill );
         pRGB[ i ].rgbtBlue = GetBValue( fill );
         pRGB[ i ].rgbtGreen = GetGValue( fill );
      }
   }
   m_ilToolBarHigh.Create( 16, 15, ILC_COLOR24, m_wndToolBar.GetCount(), 0 );
   m_ilToolBarHigh.Add( &bmpToolBarHigh, RGB( 0, 0, 0 ) );
   m_wndToolBar.GetToolBarCtrl().SetImageList( &m_ilToolBarHigh );

   if( !m_wndStatusBar.Create(
      this, 
      WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, ID_MY_STATUS_BAR ) ||
      !m_wndStatusBar.SetIndicators(
         indicators, sizeof(indicators)/sizeof(UINT) ) )
   {
      TRACE0( "Failed to create status bar\n" );
      return -1;      // fail to create
   }
   
   m_CameraInfoMode = PROCESSED_FPS;   
   GetMenu()->CheckMenuRadioItem( 
      ID_VIEW_SB_REQUESTEDFPS, 
      ID_VIEW_SB_DISPLAYEDFPS,
      ID_VIEW_SB_PROCESSEDFPS,
      MF_BYCOMMAND );

   m_ImageInfoMode = CURSOR;
   GetMenu()->CheckMenuRadioItem( 
      ID_VIEW_SB_TIMESTAMP, 
      ID_VIEW_SB_CURSOR,
      ID_VIEW_SB_CURSOR,
      MF_BYCOMMAND );  

   m_fRequestedFramerate = 0.0;

#ifdef START_FULL_SCREEN_MODE
   enterFullScreenMode();
#endif

   if( m_hTimer == NULL )
   {
      m_hTimer = ::SetTimer( m_hWnd, 123456, 100, (TIMERPROC) NULL );   
   }

   return 0;
}


BOOL 
CMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
   // remove the document title.
   cs.style &= ~FWS_ADDTOTITLE;

   if( !CFrameWnd::PreCreateWindow( cs ) )
   {
      return FALSE;
   }
  
   return TRUE;
}


BOOL
CMainFrame::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
   const CFlyCapDoc* pDoc = (CFlyCapDoc*)GetActiveDocument();

   NMHDR* pNMHDR = (NMHDR*) lParam;

   //
   // This catches clicks to the status bar fields.
   //
   if( 
      m_wndStatusBar && 
      pDoc != NULL && 
      pNMHDR->hwndFrom == m_wndStatusBar.m_hWnd && 
      pNMHDR->code == NM_CLICK )      
   {
      NMMOUSE* pNMMOUSE = (NMMOUSE*) pNMHDR;
      POINT pt = pNMMOUSE->pt;
      
      CRect itemRect;
      for( int index = 0; index < m_wndStatusBar.GetCount(); index++ )
      {
         m_wndStatusBar.GetItemRect( index, &itemRect );
         if( 
            pt.x > itemRect.left && 
            pt.x < itemRect.right &&
            pt.y > itemRect.top &&
            pt.y < itemRect.bottom )
         {
            //
            // Found the correct item.
            //
            unsigned long* pulMode;
            switch( index )
            {
            case 0:
               // Framerate
               pulMode = (unsigned long*) &m_CameraInfoMode;
               (*pulMode)++;
               if( (*pulMode) >= NUM_CAMERAINFO_MODES )
               {
                  (*pulMode) = 0;
               }
               
               //
               // Update the menu selection
               //
               switch( (*pulMode) )
               {
               case REQUESTED_FPS:
                  GetMenu()->CheckMenuRadioItem( 
                     ID_VIEW_SB_REQUESTEDFPS, 
                     ID_VIEW_SB_DISPLAYEDFPS,
                     ID_VIEW_SB_REQUESTEDFPS,
                     MF_BYCOMMAND );
                  break;
               case PROCESSED_FPS:
                  GetMenu()->CheckMenuRadioItem( 
                     ID_VIEW_SB_REQUESTEDFPS, 
                     ID_VIEW_SB_DISPLAYEDFPS,
                     ID_VIEW_SB_PROCESSEDFPS,
                     MF_BYCOMMAND );
                  break;
               case DISPLAYED_FPS:
                  GetMenu()->CheckMenuRadioItem( 
                     ID_VIEW_SB_REQUESTEDFPS, 
                     ID_VIEW_SB_DISPLAYEDFPS,
                     ID_VIEW_SB_DISPLAYEDFPS,
                     MF_BYCOMMAND );
                  break;
               }
               break;
               case 1:
                  // Image Info
                  pulMode = (unsigned long*) &m_ImageInfoMode;
                  (*pulMode)++;
                  if( (*pulMode) >= NUM_IMAGEINFO_MODES )
                  {
                     (*pulMode) = 0;
                  }
                  
                  //
                  // Update the menu selection
                  //
                  switch( (*pulMode) )
                  {
                  case TIMESTAMP:
                     GetMenu()->CheckMenuRadioItem( 
                        ID_VIEW_SB_TIMESTAMP, 
                        ID_VIEW_SB_CURSOR,
                        ID_VIEW_SB_TIMESTAMP,
                        MF_BYCOMMAND );  
                     break;
                  case CURSOR:
                     GetMenu()->CheckMenuRadioItem( 
                        ID_VIEW_SB_TIMESTAMP, 
                        ID_VIEW_SB_CURSOR,
                        ID_VIEW_SB_CURSOR,
                        MF_BYCOMMAND );  
                     break;
                  }
                  break;
            }               
            return TRUE;
         }
      }    
   }   

   return CFrameWnd::OnNotify( wParam, lParam, pResult );
}

void
CMainFrame::updateFrameRate()
{     
   static int updateCount = 0;

   const CFlyCapDoc* pDoc = (CFlyCapDoc*)GetActiveDocument();   
   if( m_wndStatusBar && pDoc != NULL )
   {
      char  pszFrameRate[ 64 ];

      //
      // Since the timer fires every 100 ms, we're only updating
      // the requested frame rate once a second to cut down on 
      // the number of register reads.
      //
      if( updateCount % 10 == 0 ){
         m_fRequestedFramerate = pDoc->getRequestedFramerate();
      }

      switch( m_CameraInfoMode )
      {
      case PROCESSED_FPS:
         sprintf( pszFrameRate, "Processed FPS: %3.2f Hz", pDoc->m_framerateGrab.getFrameRate() );
         break;
      case DISPLAYED_FPS:
         sprintf( pszFrameRate, "Displayed FPS: %3.2f Hz", pDoc->m_framerateDisplay.getFrameRate() );
         break;
      case REQUESTED_FPS:
         // Camera which do not support the FRAME_RATE register (0x83C) will
         // return -1.0
         if( m_fRequestedFramerate != -1.0f )
         {
            sprintf( pszFrameRate, "Requested FPS: %3.2f Hz", m_fRequestedFramerate );
         }
         else
         {
            sprintf( pszFrameRate, "Requested FPS: n/a" );
         }
         break;
      default:
         sprintf( pszFrameRate, "Unavailable" );
         break;
      }
      m_wndStatusBar.SetPaneText( 0, pszFrameRate );
      updateCount++;
   }
}


void
CMainFrame::updateImageInfo()
{
   const CFlyCapDoc* pDoc = (CFlyCapDoc*)GetActiveDocument();
   CView* pView = (CView*)GetActiveView();   
   
   if( m_wndStatusBar &&
      pDoc != NULL && 
      pView != NULL )
   {
      char pszText[ 64 ];
      
      if( m_ImageInfoMode == TIMESTAMP )
      {
         // Setup the timestamp information
         FlyCaptureTimestamp timeStamp = pDoc->m_imageRaw.timeStamp;  
         
#if defined (WIN64)  
         __time64_t  tmpTime = timeStamp.ulSeconds;
         char* pszTemp = ::_ctime64( &tmpTime );
#elif defined (WIN32)
         time_t lTemp = timeStamp.ulSeconds;      
         char* pszTemp = ::ctime( &lTemp );
#else
#error ** No time conversion **
#endif
         if( pszTemp == NULL )
         {
            return;
         }
         int iMilliSec = timeStamp.ulMicroSeconds / 1000;
         sprintf(
            pszText,
            "%.19s.%.03d %s (%03u,%04u)\n",
            pszTemp,
            iMilliSec,
            &pszTemp[ 20 ],
            timeStamp.ulCycleSeconds,
            timeStamp.ulCycleCount );         
      }
      else if( m_ImageInfoMode == CURSOR )
      {
         // Setup the cursor and image information
         CRect rect;
         CPoint pt;
         COLORREF cr;
         int iWidth = 0;
         int iHeight = 0;
         int iSBOffsetX = 0; // the offset of the horizontal scrollbar
         int iSBOffsetY = 0; // the offset of the vertical scrollbar

         // get the position of the scroll bars.
         // used to calculate the co-ordinates of the image.
         iSBOffsetX = pView->GetScrollPos(SB_HORZ);
         iSBOffsetY = pView->GetScrollPos(SB_VERT);
         
         pDoc->getImageSize( &iWidth, &iHeight );
         CDC* pDC = pView->GetDC();
         pDC->GetClipBox( &rect );         
         GetCursorPos( &pt );                  
         pView->ScreenToClient( &pt );
         cr = GetPixel( pDC->GetSafeHdc(), pt.x, pt.y );
         pView->ReleaseDC( pDC );

         // Check that this window is active and 
         // that the cursor is within bounds of the clipping rect
         if( this == GetActiveWindow() &&
            pt.x >= 0 && pt.x < rect.Width() && pt.y >= 0 && pt.y < rect.Height() )
         {
            sprintf( pszText, "Image(%dx%d) Cursor(%d,%d) RGB(%u,%u,%u)", 
               iWidth, 
               iHeight, 
               pt.x + iSBOffsetX,
               pt.y + iSBOffsetY,
               cr & 0xFF, 
               (cr & 0xFF00) >> 8, 
               (cr & 0xFF0000) >> 16 );
         }
         else
         {
            sprintf( pszText, "Image(%dx%d) Cursor(n/a) RGB(n/a)", 
               iWidth, 
               iHeight );
         }
      }
      else
      {
         // Error
         sprintf( pszText, "Unavailable" );       
      }
      m_wndStatusBar.SetPaneText( 1, pszText );
   }
}

void
CMainFrame::updateBusEvent( char* pszEvent )
{    
   if( m_wndStatusBar )
   {
      m_wndStatusBar.SetPaneText( 2, pszEvent );      
   }
}

void 
CMainFrame::OnFileFullScreenMode() 
{
   if( m_bFullScreenViewingMode )
   {
      exitFullScreenMode();
   }
   else
   {
      enterFullScreenMode();
   }
}


void
CMainFrame::enterFullScreenMode()
{
   ShowWindow( SW_SHOWMAXIMIZED );
   ModifyStyle( 
      WS_BORDER | WS_SIZEBOX | WS_CAPTION | WS_SYSMENU, 
      WS_MAXIMIZE, 
      SWP_FRAMECHANGED );
   SetMenu( NULL ); 
   
   m_wndStatusBar.ShowWindow( SW_HIDE );
   m_wndToolBar.ShowWindow( SW_HIDE );
   
   m_bFullScreenViewingMode = true;
}


void 
CMainFrame::exitFullScreenMode()
{
   ShowWindow( SW_SHOWNORMAL );
   ModifyStyle( 
      WS_MAXIMIZE, 
      WS_BORDER | WS_SIZEBOX | WS_CAPTION | WS_SYSMENU, 
      SWP_FRAMECHANGED );

   CMenu pMenu;
   pMenu.LoadMenu( IDR_MAINFRAME );
   SetMenu( &pMenu ); 
   
   m_wndStatusBar.ShowWindow( SW_SHOW );
   m_wndToolBar.ShowWindow( SW_SHOW );
   
   ShowControlBar( &m_wndToolBar, true, false );
   
   m_bFullScreenViewingMode = false;

   resizeToMax();
}


void 
CMainFrame::OnGetMinMaxInfo( MINMAXINFO* lpMMI ) 
{
   if( !m_bFullScreenViewingMode )
   {
      if( m_sizeMax.cx < lpMMI->ptMaxSize.x )
      {
         lpMMI->ptMaxTrackSize.x = m_sizeMax.cx;
      }
      
      if( m_sizeMax.cy < lpMMI->ptMaxSize.y )
      {
         lpMMI->ptMaxTrackSize.y = m_sizeMax.cy;
      }
   }
   
   CFrameWnd::OnGetMinMaxInfo( lpMMI );
}


void
CMainFrame::calculateMaxSize()
{
   const CFlyCapDoc* pDoc = (CFlyCapDoc*)GetActiveDocument();

   if( pDoc == NULL )
   {
      return;
   }
   
   int iX;
   int iY;

   pDoc->getImageSize( &iX, &iY );

   RECT rect;
   rect.top = 0;
   rect.left = 0;
   rect.bottom = iY;
   rect.right = iX;
   
   GetActiveView()->CalcWindowRect( &rect, adjustOutside );

   if( m_wndToolBar.IsWindowVisible() )
   {
      RECT rectTool;
      m_wndToolBar.GetWindowRect( &rectTool );
      rect.bottom += rectTool.bottom - rectTool.top;
   }

   if( m_wndStatusBar.IsWindowVisible() )
   {
      RECT rectStatus;
      m_wndStatusBar.GetWindowRect( &rectStatus );
      rect.bottom += rectStatus.bottom - rectStatus.top;
   }

   ::AdjustWindowRectEx( &rect, GetStyle(), TRUE, GetExStyle() );

   m_sizeMax.cx = rect.right - rect.left;
   m_sizeMax.cy = rect.bottom - rect.top;

}


void 
CMainFrame::resizeToMax()
{
   calculateMaxSize();

   SetWindowPos( 
      NULL, 
      0, 
      0, 
      m_sizeMax.cx, 
      m_sizeMax.cy, 
      SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER );
}

void
CMainFrame::OnHideToolbar()
{
   if( m_wndToolBar.IsWindowVisible() )
   {
      ShowControlBar( &m_wndToolBar, false, false );
      resizeToMax();
   }
   else
   {
      ShowControlBar( &m_wndToolBar, true, false );
      resizeToMax();
   }
}



