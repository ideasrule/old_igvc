//=============================================================================
// Copyright © 2005 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with PGR.
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: PGRImageView.cpp,v 1.3 2007/10/24 01:28:09 mgriffin Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <afxwin.h>

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRImageView.h"
#include "PGRResource.h"
#include "PGRMainFrm.h"

IMPLEMENT_DYNCREATE( CPGRImageView, CView )

BEGIN_MESSAGE_MAP( CPGRImageView, CView )
//{{AFX_MSG_MAP(CPGRImageView)
ON_CBN_SELCHANGE(PGRRES_IDC_COMBO_CAMERA, OnSelchangeIdcComboCamera)
ON_CBN_SELCHANGE(PGRRES_IDC_COMBO_IMAGE, OnSelchangeIdcComboImage)
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CPGRImageView::CPGRImageView()
{
   //m_nImageType = IMAGE_RIGHT_RAW;
}


CPGRImageView::~CPGRImageView()
{
}


BOOL 
CPGRImageView::PreCreateWindow( CREATESTRUCT& cs )
{
   cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

   return CView::PreCreateWindow( cs );
}


void 
CPGRImageView::OnDraw( CDC* pDC )
{
   CPGRStereoDoc* pDoc = (CPGRStereoDoc*)GetDocument();
   ASSERT_VALID( pDoc );

   //
   // Get the current image we're displaying
   //
   PGRBitmapTriclops* pbitmap = pDoc->getBitmap( m_nImageType );
   ASSERT( pbitmap != NULL );   

   CWnd* pframe = GetParentFrame();
   
   //
   // Calculate the difference between the window and client areas.
   //
   int  iDiffWidth;
   int  iDiffHeight;

   RECT Rect;
   pframe->GetWindowRect( &Rect );
   iDiffWidth  = Rect.right  - Rect.left;
   iDiffHeight = Rect.bottom - Rect.top;

   pframe->GetClientRect( &Rect );
   iDiffWidth  -= Rect.right  - Rect.left;
   iDiffHeight -= Rect.bottom - Rect.top;   

   int iWidth = Rect.right - Rect.left; 
   int iHeight;  
   
   //
   // Fix the aspect ratio using the width.
   //

   int origWidth;
   int origHeight;
   pbitmap->getImageDimensions(&origWidth, &origHeight);

   float fAspectRation = (float)origWidth / (float)origHeight;
   iWidth -= iWidth % 8;
   iHeight = (int)( (iWidth / fAspectRation) + 0.5 );

   //
   // Draw image
   //
   pbitmap->paintToDevice( pDC->GetSafeHdc(), 0, 0, iWidth, iHeight );   

   //
   // Adjust the width/height to include title bar, etc.
   //
   iWidth += iDiffWidth;
   iHeight += iDiffHeight;
      
   //
   // Resize the window.
   //
   pframe->SetWindowPos(
      NULL,
      0,
      0,
      iWidth,
      iHeight,
      SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );

   //
   //  call inheriting class's customDraw(), if it exists.
   //
   customDraw( pDC );
}


