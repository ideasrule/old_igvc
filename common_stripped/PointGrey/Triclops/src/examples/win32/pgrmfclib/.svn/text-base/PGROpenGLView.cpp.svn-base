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
// $Id: PGROpenGLView.cpp,v 1.4 2007/05/08 18:12:31 soowei Exp $
//=============================================================================

//=============================================================================
// nb:
//
//   Leave all of the outputGlError() calls in - there should be NO OpenGL
//   errors displayed on the debug output.  Just because it works on your
//   videocard, doesn't mean it will work on everyone elses.  See bug 840.
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <afxwin.h>
#include <math.h>

#if _MSC_VER >= 1400
#include <fstream>
#else
#include <fstream.h>
#endif

//=============================================================================
// PGR Includes
//=============================================================================
#include <PGRFlycapturestereo.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "PGROpenGLView.h"
#include "PGRStereoApp.h"
#include "PGRMainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE( CPGROpenGLView, CView )


CPGROpenGLView::CPGROpenGLView()
{     
   m_pDC = NULL;
   m_fColourRed   = 0.960784f;
   m_fColourBlue  = 0.870588f;
   m_fColourGreen = 0.701961f;
}


CPGROpenGLView::~CPGROpenGLView()
{
   if( m_pDC != NULL )
   {
      delete m_pDC;
      m_pDC = NULL;
   }
}


BEGIN_MESSAGE_MAP( CPGROpenGLView, CView )
//{{AFX_MSG_MAP(CPGROpenGLView)
ON_WM_CREATE()
ON_WM_ERASEBKGND()
ON_WM_MOUSEMOVE()
ON_WM_KEYDOWN()
ON_WM_SIZE()
ON_WM_DESTROY()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


#ifdef _DEBUG
CPGRStereoDoc* 
CPGROpenGLView::GetDocument() // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPGRStereoDoc)));
   return (CPGRStereoDoc*)m_pDocument;
}
#endif //_DEBUG


void 
CPGROpenGLView::outputGlError( char* pszLabel )
{
#ifdef _DEBUG
   GLenum errorno = ::glGetError();
   
   if ( errorno != GL_NO_ERROR )
   {
      TRACE( 
	 "%s had error: #(%d) %s\r\n", 
	 pszLabel, 
	 errorno, 
	 gluErrorString( errorno ) );
   }
#endif
}


inline void 
CPGROpenGLView::bindGL( CDC* pDC ) const
{
#ifdef _DEBUG
   if ( !wglMakeCurrent( pDC->m_hDC, m_hRC ) )
   {
      TRACE( "wglMakeCurrent Failed %x\n", GetLastError() );
   }
#else
   wglMakeCurrent( pDC->m_hDC, m_hRC );
#endif
}


inline void 
CPGROpenGLView::unBindGL() const
{
   wglMakeCurrent( NULL, NULL );
}


void
CPGROpenGLView::resizeGL( int iX, int iY )
{
   //
   // Set up the mapping of 3-space to screen space
   //
   GLdouble gldAspect = (GLdouble)iX / (GLdouble)iY;
   
   ::glMatrixMode( GL_PROJECTION ); 
   outputGlError("OnSize: MatrixMode");
   
   ::glLoadIdentity();
   
   ::gluPerspective( 45.0, gldAspect, 0.01, 100.0 );
   outputGlError("OnSize: gluPerspective");

   ::glViewport( 0, 0, iX, iY );
   outputGlError("OnSize: glViewport");
}


BOOL 
CPGROpenGLView::PreCreateWindow( CREATESTRUCT& cs ) 
{
   cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

   return CView::PreCreateWindow( cs );
}


void 
CPGROpenGLView::OnDestroy() 
{
   CView::OnDestroy();
   
   if( m_bAutoMode )
   {
      ::SetEvent( m_hQuitThread );
      if( ::WaitForSingleObject( m_hThreadDone, 3000 ) != WAIT_OBJECT_0 )
      {
         ::AfxMessageBox( "Could not close auto move routine thread." );
      }
   }

   ::CloseHandle( m_hThreadDone );
   ::CloseHandle( m_hQuitThread );
   
   unBindGL();
   
   if( m_hRC )
   {
      wglDeleteContext( m_hRC );
      m_hRC = NULL;
   }
}


