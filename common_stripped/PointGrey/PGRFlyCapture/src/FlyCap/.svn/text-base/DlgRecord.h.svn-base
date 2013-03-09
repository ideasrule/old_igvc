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
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: DlgRecord.h,v 1.7 2007/02/12 18:03:35 soowei Exp $
//=============================================================================
#if !defined(AFX_DLGRECORD_H__C7BE3E14_2AD8_4E85_A9EC_E0DDC51F61E8__INCLUDED_)
#define AFX_DLGRECORD_H__C7BE3E14_2AD8_4E85_A9EC_E0DDC51F61E8__INCLUDED_

//=============================================================================
// System Includes
//=============================================================================

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================

#include "CompressorSelectionDlg.h"
#include "PGRAviFile.h"

#pragma once


class CDlgRecord : public CDialog
{
public:
   CDlgRecord( CWnd* pParent = NULL ); 
   
   void setAVIMember( PGRAviFile* pAVI );
   
   enum SaveFormat
   {
      FRAMES,
      TIME,
      STREAM
   };

   SaveFormat m_saveFormat;

   enum SaveOutput
   {
      AVI,
      BMP,
      JPG,
      PPM,
      PGM,
      RAW
   };

   SaveOutput m_saveOutput;

   CCompressorSelectionDlg m_compressorDlg;
   PGRAviFile* m_pAVI;


   //{{AFX_DATA(CDlgRecord)
   enum { IDD = IDD_DIALOG_RECORD };
   CEdit	m_editCompressionQuality;
   CEdit	m_editSelectedCompressor;
   int		m_iFrames;
   int		m_iLengthTime;
   CString	m_csPath;
   double	m_dFramerate;
   CButton      m_buttonTime;
   CButton      m_buttonFrames;
   CButton      m_buttonStream;
   CEdit        m_editFrames;
   CEdit        m_editLengthTime;
   CButton      m_buttonVideo;
   CButton      m_buttonImages;
   CButton      m_buttonImageBMP;
   CButton      m_buttonImagePPM;
   CButton      m_buttonImagePGM;
   CButton      m_buttonImageRAW;
   CButton      m_buttonImageJPG;
   int		m_iCompressionQuality;
   //}}AFX_DATA
   
   //{{AFX_VIRTUAL(CDlgRecord)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);   
   //}}AFX_VIRTUAL
   
protected:
   
   //{{AFX_MSG(CDlgRecord)
   virtual BOOL OnInitDialog();
   afx_msg void OnRadioTime();
   afx_msg void OnRadioFrames();
   afx_msg void OnRadioStream();
   afx_msg void OnRadioImages();
   afx_msg void OnRadioVideo();
   afx_msg void OnRadioImageBmp();
   afx_msg void OnRadioImagePpm();
   afx_msg void OnRadioImagePgm();
   afx_msg void OnRadioImageRaw();
   afx_msg void OnButtonSelectCompressor();
   afx_msg void OnRadioImageJpg();
	afx_msg void OnKillfocusEditJpgCompressionQuality();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGRECORD_H__C7BE3E14_2AD8_4E85_A9EC_E0DDC51F61E8__INCLUDED_)
