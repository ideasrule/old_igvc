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
// $Id: timeSliceDemo.h,v 1.7 2002/06/27 22:32:24 mwhite Exp $
//=============================================================================
#if !defined(AFX_TIMESLICEDEMO_H__E2C26C43_93A1_4E8C_ABC6_6CE46DBDAF69__INCLUDED_)
#define AFX_TIMESLICEDEMO_H__E2C26C43_93A1_4E8C_ABC6_6CE46DBDAF69__INCLUDED_

//=============================================================================
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


//
// Maximum vertical image tweak.  This must also be changed in the resources.
//
#define MAX_VERTICAL_TWEAK 60  



#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


class CTimeSliceApp : public CWinApp
{
public:
   void setViews( int iViews );

   CTimeSliceApp();
   
   //{{AFX_VIRTUAL(CTimeSliceApp)
public:
   virtual BOOL InitInstance();
   //}}AFX_VIRTUAL
   
   //{{AFX_MSG(CTimeSliceApp)
   afx_msg void OnAppAbout();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      
protected:
   
   int m_iViews;
};


//{{AFX_INSERT_LOCATION}}


#endif // !defined(AFX_TIMESLICEDEMO_H__E2C26C43_93A1_4E8C_ABC6_6CE46DBDAF69__INCLUDED_)
