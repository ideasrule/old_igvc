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
// $Id: FlyCapView.cpp,v 1.41 2006/05/10 18:03:07 mgriffin Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include "stdafx.h"

//=============================================================================
// PGR Includes
//=============================================================================
#include <pgrerror.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "FlyCap.h"
#include "FlyCapDoc.h"
#include "FlyCapView.h"
#include "MainFrm.h"

//=============================================================================
// Macro Definitions
//=============================================================================
#define PGR_PROJECT_NAME "PGRFlyCap"
#define PGR_FILE_NAME    "$RCSfile: FlyCapView.cpp,v $"
#define PGR_FILE_VERSION "$Revision: 1.41 $"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE( CFlyCapView, CScrollView )

BEGIN_MESSAGE_MAP( CFlyCapView, CScrollView )
//{{AFX_MSG_MAP(CFlyCapView)
ON_WM_ERASEBKGND()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CFlyCapView::CFlyCapView()
{   
   m_hHand = (HCURSOR)LoadImage( 
      GetModuleHandle( NULL ), 
      MAKEINTRESOURCE( IDC_CURSOR_GRAB ), 
      IMAGE_CURSOR, 
      0, 
      0, 
      LR_MONOCHROME );
   m_hArrow = (HCURSOR)LoadImage( 
      NULL, 
      MAKEINTRESOURCE( IDC_ARROW ),
      IMAGE_CURSOR, 
      0, 
      0,
      LR_MONOCHROME );
}


CFlyCapView::~CFlyCapView()
{
   DestroyCursor( m_hHand );
   DestroyCursor( m_hArrow );
}


void 
CFlyCapView::newImageSize()
{
   CFlyCapDoc* pDoc = GetDocument();

   int iX;
   int iY;
   pDoc->getImageSize( &iX, &iY );

   SetScrollSizes( MM_TEXT, CSize( iX, iY ) );
   ((CMainFrame*)GetParentFrame())->resizeToMax();
}


void 
CFlyCapView::OnDraw( CDC* pDC )
{
   CFlyCapDoc* pDoc = GetDocument();
   CMainFrame* pframe = (CMainFrame*)GetParentFrame();

   if( pDoc->m_bNewImageSize )
   {
      if( pframe->m_bFullScreenViewingMode )
      {
         pframe->exitFullScreenMode();
      }

      newImageSize();

      pDoc->m_bNewImageSize = false;
   }


   if( pframe->m_bFullScreenViewingMode )
   {
      int nScreenWidth  = ::GetSystemMetrics( SM_CXSCREEN );
      int nScreenHeight = ::GetSystemMetrics( SM_CYSCREEN );

      pframe->SetWindowPos(
	 NULL,
	 0,
	 0,
	 nScreenWidth,
	 nScreenHeight,
	 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );

      ::StretchDIBits(
	 pDC->GetSafeHdc(),
	 0, 
	 0,
	 nScreenWidth, 
         nScreenHeight,
	 0,                     // x-coord of source upper-left corner
	 0,                     // y-coord of source upper-left corner
	 pDoc->m_bitmapInfo.bmiHeader.biWidth, 
	 ::abs( pDoc->m_bitmapInfo.bmiHeader.biHeight ),
	 pDoc->m_imageProcessed.pData, 
	 &pDoc->m_bitmapInfo, 
	 DIB_RGB_COLORS,
	 SRCCOPY );
   }
   else
   {
      // draw the bitmap to the screen.
      if( ::SetDIBitsToDevice(
         pDC->GetSafeHdc(),
         0, 
         0,
         pDoc->m_bitmapInfo.bmiHeader.biWidth, 
         ::abs( pDoc->m_bitmapInfo.bmiHeader.biHeight ),
         0, 
         0,
         0, 
         ::abs( pDoc->m_bitmapInfo.bmiHeader.biHeight ),
         pDoc->m_imageProcessed.pData, 
         &pDoc->m_bitmapInfo, 
         DIB_RGB_COLORS )  == 0 )
      {
         // error.
      }
   }

   setTitleBar();
   pDoc->m_framerateDisplay.newFrame();
}


#ifdef _DEBUG
CFlyCapDoc* 
CFlyCapView::GetDocument() // non-debug version is inline
{
   ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CFlyCapDoc ) ) );
   return (CFlyCapDoc*)m_pDocument;
}
#endif //_DEBUG


void 
CFlyCapView::OnInitialUpdate() 
{
   CView::OnInitialUpdate();
   
   setTitleBar();
   newImageSize();

   CFlyCapDoc* pDoc = GetDocument();
   ASSERT_VALID( pDoc );
}  


BOOL 
CFlyCapView::OnEraseBkgnd( CDC* /* pDC */ ) 
{
   // Prevent flash on resize, etc.
   return TRUE;
}


void 
CFlyCapView::setTitleBar()
{
   CFlyCapDoc* pDoc = GetDocument();
   CMainFrame* pFrame = (CMainFrame*)GetParentFrame();

   ASSERT_VALID( pDoc );
   ASSERT_VALID( pFrame );

   char pszTitle[ 128 ];

   switch( pDoc->m_CameraInfoMode )
   {
   case MODEL_AND_SERIAL:
      sprintf( 
         pszTitle,
         "PGR FlyCap - %s (%u)",
         pDoc->m_cameraInfo.pszModelName,
         pDoc->m_cameraInfo.SerialNumber );
      break;
   case DISPLAYED_FPS:
      sprintf( 
         pszTitle,
         "PGR FlyCap - %3.2f Hz",
         pDoc->m_framerateDisplay.getFrameRate() );
      break;
   case PROCESSED_FPS:
      sprintf( 
         pszTitle,
         "PGR FlyCap - %3.2f Hz",
         pDoc->m_framerateGrab.getFrameRate() );
      break;
   case REQUESTED_FPS:
      if( pFrame->m_fRequestedFramerate != -1.0f )
      {
         sprintf( 
            pszTitle,
            "PGR FlyCap - %3.2f Hz",
            pFrame->m_fRequestedFramerate );
      }
      else
      {
         sprintf( 
            pszTitle,
            "PGR FlyCap - n/a" );
      }
      break;
   }
   pFrame->SetWindowText( pszTitle );
}


void
CFlyCapView::OnLButtonDown( UINT /*nFlags*/, CPoint /*point*/ )
{
   SetCapture();
   SetCursor( m_hHand );
}


void
CFlyCapView::OnLButtonUp( UINT /*nFlags*/, CPoint /*point*/ )
{
   ReleaseCapture();
   SetCursor( m_hArrow );   
}


void 
CFlyCapView::OnMouseMove( UINT nFlags, CPoint point )
{
   static CPoint prevClick = point;
   if( MK_LBUTTON & nFlags )
   {      
      int horzLimit = GetScrollLimit( SB_HORZ );
      int vertLimit = GetScrollLimit( SB_VERT );
      
      int horzPos = GetScrollPos( SB_HORZ );
      int vertPos = GetScrollPos( SB_VERT );

      if( GetStyle() & WS_HSCROLL )
      {
         horzPos += (prevClick.x - point.x);
         if( horzPos < 0 )
            horzPos = 0;
         if( horzPos > horzLimit )
            horzPos = horzLimit;
         SetScrollPos( SB_HORZ, horzPos );
      }

      if( GetStyle() & WS_VSCROLL )
      {
         vertPos += (prevClick.y - point.y);
         if( vertPos < 0 )
            vertPos = 0;
         if( vertPos > vertLimit )
            vertPos = vertLimit;      
         SetScrollPos( SB_VERT, vertPos );
      }

      RedrawWindow();
   }
   prevClick = point;
}

