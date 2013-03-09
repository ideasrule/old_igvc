//=============================================================================
// Copyright © 2002 Point Grey Research, Inc. All Rights Reserved.
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
// $Id: timeSliceDemo.cpp,v 1.6 2006/05/05 20:34:34 marko Exp $
//=============================================================================

//=============================================================================
//
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
#include "ChildFrm.h"
#include "timeSliceDoc.h"
#include "timeSliceView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP( CTimeSliceApp, CWinApp )
//{{AFX_MSG_MAP(CTimeSliceApp)
ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
//}}AFX_MSG_MAP
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


CTimeSliceApp::CTimeSliceApp()
{
   m_iViews = 0;
}


CTimeSliceApp theApp;


BOOL
CTimeSliceApp::InitInstance()
{
   // Standard initialization
   
#ifdef _AFXDLL
   Enable3dControls();			// Call this when using MFC in a shared DLL
#else
   Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
   
   // Change the registry key under which our settings are stored.
   SetRegistryKey(_T( "Point Grey Research, Inc." ));
   
   LoadStdProfileSettings( 0 );  // Load standard INI file options (including MRU)
   
   // Register document templates
   
   CMultiDocTemplate* pDocTemplate;
   pDocTemplate = new CMultiDocTemplate(
      IDR_TIMESLTYPE,
      RUNTIME_CLASS( CTimeSliceDoc ),
      RUNTIME_CLASS( CChildFrame ), // custom MDI child frame
      RUNTIME_CLASS( CTimeSliceView ));
   AddDocTemplate( pDocTemplate );
   
   // create main MDI Frame window
   CMainFrame* pMainFrame = new CMainFrame;
   if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
      return FALSE;
   m_pMainWnd = pMainFrame;
   
   // Parse command line for standard shell commands, DDE, file open
   CCommandLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);
   
   // Dispatch commands specified on the command line
   if (!ProcessShellCommand(cmdInfo))
      return FALSE;

   //
   // Create more child frames for all the views that we need.  This is
   // the number of cameras, plus one panning view.
   //
   if( m_iViews == 0 )
   {
      return FALSE;
   }

   for( int iViews = 0; iViews < m_iViews; iViews++ )
   {
      pMainFrame->OnWindowNewCustom();
   }

   pMainFrame->ShowWindow( m_nCmdShow );
   pMainFrame->UpdateWindow();

   return TRUE;
}


void 
CTimeSliceApp::setViews( int iViews )
{
   m_iViews = iViews;
}


class CAboutDlg : public CDialog
{
public:
   CAboutDlg();
   
   //{{AFX_DATA(CAboutDlg)
   enum { IDD = IDD_ABOUTBOX };
   //}}AFX_DATA
   
   //{{AFX_VIRTUAL(CAboutDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);
   //}}AFX_VIRTUAL
   
protected:
   //{{AFX_MSG(CAboutDlg)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
   //{{AFX_DATA_INIT(CAboutDlg)
   //}}AFX_DATA_INIT
}

void 
CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAboutDlg)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void 
CTimeSliceApp::OnAppAbout()
{
   CAboutDlg aboutDlg;
   aboutDlg.DoModal();
}


