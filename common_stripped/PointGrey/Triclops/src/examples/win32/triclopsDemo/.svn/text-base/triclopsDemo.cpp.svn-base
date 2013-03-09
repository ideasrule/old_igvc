//=============================================================================
// Copyright © 2005 Point Grey Research, Inc. All Rights Reserved.
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
// $Id: triclopsDemo.cpp,v 1.2 2007/05/08 18:05:35 soowei Exp $
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "stdafx.h"
#include "triclopsDemo.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "DemoDoc.h"
#include "DemoView.h"
#include "Demo3dView.h"
#include "TipDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP( CDemoApp, CPGRStereoApp )
ON_COMMAND(CG_IDS_TIPOFTHEDAY, ShowTipOfTheDay)
//{{AFX_MSG_MAP(CDemoApp)
ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
ON_COMMAND(ID_FILE_NEW_CAMERA_DOCUMENT, OnFileNewCameraDocument)
ON_COMMAND(ID_FILE_NEW_OFFLINE_DOCUMENT, OnFileNewOfflineDocument)
//}}AFX_MSG_MAP
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


CDemoApp::CDemoApp()
{
   m_bOfflineDocument = false;
}

//
// The one and only CDemoApp object
//
CDemoApp theApp;

//
// CDemoApp initialization
//
BOOL 
CDemoApp::InitInstance()
{
   // CG: This line inserted by 'Tip of the Day' component.
   ShowTipAtStartup();
   
   AfxEnableControlContainer();
   
   // Standard initialization
   // If you are not using these features and wish to reduce the size
   //  of your final executable, you should remove from the following
   //  the specific initialization routines you do not need.

#if _MSC_VER < 1400
#ifdef _AFXDLL
   Enable3dControls();			// Call this when using MFC in a shared DLL
#else
   Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif
   
   // Change the registry key under which our settings are stored.
   // TODO: You should modify this string to be something appropriate
   // such as the name of your company or organization.
   SetRegistryKey( _T( "Point Grey Research, Inc." ) );
   
   LoadStdProfileSettings();  // Load standard INI file options (including MRU)
   
   // Register the application's document templates.  Document templates
   // serve as the connection between documents, frame windows and views.
   
   CMultiDocTemplate* pDocTemplate;
   pDocTemplate = new CMultiDocTemplate(
      IDR_DIGICLTYPE,
      RUNTIME_CLASS( CDemoDoc ),
      RUNTIME_CLASS( CChildFrame ), // custom MDI child frame
      RUNTIME_CLASS( CDemoView ) );
   AddDocTemplate(pDocTemplate);
   
   // add doc template for our 3dView
   m_pTemplate3dView = new CMultiDocTemplate(
      IDR_DIGICLTYPE,
      RUNTIME_CLASS( CDemoDoc) ,
      RUNTIME_CLASS( CChildFrame ),
      RUNTIME_CLASS( CDemo3dView ) );
   
   
   // create main MDI Frame window
   CMainFrame* pMainFrame = new CMainFrame;
   if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
   {
      return FALSE;
   }

   m_pMainWnd = pMainFrame;
   
   // Parse command line for standard shell commands, DDE, file open
   CCommandLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);

   // The main window has been initialized, so show and update it.
   pMainFrame->ShowWindow(m_nCmdShow);
   pMainFrame->UpdateWindow();
   
   // Dispatch commands specified on the command line
   if (!ProcessShellCommand(cmdInfo))
   {
      return FALSE;
   }
   
   return TRUE;
}


int 
CDemoApp::ExitInstance() 
{
   if( m_pTemplate3dView != NULL )
   {
      delete m_pTemplate3dView;
      m_pTemplate3dView = NULL;
   }
      
   return CPGRStereoApp::ExitInstance();
}


void 
CDemoApp::ShowTipAtStartup()
{
   CCommandLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);
   
   if( cmdInfo.m_bShowSplash )
   {
      CTipDlg dlg;
      if (dlg.m_bStartup)
      {
         dlg.DoModal();
      }
   }
}


void 
CDemoApp::ShowTipOfTheDay()
{
   CTipDlg dlg;
   dlg.DoModal();
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
CAboutDlg::DoDataExchange( CDataExchange* pDX )
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAboutDlg)
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( CAboutDlg, CDialog )
//{{AFX_MSG_MAP(CAboutDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void 
CDemoApp::OnAppAbout()
{
   CAboutDlg aboutDlg;
   aboutDlg.DoModal();
}



//
// Stubs that call the base-class
// Here because its easier for the class wizard to manage
//

void CDemoApp::OnFileNewCameraDocument() 
{
   // set the document type to be not offline
   m_bOfflineDocument = false;
   // create a new document
   OnFileNew();
}

void CDemoApp::OnFileNewOfflineDocument() 
{
   // set the document type to be not offline
   m_bOfflineDocument = true;
   // create a new document
   OnFileNew();
}


