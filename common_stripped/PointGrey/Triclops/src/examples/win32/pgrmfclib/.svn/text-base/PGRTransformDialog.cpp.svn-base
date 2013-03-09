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
// $Id: PGRTransformDialog.cpp,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <afxwin.h>
#include <afxcmn.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "pgrresource.h"
#include "PGRTransformDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//=============================================================================
// Macro Definitions
//=============================================================================

#define MAX_DIST           ( 50.0 )
#define SLIDER_MIN         0
#define SLIDER_MAX         500
#define SLIDER_VALUE       250
#define SLIDER_TO_VALUE(A) ( ( (A) - SLIDER_VALUE ) / ( SLIDER_VALUE / MAX_DIST ) )
#define VALUE_TO_SLIDER(A) ( (int)( ( (A) * ( SLIDER_VALUE / MAX_DIST ) ) + SLIDER_VALUE ) )




CPGRTransformDialog::CPGRTransformDialog(CWnd* pParent /*=NULL*/)
   : CDialog(CPGRTransformDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CPGRTransformDialog)
   m_dA0    = 1.0;
   m_dA1    = 0.0;
   m_dA2    = 0.0;
   m_dA3    = 0.0;
   m_dB0    = 0.0;
   m_dB1    = 1.0;
   m_dB2    = 0.0;
   m_dB3    = 0.0;
   m_dC0    = 0.0;
   m_dC1    = 0.0;
   m_dC2    = 1.0;
   m_dC3    = 0.0;
   m_dD0    = 0.0;
   m_dD1    = 0.0;
   m_dD2    = 0.0;
   m_dD3    = 1.0;
   m_dPitch = 0.0;
   m_dRoll  = 0.0;
   m_dX	    = 0.0;
   m_dYaw   = 0.0;
   m_dY	    = 0.0;
   m_dZ	    = 0.0;

   m_nRollSlider  = 0;
   m_nXSlider	  = 0;
   m_nYawSlider	  = 0;
   m_nYSlider	  = 0;
   m_nZSlider	  = 0;
   m_nPitchSlider = 0;
   //}}AFX_DATA_INIT
}


void 
CPGRTransformDialog::DoDataExchange( CDataExchange* pDX )
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPGRTransformDialog)
   DDX_Control(pDX, IDC_ZSLIDER, m_sliderZ);
   DDX_Control(pDX, IDC_YSLIDER, m_sliderY);
   DDX_Control(pDX, IDC_YAWSLIDER, m_sliderYaw);
   DDX_Control(pDX, IDC_XSLIDER, m_sliderX);
   DDX_Control(pDX, IDC_ROLLSLIDER, m_sliderRoll);
   DDX_Control(pDX, IDC_PITCHSLIDER, m_sliderPitch);
   DDX_Text(pDX, IDC_A0EDIT, m_dA0);
   DDV_MinMaxDouble(pDX, m_dA0, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_A1EDIT, m_dA1);
   DDV_MinMaxDouble(pDX, m_dA1, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_A2EDIT, m_dA2);
   DDV_MinMaxDouble(pDX, m_dA2, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_A3EDIT, m_dA3);
   DDV_MinMaxDouble(pDX, m_dA3, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_B0EDIT, m_dB0);
   DDV_MinMaxDouble(pDX, m_dB0, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_B1EDIT, m_dB1);
   DDV_MinMaxDouble(pDX, m_dB1, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_B2EDIT, m_dB2);
   DDV_MinMaxDouble(pDX, m_dB2, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_BEDIT3, m_dB3);
   DDV_MinMaxDouble(pDX, m_dB3, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_C0EDIT, m_dC0);
   DDV_MinMaxDouble(pDX, m_dC0, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_C1EDIT, m_dC1);
   DDV_MinMaxDouble(pDX, m_dC1, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_C2EDIT, m_dC2);
   DDV_MinMaxDouble(pDX, m_dC2, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_C3EDIT, m_dC3);
   DDV_MinMaxDouble(pDX, m_dC3, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_D0EDIT, m_dD0);
   DDV_MinMaxDouble(pDX, m_dD0, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_D1EDIT, m_dD1);
   DDV_MinMaxDouble(pDX, m_dD1, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_D2EDIT, m_dD2);
   DDV_MinMaxDouble(pDX, m_dD2, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_D3EDIT, m_dD3);
   DDV_MinMaxDouble(pDX, m_dD3, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_PITCHEDIT, m_dPitch);
   DDV_MinMaxDouble(pDX, m_dPitch, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_ROLLEDIT, m_dRoll);
   DDV_MinMaxDouble(pDX, m_dRoll, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_XEDIT, m_dX);
   DDV_MinMaxDouble(pDX, m_dX, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_YAWEDIT, m_dYaw);
   DDV_MinMaxDouble(pDX, m_dYaw, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_YEDIT, m_dY);
   DDV_MinMaxDouble(pDX, m_dY, -MAX_DIST, MAX_DIST);
   DDX_Text(pDX, IDC_ZEDIT, m_dZ);
   DDV_MinMaxDouble(pDX, m_dZ, -MAX_DIST, MAX_DIST);
   DDX_Slider(pDX, IDC_ROLLSLIDER, m_nRollSlider);
   DDX_Slider(pDX, IDC_XSLIDER, m_nXSlider);
   DDX_Slider(pDX, IDC_YAWSLIDER, m_nYawSlider);
   DDX_Slider(pDX, IDC_YSLIDER, m_nYSlider);
   DDX_Slider(pDX, IDC_ZSLIDER, m_nZSlider);
   DDX_Slider(pDX, IDC_PITCHSLIDER, m_nPitchSlider);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPGRTransformDialog, CDialog)
