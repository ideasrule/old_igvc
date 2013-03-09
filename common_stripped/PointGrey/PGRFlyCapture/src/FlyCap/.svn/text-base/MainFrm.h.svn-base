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
// $Id: MainFrm.h,v 1.15 2006/05/03 16:52:42 marko Exp $
//=============================================================================
#if !defined(AFX_MAINFRM_H__40BF78BF_ECDD_4AE1_B59C_83E813B70831__INCLUDED_)
#define AFX_MAINFRM_H__40BF78BF_ECDD_4AE1_B59C_83E813B70831__INCLUDED_

#pragma once

enum CameraInfoMode
{
   REQUESTED_FPS,
   PROCESSED_FPS,
   DISPLAYED_FPS,
   MODEL_AND_SERIAL,
};

enum ImageInfoMode
{
   TIMESTAMP,
   CURSOR,
};

//
// The main window's status bar supports fewer modes
// than the title bar so we need to impose some limits.
//
#define NUM_CAMERAINFO_MODES 3
#define NUM_IMAGEINFO_MODES 2

class CMainFrame : public CFrameWnd
{
   
protected: // create from serialization only
   CMainFrame();
   DECLARE_DYNCREATE(CMainFrame)
      
   
public:   

   //{{AFX_VIRTUAL(CMainFrame)
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   //}}AFX_VIRTUAL
   
   void calculateMaxSize();
   void resizeToMax();
   void enterFullScreenMode();
   void exitFullScreenMode();
   LRESULT DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam );

   virtual ~CMainFrame();

   bool m_bFullScreenViewingMode;

   // Status Bar state information
   CameraInfoMode m_CameraInfoMode;
   ImageInfoMode m_ImageInfoMode;
   float m_fRequestedFramerate;
   UINT_PTR m_hTimer;
   
protected: 

   CStatusBar  m_wndStatusBar;
   CToolBar    m_wndToolBar;
   CImageList  m_ilToolBarHigh;
   CSize       m_sizeMax;

   void updateFrameRate();   
   void updateImageInfo();
   void updateBusEvent( char* pszEvent );
   
   //{{AFX_MSG(CMainFrame)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);   
   afx_msg void OnFileFullScreenMode();
   afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
   afx_msg void OnTimer( UINT_PTR nIDEvent );
   afx_msg void OnDestroy();
   afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
   afx_msg BOOL OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );
   afx_msg void OnHideToolbar();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      
};


//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_MAINFRM_H__40BF78BF_ECDD_4AE1_B59C_83E813B70831__INCLUDED_)
