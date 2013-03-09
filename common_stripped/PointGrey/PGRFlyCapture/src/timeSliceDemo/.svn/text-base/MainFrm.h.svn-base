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
// $Id: MainFrm.h,v 1.7 2003/09/23 17:36:14 steenbur Exp $
//=============================================================================
#if !defined(AFX_MAINFRM_H__5F60CE7A_1420_47B9_AEC9_F4C50292923F__INCLUDED_)
#define AFX_MAINFRM_H__5F60CE7A_1420_47B9_AEC9_F4C50292923F__INCLUDED_

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


class CMainFrame : public CMDIFrameWnd
{
   DECLARE_DYNAMIC(CMainFrame)
public:
   CMainFrame();
   
public:

   BOOL OnWindowNewCustom();

   void updateFrameRate( 
      double        dFrameRate, 
      unsigned int  uiMissedImages, 
      unsigned int  uiOutOfSyncImages );
   
   //{{AFX_VIRTUAL(CMainFrame)
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   //}}AFX_VIRTUAL
   
public:
   virtual ~CMainFrame();

#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif
   
protected:  
   CStatusBar  m_wndStatusBar;
   CToolBar    m_wndToolBar;
   CBitmap     m_bmpToolBarHi;
   
protected:
   //{{AFX_MSG(CMainFrame)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}


#endif // !defined(AFX_MAINFRM_H__5F60CE7A_1420_47B9_AEC9_F4C50292923F__INCLUDED_)