void 
CPGROpenGLView::OnInitialUpdate() 
{
   m_ardTranslation[ 0 ]   = 0.0;
   m_ardTranslation[ 1 ]   = 0.0;
   m_ardTranslation[ 2 ]   = 0.0;
   
   m_ardRotation[ 0 ]	   = 0.0; 
   m_ardRotation[ 1 ]	   = 0.0; 
   m_ardRotation[ 2 ]	   = 0.0;
   
   setRotationPoint( C3dColourPointRC( 0, 0, 0 ) );

   //
   //  OpenGL stuff
   //
   bindGL( m_pDC );
   outputGlError( "OnInitialUpdate() bindGL()" );
   
   // specify wheat as the clear color
   ::glClearColor( m_fColourRed, m_fColourBlue, m_fColourGreen, 0.0f );
   outputGlError( "OnInitialUpdate() glClearColor()" );
   
   // enable depth calculations
   ::glEnable( GL_DEPTH_TEST ); 		
   outputGlError( "OnInitialUpdate() glEnable( GL_DEPTH_TEST )" );
   
   ::glDepthFunc( GL_LEQUAL );
   outputGlError( "OnInitialUpdate() glDepthFunc()" );

   
   // Set the material color to follow the current color
   ::glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
   outputGlError( "OnInitialUpdate() glEnable( GL_DEPTH_TEST )" );

   ::glEnable( GL_COLOR_MATERIAL );
   outputGlError( "OnInitialUpdate() glEnable( GL_COLOR_MATERIAL )" );
   
   //
   // Setup initial matricies
   //
   ::glMatrixMode( GL_MODELVIEW );
   ::glLoadIdentity();

   //
   // Find out exactly how large our draw area is, and have OpenGL
   // calculate its aspect ratio based on that.
   //
   CRect rect;
   GetClientRect( rect );
   resizeGL( rect.right, rect.bottom );

   //
   // call initialization function that may be overridden in the base class
   //
   customInit();


   CPGRStereoDoc* pDoc = GetDocument();
   ASSERT_VALID( pDoc );

   float fFocalLength = 0.0;

   if( pDoc->m_triclopsContext != NULL )
   {
      triclopsGetFocalLength( pDoc->m_triclopsContext, &fFocalLength );
   }

   if( fFocalLength <= 0.0 )
   {
      ASSERT( false );
      fFocalLength = 1.0;      
   }

   // Create a square in a list.
   m_listName = glGenLists( 1 );
   glNewList( m_listName, GL_COMPILE );

   glBegin( GL_POLYGON );
   glVertex3d( 0.0,                0.0,                0.0);
   glVertex3d( 0.0,                1.0 / fFocalLength, 0.0);
   glVertex3d( 1.0 / fFocalLength, 1.0 / fFocalLength, 0.0);
   glVertex3d( 1.0 / fFocalLength, 0.0,                0.0);

   glVertex3d( 1.0 / fFocalLength, 0.0,                0.0);
   glVertex3d( 1.0 / fFocalLength, 1.0 / fFocalLength, 0.0);
   glVertex3d( 0.00,               1.0 / fFocalLength, 0.0);
   glVertex3d( 0.00,               0.0,                0.0);
   glEnd();
   glEndList();


   unBindGL();
   //
   //  End OpenGL code
   //

   m_hQuitThread = CreateEvent( NULL, true, false, NULL );
   m_hThreadDone = CreateEvent( NULL, true, false, NULL ); 
   m_bAutoMode = false;

   Invalidate();
   CView::OnInitialUpdate();
}

UINT
CPGROpenGLView::threadMoveRoutine( void* pparam )
{
   CPGROpenGLView* pView = ((CPGROpenGLView*)pparam);

   float fT0[ 100 ];
   float fT1[ 100 ];
   float fT2[ 100 ];
   float fR0[ 100 ];
   float fR1[ 100 ];
   float fR2[ 100 ];
   int   iWait[ 100 ];
   int   iNumPoints = 0;

   char* pszBuffer = new char[128];

#if _MSC_VER >= 1400
   std::fstream infile( "digiDemoListPoints.txt", std::ios::in );
#else
   fstream infile( "digiDemoListPoints.txt", ios::in );
#endif

   while( infile.good() )
   {
      infile.getline( pszBuffer, 128 );
      if( pszBuffer[0] == 'T' )
      {
         sscanf( 
            pszBuffer,
            "T(%f,%f,%f)R(%f,%f,%f)W(%d)", 
            &fT0[ iNumPoints ],
            &fT1[ iNumPoints ],
            &fT2[ iNumPoints ],
            &fR0[ iNumPoints ],
            &fR1[ iNumPoints ],
            &fR2[ iNumPoints ],
            &iWait[ iNumPoints ] );
         iNumPoints++;
      }
   }
   infile.close();

   delete pszBuffer;

   Sleep(2000);

   bool bT0 =  false;
   bool bT1 =  false;
   bool bT2 =  false;
   bool bR0 =  false;
   bool bR1 =  false;
   bool bR2 =  false;
 
   if( iNumPoints > 0 )
   {     
      while( true )
      {
	 for( int i = 0; i < iNumPoints; i++ )
	 {
	    while( !bT0 || !bT1 || !bT2 || !bR0 || !bR1 || !bR2 )
	    {
	       //
	       // Check if we have been told to shutdown.
	       //
	       if( ::WaitForSingleObject( pView->m_hQuitThread, 0 ) == WAIT_OBJECT_0 )
	       {
		  ::SetEvent( pView->m_hThreadDone );
		  return 0;
	       }
	       
	       //
	       // Move to the specified translations/rotations.
	       //
	       
	       if(   (pView->m_ardTranslation[0] > (fT0[ i ]-0.01)) 
		  && (pView->m_ardTranslation[0] < (fT0[ i ]+0.01)) )
	       {
		  bT0 = true;            
	       }
	       else if( pView->m_ardTranslation[0] > fT0[ i ] )
	       {
		  pView->m_ardTranslation[0] -= 0.01;
	       }
	       else
	       {
		  pView->m_ardTranslation[0] += 0.01;
	       }
	       
	       if(   (pView->m_ardTranslation[1] > (fT1[ i ]-0.01)) 
		  && (pView->m_ardTranslation[1] < (fT1[ i ]+0.01)) )
	       {
		  bT1 = true;            
	       }
	       else if( pView->m_ardTranslation[1] > fT1[ i ] )
	       {
		  pView->m_ardTranslation[1] -= 0.01;
	       }
	       else
	       {
		  pView->m_ardTranslation[1] += 0.01;
	       }
	       
	       if(   (pView->m_ardTranslation[2] > (fT2[ i ]-0.01)) 
		  && (pView->m_ardTranslation[2] < (fT2[ i ]+0.01)) )
	       {
		  bT2 = true;            
	       }
	       else if( pView->m_ardTranslation[2] > fT2[ i ] )
	       {
		  pView->m_ardTranslation[2] -= 0.01;
	       }
	       else
	       {
		  pView->m_ardTranslation[2] += 0.01;
	       }
	       
	       if(   (pView->m_ardRotation[0] > (fR0[ i ]-0.01)) 
		  && (pView->m_ardRotation[0] < (fR0[ i ]+0.01)) )
	       {
		  bR0 = true;            
	       }
	       else if( pView->m_ardRotation[0] > fR0[ i ] )
	       {
		  pView->m_ardRotation[0] -= 1.0;
	       }
	       else
	       {
		  pView->m_ardRotation[0] += 1.0;
	       }
	       
	       if(   (pView->m_ardRotation[1] > (fR1[ i ]-0.01)) 
		  && (pView->m_ardRotation[1] < (fR1[ i ]+0.01)) )
	       {
		  bR1 = true;            
	       }
	       else if( pView->m_ardRotation[1] > fR1[ i ] )
	       {
		  pView->m_ardRotation[1] -= 1.0;
	       }
	       else
	       {
		  pView->m_ardRotation[1] += 1.0;
	       }
	       
	       if(   (pView->m_ardRotation[2] > (fR2[ i ]-0.01)) 
		  && (pView->m_ardRotation[2] < (fR2[ i ]+0.01)) )
	       {
		  bR2 = true;            
	       }
	       else if( pView->m_ardRotation[2] > fR2[ i ] )
	       {
		  pView->m_ardRotation[2] -= 1.0;
	       }
	       else
	       {
		  pView->m_ardRotation[2] += 1.0;
	       }
	       
	       Sleep(10);
	    }
	    
	    bT0 =  false;
	    bT1 =  false;
	    bT2 =  false;
	    bR0 =  false;
	    bR1 =  false;
	    bR2 =  false;  
	    Sleep( iWait[ i ] );  
         }
      }
  }
  
  ::SetEvent( pView->m_hThreadDone );
  return 0;
}


