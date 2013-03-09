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
// $Id: PGRStereoDialogBar.h,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================
#ifndef __PGRSTEREODIALOGBAR_H__
#define __PGRSTEREODIALOGBAR_H__

#pragma once

//=============================================================================
// PGR Includes
//=============================================================================
#include "PGRFlyCaptureStereo.h"

//=============================================================================
// Project Includes
//=============================================================================
#include "pgrresource.h"


/**
 * This class defines the dialog bar that is seen at the top of PGRMFC
 * applications.
 */
class CPGRStereoDialogBar : public CDialogBar
{
public:
   CPGRStereoDialogBar();
   
   //{{AFX_DATA(CPGRStereoDialogBar)
   enum { IDD = PGRRES_STEREO_DIALOGBAR };
   CButton	m_buttonCameraControl;
   CButton	m_buttonTransformation;
   CButton	m_buttonStereoParams;
   CComboBox	m_comboBoxResolution;
   CComboBox	m_comboBoxImage;
   CComboBox	m_comboBoxCamera;
   int		m_idxCamera;
   int		m_idxImage;
   int		m_idxResolution;
   //}}AFX_DATA


   int getImageType();

   void	setbyImageType( int nImageType );

   void getResolution( int* iCols, int* iRows );

   void getResolution( StereoImageResolution resType, int* iRows, int* iCols );

   StereoImageResolution getResolution();

   void	setResolution( int iCols, int iRows );

   void setResolution( StereoImageResolution resolution );

   void setComboBoxSelections(BOOL bColor, 
      StereoImageResolution maxResolution,
      BOOL bBumblebee );

   //{{AFX_VIRTUAL(CPGRStereoDialogBar)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL
   
protected:
   //{{AFX_MSG(CPGRStereoDialogBar)
	//}}AFX_MSG

   /** @note copied from knowledgebase article Q185672. */
   afx_msg LONG OnInitDialog( UINT wParam, LONG lParam );

   DECLARE_MESSAGE_MAP()

};


//{{AFX_INSERT_LOCATION}}


#endif // ifndef __PGRSTEREODIALOGBAR_H__
