//=============================================================================
// Copyright © 2004 Point Grey Research, Inc. All Rights Reserved.
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
//=============================================================================
//=============================================================================
// $Id: ComboBoxColour.h,v 1.3 2007/12/13 23:35:19 soowei Exp $
//=============================================================================
#if !defined(AFX_COMBOBOXCOLOUR_H__1385AA17_2965_4420_8ACC_F8D66DD674CF__INCLUDED_)
#define AFX_COMBOBOXCOLOUR_H__1385AA17_2965_4420_8ACC_F8D66DD674CF__INCLUDED_
//=============================================================================
// System Includes
//=============================================================================
#include "stdafx.h"
//=============================================================================
// PGR Includes
//=============================================================================
//=============================================================================
// Project Includes
//=============================================================================
#include "vfw.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable:4201 )

class CComboBoxColour : public CComboBox
{
// Construction
public:
	CComboBoxColour();

        void setICInfos( ICINFO* picinfo, int iNumICInfo );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxColour)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboBoxColour();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboBoxColour)
	//}}AFX_MSG

        ICINFO* m_picinfo;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOBOXCOLOUR_H__1385AA17_2965_4420_8ACC_F8D66DD674CF__INCLUDED_)