//{{AFX_MSG_MAP(CPGRTransformDialog)
ON_WM_HSCROLL()
ON_EN_KILLFOCUS(IDC_A0EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_PITCHEDIT, OnKillfocusComponent)
ON_EN_KILLFOCUS(IDC_A1EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_A2EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_A3EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_B0EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_B1EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_B2EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_BEDIT3, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_C0EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_C1EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_C2EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_C3EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_D0EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_D1EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_D2EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_D3EDIT, OnKillfocusMatrixElement)
ON_EN_KILLFOCUS(IDC_ROLLEDIT, OnKillfocusComponent)
ON_EN_KILLFOCUS(IDC_XEDIT, OnKillfocusComponent)
ON_EN_KILLFOCUS(IDC_YAWEDIT, OnKillfocusComponent)
ON_EN_KILLFOCUS(IDC_YEDIT, OnKillfocusComponent)
ON_EN_KILLFOCUS(IDC_ZEDIT, OnKillfocusComponent)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL 
CPGRTransformDialog::Create()
{
   return CDialog::Create( CPGRTransformDialog::IDD );
}


void 
CPGRTransformDialog::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
   UpdateData(TRUE);
   
   m_dX     = SLIDER_TO_VALUE( m_nXSlider );
   m_dY     = SLIDER_TO_VALUE( m_nYSlider );
   m_dZ     = SLIDER_TO_VALUE( m_nZSlider );
   m_dRoll  = SLIDER_TO_VALUE( m_nRollSlider );
   m_dPitch = SLIDER_TO_VALUE( m_nPitchSlider );
   m_dYaw   = SLIDER_TO_VALUE( m_nYawSlider );
   
   updateTransform();
   
   UpdateData(FALSE);
   
   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void 
CPGRTransformDialog::OnKillfocusMatrixElement() 
{
   UpdateData( TRUE );

   double (*pData)[4] = (double(*)[4])m_transform.getData();
   pData[0][0] = m_dA0;
   pData[0][1] = m_dA1;
   pData[0][2] = m_dA2;
   pData[0][3] = m_dA3;
   pData[1][0] = m_dB0;
   pData[1][1] = m_dB1;
   pData[1][2] = m_dB2;
   pData[1][3] = m_dB3;
   pData[2][0] = m_dC0;
   pData[2][1] = m_dC1;
   pData[2][2] = m_dC2;
   pData[2][3] = m_dC3;
   pData[3][0] = m_dD0;
   pData[3][1] = m_dD1;
   pData[3][2] = m_dD2;
   pData[3][3] = m_dD3;
   
   m_transform.unsetMatrix();
   m_transform.setMatrix();
   
   
   m_dX     = m_transform.tX();
   m_dY     = m_transform.tY();
   m_dZ     = m_transform.tZ();
   m_dRoll  = m_transform.rX();
   m_dPitch = m_transform.rY();
   m_dYaw   = m_transform.rZ();
   
   m_nXSlider     = VALUE_TO_SLIDER( m_dX );
   m_nYSlider     = VALUE_TO_SLIDER( m_dY );
   m_nZSlider     = VALUE_TO_SLIDER( m_dZ );
   m_nRollSlider  = VALUE_TO_SLIDER( m_dRoll );
   m_nPitchSlider = VALUE_TO_SLIDER( m_dPitch );
   m_nYawSlider   = VALUE_TO_SLIDER( m_dYaw );
   
   UpdateData( FALSE );
}


