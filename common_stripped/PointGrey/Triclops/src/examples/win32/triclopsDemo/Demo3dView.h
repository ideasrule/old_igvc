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
// $Id: Demo3dView.h,v 1.1 2007/03/27 21:33:00 demos Exp $
//=============================================================================
#if !defined(AFX_Demo3DVIEW_H__61FFEABF_CA7B_4A55_AC02_046E8F81961C__INCLUDED_)
#define AFX_Demo3DVIEW_H__61FFEABF_CA7B_4A55_AC02_046E8F81961C__INCLUDED_

#pragma once

//=============================================================================
// PGR Includes
//=============================================================================
#include "PGROpenGLView.h"


/**
 * This is the 3d (opengl) view class.
 */
class CDemo3dView : public CPGROpenGLView
{
protected:
   CDemo3dView();
   DECLARE_DYNCREATE( CDemo3dView )
      
public:
   
   //{{AFX_VIRTUAL(CDemo3dView)
protected:
   //}}AFX_VIRTUAL
   
protected:
   virtual ~CDemo3dView();
   
protected:
   //{{AFX_MSG(CDemo3dView)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_Demo3DVIEW_H__61FFEABF_CA7B_4A55_AC02_046E8F81961C__INCLUDED_)

