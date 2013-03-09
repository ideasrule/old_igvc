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
// $Id: triclopsDemo.h,v 1.1 2007/03/27 21:33:01 demos Exp $
//=============================================================================
#if !defined(AFX_TRICLOPSDEMO_H__166BDD21_1CD3_47DF_836E_0AFAF3ABAE97__INCLUDED_)
#define AFX_TRICLOPSDEMO_H__166BDD21_1CD3_47DF_836E_0AFAF3ABAE97__INCLUDED_

#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif


//=============================================================================
// System Includes
//=============================================================================

//=============================================================================
// PGR Includes
//=============================================================================
#include <PGRStereoApp.h>
#include <PGRResource.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "resource.h"


/**
 * This is the main MFC CApp class.
 */
class CDemoApp : public CPGRStereoApp
{
public:
   CDemoApp();

   // 
   // This is a little trick : this variable defines what
   // kind of document to start.  The document gets a pointer back to
   // the parent app and checks this variable to determine if it should
   // start an offline or camera-connected document
   //
   bool	 m_bOfflineDocument;
   
   //{{AFX_VIRTUAL(CDemoApp)
public:
   virtual BOOL InitInstance();
   virtual int ExitInstance();
   //}}AFX_VIRTUAL
   
   // Implementation
   //{{AFX_MSG(CDemoApp)
   afx_msg void OnAppAbout();
   afx_msg void OnFileNewCameraDocument();
   afx_msg void OnFileNewOfflineDocument();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      
public:

   // Template for live view of 3d data.
   CMultiDocTemplate*  m_pTemplate3dView;


protected:

   void ShowTipAtStartup();
   void ShowTipOfTheDay();
};


//{{AFX_INSERT_LOCATION}}


#endif // !defined(AFX_TRICLOPSDEMO_H__166BDD21_1CD3_47DF_836E_0AFAF3ABAE97__INCLUDED_)
