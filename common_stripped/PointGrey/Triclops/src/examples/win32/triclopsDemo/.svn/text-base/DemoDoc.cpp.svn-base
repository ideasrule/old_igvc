//=============================================================================
// Copyright © 2004 Point Grey Research, Inc. All Rights Reserved.
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
// $Id: DemoDoc.cpp,v 1.1 2007/03/27 21:33:00 demos Exp $
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
#include "triclopsDemo.h"
#include "DemoDoc.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE( CDemoDoc, CPGRStereoDoc )

BEGIN_MESSAGE_MAP( CDemoDoc, CPGRStereoDoc )
//{{AFX_MSG_MAP(CDemoDoc)
ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
ON_COMMAND(ID_FILE_LOAD_CAL, OnFileLoadCal)
ON_COMMAND(ID_FILE_LOAD_STEREO, OnFileLoadStereoImage)
ON_COMMAND(ID_FILE_SAVE_CURR_CAL, OnFileSaveCurrCal)
ON_COMMAND(ID_FILE_SAVE_DEFAULT_CAL, OnFileSaveDefaultCal)
ON_COMMAND(ID_FILE_SAVE_DISPARITY, OnFileSaveDisparity)
ON_COMMAND(ID_FILE_SAVE_EDGE_LEFT, OnFileSaveEdgeLeft)
ON_COMMAND(ID_FILE_SAVE_EDGE_RIGHT, OnFileSaveEdgeRight)
ON_COMMAND(ID_FILE_SAVE_EDGE_CENTER, OnFileSaveEdgeCenter)
ON_COMMAND(ID_FILE_SAVE_POINTCLOUD, OnFileSavePointcloud)
ON_COMMAND(ID_FILE_SAVE_RAW_LEFT, OnFileSaveRawLeft)
ON_COMMAND(ID_FILE_SAVE_RAW_RIGHT, OnFileSaveRawRight)
ON_COMMAND(ID_FILE_SAVE_RAW_CENTER, OnFileSaveRawCenter)
ON_COMMAND(ID_FILE_SAVE_RAW_STEREO, OnFileSaveRawStereo)
ON_COMMAND(ID_FILE_SAVE_RECTIFIED_COLOUR_LEFT, OnFileSaveRectifiedColorLeft)
ON_COMMAND(ID_FILE_SAVE_RECTIFIED_COLOUR_RIGHT, OnFileSaveRectifiedColorRight)
ON_COMMAND(ID_FILE_SAVE_RECTIFIED_COLOUR_CENTER, OnFileSaveRectifiedColorCenter)
ON_COMMAND(ID_FILE_SAVE_RECTIFIED_LEFT, OnFileSaveRectifiedLeft)
ON_COMMAND(ID_FILE_SAVE_RECTIFIED_RIGHT, OnFileSaveRectifiedRight)
ON_COMMAND(ID_FILE_SAVE_RECTIFIED_CENTER, OnFileSaveRectifiedCenter)
ON_COMMAND(ID_FILE_SAVE_STEREO, OnFileSaveStereo)
ON_UPDATE_COMMAND_UI(ID_FILE_LOAD_STEREO, OnUpdateFileLoadStereoImage)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_RAW_RIGHT, OnUpdateFileSaveRawRight)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_RAW_LEFT, OnUpdateFileSaveRawLeft)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_RAW_CENTER, OnUpdateFileSaveRawCenter)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_RECTIFIED_CENTER, OnUpdateFileSaveRectifiedCenter)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_RECTIFIED_COLOUR_CENTER, OnUpdateFileSaveRectifiedColorCenter)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_EDGE_CENTER, OnUpdateFileSaveEdgeCenter)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_POINTCLOUD, OnUpdateFileSavePointcloud)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_POINTCLOUD, OnUpdateFileSaveStereo)
ON_COMMAND(ID_APP_VERSION_INFO, OnAppVersionInfo)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CDemoDoc::CDemoDoc()
{
}


CDemoDoc::~CDemoDoc()
{
}

void	
CDemoDoc::customProcess()
{  
   CPGRStereoDoc::customProcess();
}


void
CDemoDoc::turnOnComputePoints()
{
   m_dialogStereo.m_bPoints = TRUE;
   if (m_bStereoDialogActive)
   {
      m_dialogStereo.UpdateData( FALSE );
   }
}

BOOL CDemoDoc::OnFileNewCameraDocument()
{
   BOOL ret = CPGRStereoDoc::OnFileNewCameraDocument();
   if ( !ret )
   {
      AfxMessageBox( "Document creation failed.\n"
		     "Use File menu to create an offline document.\n" );
      return FALSE;
   }
   return TRUE;
}