int 
CPGROpenGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if( CView::OnCreate(lpCreateStruct) == -1 )
   {
      return -1;
   }
   
   m_pDC = new CClientDC( this );
   ASSERT( m_pDC );
   
   //
   // Fill in the Pixel Format Descriptor
   //
   PIXELFORMATDESCRIPTOR pfd ;
   memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
   
   pfd.nSize      = sizeof( PIXELFORMATDESCRIPTOR );   
   pfd.nVersion   = 1;                           // Version number
   pfd.dwFlags    =	
      PFD_DOUBLEBUFFER   |         // Use double buffer
      PFD_SUPPORT_OPENGL |         // Use OpenGL
      PFD_DRAW_TO_WINDOW;          // Pixel format is for a window.
   pfd.iPixelType = PFD_TYPE_RGBA;
   pfd.cColorBits = 24;              // 8-bit color
   pfd.cDepthBits = 32;	   	     // 32-bit depth buffer
   pfd.iLayerType = PFD_MAIN_PLANE;  // Layer type
   
   int nPixelFormat = ChoosePixelFormat( m_pDC->m_hDC, &pfd );
   if ( nPixelFormat == 0 )
   {
      TRACE( "ChoosePixelFormat Failed %d\r\n", GetLastError() );
      return -1 ;
   }
   
   if ( !SetPixelFormat( m_pDC->m_hDC, nPixelFormat, &pfd ) )
   {
      TRACE("SetPixelFormat Failed %d\r\n",GetLastError()) ;
      return -1 ;
   }
   
   //
   // Create a rendering context.
   //
   m_hRC = wglCreateContext( m_pDC->m_hDC );
   if ( !m_hRC )
   {
      TRACE( "wglCreateContext Failed %x\r\n", GetLastError() );
      return -1;
   }

   return 0;
}


void 
CPGROpenGLView::OnDraw( CDC* pDC )
{
   CPGRStereoDoc* pDoc = GetDocument();
   ASSERT_VALID( pDoc );
   
   bindGL( pDC );

   ::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   outputGlError("OnDraw: glClear()");

  
   ::glMatrixMode( GL_MODELVIEW );
   outputGlError("OnDraw: glMatrixMode()");

   ::glLoadIdentity();
   outputGlError("OnDraw: glLoadIdentity()");

   ::glPushMatrix();
   outputGlError("OnDraw: glPushMatrix()");

   //
   // Do mouse-movement-defined translation
   //
   ::glTranslated( 
      m_ardTranslation[ 0 ], 
      m_ardTranslation[ 1 ], 
      m_ardTranslation[ 2 ] );
   outputGlError("OnDraw: glTranslated()");


   //
   // We must reflect across the Y and Z axes to transform between openGL
   // space and Triclops space.
   //
   ::glScaled( 1.0, -1.0, -1.0 );
   outputGlError("OnDraw: glScaled()");
   
   //
   // Do rotation point translation
   //
   ::glTranslated( 
      m_ardRotationPoint[ 0 ], 
      m_ardRotationPoint[ 1 ], 
      m_ardRotationPoint[ 2 ] );
   outputGlError("OnDraw: glTranslated()");

   //
   // Do mouse-movement-defined rotation.
   //
   ::glRotated( m_ardRotation[ 0 ], 1, 0, 0 );
   ::glRotated( m_ardRotation[ 1 ], 0, 1, 0 );
   ::glRotated( m_ardRotation[ 2 ], 0, 0, 1 );
   outputGlError("OnDraw: glRotated()");


   //
   // Move the viewport back.
   //
   ::glTranslated( 
      -m_ardRotationPoint[ 0 ], 
      -m_ardRotationPoint[ 1 ], 
      -m_ardRotationPoint[ 2 ] );
   outputGlError("OnDraw: glTranslated()");

   drawPointsAndCamera( pDoc );
   drawTriangles( pDC );

   //
   // call the virtual method in the inheriting class to draw
   // other stuff
   //
   customDraw();

   ::glPopMatrix();
   outputGlError("OnDraw: glPopMatrix()");

   
   // set the openGL perspective mapping to match the camera
   ::glMatrixMode( GL_PROJECTION );
   outputGlError("OnDraw: glMatrixMode()");
   ::glLoadIdentity();
   outputGlError("OnDraw: glLoadIdentity()");
   
   CRect rect;
   GetClientRect( rect );
   resizeGL( rect.right, rect.bottom );

   ::glFlush();	
   outputGlError( "OnDraw: glFlush()" );
   
   SwapBuffers( pDC->m_hDC );
   unBindGL();

}


