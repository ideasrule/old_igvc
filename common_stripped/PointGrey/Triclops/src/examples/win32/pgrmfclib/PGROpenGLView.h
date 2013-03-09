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
// $Id: PGROpenGLView.h,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================
#ifndef __PGROPENGLVIEW_H__
#define __PGROPENGLVIEW_H__

#pragma once

//=============================================================================
// System Includes
//=============================================================================
#include <afxwin.h>

//=============================================================================
// OpenGL Includes
//=============================================================================
#include <gl/gl.h>
#include <gl/glu.h>

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRStereoDoc.h"



/**
 * This is the OpenGL using CView class.  It displays a 3d point cloud.
 */
class CPGROpenGLView : public CView
{
public:
   CPGROpenGLView(); 
   DECLARE_DYNCREATE( CPGROpenGLView )

   /**
    * Empty but overridable step to do custom drawing after the the other
    * objects have been drawn.
    */
   virtual void	customDraw();

   /** Set the point to rotate about. */
   //virtual bool setRotationPoint( PGRPoint3d point );
   virtual bool setRotationPoint( const C3dColourPointRC& point );

   /** Reset the OpenGL transformations. */
   virtual bool resetTransformations();

   
   //{{AFX_VIRTUAL(CPGROpenGLView)
public:
   virtual void OnInitialUpdate();
protected:
   virtual void OnDraw(CDC* pDC);
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
   //}}AFX_VIRTUAL
   
protected:
   
   /** OpenGL rendering context. */
   HGLRC m_hRC;        

   /** Device context for drawing. */
   CDC* m_pDC;

   /** GL-space translation. */
   GLdouble  m_ardTranslation[ 3 ];

   /** GL-space rotation. */
   GLdouble  m_ardRotation[ 3 ];

   /**
    * GL-space point to rotate about.  Default is (0,0,0), which is the 
    * location of the right camera.  This is set by clicking on the disparity
    * image.
    */
   GLdouble  m_ardRotationPoint[ 3 ];

   GLUquadricObj* m_sphereObj;
   GLuint         m_listName;

   /** Handle to quit thread event. */
   HANDLE    m_hQuitThread;

   /** Handle to thread done event. */
   HANDLE    m_hThreadDone;

   /** Indicates we are in auto routine mode. */
   bool      m_bAutoMode;

   float     m_fColourRed;
   float     m_fColourBlue;
   float     m_fColourGreen;
   
   /** Thread which does the auto move routine. */
   static UINT threadMoveRoutine( void* pparam );

   /** Returns pre-casted document object. */
   CPGRStereoDoc* GetDocument();

   virtual ~CPGROpenGLView();

   /**
    * Empty but overridable method for custom initialization - called after
    * default initialization steps.
    */
   virtual void customInit();

   /** Draw the current pointcloud and camera to the opengl window. */
   virtual void drawPointsAndCamera( CPGRStereoDoc* pDoc );

   /** Get the 3d point cloud from the document for drawing. */
   virtual CPointList* getDocumentPointList( CDocument* pDocument );

   /** Draw a representation of the camera to the opengl window. */
   virtual void	drawCamera();

   /** Draw all triangles to the opengl window. */
   virtual void drawTriangles( CDC *pDC );

   /** From the doc, get the image to texture map onto the generated 3d surface. */
   virtual PGRBitmapTriclops* getTextureMapBitmap( CPGRStereoDoc* pStereoDoc );

   /** From the doc, get the point array with which to generate the 3d surface. */
   virtual C3dColourPointRC* getPointArray( CPGRStereoDoc* pStereoDoc );

   /**
    * From the doc, get the number of rows and columns of the point array which
    * are filled in with data for this view.
    */
   virtual void getPointArrayResolution( CPGRStereoDoc* pStereoDoc, int* nRows, int* nCols );

   /** Draw a triangle to the opengl window. */
   virtual void drawTriangle( 
      C3dColourPointRC& p0, C3dColourPointRC& p1, C3dColourPointRC& p2 );

   /** Wrapper for the OpenGL context binding. */
   void unBindGL() const;

   /** Wrapper for the OpenGL context binding. */
   void bindGL( CDC* pDC ) const;

   /** 
    * Synchronize the GL space representation to the screen-space 
    * representation. 
    */
   void resizeGL( int iX, int iY );

   /** Display the last opengl error if it is actually an error. */
   void outputGlError( char* pszLabel );


   //{{AFX_MSG(CPGROpenGLView)
   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnDestroy();
   afx_msg void On3dReset();
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in PGROpenGLView.cpp
inline CPGRStereoDoc* 
CPGROpenGLView::GetDocument()
{ 
   return ( CPGRStereoDoc* )m_pDocument; 
}
#endif

//{{AFX_INSERT_LOCATION}}


#endif // #ifndef __PGROPENGLVIEW_H__
