//=============================================================================
// Copyright © 2001 Point Grey Research, Inc. All Rights Reserved.
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
//
// Digiclops® is a registered Trademark of Point Grey Research Inc.
//=============================================================================
//=============================================================================
// $Id: timeSliceView.cpp,v 1.20 2007/11/16 18:58:21 soowei Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include "stdafx.h"

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "timeSliceDemo.h"
#include "timeSliceDoc.h"
#include "mainfrm.h"
#include "timeSliceView.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE( CTimeSliceView, CView )

BEGIN_MESSAGE_MAP( CTimeSliceView, CView )
//{{AFX_MSG_MAP(CTimeSliceView)
ON_COMMAND(ID_CAMERA_PROPERTIES, OnCameraProperties)
ON_UPDATE_COMMAND_UI(ID_CAMERA_PROPERTIES, OnUpdateCameraProperties)
ON_WM_KEYDOWN()
ON_WM_SETFOCUS()
ON_WM_KILLFOCUS()
ON_WM_LBUTTONUP()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CTimeSliceView::CTimeSliceView()
{
   m_iCamera = 0;
   
   m_viewType = CTimeSliceDoc::SINGLE_CAMERA;

   m_bDrawColourImage = false;
}


CTimeSliceView::~CTimeSliceView()
{
}


void 
CTimeSliceView::setCamera( int iCamera )
{
   m_iCamera = iCamera;
   
   CTimeSliceDoc* pDoc = GetDocument();
   ASSERT_VALID( pDoc );
   
   //
   // Update the window title.
   //
   char	 pszTitle[ 256 ];
   sprintf( 
      pszTitle, 
      "Camera %02d - %d", 
      m_iCamera,
      pDoc->m_arCameraInfo[ m_iCamera ].m_info.SerialNumber );
   
   GetParentFrame()->SetWindowText( pszTitle );
}


void
CTimeSliceView::setViewType( CTimeSliceDoc::ViewType type )
{
   m_viewType = type;
}


void 
CTimeSliceView::drawImage( 
			  CTimeSliceDoc::CameraInfo   cameraInfo, 
			  unsigned char*	      pImageBuffer,
			  CDC*			      pDC )
{
   if(	 cameraInfo.m_iVerticalTweak < 0 || 
      cameraInfo.m_iVerticalTweak > MAX_VERTICAL_TWEAK )
   {
      AfxMessageBox( "Problem with vertical tweak mechanism!!" );
      ASSERT( FALSE );
      return;
   }

   if( pImageBuffer != NULL )
   {   
      ::SetDIBitsToDevice(
         pDC->GetSafeHdc(),
         0, 
         0,
         cameraInfo.m_pbitmapinfo32->bmiHeader.biWidth, 
         ::abs( cameraInfo.m_pbitmapinfo32->bmiHeader.biHeight ),
         0, 
         0,
         0, 
         ::abs( cameraInfo.m_pbitmapinfo32->bmiHeader.biHeight ),
         
         //
         // Here's where we do the image vertical tweak.
         //
         pImageBuffer + 
         ( cameraInfo.m_iVerticalTweak * 
         cameraInfo.m_pbitmapinfo32->bmiHeader.biWidth),
         
         cameraInfo.m_pbitmapinfo32,
         DIB_RGB_COLORS );
   }
}


void 
CTimeSliceView::drawAlignCross( CTimeSliceDoc* pDoc, CDC* pDC )
{
   //
   // Only draw the cross on the view that currently has the focus.
   //
   if( pDoc->m_bDrawAlignCross && m_bDrawColourImage )
   {
      CPoint& point = pDoc->m_pointAlignCross;

      CRect rect;
      pDC->GetBoundsRect( &rect, 0 );

      pDC->SelectStockObject( GRAY_BRUSH );

      pDC->Rectangle( 0, point.y - 2, rect.BottomRight().x, point.y + 2 );
      pDC->Rectangle( point.x - 2, 0, point.x + 2, rect.BottomRight().y );
   }
}


void 
CTimeSliceView::OnCameraProperties() 
{
   /*
   if( m_viewType == CTimeSliceDoc::SINGLE_CAMERA )
   {
      CTimeSliceDoc* pDoc = GetDocument();
      ASSERT_VALID( pDoc );
      
      ::pgrcamguiToggleSettingsWindowState(
	 pDoc->m_arCameraInfo[ m_iCamera ].guicontext,
	 ::AfxGetApp()->m_pMainWnd->GetSafeHwnd()  );
   }
   else
   {
      AfxMessageBox( 
	 "Camera properties unavailable for panning view - click on one of"
	 " the streaming views." );
   }
   */
}