void 
CPGRImageView::OnMouseMove( UINT nFlags, CPoint point ) 
{
   CView::OnMouseMove( nFlags, point );

   CPGRStereoDoc* pDoc = (CPGRStereoDoc*)GetDocument();
   ASSERT_VALID( pDoc );

   CPGRMainFrame* pFrame = (CPGRMainFrame*)AfxGetApp()->m_pMainWnd;
   ASSERT_VALID( pFrame );

   PGRBitmapTriclops* pbitmap = pDoc->getBitmap( m_nImageType );

   char	 pszText[ 64 ];

   unsigned char ucRed    = 0;
   unsigned char ucGreen  = 0;
   unsigned char ucBlue   = 0;

   //
   // Map the pixels if window is streched.
   //
   RECT Rect;
   GetParentFrame()->GetClientRect( &Rect );

   float fMapPointx = (float)(Rect.right - Rect.left);
   float fMapPointy = (float)(Rect.bottom - Rect.top);

   switch ( pDoc->getResolution() )
   {  
   case STEREO_160x120:
      fMapPointx = 160 / fMapPointx;
      fMapPointy = 120 / fMapPointy;
      break;

   case STEREO_256x192:
      fMapPointx = 256 / fMapPointx;
      fMapPointy = 192 / fMapPointy;
      break;
      
   case STEREO_320x240:
      fMapPointx = 320 / fMapPointx;
      fMapPointy = 240 / fMapPointy;
      break;

   case STEREO_400x300:
      fMapPointx = 400 / fMapPointx;
      fMapPointy = 300 / fMapPointy;
      break;

   case STEREO_512x384:
      fMapPointx = 512 / fMapPointx;
      fMapPointy = 384 / fMapPointy;
      break;
      
   case STEREO_640x480:
      fMapPointx = 640 / fMapPointx;
      fMapPointy = 480 / fMapPointy;
      break;

   case STEREO_800x600:
      fMapPointx = 800 / fMapPointx;
      fMapPointy = 600 / fMapPointy;
      break;

   case STEREO_1024x768:
      fMapPointx = 1024 / fMapPointx;
      fMapPointy = 768  / fMapPointy;
      break;

   case STEREO_1280x960:
      fMapPointx = 1280 / fMapPointx;
      fMapPointy = 960 / fMapPointy;
      break;
   
   default:
      fMapPointx = 640 / fMapPointx;
      fMapPointy = 480 / fMapPointy;
   }

   long pointx = (long)(point.x * fMapPointx);
   long pointy = (long)(point.y * fMapPointy);
   
   switch( m_nImageType )
   {
      
   //
   // Disparity information.
   //      
   case IMAGE_DISPARITY:
      {
	 double	  dDisparity  = 0;
	 double   dX	      = 0;
	 double   dY	      = 0;
	 double   dZ	      = 0;

	 if( pDoc->getPointData( pointy, pointx, &dDisparity, &dX, &dY, &dZ ) )
	 {
	    sprintf( 
	       pszText, 
	       "RCD(%d,%d,%3.2f) = XYZ(%1.3f,%1.3f,%1.3f)",
	       pointy,
	       pointx,
	       dDisparity,
	       dX,
	       dY,
	       dZ );
	 }
	 else
	 {
	    sprintf( pszText, "Invalid" );
	 }

	 pFrame->updateStatusBar( CPGRMainFrame::STEREO, pszText );

      }      
      //
      // no break - Fall through to update the appearance data
      //

   case IMAGE_RIGHT_RAW:
   case IMAGE_CENTER_RAW:
   case IMAGE_LEFT_RAW:

   case IMAGE_RIGHT_RECTIFIED:
   case IMAGE_CENTER_RECTIFIED:
   case IMAGE_LEFT_RECTIFIED:

   case IMAGE_RIGHT_COLOR_RECTIFIED:
   case IMAGE_CENTER_COLOR_RECTIFIED:
   case IMAGE_LEFT_COLOR_RECTIFIED:

   case IMAGE_RIGHT_EDGE:
   case IMAGE_CENTER_EDGE:
   case IMAGE_LEFT_EDGE:
      pbitmap->getPixel( pointy, pointx, &ucRed, &ucGreen, &ucBlue );
      break;
      
   default:
      ASSERT( FALSE );
      return;      
   }

   sprintf( 
      pszText, 
      "RC(%d,%d) = RGB(%d,%d,%d)",
      pointy,
      pointx,
      ucRed,
      ucGreen,
      ucBlue );

   pFrame->updateStatusBar( CPGRMainFrame::RGB, pszText );

}


