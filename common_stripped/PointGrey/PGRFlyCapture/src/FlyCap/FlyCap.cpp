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
// $Id: FlyCap.cpp,v 1.11 2006/04/14 22:32:16 tvlaar Exp $
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "stdafx.h"
#include "MainFrm.h"
#include "FlyCap.h"
#include "FlyCapDoc.h"
#include "FlyCapView.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
  

BEGIN_MESSAGE_MAP( CFlyCapApp, CWinApp )
//{{AFX_MSG_MAP(CFlyCapApp)
//}}AFX_MSG_MAP
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


CFlyCapApp::CFlyCapApp()
{
}


CFlyCapApp theApp;


BOOL CFlyCapApp::InitInstance()
{
   AfxEnableControlContainer();
   
   // Standard initialization
   // If you are not using these features and wish to reduce the size
   //  of your final executable, you should remove from the following
   //  the specific initialization routines you do not need.
   
#ifdef _AFXDLL
#ifdef _WIN64
#else
   Enable3dControls();			// Call this when using MFC in a shared DLL
#endif

#else
   Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
   
   // Change the registry key under which our settings are stored.
   // TODO: You should modify this string to be something appropriate
   // such as the name of your company or organization.
   SetRegistryKey( _T( "Point Grey Research, Inc." ) );
   
   LoadStdProfileSettings();  // Load standard INI file options (including MRU)
   
   // Register the application's document templates.  Document templates
   //  serve as the connection between documents, frame windows and views.
   
   CSingleDocTemplate* pDocTemplate;
   pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CFlyCapDoc),
      RUNTIME_CLASS(CMainFrame),       // main SDI frame window
      RUNTIME_CLASS(CFlyCapView));
   AddDocTemplate( pDocTemplate );
   
   // Parse command line for standard shell commands, DDE, file open
   CCommandLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);
   
   // Dispatch commands specified on the command line
   if( !ProcessShellCommand(cmdInfo) )
   {
      return FALSE;
   }
   
   // The one and only window has been initialized, so show and update it.
   m_pMainWnd->ShowWindow( SW_SHOW );
   m_pMainWnd->UpdateWindow();

   ((CMainFrame*)m_pMainWnd)->resizeToMax();
   
   return TRUE;
}
