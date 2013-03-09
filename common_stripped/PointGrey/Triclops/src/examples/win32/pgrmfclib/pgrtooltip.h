//=============================================================================
// Copyright © 2005 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with Point Grey Research, Inc. (PGR).
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: pgrtooltip.h,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================
#ifndef __PGRTOOLTIP_H__
#define __PGRTOOLTIP_H__

#pragma once

//=============================================================================
// System Includes
//=============================================================================

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================



/**
 * A subclass implementation of CToolTipCtrl as explained in Chap 16 of
 * Programming Windows with MFC 2nd Edtn
 */
class CPGRToolTipCtrl : public CToolTipCtrl
{
public:

   BOOL	AddWindowTool( CWnd* pWnd, LPCTSTR pszText );

   BOOL	AddRectTool( 
      CWnd* pWnd, LPCTSTR pszText, LPCRECT pRect, UINT nIDTool );

protected:

};


inline BOOL	 
CPGRToolTipCtrl::AddWindowTool( CWnd* pWnd, LPCTSTR pszText )
{
   TOOLINFO ti;
   ti.cbSize   = sizeof( TOOLINFO );
   ti.uFlags   = TTF_IDISHWND | TTF_SUBCLASS;
   ti.hwnd     = pWnd->GetParent()->GetSafeHwnd();
   ti.uId      = (UINT) pWnd->GetSafeHwnd();
   ti.hinst    = AfxGetInstanceHandle();
   ti.lpszText = (LPTSTR) pszText;

   return (BOOL)SendMessage( TTM_ADDTOOL, 0, (LPARAM) &ti );
}


inline BOOL	 
CPGRToolTipCtrl::AddRectTool( CWnd* pWnd, 
   			      LPCTSTR pszText,
			      LPCRECT lpRect,
			      UINT nIDTool )
{
   TOOLINFO ti;
   ti.cbSize   = sizeof( TOOLINFO );
   ti.uFlags   = TTF_SUBCLASS;
   ti.hwnd     = pWnd->GetSafeHwnd();
   ti.uId      = nIDTool,
   ti.hinst    = AfxGetInstanceHandle();
   ti.lpszText = (LPTSTR) pszText;
   ::CopyRect( &ti.rect, lpRect );

   return (BOOL)SendMessage( TTM_ADDTOOL, 0, (LPARAM) &ti );
}


#endif // #ifndef __PGRTOOLTIP_H__
