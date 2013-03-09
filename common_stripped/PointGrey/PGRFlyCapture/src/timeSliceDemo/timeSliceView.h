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
// $Id: timeSliceView.h,v 1.10 2002/07/29 19:10:01 mwhite Exp $
//=============================================================================
#if !defined(AFX_TIMESLICEVIEW_H__19B503AA_F9D7_4209_B68B_4AFBBA6B85B7__INCLUDED_)
#define AFX_TIMESLICEVIEW_H__19B503AA_F9D7_4209_B68B_4AFBBA6B85B7__INCLUDED_

//=============================================================================
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "timeslicedoc.h"



class CTimeSliceView : public CView
{
protected: 
   CTimeSliceView();
   DECLARE_DYNCREATE( CTimeSliceView )
      
public:
   CTimeSliceDoc* GetDocument();
   
public:
   
   //{{AFX_VIRTUAL(CTimeSliceView)
public:
   virtual void OnDraw(CDC* pDC);  
   virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
   virtual void OnInitialUpdate();
   //}}AFX_VIRTUAL
   
public:
   
   
   void setCamera( int iCamera );
   void setViewType( CTimeSliceDoc::ViewType type );

   
   virtual ~CTimeSliceView();
   
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump( CDumpContext& dc ) const;
#endif
   
   
protected:
   
   int m_iCamera;

   //
   // Draw the colour on this call to Draw().
   //
   bool	 m_bDrawColourImage;

   //
   // Draw the align cross.
   //
   void drawAlignCross( CTimeSliceDoc* pDoc, CDC* pDC );

   void drawImage( 
      CTimeSliceDoc::CameraInfo	 cameraInfo, 
      unsigned char*		 pImageBuffer,
      CDC*			 pDC   );

   
   CTimeSliceDoc::ViewType m_viewType;
   
   //{{AFX_MSG(CTimeSliceView)
   afx_msg void OnCameraProperties();
   afx_msg void OnUpdateCameraProperties(CCmdUI* pCmdUI);
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnSetFocus(CWnd* pOldWnd);
   afx_msg void OnKillFocus(CWnd* pNewWnd);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      
private:
	void displayFrameCountDown( CDC* pDC, int iFrames );
};

#ifndef _DEBUG  // debug version in timeSliceView.cpp
inline CTimeSliceDoc* CTimeSliceView::GetDocument()
{
   return (CTimeSliceDoc*)m_pDocument; 
}
#endif


//{{AFX_INSERT_LOCATION}}


#endif // !defined(AFX_TIMESLICEVIEW_H__19B503AA_F9D7_4209_B68B_4AFBBA6B85B7__INCLUDED_)
