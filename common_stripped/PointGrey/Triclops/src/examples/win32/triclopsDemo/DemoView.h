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
// $Id: DemoView.h,v 1.1 2007/03/27 21:33:00 demos Exp $
//=============================================================================
#if !defined(AFX_DemoVIEW_H__375EE7BD_5F8B_4999_A524_F4A31308BAFE__INCLUDED_)
#define AFX_DemoVIEW_H__375EE7BD_5F8B_4999_A524_F4A31308BAFE__INCLUDED_

#pragma once

//=============================================================================
// System Includes
//=============================================================================

//=============================================================================
// PGR Includes
//=============================================================================
#include "PGRImageView.h"

//=============================================================================
// Project Includes
//=============================================================================


/**
 * This is the image view class.
 */
class CDemoView : public CPGRImageView
{
protected: 
   CDemoView();
   DECLARE_DYNCREATE(CDemoView)
      
public:
   CDemoDoc* GetDocument();
   
public:
   virtual ~CDemoView();

protected:
   //{{AFX_MSG(CDemoView)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline 
CDemoDoc* CDemoView::GetDocument()
{ 
   return (CDemoDoc*)m_pDocument; 
}
#endif


//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DemoVIEW_H__375EE7BD_5F8B_4999_A524_F4A31308BAFE__INCLUDED_)