BOOL CDemoDoc::OnFileNewOfflineDocument()
{
   BOOL ret = CPGRStereoDoc::OnFileNewOfflineDocument();
   if ( !ret )
   {
      AfxMessageBox( "Document creation failed\n" );
      return FALSE;
   }
   return TRUE;
}


//
// Pass through functions - these are just stubs that call the corresponding
// PGRStereoDoc function
//

void CDemoDoc::OnFileClose() 
{
   CPGRStereoDoc::OnFileClose();	
}

void CDemoDoc::OnFileLoadCal() 
{
   CPGRStereoDoc::OnFileLoadCal();	
}

void CDemoDoc::OnFileLoadStereoImage() 
{
   CPGRStereoDoc::OnFileLoadStereoImage();	
}


void CDemoDoc::OnFileSaveCurrCal() 
{
   CPGRStereoDoc::OnFileSaveCurrCal();	
}

void CDemoDoc::OnFileSaveDefaultCal() 
{
   CPGRStereoDoc::OnFileSaveDefaultCal();	
}

void CDemoDoc::OnFileSaveDisparity() 
{
   CPGRStereoDoc::OnFileSaveDisparity();	
}

void CDemoDoc::OnFileSaveEdgeLeft() 
{
   CPGRStereoDoc::OnFileSaveEdgeLeft();	
}

void CDemoDoc::OnFileSaveEdgeRight() 
{
   CPGRStereoDoc::OnFileSaveEdgeRight();	
}

void CDemoDoc::OnFileSaveEdgeCenter() 
{
   CPGRStereoDoc::OnFileSaveEdgeCenter();
}

void CDemoDoc::OnFileSavePointcloud() 
{
   CPGRStereoDoc::OnFileSavePointcloud();	
}

void CDemoDoc::OnFileSaveRawLeft() 
{
   CPGRStereoDoc::OnFileSaveRawLeft();
}

void CDemoDoc::OnFileSaveRawStereo() 
{
   CPGRStereoDoc::OnFileSaveRawStereo();	
}

void CDemoDoc::OnFileSaveRawRight() 
{
   CPGRStereoDoc::OnFileSaveRawRight();	
}

void CDemoDoc::OnFileSaveRawCenter() 
{
   CPGRStereoDoc::OnFileSaveRawCenter();	
}

void CDemoDoc::OnFileSaveRectifiedColorLeft() 
{
   CPGRStereoDoc::OnFileSaveRectifiedColorLeft();
}

void CDemoDoc::OnFileSaveRectifiedColorRight() 
{
   CPGRStereoDoc::OnFileSaveRectifiedColorRight();
}

void CDemoDoc::OnFileSaveRectifiedColorCenter() 
{
   CPGRStereoDoc::OnFileSaveRectifiedColorCenter();	
}

void CDemoDoc::OnFileSaveRectifiedLeft() 
{
   CPGRStereoDoc::OnFileSaveRectifiedLeft();	
}

void CDemoDoc::OnFileSaveRectifiedRight() 
{
   CPGRStereoDoc::OnFileSaveRectifiedRight();	
}

void CDemoDoc::OnFileSaveRectifiedCenter() 
{
   CPGRStereoDoc::OnFileSaveRectifiedCenter();
}

void CDemoDoc::OnFileSaveStereo() 
{
   CPGRStereoDoc::OnFileSaveStereo();
}

BOOL CDemoDoc::OnNewDocument()
{
   // checks the app default for creating an offline or
   // camera document.  The default is camera and
   // this gets called when the app starts (from 
   // ProcessShellCommand()) so the first thing the app does
   // is try to connect to a camera.
   CDemoApp* pApp = (CDemoApp*)AfxGetApp();
   if ( pApp->m_bOfflineDocument )
   {
      return OnFileNewOfflineDocument();
   }
   return OnFileNewCameraDocument();
}


void CDemoDoc::OnUpdateFileLoadStereoImage(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( m_bOffline );	
}

void CDemoDoc::OnUpdateFileSavePointcloud(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( m_dialogStereo.m_bPoints );	
}

void CDemoDoc::OnUpdateFileSaveStereo(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( FALSE );	
}

void CDemoDoc::OnUpdateFileSaveRawRight(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( FALSE );	
}

void CDemoDoc::OnUpdateFileSaveRawLeft(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( FALSE );	
}

void CDemoDoc::OnUpdateFileSaveRawCenter(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( FALSE );	
}

void CDemoDoc::OnUpdateFileSaveRectifiedCenter(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( FALSE );	
}

void CDemoDoc::OnUpdateFileSaveRectifiedColorCenter(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( FALSE );	
}

void CDemoDoc::OnUpdateFileSaveEdgeCenter(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable( FALSE );	
}

void CDemoDoc::OnAppVersionInfo() 
{
   CPGRStereoDoc::OnAppVersionInfo();
}
