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
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: SerialPortExDlg.h,v 1.3 2005/03/17 01:29:41 matt Exp $
//=============================================================================

#if !defined(AFX_SERIALPORTEXDLG_H__4E71923C_13BD_4BF9_9745_3E02AD7ED3FB__INCLUDED_)
#define AFX_SERIALPORTEXDLG_H__4E71923C_13BD_4BF9_9745_3E02AD7ED3FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//=============================================================================
// PGR Includes
//=============================================================================
#include "pgrflycapture.h"
#include "pgrflycaptureplus.h"
#include "pgrcameragui.h"

class CSerialPortExDlg : public CDialog
{
   // Construction
public:
   CSerialPortExDlg(CWnd* pParent = NULL);	// standard constructor
   
   // Dialog Data
   //{{AFX_DATA(CSerialPortExDlg)
	enum { IDD = IDD_SERIALPORTEX_DIALOG };
   CString	m_csSendData;
   CString	m_csReceiveData;
	CString	m_csRegister;
	CString	m_csValue_0_7;
	CString	m_csValue_16_23;
	CString	m_csValue_24_31;
	CString	m_csValue_8_15;
	BOOL	m_boolBroadcast;
	//}}AFX_DATA
   
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CSerialPortExDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
   //}}AFX_VIRTUAL
   
   // Implementation
protected:
   HICON m_hIcon;
   
   // Generated message map functions
   //{{AFX_MSG(CSerialPortExDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
   afx_msg void OnPaint();
   afx_msg HCURSOR OnQueryDragIcon();
   afx_msg void OnSendButton();
	afx_msg void OnButtonGetRegister();
	afx_msg void OnButtonSetRegister();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      
   FlyCaptureContext m_context;
   CameraGUIContext  m_guicontext;

   static UINT threadReceive( void* pParam );
   UINT receiveLoop();
   void updateConnectionParameters();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALPORTEXDLG_H__4E71923C_13BD_4BF9_9745_3E02AD7ED3FB__INCLUDED_)
