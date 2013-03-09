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
// $Id: PGRChildFrm.h,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================
#ifndef __PGRCHILDFRM_H__
#define __PGRCHILDFRM_H__

#pragma once

//=============================================================================
// System Includes
//=============================================================================
#include <afxwin.h>



/**
 * This is PGRMFC version of CMDIChildWnd.  The only thing it overrides from
 * the default MFC class is PreCreateWindow(), where it sets a default size
 * of the window.  This size is only relevant to PGROpenGLView windows,
 * since PGRImageView windows resize themselves to the image resolution as 
 * soon as they are created.
 */
class CPGRChildFrame : public CMDIChildWnd
{
   DECLARE_DYNCREATE( CPGRChildFrame )
public:
   CPGRChildFrame(); 
   
public:
   
   //{{AFX_VIRTUAL(CPGRChildFrame)
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   //}}AFX_VIRTUAL
   
public:
   virtual ~CPGRChildFrame();

   
protected:
   //{{AFX_MSG(CPGRChildFrame)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};



//{{AFX_INSERT_LOCATION}}

#endif // #ifndef __PGRCHILDFRM_H__