void 
CPGRImageView::OnLButtonUp( UINT nFlags, CPoint point ) 
{
   ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CPGRStereoDoc )) );
   CPGRStereoDoc* pDoc = (CPGRStereoDoc*)GetDocument();
   ASSERT_VALID( pDoc );

   //
   // Map the pixels if window is streched.
   //
   RECT Rect;
   GetParentFrame()->GetClientRect( &Rect );

   float fMapPointx = (float)(Rect.right - Rect.left);
   float fMapPointy = (float)(Rect.bottom - Rect.top);

   switch ( pDoc->getResolution() )
   {  
   case STEREO_160x120:
      fMapPointx = 160 / fMapPointx;
      fMapPointy = 120 / fMapPointy;
      break;

   case STEREO_256x192:
      fMapPointx = 256 / fMapPointx;
      fMapPointy = 192 / fMapPointy;
      break;
   
   case STEREO_320x240:
      fMapPointx = 320 / fMapPointx;
      fMapPointy = 240 / fMapPointy;
      break;

   case STEREO_400x300:
      fMapPointx = 400 / fMapPointx;
      fMapPointy = 300 / fMapPointy;
      break;

   case STEREO_512x384:
      fMapPointx = 512 / fMapPointx;
      fMapPointy = 384 / fMapPointy;
      break;
   
   case STEREO_640x480:
      fMapPointx = 640 / fMapPointx;
      fMapPointy = 480 / fMapPointy;
      break;

   case STEREO_800x600:
      fMapPointx = 800 / fMapPointx;
      fMapPointy = 600 / fMapPointy;
      break;
      
   case STEREO_1024x768:
      fMapPointx = 1024 / fMapPointx;
      fMapPointy = 768  / fMapPointy;
      break;

   case STEREO_1280x960:
      fMapPointx = 1280 / fMapPointx;
      fMapPointy = 960 / fMapPointy;
      break;

   default:
      fMapPointx = 640 / fMapPointx;
      fMapPointy = 480 / fMapPointy;
   }

   long pointx = (long)(point.x * fMapPointx);
   long pointy = (long)(point.y * fMapPointy);

   switch( m_nImageType )
   {
   case IMAGE_DISPARITY:
      {
	 double	  dDisparity  = 0;
	 double   dX	      = 0;
	 double   dY	      = 0;
	 double   dZ	      = 0;

	 if( pDoc->getPointData( pointy, pointx, &dDisparity, &dX, &dY, &dZ ) )
	 {
	    //
	    // Point is valid, update origin of rotation.
	    //
	    CString csMsg;
	    csMsg.Format( 
	       "Setting 3d origin of rotation to (%1.2f, %1.2f, %1.2f)",
	       dX,
	       dY,
	       dZ );

	    ::AfxMessageBox( csMsg );

	    pDoc->updatePointOfRotation( dX, dY, dZ );
	 }

      }      
      break;

   case IMAGE_RIGHT_RAW:
   case IMAGE_CENTER_RAW:
   case IMAGE_LEFT_RAW:
   case IMAGE_RIGHT_RECTIFIED:
   case IMAGE_CENTER_RECTIFIED:
   case IMAGE_LEFT_RECTIFIED:
   case IMAGE_RIGHT_COLOR_RECTIFIED:
   case IMAGE_CENTER_COLOR_RECTIFIED:
   case IMAGE_LEFT_COLOR_RECTIFIED:
   case IMAGE_RIGHT_EDGE:
   case IMAGE_CENTER_EDGE:
   case IMAGE_LEFT_EDGE:
      break;
      
   default:
      ASSERT( FALSE );
      return;      
   }

   CView::OnLButtonUp( nFlags, point );
}



void	  
CPGRImageView::customDraw( CDC* /* pDC */ )
{
   // empty - overridden by inheriting class.
}


#ifdef _DEBUG
CPGRStereoDoc* 
CPGRImageView::GetDocument() // non-debug version is inline
{
   ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CPGRStereoDoc )) );
   return ( CPGRStereoDoc* )m_pDocument;
}
#endif //_DEBUG