BOOL 
CPGROpenGLView::OnEraseBkgnd( CDC* ) 
{
   // prevent white flash on draw
   return true;
}

void
CPGROpenGLView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if( nChar == VK_F8 )
   {
      if( m_bAutoMode )
      {
         ::SetEvent( m_hQuitThread );
         if( ::WaitForSingleObject( m_hThreadDone, 3000 ) != WAIT_OBJECT_0 )
         {
            ::AfxMessageBox( "Could not close auto move routine thread." );
         }
         m_bAutoMode = false;
      }
      else
      {
         ::ResetEvent( m_hQuitThread );
         ::ResetEvent( m_hThreadDone );
         ::AfxBeginThread( threadMoveRoutine, this );
         m_bAutoMode = true;
      }
   }


   CView::OnKeyDown( nChar, nRepCnt, nFlags );
}


void 
CPGROpenGLView::OnMouseMove( UINT nFlags, CPoint point )
{
   static CPoint  pLastPoint;
   
   const double dScaleFactor = 500.0;
   
   //
   //	Left button - rotation
   //
   if( nFlags & MK_LBUTTON )
   {
      if( nFlags & MK_SHIFT )
      {
	 m_ardRotation[0] -= ( pLastPoint.y - point.y );
	 m_ardRotation[2] -= ( pLastPoint.x - point.x );
      }
      else
      {
	 m_ardRotation[0] += ( pLastPoint.y - point.y );
	 m_ardRotation[1] -= ( pLastPoint.x - point.x );
      }

      Invalidate();
   }
   
   //
   //	Right button - translation
   //
   if( nFlags & MK_RBUTTON )
   {
      if ( nFlags & MK_SHIFT )
      {
	 m_ardTranslation[0] -= ( pLastPoint.x - point.x )/dScaleFactor;
	 m_ardTranslation[2] -= ( pLastPoint.y - point.y )/dScaleFactor;
      }
      else
      {
	 m_ardTranslation[0] -= ( pLastPoint.x - point.x )/dScaleFactor;
	 m_ardTranslation[1] += ( pLastPoint.y - point.y )/dScaleFactor;
      }

      Invalidate();
   }
   
   pLastPoint = point;
   
   CView::OnMouseMove( nFlags, point );
}


void 
CPGROpenGLView::OnSize( UINT nType, int cx, int cy ) 
{
   CView::OnSize( nType, cx, cy );
   
   if( ( cx <= 0 ) || ( cy <= 0 ) )
   {
      return;
   }

   bindGL( m_pDC );
   
   resizeGL( cx, cy );

   unBindGL();
}


void 
CPGROpenGLView::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView ) 
{
   CPGRMainFrame* pFrame = (CPGRMainFrame*)AfxGetMainWnd();
   ASSERT_VALID( pFrame );
   pFrame->m_dialogBarStereo.AssertValid();

   if( bActivate )
   {
      pFrame->m_dialogBarStereo.m_comboBoxCamera.EnableWindow( FALSE );
      pFrame->m_dialogBarStereo.m_comboBoxImage.EnableWindow( FALSE );
      pFrame->m_dialogBarStereo.m_comboBoxResolution.EnableWindow( FALSE );
   }
   else
   {
      pFrame->m_dialogBarStereo.m_comboBoxCamera.EnableWindow( TRUE );
      pFrame->m_dialogBarStereo.m_comboBoxImage.EnableWindow( TRUE );
      pFrame->m_dialogBarStereo.m_comboBoxResolution.EnableWindow( TRUE );
   }

   CView::OnActivateView( bActivate, pActivateView, pDeactiveView );
}


void 
CPGROpenGLView::On3dReset() 
{
   m_ardTranslation[0] =  0.0; 
   m_ardTranslation[1] =  0.0; 
   m_ardTranslation[2] =  0.0;
   
   m_ardRotation[0] =  0.0; 
   m_ardRotation[1] =  0.0; 
   m_ardRotation[2] =  0.0;

   Invalidate();
}


