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
// $Id: TipDlg.h,v 1.1 2007/03/27 21:33:01 demos Exp $
//=============================================================================
#ifndef __TIPDLG_H__
#define __TIPDLG_H__

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
 * This class implements the 'tip of the day' functionality'.  See 
 * the 'TriclopsDemoTips.txt' file.
 */
class CTipDlg : public CDialog
{
public:
   CTipDlg(CWnd* pParent = NULL);
   
   //{{AFX_DATA(CTipDlg)
   // enum { IDD = IDD_TIP };
   BOOL	m_bStartup;
   CString	m_strTip;
   //}}AFX_DATA
   
   FILE* m_pStream;
   
   //{{AFX_VIRTUAL(CTipDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);
   //}}AFX_VIRTUAL
   
public:
   virtual ~CTipDlg();
   
protected:
   //{{AFX_MSG(CTipDlg)
   afx_msg void OnNextTip();
   afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   afx_msg void OnPaint();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      
   void GetNextTipString( CString& strNext );
};


#endif // #ifndef __TIPDLG_H__
