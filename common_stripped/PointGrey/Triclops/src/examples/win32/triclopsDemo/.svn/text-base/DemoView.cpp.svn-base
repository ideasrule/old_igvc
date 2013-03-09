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
// $Id: DemoView.cpp,v 1.1 2007/03/27 21:33:00 demos Exp $
//=============================================================================
//=============================================================================
// Project Includes
//=============================================================================
#include "stdafx.h"
#include "triclopsDemo.h"
#include "DemoDoc.h"
#include "DemoView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




IMPLEMENT_DYNCREATE( CDemoView, CPGRImageView )

BEGIN_MESSAGE_MAP( CDemoView, CPGRImageView )
//{{AFX_MSG_MAP(CDemoView)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()



CDemoView::CDemoView()
{
}


CDemoView::~CDemoView()
{
}


#ifdef _DEBUG
CDemoDoc* 
CDemoView::GetDocument() // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDemoDoc)));
   return (CDemoDoc*)m_pDocument;
}
#endif //_DEBUG