void
CPGROpenGLView::drawPointsAndCamera( CPGRStereoDoc* /* pDoc */ )
{
   int iDocNum = 0;

   CPGRStereoApp* pStereoApp = (CPGRStereoApp*)AfxGetApp();

   POSITION posTemplate = 
      pStereoApp->GetFirstDocTemplatePosition();

   CDocTemplate* pTemplate = (CDocTemplate*)pStereoApp->GetNextDocTemplate( posTemplate );

   POSITION posDoc = pTemplate->GetFirstDocPosition();
   
   while( posDoc != NULL )
   {
      CDocument* pDocument;

      if ( ( pDocument = pTemplate->GetNextDoc( posDoc )) != NULL)
      {
	 if ( pDocument->IsKindOf( RUNTIME_CLASS( CPGRStereoDoc )) )
	 {
            CPGRStereoDoc* pStereoDoc = (CPGRStereoDoc*)pDocument;

            CPointList* pPointCloud = getDocumentPointList( pDocument );
	    if( pPointCloud == NULL )
	    {
	       continue;
	    }

            CTransformD* pTransform  = pStereoDoc->getTransform();

	    //
	    // Change the colour for this pointcloud based on which document it
	    // is from - later the colour will be stored in the document itself
	    // and will be user adjustable.  the first document point cloud is
	    // red, then blue, then green.  all others are white.
	    //
	    float fRed;
	    float fGreen;
	    float fBlue;

	    switch ( iDocNum )
	    {
	    case 0:
	       fRed   = 1.0;
	       fGreen = 0.0;
	       fBlue  = 0.0;
	       break;

	    case 1:
	       fRed   = 0.0;
	       fGreen = 1.0;
	       fBlue  = 0.0;
	       break;
	    
	    case 2:
	       fRed   = 0.0;
	       fGreen = 0.0;
	       fBlue  = 1.0;
	       break;

	    default:
	       fRed   = 1.0;
	       fGreen = 1.0;
	       fBlue  = 1.0;
	       break;
	    }
	    
	    ::glPushMatrix();
	    ::glMultMatrixf( pTransform->glMatrixf() );   
            
            float fPointSize = pStereoDoc->getPointSize();
            bool  bDrawPoints = false;
            if( fPointSize == 0.0f )
            {
               bDrawPoints = true;
            }

	    if( pStereoDoc->drawingPoints() )
	    {
               if( bDrawPoints )
               {
	         ::glBegin( GL_POINTS );
               }
	       //
	       // in order to make sure we're not doing an if() for each point,
	       // we need two for loops, depending on whether we're drawing colour
	       // points or not.
	       //
	       if( pStereoDoc->drawingColorPoints() )
	       {
		  for ( int iPoint = 0; iPoint < pPointCloud->size(); iPoint++ )
		  {
                     const C3dColourPointRC* ppoint = pPointCloud->elementAt( iPoint );
                     const RGBQUAD* pcolour = ppoint->colour();

                     ::glColor3ub( pcolour->rgbRed, pcolour->rgbGreen, pcolour->rgbBlue );

                     if( bDrawPoints )
                     {
                        ::glVertex3d( ppoint->x(), ppoint->y(), ppoint->z() );
                     }
                     else
                     {                      
                        ::glPushMatrix();
                        ::glTranslated( ppoint->x(), ppoint->y(), ppoint->z() );                                             
                        ::glScaled( ppoint->z()*fPointSize, ppoint->z()*fPointSize, ppoint->z()*fPointSize );                     
                        ::glCallList( m_listName );
                        ::glPopMatrix();	
                     }
		  }
	       }
	       else
	       {
		  for ( int iPoint = 0; iPoint < pPointCloud->size(); iPoint++ )
		  {
                     const C3dColourPointRC* ppoint = pPointCloud->elementAt( iPoint );

                     ::glColor3f(  fRed, fGreen, fBlue );
                     
                     if( bDrawPoints )
                     {
                        ::glVertex3d( ppoint->x(), ppoint->y(), ppoint->z() );
                     }
                     else
                     {
                        ::glPushMatrix();
                        ::glTranslated( ppoint->x(), ppoint->y(), ppoint->z() );                                             
                        ::glScaled( ppoint->z()*fPointSize, ppoint->z()*fPointSize, ppoint->z()*fPointSize );                     
                        ::glCallList( m_listName );
                        ::glPopMatrix();	
                     }
		  }
	       }
               if( bDrawPoints )
               {
	         ::glEnd();
               }
	    }

	    ::glColor3f(  fRed, fGreen, fBlue );
	    drawCamera();

	    ::glPopMatrix();        	

	    iDocNum++;
	 }
      }
   }
}


