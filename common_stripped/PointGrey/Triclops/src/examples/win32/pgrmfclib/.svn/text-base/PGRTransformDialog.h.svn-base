//=============================================================================
// Copyright © 2000 Point Grey Research, Inc. All Rights Reserved.
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
// $Id: PGRTransformDialog.h,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================
#ifndef __PGRTRANSFORMDIALOG_H__
#define __PGRTRANSFORMDIALOG_H__

#pragma once

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "Transform.h"


/**
 * A dialog that manipulates the transformation matrix for the point cloud.
 */
class CPGRTransformDialog : public CDialog
{
public:

   //PGRTransform3d m_transform;
   CTransformD m_transform;
   
   CPGRTransformDialog( CWnd* pParent = NULL );   
   BOOL Create();
   
   //{{AFX_DATA(CPGRTransformDialog)
   enum { IDD = PGRRES_TRANSFORMDIALOG };
   CSliderCtrl	m_sliderZ;
   CSliderCtrl	m_sliderY;
   CSliderCtrl	m_sliderYaw;
   CSliderCtrl	m_sliderX;
   CSliderCtrl	m_sliderRoll;
   CSliderCtrl	m_sliderPitch;
   double	m_dA0;
   double	m_dA1;
   double	m_dA2;
   double	m_dA3;
   double	m_dB0;
   double	m_dB1;
   double	m_dB2;
   double	m_dB3;
   double	m_dC0;
   double	m_dC1;
   double	m_dC2;
   double	m_dC3;
   double	m_dD0;
   double	m_dD1;
   double	m_dD2;
   double	m_dD3;
   double	m_dPitch;
   double	m_dRoll;
   double	m_dX;
   double	m_dYaw;
   double	m_dY;
   double	m_dZ;
   int		m_nRollSlider;
   int		m_nXSlider;
   int		m_nYawSlider;
   int		m_nYSlider;
   int		m_nZSlider;
   int		m_nPitchSlider;
   //}}AFX_DATA
   
   
   //{{AFX_VIRTUAL(CPGRTransformDialog)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);   
   //}}AFX_VIRTUAL
   virtual void OnOK();
   virtual void OnCancel();
   
   
protected:
   
   //{{AFX_MSG(CPGRTransformDialog)
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnKillfocusMatrixElement();
   afx_msg void OnKillfocusComponent();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      
   void updateTransform();
};

//{{AFX_INSERT_LOCATION}}

#endif // ifndef __PGRTRANSFORMDIALOG_H__
