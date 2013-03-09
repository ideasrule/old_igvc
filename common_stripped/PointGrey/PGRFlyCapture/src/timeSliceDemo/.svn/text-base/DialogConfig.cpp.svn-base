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
// $Id: DialogConfig.cpp,v 1.8 2002/06/27 22:32:24 mwhite Exp $
//=============================================================================
//=============================================================================
//
//
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "stdafx.h"
#include "timeSliceDemo.h"
#include "DialogConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDialogConfig::CDialogConfig( CWnd* pParent /*=NULL*/ )
   : CDialog( CDialogConfig::IDD, pParent )
{
   //{{AFX_DATA_INIT(CDialogConfig)
   m_iTweak0   = 0;
   m_iTweak1   = 0;
   m_iTweak2   = 0;
   m_iTweak3   = 0;
   m_iTweak4   = 0;
   m_iTweak5   = 0;
   m_iTweak6   = 0;
   m_iTweak7   = 0;
   m_iTweak8   = 0;
   m_iTweak9   = 0;
   m_iTweak10  = 0;
   m_iTweak11  = 0;
   m_iTweak12  = 0;
   m_iTweak13  = 0;
   m_iTweak14  = 0;

   //m_iTimer    = -1;
   //m_iTrigger  = -1;
   //}}AFX_DATA_INIT
}


void 
CDialogConfig::DoDataExchange( CDataExchange* pDX )
{
   CDialog::DoDataExchange( pDX );
   //{{AFX_DATA_MAP(CDialogConfig)
   DDX_Text(pDX, IDC_EDIT_TWEAK0, m_iTweak0);
   DDV_MinMaxInt(pDX, m_iTweak0, 0, 60);
   DDX_Text(pDX, IDC_EDIT_TWEAK1, m_iTweak1);
   DDV_MinMaxInt(pDX, m_iTweak1, 0, 60);
   DDX_Text(pDX, IDC_EDIT_TWEAK2, m_iTweak2);
   DDV_MinMaxInt(pDX, m_iTweak2, 0, 60);
   DDX_Text(pDX, IDC_EDIT_TWEAK3, m_iTweak3);
   DDV_MinMaxInt(pDX, m_iTweak3, 0, 60);
   DDX_Text(pDX, IDC_EDIT_TWEAK4, m_iTweak4);
   DDV_MinMaxInt(pDX, m_iTweak4, 0, 60);
   DDX_Text(pDX, IDC_EDIT_TWEAK5, m_iTweak5);
   DDV_MinMaxInt(pDX, m_iTweak5, 0, 60);
   DDX_Text(pDX, IDC_EDIT_TWEAK6, m_iTweak6);
   DDV_MinMaxInt(pDX, m_iTweak6, 0, 60);
   DDX_Text(pDX, IDC_EDIT_TWEAK7, m_iTweak7);
   DDV_MinMaxInt(pDX, m_iTweak7, 0, 60);

   DDX_Text(pDX, IDC_EDIT_TWEAK8, m_iTweak8);
   DDV_MinMaxInt(pDX, m_iTweak8, 0, 60);
   DDX_Text(pDX, IDC_EDIT_TWEAK9, m_iTweak9);
   DDV_MinMaxInt(pDX, m_iTweak9, 0, 60);
   DDX_Text(pDX, IDC_EDIT_TWEAK10, m_iTweak10);
   DDV_MinMaxInt(pDX, m_iTweak10, 0, 60);
   DDX_Text(pDX, IDC_EDIT_TWEAK11, m_iTweak11);
   DDV_MinMaxInt(pDX, m_iTweak11, 0, 60);
   DDX_Text(pDX, IDC_EDIT_TWEAK12, m_iTweak12);
   DDV_MinMaxInt(pDX, m_iTweak12, 0, 60);
   DDX_Text(pDX, IDC_EDIT_TWEAK13, m_iTweak13);
   DDV_MinMaxInt(pDX, m_iTweak13, 0, 60);

   //DDX_Radio(pDX, IDC_RADIO_TIMER, m_iTimer);
   //DDX_Radio(pDX, IDC_RADIO_TIMER, m_iTrigger);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( CDialogConfig, CDialog )
//{{AFX_MSG_MAP(CDialogConfig)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL 
CDialogConfig::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   return TRUE;
}
