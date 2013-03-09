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
// $Id: ChildFrm.h,v 1.4 2002/06/27 22:32:24 mwhite Exp $
//=============================================================================
#if !defined(AFX_CHILDFRM_H__01728922_BD6E_498D_B461_C791DD07A300__INCLUDED_)
#define AFX_CHILDFRM_H__01728922_BD6E_498D_B461_C791DD07A300__INCLUDED_

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



class CChildFrame : public CMDIChildWnd
{
   DECLARE_DYNCREATE(CChildFrame)
public:
   CChildFrame();
   
public:
   
public:
   
   //{{AFX_VIRTUAL(CChildFrame)
public:
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   //}}AFX_VIRTUAL
   
public:
   virtual ~CChildFrame();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif
   
protected:
   //{{AFX_MSG(CChildFrame)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}


#endif // !defined(AFX_CHILDFRM_H__01728922_BD6E_498D_B461_C791DD07A300__INCLUDED_)