void 
CPGROpenGLView::drawCamera()
{
   const float deg2rad = (float) ( 3.1415926 / 180 );
   const float r = 0.0075f;

   //
   // draw a wire-frame model of the stereo camera at (0,0,0)
   //
   if( GetDocument()->isBumblebee() )
   {
      // front panel
      ::glBegin( GL_LINE_STRIP );
      glVertex2f(  0.02f,  0.02f );
      glVertex2f( -0.14f,  0.02f );
      glVertex2f( -0.14f, -0.02f );
      glVertex2f(  0.02f, -0.02f );
      glVertex2f(  0.02f,  0.02f );
      ::glEnd();
      
      // rear panel
      ::glBegin( GL_LINE_STRIP );
      glVertex3f(  0.02f,  0.02f, -0.04f );
      glVertex3f( -0.14f,  0.02f, -0.04f );
      glVertex3f( -0.14f, -0.02f, -0.04f );
      glVertex3f(  0.02f, -0.02f, -0.04f );
      glVertex3f(  0.02f,  0.02f, -0.04f );
      ::glEnd();
      
      // lines connecting front and rear panels
      ::glBegin( GL_LINES );
      glVertex2f(  0.02f,  0.02f);
      glVertex3f(  0.02f,  0.02f, -0.04f );
      glVertex2f( -0.14f,  0.02f);
      glVertex3f( -0.14f,  0.02f, -0.04f );
      glVertex2f( -0.14f, -0.02f);
      glVertex3f( -0.14f, -0.02f, -0.04f );
      glVertex2f(  0.02f, -0.02f);
      glVertex3f(  0.02f, -0.02f, -0.04f );
      ::glEnd();

      // lenses
      float x[] = { 0, -0.12f };
      float y[] = { 0,  0 };
      for( int j = 0; j < 2; j++ )
      {
         glBegin( GL_LINE_LOOP );
         for( int i = 0; i < 360; i += 30 )
         {
            float angle = i * deg2rad;
            glVertex2f( (float) ( x[j] + cos( angle ) * r ), 
               (float) ( y[j] + sin( angle ) * r ) );
         }
         glEnd();
      }
   }
   else
   {
      // front panel
      ::glBegin( GL_LINE_STRIP );
      glVertex2f(  0.02f,  0.02f );
      glVertex2f( -0.12f,  0.02f );
      glVertex2f( -0.12f, -0.02f );
      glVertex2f( -0.02f, -0.12f );
      glVertex2f(  0.02f, -0.12f );
      glVertex2f(  0.02f,  0.02f );
      ::glEnd();
      
      // rear panel
      ::glBegin( GL_LINE_STRIP );
      glVertex3f(  0.02f,  0.02f, -0.04f );
      glVertex3f( -0.12f,  0.02f, -0.04f );
      glVertex3f( -0.12f, -0.02f, -0.04f );
      glVertex3f( -0.02f, -0.12f, -0.04f );
      glVertex3f(  0.02f, -0.12f, -0.04f );
      glVertex3f(  0.02f,  0.02f, -0.04f );
      ::glEnd();
      
      // lines connecting front and rear panels
      ::glBegin( GL_LINES );
      glVertex2f(  0.02f,  0.02f);
      glVertex3f(  0.02f,  0.02f, -0.04f );
      glVertex2f( -0.12f,  0.02f);
      glVertex3f( -0.12f,  0.02f, -0.04f );
      glVertex2f( -0.12f, -0.02f);
      glVertex3f( -0.12f, -0.02f, -0.04f );
      glVertex2f( -0.02f, -0.12f);
      glVertex3f( -0.02f, -0.12f, -0.04f );
      glVertex2f(  0.02f, -0.12f);
      glVertex3f(  0.02f, -0.12f, -0.04f );
      glVertex2f(  0.02f,  0.02f);
      glVertex3f(  0.02f,  0.02f, -0.04f );
      ::glEnd();

      // lenses
      float x[] = { 0, -0.1f,    0 };
      float y[] = { 0,    0, -0.1f };
      for( int j = 0; j < 3; j++ )
      {
         glBegin( GL_LINE_LOOP );
         for( int i = 0; i < 360; i += 30 )
         {
            float angle = i * deg2rad;
            glVertex2f( (float) ( x[j] + cos( angle ) * r ), 
               (float) ( y[j] + sin( angle ) * r ) );
         }
         glEnd();
      }
   }

   //
   // draw optical axis in the -z direction
   //
   ::glBegin( GL_LINES );
   ::glVertex3f( 0, 0, 0);
   ::glVertex3f( 0, 0, 5);
   ::glEnd();
}


PGRBitmapTriclops* 
CPGROpenGLView::getTextureMapBitmap( CPGRStereoDoc* pStereoDoc )
{
   if ( pStereoDoc->hasColorData() == TRUE )
   {
      return pStereoDoc->getBitmap( IMAGE_RIGHT_COLOR_RECTIFIED );
   }
   else
   {
      return pStereoDoc->getBitmap( IMAGE_RIGHT_RECTIFIED );
   }
}


C3dColourPointRC*
CPGROpenGLView::getPointArray( CPGRStereoDoc* pStereoDoc )
{
   return (C3dColourPointRC*)pStereoDoc->getPointArray();
}


void
CPGROpenGLView::getPointArrayResolution( CPGRStereoDoc* pStereoDoc, int* nRows, int* nCols )
{
   StereoImageResolution resolution = pStereoDoc->getResolution();
   switch( resolution )
   {
   case STEREO_160x120:
      *nRows = 120;
      *nCols = 160;
      break;

   case STEREO_256x192:
      *nRows = 192;
      *nCols = 256;
      break;

   case STEREO_320x240:
      *nRows = 240;
      *nCols = 320;
      break;

   case STEREO_400x300:
      *nRows = 300;
      *nCols = 400;

   case STEREO_512x384:
      *nRows = 384;
      *nCols = 512;
      break;

   case STEREO_640x480:
      *nRows = 480;
      *nCols = 640;
      break;

   case STEREO_800x600:
      *nRows = 600;
      *nCols = 800;
      break;

   case STEREO_1024x768:
      *nRows = 768;
      *nCols = 1024;
      break;

   case STEREO_1280x960:
      *nRows = 960;
      *nCols = 1280;
      break;

   default:
      ASSERT( FALSE );
      break;
   }
}


