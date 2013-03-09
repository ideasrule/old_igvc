//=============================================================================
// Copyright © 2003 Point Grey Research, Inc. All Rights Reserved.
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
// $Id: PGRMainFrm.h,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================
#ifndef __PGRMAINFRM_H__
#define __PGRMAINFRM_H__

#pragma once

//=============================================================================
// System Includes
//=============================================================================
#include <afxext.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRStereoDialogBar.h"



/**
 * The PGRMFC version of CMDIFrameWnd.  The default imlementation only 
 * overrides OnCreate() so that it can set up the Dialog Bar. 
 */
class CPGRMainFrame : public CMDIFrameWnd
{
   DECLARE_DYNAMIC(CPGRMainFrame)
public:
   CPGRMainFrame();
   virtual ~CPGRMainFrame();
   
public:

   /**
    * Enum of the fields that can be updated - since the status bar indicies
    * have to be hardcoded, just hardcode them in one place.
    */
   enum StatusBarField
   {
      FRAMERATE,
      STEREO,
      RGB,
      TIMESTAMP,
      TIMESTAMP1394,
   };

   /**
    * Our custom Dialog Bar for allowing image selection, resolution, etc.
    * this need to be public since it is a coordinating class for the
    * PGRImageView
    */
   CPGRStereoDialogBar m_dialogBarStereo;

   /** Update a status bar field. */
   void updateStatusBar( StatusBarField field, const char* pszText );

   
public:
   
   //{{AFX_VIRTUAL(CPGRMainFrame)
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   //}}AFX_VIRTUAL
   
protected:  

   /** Default MFC Status Bar class. */
   CStatusBar  m_wndStatusBar;


protected:
   //{{AFX_MSG(CPGRMainFrame)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}


#endif // #ifndef __PGRMAINFRM_H__
