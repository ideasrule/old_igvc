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
// $Id: DialogScrollMode.h,v 1.3 2006/10/04 00:15:47 tvlaar Exp $
//=============================================================================
#if !defined(AFX_DIALOGSCROLLMODE_H__F031C314_8B3B_42C8_A3DD_157594961DC9__INCLUDED_)
#define AFX_DIALOGSCROLLMODE_H__F031C314_8B3B_42C8_A3DD_157594961DC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTimeSliceDoc;

class DialogScrollMode : public CDialog
{
public:
   DialogScrollMode( CWnd* pParent = NULL );   
   
   CTimeSliceDoc* m_pDoc;
   
   //{{AFX_DATA(DialogScrollMode)
   enum { IDD = IDD_DIALOG_SCROLLMODE };
   CSliderCtrl	m_sliderFrame;
   CSliderCtrl	m_sliderCamera;
   CSliderCtrl	m_sliderBoth;
   CEdit	m_editCamera;
   CEdit	m_editFrame;
   int		m_iBoth;
   int		m_iCamera;
   int		m_iFrame;
   UINT	        m_uiUpdateDelay;
   //}}AFX_DATA
   
   
   //{{AFX_VIRTUAL(DialogScrollMode)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);  
   //}}AFX_VIRTUAL
   
protected:
   
   UINT_PTR m_uipTimerUID;
   
   bool m_bContUpdateFrame;
   bool m_bContUpdateCamera;
   
   //{{AFX_MSG(DialogScrollMode)
   virtual void OnCancel();
   virtual void OnOK();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   virtual BOOL OnInitDialog();
   afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
   afx_msg void OnCheckContinuousUpdate();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   afx_msg void OnRadioCamera();
   afx_msg void OnRadioFrame();
   afx_msg void OnRadioBoth();
   afx_msg void OnRadioNone();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DIALOGSCROLLMODE_H__F031C314_8B3B_42C8_A3DD_157594961DC9__INCLUDED_)