void 
CPGRTransformDialog::OnKillfocusComponent() 
{
   UpdateData( TRUE );
   
   m_nXSlider     = VALUE_TO_SLIDER( m_dX );
   m_nYSlider     = VALUE_TO_SLIDER( m_dY );
   m_nZSlider     = VALUE_TO_SLIDER( m_dZ );
   m_nRollSlider  = VALUE_TO_SLIDER( m_dRoll );
   m_nPitchSlider = VALUE_TO_SLIDER( m_dPitch );
   m_nYawSlider   = VALUE_TO_SLIDER( m_dYaw );
   
   updateTransform();
   
   UpdateData( FALSE );
}


BOOL 
CPGRTransformDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_sliderX.SetRange( SLIDER_MIN, SLIDER_MAX );
   m_sliderY.SetRange( SLIDER_MIN, SLIDER_MAX );
   m_sliderZ.SetRange( SLIDER_MIN, SLIDER_MAX );
   m_sliderRoll.SetRange( SLIDER_MIN, SLIDER_MAX );
   m_sliderPitch.SetRange( SLIDER_MIN, SLIDER_MAX );
   m_sliderYaw.SetRange( SLIDER_MIN, SLIDER_MAX );

   m_sliderX.SetPos( VALUE_TO_SLIDER( m_dX ) );
   m_sliderY.SetPos( VALUE_TO_SLIDER( m_dY ) );
   m_sliderZ.SetPos( VALUE_TO_SLIDER( m_dZ ) );
   m_sliderRoll.SetPos( VALUE_TO_SLIDER( m_dRoll ) );
   m_sliderPitch.SetPos( VALUE_TO_SLIDER( m_dPitch ) );
   m_sliderYaw.SetPos( VALUE_TO_SLIDER( m_dYaw ) );

   m_nXSlider     = VALUE_TO_SLIDER( m_dX );
   m_nYSlider     = VALUE_TO_SLIDER( m_dY );
   m_nZSlider     = VALUE_TO_SLIDER( m_dZ );
   m_nRollSlider  = VALUE_TO_SLIDER( m_dRoll );
   m_nPitchSlider = VALUE_TO_SLIDER( m_dPitch );
   m_nYawSlider   = VALUE_TO_SLIDER( m_dYaw );

   updateTransform();

   UpdateData( FALSE );
   
   return TRUE;  // return TRUE unless you set the focus to a control
}


void 
CPGRTransformDialog::updateTransform()
{
   m_transform.setEulerZYX( m_dRoll, m_dPitch, m_dYaw, m_dX, m_dY, m_dZ );
   
   double (*pData)[4] = (double(*)[4])m_transform.getData();
   
   m_dA0 = pData[0][0];
   m_dA1 = pData[0][1];
   m_dA2 = pData[0][2];
   m_dA3 = pData[0][3];
   m_dB0 = pData[1][0];
   m_dB1 = pData[1][1];
   m_dB2 = pData[1][2];
   m_dB3 = pData[1][3];
   m_dC0 = pData[2][0];
   m_dC1 = pData[2][1];
   m_dC2 = pData[2][2];
   m_dC3 = pData[2][3];
   m_dD0 = pData[3][0];
   m_dD1 = pData[3][1];
   m_dD2 = pData[3][2];
   m_dD3 = pData[3][3];
}


void 
CPGRTransformDialog::OnCancel()
{
   // do nothing - disable cancel.
}


void 
CPGRTransformDialog::OnOK()
{
   //
   // Assume the user is modifying the one of the X, Y, Z, Roll, Pitch, Yaw
   // values and update the sliders, and the transform accordingly.
   //
   UpdateData( TRUE );
   
   m_nXSlider     = VALUE_TO_SLIDER( m_dX );
   m_nYSlider     = VALUE_TO_SLIDER( m_dY );
   m_nZSlider     = VALUE_TO_SLIDER( m_dZ );
   m_nRollSlider  = VALUE_TO_SLIDER( m_dRoll );
   m_nPitchSlider = VALUE_TO_SLIDER( m_dPitch );
   m_nYawSlider   = VALUE_TO_SLIDER( m_dYaw );
   
   updateTransform();
   
   UpdateData( FALSE );
}


