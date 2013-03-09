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
// $Id: PGRStereoControlDialog.h,v 1.3 2007/09/26 00:53:42 mgriffin Exp $
//=============================================================================
#ifndef __PGRSTEREOCONTROLDIALOG_H__
#define __PGRSTEREOCONTROLDIALOG_H__

#pragma once

//=============================================================================
// System Includes
//=============================================================================
#include <afxcmn.h>
#include <afxwin.h>

//=============================================================================
// PGR Includes
//=============================================================================
#include <PGRFlycapture.h>
#include <triclops.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRResource.h"
#include "PGRToolTip.h"


/**
 * This dialog controls the triclops library stereo parameters by manipulating
 * a triclopscontext given to it by surrounding objects.
 */
class CPGRStereoControlDialog : public CDialog
{

public:
   DECLARE_DYNCREATE( CPGRStereoControlDialog )
   CPGRStereoControlDialog( CWnd* pParent = NULL ); 
   
   /** Dialog value. */
   int   m_nStereoMaskSize;        
   /** Dialog value. */
   int   m_nEdgeMaskSize;          
   /** Dialog value. */
   int   m_nMaxDisparity;          
   /** Dialog value. */
   int   m_nMinDisparity;          
   /** Dialog value. */
   int   m_nSurfaceValidationSize; 
   /** Dialog value. */
   float m_fPointSize;
   /** Dialog value. */
   float m_fSurfaceValidationDiff; 
   /** Dialog value. */
   float m_fUniquenessValidation;  
   /** Dialog value. */
   float m_fTextureValidation;  
   /** Dialog value */
   bool  m_bPreprocess;
   
   /** used to determine if current camera is an XB3 or not **/
   bool m_bIsXB3;
   
   /** handle to the dialog. */
   HWND	 m_hDialogBox;    
   

   BOOL Create();
   
   /** This function allows the user to set the context for the control. */
   TriclopsError setContext( TriclopsContext* pTriclopsContext, int iHeight, int iWidth );

   /** This function allows the user to set the flycapture context for register control. */
   TriclopsError setFlycaptureContext( FlyCaptureContext* pFlycaptureContext );
   
   //{{AFX_DATA(CPGRStereoControlDialog)
   enum { IDD = PGRRES_STEREO_CONTROL_DIALOG };
   CButton	m_checkHeatmap;
   CButton	m_checkInvalidColors;
   CButton	m_buttonAlg3Cam;
   CButton	m_buttonAlg2CamV;
   CButton	m_buttonAlg2CamH;
   CSliderCtrl	m_sliderSurfaceValidationSize;
   CSliderCtrl	m_sliderSurfaceValidationDiff;
   CSliderCtrl	m_sliderUniqueness;
   CSliderCtrl	m_sliderPointSize;
   CSliderCtrl	m_sliderTexture;
   CSliderCtrl	m_sliderStereo;
   CSliderCtrl	m_sliderEdge;
   CSliderCtrl	m_sliderDisparity;
   CSliderCtrl	m_sliderMinDisparity;
   CButton	m_checkSmooth;
   CButton	m_checkEdge;
   CButton	m_checkStereoButton;
   CButton	m_checkSubpixel;
   CButton	m_checkUniqueness;
   CButton	m_checkTexture;
   CButton	m_checkBackForth;
   CButton	m_checkSurfaceValidation;
   CComboBox	m_comboBoxRectification;
   CButton	m_checkStereoAlgorithm;
   BOOL		m_flagEdge;
   BOOL		m_flagSmooth;
   BOOL	        m_flagSubpixel;
   BOOL	        m_flagTexture;
   BOOL	        m_flagBackForth;
   BOOL		m_flagUniqueness;
   BOOL	        m_flagInput;
   BOOL	        m_flagSurfaceValidation;
   BOOL		m_flagStereoAlgorithm;
   BOOL	        m_bGrab;
   BOOL	        m_bPoints;
   BOOL		m_bStereo;
   BOOL		m_bDrawColorPoints;
   BOOL         m_bDrawPoints;
   BOOL         m_bDrawSurfaces;
   BOOL         m_bDrawTextureMap;
   BOOL	        m_bHeatmap;
   BOOL	m_bInvalidColors;
   //}}AFX_DATA
   
   
   //{{AFX_VIRTUAL(CPGRStereoControlDialog)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);   
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

   virtual void OnOK();
   virtual void OnCancel();

protected:

   /** The triclops context that this dialog is manipulating the settings of. */
   TriclopsContext*  m_pTriclopsContext;

   /** The flycapture context used to change camera register settings */
   FlyCaptureContext*  m_pFlycaptureContext;

   /** A FlyCaptureInfoEx structure describing the camera that we're using. */
   FlyCaptureInfoEx m_flycaptureInfo;

   /** The current size of the stereo image */
   int m_iHeight;
   int m_iWidth;

   //{{AFX_MSG(CPGRStereoControlDialog)
   virtual BOOL OnInitDialog();
   afx_msg void OnCHECKStereo();
   afx_msg void OnCHECKEdge();
   afx_msg void OnCHECKTexture();
   afx_msg void OnCHECKBackForth();
   afx_msg void OnCHECKUniqueness();
   afx_msg void OnCHECKSurfaceValidation();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnCHECKSubpixel();
   afx_msg void OnCHECKSmooth();
   afx_msg void OnSELCHANGEIdcComboRectification() ;
   afx_msg void OnCHECKStereoAlgorithm();
   afx_msg void OnRadioStereoalg2camhorz();
   afx_msg void OnRadioStereoalg2camvert();
   afx_msg void OnRadioStereoalg3cam();
   afx_msg void OnCheckDisplayHeatmap();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      
   CPGRToolTipCtrl   m_tipCtrl;

   int getMaxDisparityAllowed( bool bSaveAndValidate );
   void updateDisparitySliders( bool bSaveAndValidate );
   void setRectificationQualityFromTriclops();
   void setRectificationQualityToTriclops();

};

//{{AFX_INSERT_LOCATION}} 


#endif // #ifndef __PGRSTEREOCONTROLDIALOG_H__
