//=============================================================================
// Copyright © 2003 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with Point Grey Research Inc.
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//
// Digiclops® is a registered trademark of Point Grey Research Inc.
//=============================================================================
//=============================================================================
// $Id: MainFrm.cpp,v 1.1 2007/03/27 21:33:00 demos Exp $
//=============================================================================
//=============================================================================
// Project Includes
//=============================================================================
#include "stdafx.h"
#include "triclopsDemo.h"
#include "DemoDoc.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC( CMainFrame, CPGRMainFrame )

BEGIN_MESSAGE_MAP( CMainFrame, CPGRMainFrame )
//{{AFX_MSG_MAP(CMainFrame)
ON_WM_CREATE()
ON_COMMAND(ID_WINDOW_NEW3D, OnWindowNew3d)
ON_WM_INITMENU()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}


int 
CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
   if (CPGRMainFrame::OnCreate(lpCreateStruct) == -1)
   {
      return -1;
   }
   
   return 0;
}


BOOL 
CMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
   if( !CPGRMainFrame::PreCreateWindow( cs ) )
   {
      return FALSE;
   }
   
   return TRUE;
}


void 
CMainFrame::OnWindowNew3d() 
{
   CMDIChildWnd*  pActiveChild = MDIGetActive();
   CDocument* pDocument;
   
   if ( pActiveChild == NULL ||
      ( pDocument = pActiveChild->GetActiveDocument() ) == NULL ) 
   {
      TRACE("Warning:  No active document for WindowNew command\n");
      AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
      return; // Command failed
   }
   
   // Otherwise, we have a new frame!
   CDocTemplate* pTemplate = ((CDemoApp*)AfxGetApp())->m_pTemplate3dView;
   ASSERT_VALID( pTemplate );
   
   CFrameWnd* pFrame =
      pTemplate->CreateNewFrame( pDocument, pActiveChild );
   
   if (pFrame == NULL) 
   {
      TRACE( "Warning:  failed to create new frame\n" );
      AfxMessageBox( AFX_IDP_COMMAND_FAILURE );
      return; // Command failed
   }
   
   pTemplate->InitialUpdateFrame( pFrame, pDocument );

   ((CDemoDoc*)pDocument)->turnOnComputePoints();
}


void 
CMainFrame::OnInitMenu( CMenu* pMenu )
{
   CPGRMainFrame::OnInitMenu( pMenu );
   
   {
      // Add Tip of the Day menu item on the fly!
      static CMenu* pSubMenu = NULL;
      
      CString strHelp; strHelp.LoadString(CG_IDS_TIPOFTHEDAYHELP);
      CString strMenu;
      int nMenuCount = pMenu->GetMenuItemCount();
      BOOL bFound = FALSE;
      for (int i=0; i < nMenuCount; i++) 
      {
         pMenu->GetMenuString(i, strMenu, MF_BYPOSITION);
         if (strMenu == strHelp)
         { 
            pSubMenu = pMenu->GetSubMenu(i);
            bFound = TRUE;
            ASSERT(pSubMenu != NULL);
         }
      }
      
      CString strTipMenu;
      strTipMenu.LoadString(CG_IDS_TIPOFTHEDAYMENU);
      if (!bFound)
      {
         // Help menu is not available. Please add it!
         if (pSubMenu == NULL) 
         {
            // The same pop-up menu is shared between mainfrm and frame 
            // with the doc.
            static CMenu popUpMenu;
            pSubMenu = &popUpMenu;
            pSubMenu->CreatePopupMenu();
            pSubMenu->InsertMenu(0, MF_STRING|MF_BYPOSITION, 
               CG_IDS_TIPOFTHEDAY, strTipMenu);
         } 
         pMenu->AppendMenu(MF_STRING|MF_BYPOSITION|MF_ENABLED|MF_POPUP, 
            (UINT)pSubMenu->m_hMenu, strHelp);
         DrawMenuBar();
      } 
      else
      {      
         // Check to see if the Tip of the Day menu has already been added.
         pSubMenu->GetMenuString(0, strMenu, MF_BYPOSITION);
         
         if (strMenu != strTipMenu) 
         {
            // Tip of the Day submenu has not been added to the 
            // first position, so add it.
            pSubMenu->InsertMenu(0, MF_BYPOSITION);  // Separator
            pSubMenu->InsertMenu(0, MF_STRING|MF_BYPOSITION, 
               CG_IDS_TIPOFTHEDAY, strTipMenu);
         }
      }
   }
}