void 
CPGROpenGLView::drawTriangles( CDC* /* pDC */ )
{
   int	 iDocNum = 0;

   // specify wheat as clear color
   ::glClearColor( m_fColourRed, m_fColourBlue, m_fColourGreen, 0.0f );
   // enable depth testing
   ::glEnable( GL_DEPTH_TEST );
   ::glDepthFunc( GL_LEQUAL );
   
   // back face culling
   ::glEnable( GL_CULL_FACE );
   ::glCullFace( GL_BACK );
   
   // parameter for retrieving data
   glPixelStorei( GL_PACK_ALIGNMENT, 1 );
   // texture parameters
   glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
   glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
   
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   
   // set up a light behind the camera which can be turned on later
   ::GLfloat position[] = { 3.0, 3.0, -1.0, 0.0 };
   ::glLightfv( GL_LIGHT0, GL_POSITION, position );
   

   CPGRStereoApp* pStereoApp = (CPGRStereoApp*)AfxGetApp();

   POSITION posTemplate = pStereoApp->GetFirstDocTemplatePosition();

   CDocTemplate* pTemplate = (CDocTemplate*)pStereoApp->GetNextDocTemplate( posTemplate );

   POSITION posDoc = pTemplate->GetFirstDocPosition();
   
   while( posDoc != NULL )
   {
      CDocument*  pDocument;

      if ( ( pDocument = pTemplate->GetNextDoc( posDoc )) != NULL)
      {
	 if ( pDocument->IsKindOf( RUNTIME_CLASS( CPGRStereoDoc )) )
	 {
	    CPGRStereoDoc* pStereoDoc = (CPGRStereoDoc*)pDocument;

	    if( !pStereoDoc->drawingTriangles() )
	    {
	       continue;  // skip this doc
	    }

	    //PGRTransform3d* pTransform = pStereoDoc->getTransform();
            CTransformD* pTransform = pStereoDoc->getTransform();
	    
	    ::glPushMatrix();
	    ::glMultMatrixf( pTransform->glMatrixf() );

	    int r, c;

	    if( pStereoDoc->drawingTexture() && (getTextureMapBitmap(pStereoDoc)!=NULL) )
	    {
	       // buffer for copying texture
	       static unsigned char pTextureBuffer[1280 * 960 * 3];

	       // get the texture map
	       PGRBitmapTriclops* pBitmap;
	       int nWidth, nHeight;
	       unsigned char* tptr = pTextureBuffer;
	       unsigned char* iptr;

	       pBitmap = getTextureMapBitmap( pStereoDoc );
	       pBitmap->getImageDimensions( &nWidth, &nHeight );
	       iptr = pBitmap->getDataPointer();

	       if( pStereoDoc->hasColorData() == TRUE )
	       {
		  for ( r = 0; r < nHeight; r++)
		  {
		     int k = 0;
		     c = 0;
		     for ( int i = 0; i < nWidth; i++)
		     {
			tptr[k++] = iptr[c++];
			tptr[k++] = iptr[c++];
			tptr[k++] = iptr[c++];
			c++;
		     }
		     tptr += 3840;  // 3 * 1280
		     iptr += 4 * nWidth;
		  }
	       }
	       else
	       {
		  for ( r = 0; r < nHeight; r++)
		  {
		     int k = 0;
		     for ( c = 0; c < nWidth; c++)
		     {
			tptr[k++] = iptr[c];
			tptr[k++] = iptr[c];
			tptr[k++] = iptr[c];
		     }
		     tptr += 3840;  // 3 * 1280
		     iptr += nWidth;
		  }
	       }
	       // windoze: bgr extension, so I don't have to shuffle bytes around
	       glTexImage2D(
                  GL_TEXTURE_2D, 
                  0, 
                  3, 
                  1024, 
                  512, 
                  0, 
                  GL_BGR_EXT, 
                  GL_UNSIGNED_BYTE, 
                  pTextureBuffer );
	       glEnable(GL_TEXTURE_2D);
	    }
	    else
	    {
	       //
	       // Change the colour for this pointcloud based on which document it
	       // is from - later the colour will be stored in the document itself
	       // and will be user adjustable.  the first document point cloud is
	       // red, then blue, then green.  all others are white.
	       //
	       float fRed;
	       float fGreen;
	       float fBlue;
	       
	       switch ( iDocNum )
	       {
	       case 0:
		  fRed   = 1.0;
		  fGreen = 0.0;
		  fBlue  = 0.0;
		  break;
		  
	       case 1:
		  fRed   = 0.0;
		  fGreen = 1.0;
		  fBlue  = 0.0;
		  break;
		  
	       case 2:
		  fRed   = 0.0;
		  fGreen = 0.0;
		  fBlue  = 1.0;
		  break;
		  
	       default:
		  fRed   = 1.0;
		  fGreen = 1.0;
		  fBlue  = 1.0;
		  break;
	       }
	       
	       glColor3f( fRed, fGreen, fBlue );
	       
	       // turn on lighting
	       glEnable( GL_LIGHTING );
	       glEnable( GL_LIGHT0 );
	    }
	    
	    glBegin(GL_TRIANGLES);
    	    
            C3dColourPointRC* pPointArray = getPointArray( pStereoDoc );

	    int nRows = 0;
	    int nCols = 0;
	    getPointArrayResolution( pStereoDoc, &nRows, &nCols );

	    for ( r = 0; r < nRows - 1; r++ )
	    {
               C3dColourPointRC* pRow = &pPointArray[r * nCols];
               C3dColourPointRC* pNextRow = &pPointArray[( r + 1 ) * nCols];

	       for ( c = 0; c < nCols - 1; c++ )
	       {
                  C3dColourPointRC* pPoints[4] =
		  {
		     &pRow[c],
		     &pNextRow[c],
		     &pNextRow[c + 1],
		     &pRow[c + 1]
		  };

		  ASSERT( pPoints[0]->row() == r      || pPoints[0]->row() < 0 );
		  ASSERT( pPoints[0]->col() == c      || pPoints[0]->col() < 0 );
		  ASSERT( pPoints[1]->row() == r + 1  || pPoints[1]->row() < 0 );
		  ASSERT( pPoints[1]->col() == c      || pPoints[1]->col() < 0 );
		  ASSERT( pPoints[2]->row() == r + 1  || pPoints[2]->row() < 0 );
		  ASSERT( pPoints[2]->col() == c + 1  || pPoints[2]->col() < 0 );
		  ASSERT( pPoints[3]->row() == r      || pPoints[3]->row() < 0 );
		  ASSERT( pPoints[3]->col() == c + 1  || pPoints[3]->col() < 0 );

		  BOOL valid[4] =
		  {
		     pPoints[0]->row() >= 0,
		     pPoints[1]->row() >= 0,
		     pPoints[2]->row() >= 0,
		     pPoints[3]->row() >= 0
		  };

		  if( valid[0] && valid[1] && valid[2] && valid[3] )
		  {
		     // full square
		     drawTriangle( *pPoints[0], *pPoints[1], *pPoints[3] );
		     drawTriangle( *pPoints[1], *pPoints[2], *pPoints[3] );
		  }
		  else if (valid[0] && valid[1] && valid[3])
		  {
		     drawTriangle( *pPoints[0], *pPoints[1], *pPoints[3] );
		  }
		  else if (valid[1] && valid[2] && valid[3])
		  {
		     drawTriangle( *pPoints[1], *pPoints[2], *pPoints[3] );
		  }
		  else if (valid[0] && valid[1] && valid[2])
		  {
		     drawTriangle( *pPoints[0], *pPoints[1], *pPoints[2] );
		  }
		  else if (valid[0] && valid[2] && valid[3])
		  {
		     drawTriangle( *pPoints[0], *pPoints[2], *pPoints[3] );
		  }
	       }
	    }

	    glEnd();

	    if( pStereoDoc->drawingTexture() )
	    {
	       glDisable( GL_TEXTURE_2D );
	    }
	    else
	    {
	       glDisable( GL_LIGHTING );
	       glDisable( GL_LIGHT0 );
	    }
	    glPopMatrix();
	    
            iDocNum++;
	 }
      }
   }
}


