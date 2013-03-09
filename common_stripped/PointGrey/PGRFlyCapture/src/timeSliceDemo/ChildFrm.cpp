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
// $Id: ChildFrm.cpp,v 1.4 2002/06/27 22:32:24 mwhite Exp $
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
#include "ChildFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE( CChildFrame, CMDIChildWnd )

BEGIN_MESSAGE_MAP( CChildFrame, CMDIChildWnd )
//{{AFX_MSG_MAP(CChildFrame)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()



CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

BOOL 
CChildFrame::PreCreateWindow( CREATESTRUCT& cs )
{
   if( !CMDIChildWnd::PreCreateWindow(cs) )
      return FALSE;
   
   cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
      | FWS_ADDTOTITLE | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

   cs.style &= ~FWS_ADDTOTITLE;
   cs.style &= ~WS_SIZEBOX;
      
   return TRUE;
}


#ifdef _DEBUG
void 
CChildFrame::AssertValid() const
{
   CMDIChildWnd::AssertValid();
}

void 
CChildFrame::Dump(CDumpContext& dc) const
{
   CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

