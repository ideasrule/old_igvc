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
// $Id: MainFrm.cpp,v 1.7 2003/09/23 17:36:14 steenbur Exp $
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
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC( CMainFrame, CMDIFrameWnd )

BEGIN_MESSAGE_MAP( CMainFrame, CMDIFrameWnd )
//{{AFX_MSG_MAP(CMainFrame)
ON_WM_CREATE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
   ID_SEPARATOR,           // status line indicator
   ID_MISSED_IMAGES,
   ID_OUT_OF_SYNC_IMAGES,
   ID_FRAMERATE,

};


CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
   m_bmpToolBarHi.DeleteObject();
}


void
CMainFrame::updateFrameRate( 
                            double        dFrameRate, 
                            unsigned int  uiMissedImages, 
                            unsigned int  uiOutOfSyncImages )
{
   if( m_wndStatusBar.IsWindowVisible() )
   {
      char pszText[ 32 ];

      sprintf( pszText, "Missed: %06u", uiMissedImages );
      m_wndStatusBar.SetPaneText( 1, pszText );

      sprintf( pszText, "Out of sync: %06u", uiOutOfSyncImages );
      m_wndStatusBar.SetPaneText( 2, pszText );

      sprintf( pszText, "%02.1lfHz", dFrameRate );
      m_wndStatusBar.SetPaneText( 3, pszText );
   }
}


int 
CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
   if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
      return -1;
   
   if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
      | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
   {
      TRACE0("Failed to create toolbar\n");
      return -1;      // fail to create
   }

   m_bmpToolBarHi.LoadBitmap(IDB_TOOLBARHI);
   m_wndToolBar.SetBitmap((HBITMAP)m_bmpToolBarHi);
   
   if (!m_wndStatusBar.Create(this) ||
      !m_wndStatusBar.SetIndicators(indicators,
      sizeof(indicators)/sizeof(UINT)))
   {
      TRACE0("Failed to create status bar\n");
      return -1;      // fail to create
   }
   
   m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
   EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_wndToolBar);
   
   return 0;
}

BOOL 
CMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
   if( !CMDIFrameWnd::PreCreateWindow( cs ) )
      return FALSE;

   return TRUE;
}


#ifdef _DEBUG
void 
CMainFrame::AssertValid() const
{
   CMDIFrameWnd::AssertValid();
}

void 
CMainFrame::Dump(CDumpContext& dc) const
{
   CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


BOOL 
CMainFrame::OnWindowNewCustom()
{
   CMDIChildWnd* pActiveChild = MDIGetActive();
   CDocument* pDocument;
   if (pActiveChild == NULL ||
      (pDocument = pActiveChild->GetActiveDocument()) == NULL)
   {
      TRACE0("Warning: No active document for WindowNew command.\n");
      AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
      return FALSE;     // command failed
   }
   
   // otherwise we have a new frame !
   CDocTemplate* pTemplate = pDocument->GetDocTemplate();
   ASSERT_VALID(pTemplate);
   CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDocument, pActiveChild);
   if (pFrame == NULL)
   {
      TRACE0("Warning: failed to create new frame.\n");
      return FALSE;     // command failed
   }
   
   pTemplate->InitialUpdateFrame(pFrame, pDocument);


   return TRUE;
}