void 
CPGROpenGLView::drawTriangle( 
                             C3dColourPointRC& p0, 
                             C3dColourPointRC& p1, 
                             C3dColourPointRC& p2 )
{   
#define ANGLE_FILTER
#ifdef ANGLE_FILTER

    double v1[3]; 
    double v2[3]; 
    double normal[3]; 
    double length; 
    double angle;

    // v1 = p1 - p0
    v1[0] = p1.x() - p0.x();
    v1[1] = p1.y() - p0.y();
    v1[2] = p1.z() - p0.z();

    // v2 = p2 - p0                    
    v2[0] = p2.x() - p0.x();
    v2[1] = p2.y() - p0.y();
    v2[2] = p2.z() - p0.z();
    
    normal[0] = v1[1] * v2[2] - v1[2] * v2[1];
    normal[1] = v1[2] * v2[0] - v1[0] * v2[2];
    normal[2] = v1[0] * v2[1] - v1[1] * v2[0];
    
    // normalize the normal
    length = 1.0f / sqrt(
       normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    
    // note: this normal should be pointing somewhat towards -z, so the
    // z coordinate should be negative
    // if not, clip it
    if (normal[2] < 0) {
       
       normal[0] *= length;
       normal[1] *= length;
       normal[2] *= length;
       
       // the "optical" axis through one vertex of the triangle - from
       // the vertex to 0, 0, 0
       double axis[3];

       axis[0] = -p1.x();
       axis[1] = -p1.y();
       axis[2] = -p1.z();

       length = 1.0f / sqrt(
	  axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);

       axis[0] *= length;
       axis[1] *= length;
       axis[2] *= length;
       
       double dotp = axis[0] * normal[0] + axis[1] * normal[1] + axis[2] * normal[2];

       angle = acos( dotp );

       // if the angle is more than some threshold, then scrap the triangle.
       // let's try 75 degrees 
       if ( angle < 1.308996939 )
       {
	  // 0
	  glTexCoord2d( (p0.col() / 1024.0f), ( p0.row() / 512.0f ));
	  glNormal3d( normal[0], normal[1], normal[2] );
	  glVertex3d( p0.x(), p0.y(), p0.z());

	  // 1
	  glTexCoord2d( (p1.col() / 1024.0f), (p1.row() / 512.0f ));
	  glVertex3d(p1.x(), p1.y(), p1.z());

	  // 2
	  glTexCoord2d((p2.col() / 1024.0f), (p2.row() / 512.0f ));
	  glVertex3d(p2.x(), p2.y(), p2.z());
       }
    }
#else
    // 0
    glTexCoord2d( (p0.col() / 1024.0f), (p0.row() / 512.0f ));
    glVertex3d( p0.x(), p0.y(), p0.z());

    // 1
    glTexCoord2d( (p1.col() / 1024.0f), (p1.row() / 512.0f ));
    glVertex3d( p1.x(), p1.y(), p1.z());

    // 2
    glTexCoord2d( (p2.col() / 1024.0f), (p2.row() / 512.0f ));
    glVertex3d( p2.x(), p2.y(), p2.z());
#endif
}


bool 
CPGROpenGLView::setRotationPoint( const C3dColourPointRC& point )
{
   m_ardRotationPoint[ 0 ] = point.x();
   m_ardRotationPoint[ 1 ] = point.y();
   m_ardRotationPoint[ 2 ] = point.z();

   return true;
}


bool
CPGROpenGLView::resetTransformations()
{
   m_ardTranslation[ 0 ]   = 0.0;
   m_ardTranslation[ 1 ]   = 0.0;
   m_ardTranslation[ 2 ]   = 0.0;
   
   m_ardRotation[ 0 ]	   = 0.0; 
   m_ardRotation[ 1 ]	   = 0.0; 
   m_ardRotation[ 2 ]	   = 0.0;

   return true;
}


CPointList*
CPGROpenGLView::getDocumentPointList( CDocument* pDocument )
{
   return ((CPGRStereoDoc*)pDocument)->getPointCloud();
}


void 
CPGROpenGLView::customDraw()
{
   //
   // nothing here.  this should be overidden in the derived class
   //
}


void 
CPGROpenGLView::customInit()
{
   //
   // nothing here.  this should be overidden in the derived class
   //
}


