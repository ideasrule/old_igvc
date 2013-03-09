//=============================================================================
// Copyright © 2005 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with PGR.
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: PGRMainFrm.cpp,v 1.1 2007/03/27 21:36:22 demos Exp $
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRMainFrm.h"
#include "PGRResource.h"
#include "PGRStereoDialogBar.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC( CPGRMainFrame, CMDIFrameWnd )

BEGIN_MESSAGE_MAP( CPGRMainFrame, CMDIFrameWnd )
//{{AFX_MSG_MAP(CPGRMainFrame)
ON_WM_CREATE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


static UINT g_indicators[] =
{
   ID_SEPARATOR,
   ID_INDICATOR_FRAMERATE,
   ID_INDICATOR_STEREO,
   ID_INDICATOR_RGB,
   ID_INDICATOR_TIMESTAMP,
   ID_INDICATOR_1394TIMESTAMP,
};


CPGRMainFrame::CPGRMainFrame()
{
}


CPGRMainFrame::~CPGRMainFrame()
{
}


void 
CPGRMainFrame::updateStatusBar( StatusBarField field, const char* pszText )
{
   if( m_wndStatusBar.IsWindowVisible() )
   {
      int iIndex;

      switch( field )
      {
      case FRAMERATE:
	 iIndex = 1;
	 break;

      case STEREO:
	 iIndex = 2;
	 break;

      case RGB:
	 iIndex = 3;
	 break;

      case TIMESTAMP:
	 iIndex = 4;
	 break;

      case TIMESTAMP1394:
	 iIndex = 5;
	 break;

      default:
	 ASSERT( FALSE );
	 return;
      }

      m_wndStatusBar.SetPaneText( iIndex, pszText );
   }
}


int 
CPGRMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
   if( CMDIFrameWnd::OnCreate( lpCreateStruct ) == -1 )
   {
      return -1;
   }

   //
   // Create stereo dialog bar.
   //
   if( !m_dialogBarStereo.Create( 
      this, PGRRES_STEREO_DIALOGBAR, CBRS_TOP, PGRRES_STEREO_DIALOGBAR ) )
   {
      TRACE0( "Failed to create dialog bar\n" );
      return -1;
   }

   //
   // Create status bar.
   //
   if ( !m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators( 
	    g_indicators, sizeof( g_indicators )/sizeof( UINT ) ) )
   {
      TRACE0("Failed to create status bar\n");
      return -1;      // fail to create
   }
   
   return 0;
}


BOOL 
CPGRMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
   if( !CMDIFrameWnd::PreCreateWindow( cs ) )
   {
      return FALSE;
   }

   return TRUE;
}