void 
CPGRImageView::OnInitialUpdate() 
{
   CView::OnInitialUpdate();

   ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CPGRStereoDoc )) );
   CPGRStereoDoc* pDoc = (CPGRStereoDoc*)GetDocument();
   ASSERT_VALID( pDoc );
   
   //
   // Set the initial image to be shown
   //
   CPGRMainFrame* pFrame = (CPGRMainFrame*)AfxGetMainWnd();
   ASSERT_VALID( pFrame );

   pFrame->m_dialogBarStereo.setbyImageType( IMAGE_RIGHT_RAW );
   m_nImageType	  = IMAGE_RIGHT_RAW;

   int iRows = 0;
   int iCols = 0;

   pDoc->getResolution( pDoc->getResolution(), &iRows, &iCols );
   ASSERT( iRows > 0 && iCols > 0 );
   
   if( (m_nImageType == IMAGE_RIGHT_RAW) || (m_nImageType == IMAGE_LEFT_RAW) )
   {
      resizeToDimensions( iRows, iCols*2 );
      pFrame->m_dialogBarStereo.m_comboBoxCamera.EnableWindow( FALSE );
   } 
   else 
   {
      resizeToDimensions( iRows, iCols );
      pFrame->m_dialogBarStereo.m_comboBoxCamera.EnableWindow( TRUE );
   }
}


void 
CPGRImageView::OnUpdate( CView*, LPARAM, CObject* ) 
{
   // prevent white flash on clear.
   CView::Invalidate( FALSE );
}


void	  
CPGRImageView::resizeToResolution( StereoImageResolution resolution )
{
   int	 iCols = 0;
   int	 iRows = 0;

   switch ( resolution )
   {  
   case STEREO_160x120:
      iCols = 160;
      iRows = 120;
      break;

   case STEREO_256x192:
      iCols = 256;
      iRows = 192;
      break;

   case STEREO_320x240:
      iCols = 320;
      iRows = 240;
      break;

   case STEREO_400x300:
      iCols = 400;
      iRows = 300;
      break;

   case STEREO_512x384:
      iCols = 512;
      iRows = 384;
      break;
   
   case STEREO_640x480:
      iCols = 640;
      iRows = 480;
      break;

   case STEREO_800x600:
      iCols = 800;
      iRows = 600;
      break;

   case STEREO_1024x768:
      iCols = 1024;
      iRows = 768;
      break;

   case STEREO_1280x960:
      iCols = 1280;
      iRows = 960;
      break;

   default:
      iCols = 320;
      iRows = 240;
   }

   CWnd* pframe = GetParentFrame(); 
   
   RECT Rect;
   int  iDiffWidth;
   int  iDiffHeight;

   pframe->GetWindowRect( &Rect );
   iDiffWidth  = Rect.right  - Rect.left;
   iDiffHeight = Rect.bottom - Rect.top;

   pframe->GetClientRect( &Rect );
   iDiffWidth  -= Rect.right  - Rect.left;
   iDiffHeight -= Rect.bottom - Rect.top;  

   pframe->SetWindowPos(
      NULL,
      0,
      0,
      iCols + iDiffWidth,
      iRows + iDiffHeight,
      SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );

   pframe->GetClientRect( &Rect );
}

void	  
CPGRImageView::resizeToDimensions( int iRows, int iCols )
{
   CWnd* pframe = GetParentFrame(); 
   
   RECT Rect;
   int  iDiffWidth;
   int  iDiffHeight;

   pframe->GetWindowRect( &Rect );
   iDiffWidth  = Rect.right  - Rect.left;
   iDiffHeight = Rect.bottom - Rect.top;

   pframe->GetClientRect( &Rect );
   iDiffWidth  -= Rect.right  - Rect.left;
   iDiffHeight -= Rect.bottom - Rect.top;  

   pframe->SetWindowPos(
      NULL,
      0,
      0,
      iCols + iDiffWidth,
      iRows + iDiffHeight,
      SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );

   pframe->GetClientRect( &Rect );
}

void 
CPGRImageView::changeDisplay()
{
   CPGRStereoDoc* pDoc = (CPGRStereoDoc*)GetDocument();
   ASSERT_VALID( pDoc );

   GetParentFrame()->ModifyStyle( 
      //WS_SIZEBOX | WS_MAXIMIZEBOX, 
      WS_MAXIMIZEBOX, 
      0, 
      SWP_DRAWFRAME | SWP_NOREDRAW   );

   int iRows = 0;
   int iCols = 0;

   pDoc->getResolution( pDoc->getResolution(), &iRows, &iCols );
   ASSERT( iRows > 0 && iCols > 0 );

   if ( (m_nImageType == IMAGE_RIGHT_RAW) || (m_nImageType == IMAGE_LEFT_RAW) )
   {
	   resizeToDimensions( iRows, iCols*2 );
   } else {
	   resizeToDimensions( iRows, iCols );
   }
}


