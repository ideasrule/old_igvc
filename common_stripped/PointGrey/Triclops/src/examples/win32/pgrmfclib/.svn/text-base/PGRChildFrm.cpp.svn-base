//=============================================================================
// Copyright © 2000 Point Grey Research, Inc. All Rights Reserved.
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
// $Id: PGRChildFrm.cpp,v 1.1 2007/03/27 21:36:22 demos Exp $
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRChildFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE( CPGRChildFrame, CMDIChildWnd )


BEGIN_MESSAGE_MAP( CPGRChildFrame, CMDIChildWnd )
//{{AFX_MSG_MAP(CPGRChildFrame)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CPGRChildFrame::CPGRChildFrame()
{
}


CPGRChildFrame::~CPGRChildFrame()
{
}


BOOL 
CPGRChildFrame::PreCreateWindow( CREATESTRUCT& cs )
{
   //
   // This Child Frame use used by both the image view and the open gl
   // view, but since the image views are resized before they're shown
   // we can set the size of the opengl view here without anyone noticing.
   // The alternative is to have another childframe object for the opengl
   // view.
   //
   cs.cx = 400;
   cs.cy = 300;

   return CMDIChildWnd::PreCreateWindow( cs );
}