void 
CTimeSliceView::OnUpdateCameraProperties( CCmdUI* pCmdUI ) 
{
   CTimeSliceDoc* pDoc = GetDocument();
   ASSERT_VALID( pDoc );
   
   BOOL bShowing = FALSE;
   
   /*
   ::pgrcamguiGetSettingsWindowState(
      pDoc->m_arCameraInfo[ m_iCamera ].guicontext,
      &bShowing	  );
   */
   
   if( bShowing )
   {
      pCmdUI->SetCheck( 1 );
   }
   else
   {
      pCmdUI->SetCheck( 0 );
   }
}


BOOL 
CTimeSliceView::PreCreateWindow( CREATESTRUCT& cs )
{
   return CView::PreCreateWindow( cs );
}


void 
CTimeSliceView::OnDraw( CDC* pDC )
{
   CTimeSliceDoc* pDoc = GetDocument();
   ASSERT_VALID( pDoc );

   CTimeSliceDoc::CameraInfo& camera = pDoc->m_arCameraInfo[ m_iCamera ];
   
   //
   // Resize the window for the current bitmap size - this takes into account
   // the vertical tweak.
   //
   CRect vsize(
      0,
      0, 
      camera.m_pbitmapinfo32->bmiHeader.biWidth, 
      -camera.m_pbitmapinfo32->bmiHeader.biHeight );
   
   CWnd* pframe = GetParentFrame();
   
   ::AdjustWindowRectEx( &vsize, GetStyle(), FALSE, GetExStyle() );
   ::AdjustWindowRectEx( &vsize, pframe->GetStyle(), FALSE, pframe->GetExStyle() );
   
   pframe->SetWindowPos(
      NULL,
      0,
      0,
      vsize.Width(),
      vsize.Height(),
      SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );
   
   switch( m_viewType )
   {
   case CTimeSliceDoc::SINGLE_CAMERA:
      
      drawImage( camera, camera.m_imageProcessed.pData, pDC );
      drawAlignCross( pDoc, pDC );
      break;
      
   case CTimeSliceDoc::PANNING:
      {	 
	 char  pszTitle[ 256 ];
	 sprintf( 
	    pszTitle, 
	    "Panning - %d", 
	    pDoc->m_cameraInfoTimeSlice.m_info.SerialNumber );	 
	 
	 GetParentFrame()->SetWindowText( pszTitle );
	 
         drawImage( 
	    pDoc->m_cameraInfoTimeSlice, 
	    pDoc->m_cameraInfoTimeSlice.m_pTimesliceImageBuffer,
	    pDC );
 
	 //
	 // Update the frame rate in the status bar (do it here since
	 // there's only one panning view.
	 //	 
	 CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	 pFrame->updateFrameRate( 
            pDoc->m_dFrameRate,
            pDoc->m_uiMissedImages, 
            pDoc->m_uiOutOfSyncImages );

      }
      break;
      
   default:
      ASSERT( FALSE );
   }
}


void 
CTimeSliceView::OnInitialUpdate() 
{
   CView::OnInitialUpdate();
   
   CTimeSliceDoc* pDoc = GetDocument();
   ASSERT_VALID( pDoc );
   
   pDoc->initAllViews();
}

void 
CTimeSliceView::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
   CTimeSliceDoc* pDoc = GetDocument();
   ASSERT_VALID( pDoc );
   
   if( m_viewType == CTimeSliceDoc::PANNING && nChar == ' ' )
   {
      pDoc->spaceBarEvent();
   }
   
   CView::OnKeyDown( nChar, nRepCnt, nFlags );
}


void 
CTimeSliceView::OnSetFocus( CWnd* pOldWnd ) 
{
   CView::OnSetFocus( pOldWnd );

   m_bDrawColourImage = true;
}


void 
CTimeSliceView::OnKillFocus( CWnd* pNewWnd ) 
{
   CView::OnKillFocus( pNewWnd );

   m_bDrawColourImage = false;
}


void 
CTimeSliceView::OnLButtonUp( UINT nFlags, CPoint point ) 
{
   if( m_viewType == CTimeSliceDoc::SINGLE_CAMERA )
   {
      CTimeSliceDoc* pDoc = GetDocument();
      ASSERT_VALID( pDoc );

      pDoc->m_bDrawAlignCross = !pDoc->m_bDrawAlignCross;

      if( pDoc->m_bDrawAlignCross )
      {
	 pDoc->m_pointAlignCross = point;
      }
   }
   
   CView::OnLButtonUp( nFlags, point );
}


#ifdef _DEBUG
void 
CTimeSliceView::AssertValid() const
{
   CView::AssertValid();
}

void 
CTimeSliceView::Dump( CDumpContext& dc ) const
{
   CView::Dump(dc);
}

CTimeSliceDoc* 
CTimeSliceView::GetDocument() // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTimeSliceDoc)));
   return (CTimeSliceDoc*)m_pDocument;
}
#endif //_DEBUG