void 
CPGRImageView::OnSelchangeIdcComboCamera() 
{
   CPGRMainFrame* pFrame = (CPGRMainFrame*)AfxGetMainWnd();
   ASSERT_VALID( pFrame );
   pFrame->m_dialogBarStereo.AssertValid();

   m_nImageType = (enum_IMAGETYPE)( pFrame->m_dialogBarStereo.getImageType() );

   changeDisplay();
}


void 
CPGRImageView::OnSelchangeIdcComboImage() 
{
   CPGRMainFrame* pFrame = (CPGRMainFrame*)AfxGetMainWnd();
   ASSERT_VALID( pFrame );
   pFrame->m_dialogBarStereo.AssertValid();

   if ( m_nImageType >= IMAGE_RIGHT_COLOR_RECTIFIED &&
        m_nImageType <= IMAGE_LEFT_COLOR_RECTIFIED )
   {
      CPGRStereoDoc* pDoc = GetDocument();
      pDoc->decrementColorRectifiedViews();
   }

   m_nImageType = (enum_IMAGETYPE)( pFrame->m_dialogBarStereo.getImageType() );
   if ( m_nImageType >= IMAGE_RIGHT_COLOR_RECTIFIED &&
        m_nImageType <= IMAGE_LEFT_COLOR_RECTIFIED )
   {
      CPGRStereoDoc* pDoc = GetDocument();
      pDoc->incrementColorRectifiedViews();
   }
   
   if ( (m_nImageType == IMAGE_RIGHT_RAW) || (m_nImageType == IMAGE_LEFT_RAW) )
   {
      pFrame->m_dialogBarStereo.m_comboBoxCamera.EnableWindow( FALSE );
   } 
   else 
   {
      pFrame->m_dialogBarStereo.m_comboBoxCamera.EnableWindow( TRUE );
   }
   
   changeDisplay();

}


void 
CPGRImageView::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView ) 
{
   CPGRStereoDoc*  pDoc = GetDocument();
   ASSERT_VALID( pDoc );

   // adjust whether the owning document is the active document
   pDoc->setDocumentActive( bActivate );

   if ( bActivate )
   {
      CPGRMainFrame* pFrame = (CPGRMainFrame*)AfxGetMainWnd();
      ASSERT_VALID( pFrame );
      pFrame->m_dialogBarStereo.AssertValid();

      //
      // Set checkbox status'
      //
      if ( pDoc->isStereoDialogActive() )
      {
	 pFrame->m_dialogBarStereo.m_buttonStereoParams.SetCheck( 1 );
      }
      else
      {
	 pFrame->m_dialogBarStereo.m_buttonStereoParams.SetCheck( 0 );
      }

      if ( pDoc->isTransformDialogActive() )
      {
	 pFrame->m_dialogBarStereo.m_buttonTransformation.SetCheck( 1 );
      }
      else
      {
	 pFrame->m_dialogBarStereo.m_buttonTransformation.SetCheck( 0 );
      }

      // Gray out the Camera Control button if we are working off line
      if ( pDoc->isOffline() )
      {
	 pFrame->m_dialogBarStereo.m_buttonCameraControl.EnableWindow( FALSE );
      }
      else
      {
	 pFrame->m_dialogBarStereo.m_buttonCameraControl.EnableWindow( TRUE );
      }


      // Set the combo box information
      pFrame->m_dialogBarStereo.setComboBoxSelections(
	 pDoc->hasColorData(),
	 pDoc->getMaxResolution(),
	 pDoc->isBumblebee() );

      // Set image combo box
      pFrame->m_dialogBarStereo.setbyImageType( m_nImageType );   


      // Set resolution combo box
      pFrame->m_dialogBarStereo.setResolution( pDoc->getResolution() );
   }

   CView::OnActivateView( bActivate, pActivateView, pDeactiveView );
}


