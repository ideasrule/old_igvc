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
// $Id: PGRImageView.h,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================
#ifndef __PGRIMAGEVIEW_H__
#define __PGRIMAGEVIEW_H__ 

#pragma once

//=============================================================================
// System Includes
//=============================================================================
#include <afxwin.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRStereoDoc.h"


/**
 * The PGRMFC version of the MFC CView class.  We inherit from CScrollView
 * instead of CView because it makes automatic resizing a lot easier (for 
 * resolution changes, etc)  The default version of this class does little
 * more than query the document class for the latest image and display it to 
 * its window.  It also handles some GUI callbacks for making sure the dialog
 * bar displays the correct image type, etc.
 */
class CPGRImageView : public CView  
{
public:

   CPGRImageView();
   DECLARE_DYNCREATE( CPGRImageView )

   virtual ~CPGRImageView();
   
public:

   /** Returns the pre-casted document object. */
   CPGRStereoDoc* GetDocument();

   /**
    * Called from the document whenever it thinks the view should reset.
    * ie, a new resolution.
    */
   void	 changeDisplay();


   //{{AFX_VIRTUAL(CPGRImageView)
public:
   virtual void OnDraw(CDC* pDC);
   virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
   virtual void OnInitialUpdate();
protected:
   virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint );
   virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
   //}}AFX_VIRTUAL
   
protected:

   //{{AFX_MSG(CPGRImageView)
   afx_msg void OnSelchangeIdcComboCamera();
   afx_msg void OnSelchangeIdcComboImage();
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      
   /**
    * Overridable method for doing extra draw steps for each frame.  Ie, 
    * lines, text.
    */
   virtual void	  customDraw( CDC* pDC );

   /**
    * Resizes this window based on the requested resolution, ie 320 pixels by
    * 240 pixels.
    */
   virtual void	resizeToResolution( StereoImageResolution resolution );

   virtual void	resizeToDimensions( int iRows, int iCols );
   
      
protected:

   /** The current type of image that this view is displaying. */
   enum_IMAGETYPE m_nImageType;

};


#ifndef _DEBUG  // debug version in PGRImageView.cpp
inline CPGRStereoDoc* 
CPGRImageView::GetDocument()
{ 
   return ( CPGRStereoDoc* )m_pDocument; 
}
#endif


//{{AFX_INSERT_LOCATION}}


#endif // #ifndef __PGRIMAGEVIEW_H__ 
