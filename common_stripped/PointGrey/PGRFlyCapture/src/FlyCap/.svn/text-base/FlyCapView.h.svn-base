//=============================================================================
// Copyright © 2003 Point Grey Research, Inc. All Rights Reserved.
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
// $Id: FlyCapView.h,v 1.20 2006/05/03 21:21:22 mgriffin Exp $
//=============================================================================
#if !defined(AFX_FlyCAPVIEW_H__6296BE71_D3B3_4369_A60D_22A2608BDABB__INCLUDED_)
#define AFX_FlyCAPVIEW_H__6296BE71_D3B3_4369_A60D_22A2608BDABB__INCLUDED_

#pragma once

//=============================================================================
// System Includes
//=============================================================================

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================


class CFlyCapView : public CScrollView
{
public:
   DECLARE_DYNCREATE( CFlyCapView )
      
   CFlyCapView();
   virtual ~CFlyCapView();

   CFlyCapDoc* GetDocument();
   void newImageSize();
   
   //{{AFX_VIRTUAL( CFlyCapView )
   virtual void OnDraw( CDC* pDC );  
   virtual void OnInitialUpdate();
   //}}AFX_VIRTUAL
   
protected:
   HCURSOR m_hHand;
   HCURSOR m_hArrow;

   void setTitleBar();
   
   //{{AFX_MSG(CFlyCapView)
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
   afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
   afx_msg void OnMouseMove( UINT nFlags, CPoint point );
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()      
};

#ifndef _DEBUG  // debug version in FlyCapView.cpp
inline CFlyCapDoc* 
CFlyCapView::GetDocument()
{ 
   return ( CFlyCapDoc* )m_pDocument; 
}
#endif


//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_FlyCAPVIEW_H__6296BE71_D3B3_4369_A60D_22A2608BDABB__INCLUDED_)
