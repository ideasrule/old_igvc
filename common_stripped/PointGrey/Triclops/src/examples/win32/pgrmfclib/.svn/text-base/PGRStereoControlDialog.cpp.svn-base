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
// $Id: PGRStereoControlDialog.cpp,v 1.5 2007/10/24 01:29:43 mgriffin Exp $
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRStereoControlDialog.h"


IMPLEMENT_DYNCREATE( CPGRStereoControlDialog, CDialog )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define _VERIFY_TRICLOPS_ERROR \
{ \
   if( error != TriclopsErrorOk ) \
   { \
      CString csMsg; \
      \
      csMsg.Format( \
	 "Error: Triclops library reported: \"%s\"",  \
	 ::triclopsErrorToString( error ) ); \
      \
      ::AfxMessageBox( csMsg ); \
   } \
} \
\


CPGRStereoControlDialog::CPGRStereoControlDialog( CWnd* pParent /*=NULL*/)
   : CDialog(CPGRStereoControlDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CPGRStereoControlDialog)
   m_flagEdge              = FALSE;
   m_flagSmooth            = FALSE;
   m_flagSubpixel          = FALSE;
   m_flagTexture           = FALSE;
   m_flagBackForth         = FALSE;
   m_flagUniqueness        = FALSE;
   m_flagSurfaceValidation = FALSE;
   m_flagStereoAlgorithm   = FALSE;
   m_bGrab		   = TRUE;
   m_bPoints		   = FALSE;
   m_bStereo		   = TRUE;
   m_bDrawColorPoints	   = TRUE;
   m_bDrawPoints	   = TRUE;
   m_bDrawSurfaces	   = FALSE;
   m_bDrawTextureMap	   = FALSE;
   m_bHeatmap		   = TRUE;
   m_bInvalidColors = FALSE;
   //}}AFX_DATA_INIT
   
   m_bPreprocess = TRUE;

   m_nStereoMaskSize        = 0;
   m_nEdgeMaskSize          = 0;
   m_nMaxDisparity          = 0;
   m_nMinDisparity          = 0;
   m_nSurfaceValidationSize = 0;
   m_fPointSize             = 1.0f;
   m_fSurfaceValidationDiff = 0;
   m_fTextureValidation     = 0;
   m_fUniquenessValidation  = 0;

   m_pTriclopsContext = NULL;
   m_pFlycaptureContext = NULL;
   m_iHeight = -1;
   m_iWidth = -1;
}


void 
CPGRStereoControlDialog::DoDataExchange( CDataExchange* pDX )
{
   int	    sliderValue;
   char	    pszValueText[ 10 ];
   
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPGRStereoControlDialog)
   DDX_Control(pDX, PGRRES_CHECK_DISPLAYHEATMAP, m_checkHeatmap);
   DDX_Control(pDX, PGRRES_CHECK_DISPLAYVALIDATIONCOLORS, m_checkInvalidColors);
   DDX_Control(pDX, PGRRES_RADIO_STEREOALG_3CAM, m_buttonAlg3Cam);
   DDX_Control(pDX, PGRRES_RADIO_STEREOALG_2CAMVERT, m_buttonAlg2CamV);
   DDX_Control(pDX, PGRRES_RADIO_STEREOALG_2CAMHORZ, m_buttonAlg2CamH);
   DDX_Control(pDX, PGRRES_SLIDER_SURFACEVALIDATIONSIZE, m_sliderSurfaceValidationSize);
   DDX_Control(pDX, PGRRES_SLIDER_SURFACEVALIDATIONDIFF, m_sliderSurfaceValidationDiff);
   DDX_Control(pDX, PGRRES_SLIDER_UNIQUENESS, m_sliderUniqueness);
   DDX_Control(pDX, PGRRES_SLIDER_POINT_SIZE, m_sliderPointSize);
   DDX_Control(pDX, PGRRES_SLIDER_TEXTURE, m_sliderTexture);
   DDX_Control(pDX, PGRRES_SLIDER_STEREO, m_sliderStereo);
   DDX_Control(pDX, PGRRES_SLIDER_EDGE, m_sliderEdge);
   DDX_Control(pDX, PGRRES_SLIDER_DISPARITY, m_sliderDisparity);
   DDX_Control(pDX, PGRRES_SLIDER_MINDISPARITY, m_sliderMinDisparity);
   DDX_Control(pDX, PGRRES_CHECK_SMOOTH, m_checkSmooth);
   DDX_Control(pDX, PGRRES_CHECK_EDGE, m_checkEdge);
   DDX_Control(pDX, PGRRES_CHECK_STEREO, m_checkStereoButton);
   DDX_Control(pDX, PGRRES_CHECK_SUBPIXEL, m_checkSubpixel);
   DDX_Control(pDX, PGRRES_CHECK_UNIQUENESS, m_checkUniqueness);
   DDX_Control(pDX, PGRRES_CHECK_TEXTURE, m_checkTexture);
   DDX_Control(pDX, PGRRES_CHECK_BACKFORTH, m_checkBackForth);
   DDX_Control(pDX, PGRRES_CHECK_SURFACEVALIDATION, m_checkSurfaceValidation);
   DDX_Control(pDX, PGRRES_IDC_COMBO_RECTIFICATION, m_comboBoxRectification);
   DDX_Control(pDX, PGRRES_CHECK_STEREO_ALGORITHM, m_checkStereoAlgorithm);
   DDX_Check(pDX, PGRRES_CHECK_EDGE, m_flagEdge);
   DDX_Check(pDX, PGRRES_CHECK_SMOOTH, m_flagSmooth);
   DDX_Check(pDX, PGRRES_CHECK_SUBPIXEL, m_flagSubpixel);
   DDX_Check(pDX, PGRRES_CHECK_TEXTURE, m_flagTexture);
   DDX_Check(pDX, PGRRES_CHECK_BACKFORTH, m_flagBackForth);
   DDX_Check(pDX, PGRRES_CHECK_UNIQUENESS, m_flagUniqueness);
   DDX_Check(pDX, PGRRES_CHECK_SURFACEVALIDATION, m_flagSurfaceValidation);
   DDX_Check(pDX, PGRRES_CHECK_STEREO_ALGORITHM, m_flagStereoAlgorithm);
   DDX_Check(pDX, PGRRES_CHECK_GRAB, m_bGrab);
   DDX_Check(pDX, PGRRES_CHECK_POINTS, m_bPoints);
   DDX_Check(pDX, PGRRES_CHECK_STEREO, m_bStereo);
   DDX_Check(pDX, PGRRES_CHECK_DRAWCOLOURPOINTS, m_bDrawColorPoints);
   DDX_Check(pDX, PGRRES_CHECK_DRAWPOINTS, m_bDrawPoints);
   DDX_Check(pDX, PGRRES_CHECK_DRAWSURFACES, m_bDrawSurfaces);
   DDX_Check(pDX, PGRRES_CHECK_DRAWTEXTUREMAP, m_bDrawTextureMap);
   DDX_Check(pDX, PGRRES_CHECK_DISPLAYHEATMAP, m_bHeatmap);
   DDX_Check(pDX, PGRRES_CHECK_DISPLAYVALIDATIONCOLORS, m_bInvalidColors);
   //}}AFX_DATA_MAP
   
   
   if( m_pTriclopsContext == NULL )
   {
      ASSERT( FALSE );
      return;
   }
   
   if( pDX->m_bSaveAndValidate )
   {
      m_nStereoMaskSize        = m_sliderStereo.GetPos() * 2 + 1;    // make it odd;
      m_nEdgeMaskSize          = m_sliderEdge.GetPos() * 2 + 1;      // make it odd;
      m_nMaxDisparity          = m_sliderDisparity.GetPos(); 
      m_nMinDisparity          = m_sliderMinDisparity.GetPos(); 
      m_nSurfaceValidationSize = m_sliderSurfaceValidationSize.GetPos();
      m_fPointSize             = m_sliderPointSize.GetPos() / 100.0f;
      m_fSurfaceValidationDiff = (float)( m_sliderSurfaceValidationDiff.GetPos() / 100.0 );
      m_fTextureValidation     = m_sliderTexture.GetPos() / 100.0f;   // unit 1/100
      m_fUniquenessValidation  = m_sliderUniqueness.GetPos() / 100.0f;  // unit 1/100
      m_bStereo		       = (m_checkStereoButton.GetCheck() == 1) ? true : false;


      ::triclopsSetDoStereo( *m_pTriclopsContext, m_checkStereoButton.GetCheck() );
      ::triclopsSetEdgeMask( *m_pTriclopsContext, m_nEdgeMaskSize );
      ::triclopsSetDisparity( *m_pTriclopsContext, m_nMinDisparity, m_nMaxDisparity );   
      ::triclopsSetStereoMask( *m_pTriclopsContext, m_nStereoMaskSize );   
      ::triclopsSetTextureValidationThreshold( *m_pTriclopsContext, m_fTextureValidation );   
      ::triclopsSetUniquenessValidationThreshold( *m_pTriclopsContext, m_fUniquenessValidation );   
      ::triclopsSetSurfaceValidationSize( *m_pTriclopsContext, m_nSurfaceValidationSize );   
      ::triclopsSetSurfaceValidationDifference( *m_pTriclopsContext, m_fSurfaceValidationDiff );
   }
   else
   {
      ::triclopsGetStereoMask( *m_pTriclopsContext, &m_nStereoMaskSize );
      ::triclopsGetEdgeMask( *m_pTriclopsContext, &m_nEdgeMaskSize );      
      ::triclopsGetDisparity( *m_pTriclopsContext, &m_nMinDisparity, &m_nMaxDisparity );      
      ::triclopsGetSurfaceValidationSize( *m_pTriclopsContext, &m_nSurfaceValidationSize );      
      ::triclopsGetSurfaceValidationDifference( *m_pTriclopsContext, &m_fSurfaceValidationDiff );      
      ::triclopsGetTextureValidationThreshold( *m_pTriclopsContext, &m_fTextureValidation );      
      ::triclopsGetUniquenessValidationThreshold( *m_pTriclopsContext, &m_fUniquenessValidation );

      // Do this first -- it may update the slider ranges, so that they can be set 
      // in the code that follows.
      updateDisparitySliders(pDX->m_bSaveAndValidate==0);

      m_sliderStereo.SetPos( ( m_nStereoMaskSize - 1 ) / 2 );
      sliderValue = m_sliderStereo.GetPos();
      sprintf( pszValueText, "%d",  sliderValue * 2 + 1  );   
      GetDlgItem( PGRRES_STATIC_STEREO )->SetWindowText( pszValueText );
      
      m_sliderEdge.SetPos( ( m_nEdgeMaskSize - 1 ) / 2 );
      sliderValue = m_sliderEdge.GetPos();
      sprintf( pszValueText, "%d",  sliderValue * 2 + 1  );   
      GetDlgItem( PGRRES_STATIC_EDGE )->SetWindowText( pszValueText );
      
      m_sliderDisparity.SetPos( m_nMaxDisparity );
      sliderValue = m_sliderDisparity.GetPos();
      sprintf( pszValueText, "%d",  sliderValue );   
      GetDlgItem( PGRRES_STATIC_DISPARITY)->SetWindowText( pszValueText );
      
      m_sliderMinDisparity.SetPos( m_nMinDisparity );
      sliderValue = m_sliderMinDisparity.GetPos();
      sprintf( pszValueText, "%d",  sliderValue );   
      GetDlgItem( PGRRES_STATIC_MINDISPARITY )->SetWindowText( pszValueText );
      
      m_sliderSurfaceValidationSize.SetPos( m_nSurfaceValidationSize );
      sliderValue = m_sliderSurfaceValidationSize.GetPos();
      sprintf( pszValueText, "%d",  sliderValue );   
      GetDlgItem( PGRRES_STATIC_SURFACEVALIDATIONSIZE )->SetWindowText( pszValueText );

      m_sliderPointSize.SetPos( (int)(m_fPointSize * 100.0) );
      sliderValue = m_sliderPointSize.GetPos();
      sprintf( pszValueText, "%d%%", sliderValue );   
      GetDlgItem( PGRRES_STATIC_POINT_SIZE )->SetWindowText( pszValueText );
      
      m_sliderSurfaceValidationDiff.SetPos( (int)(m_fSurfaceValidationDiff * 100.0) );
      sliderValue = m_sliderSurfaceValidationDiff.GetPos();
      sprintf( pszValueText, "%3.2f",  sliderValue/100.0  );   
      GetDlgItem( PGRRES_STATIC_SURFACEVALIDATIONDIFF )->SetWindowText( pszValueText );
      
      m_sliderTexture.SetPos( (int)( m_fTextureValidation * 100.0 ) );
      sliderValue = m_sliderTexture.GetPos();
      sprintf( pszValueText, "%5.2f", sliderValue / 100.0  );   
      GetDlgItem( PGRRES_STATIC_TEXTURE )->SetWindowText( pszValueText );
      
      m_sliderUniqueness.SetPos( (int)( m_fUniquenessValidation * 100.00 ) );
      sliderValue = m_sliderUniqueness.GetPos();
      sprintf( pszValueText, "%5.2f", sliderValue/100.0);   
      GetDlgItem( PGRRES_STATIC_UNIQUENESS )->SetWindowText( pszValueText );


      // Initialize check boxes:
      TriclopsBool bOn;

      // Smooth:
      ::triclopsGetLowpass( *m_pTriclopsContext, &bOn );
      m_checkSmooth.SetCheck( bOn ? 1 : 0 );
      
      // Edge:
      ::triclopsGetEdgeCorrelation( *m_pTriclopsContext, &bOn );
      m_checkEdge.SetCheck( bOn ? 1 : 0 );
      
      // Stereo:
      ::triclopsGetDoStereo( *m_pTriclopsContext, &bOn );
      m_checkStereoButton.SetCheck( bOn ? 1 : 0 );

      // Texture:
      ::triclopsGetTextureValidation( *m_pTriclopsContext, &bOn );
      m_checkTexture.SetCheck( bOn ? 1 : 0 );

      // Back-forth:
      ::triclopsGetBackForthValidation( *m_pTriclopsContext, &bOn );
      m_checkBackForth.SetCheck( bOn ? 1 : 0 );
      
      // Uniqueness:
      ::triclopsGetUniquenessValidation( *m_pTriclopsContext, &bOn );
      m_checkUniqueness.SetCheck( bOn ? 1 : 0 );
      
      // Subpixel:
      ::triclopsGetSubpixelInterpolation( *m_pTriclopsContext, &bOn );
      m_checkSubpixel.SetCheck( bOn ? 1 : 0 );
      
      // SurfaceValidation:
      ::triclopsGetSurfaceValidation( *m_pTriclopsContext, &bOn );
      m_checkSurfaceValidation.SetCheck( bOn ? 1 : 0 );
      
      // Rectification quality:      
      //m_checkRectification.SetCheck( (rQuality==TriRectQlty_ENHANCED) ? 1 : 0 );
      setRectificationQualityFromTriclops();

      // Stereo algorithm quality:
      TriclopsStereoQuality sQuality;
      ::triclopsGetStereoQuality( *m_pTriclopsContext, &sQuality );
      m_checkStereoAlgorithm.SetCheck( (sQuality==TriStereoQlty_ENHANCED) ? 1 : 0 );
      
      //
      // Stereo algorithm configuration
      //
      TriclopsCameraConfiguration	 config;
      ::triclopsGetCameraConfiguration( *m_pTriclopsContext, &config );
      

      CString csMsg;

      switch( config )
      {
      case TriCfg_L:
	 m_buttonAlg3Cam.SetCheck( 1 );
	 m_buttonAlg2CamH.SetCheck( 0 );
	 m_buttonAlg2CamV.SetCheck( 0 );
	 m_checkBackForth.EnableWindow( FALSE );
	 break;
      case TriCfg_2CAM_HORIZONTAL:
	 m_buttonAlg3Cam.SetCheck( 0 );
	 m_buttonAlg2CamH.SetCheck( 1 );
	 m_buttonAlg2CamV.SetCheck( 0 );

	 m_checkBackForth.EnableWindow( TRUE );
	 break;
      case TriCfg_2CAM_HORIZONTAL_WIDE:
	 m_buttonAlg3Cam.SetCheck( 0 );
	 m_buttonAlg2CamH.SetCheck( 0 );
	 m_buttonAlg2CamV.SetCheck( 1 );

	 m_checkBackForth.EnableWindow( TRUE );
	 break;
      case TriCfg_2CAM_VERTICAL:
	 m_buttonAlg3Cam.SetCheck( 0 );
	 m_buttonAlg2CamH.SetCheck( 0 );
	 m_buttonAlg2CamV.SetCheck( 1 );
	 m_checkBackForth.EnableWindow( FALSE );
	 break;
	 
      default:

	 m_checkBackForth.EnableWindow( FALSE );
	 ASSERT( false );
	 
	 //
	 // Default to 3-cam stereo.
	 //
	 m_buttonAlg3Cam.SetCheck( 1 );
	 m_buttonAlg2CamH.SetCheck( 0 );
	 m_buttonAlg2CamV.SetCheck( 0 );
	 break;
      }
     
      //
      // Physical camera configuration.
      //
      ::triclopsGetDeviceConfiguration( *m_pTriclopsContext, &config );
      
      switch( config )
      {
      case TriCfg_L:
	 break;
	 
      case TriCfg_2CAM_HORIZONTAL:
	 m_buttonAlg3Cam.EnableWindow( FALSE );

	 if (!m_bIsXB3)
	    m_buttonAlg2CamV.EnableWindow( FALSE );

	 break;

      case TriCfg_2CAM_HORIZONTAL_WIDE:
	 m_buttonAlg3Cam.EnableWindow( FALSE );
	 break;

      case TriCfg_2CAM_VERTICAL:
	 m_buttonAlg3Cam.EnableWindow( FALSE );
	 //m_buttonAlg2CamH.EnableWindow( FALSE );
	 break;
	 
      default:
	 ASSERT( false );
      }
   }
}


BEGIN_MESSAGE_MAP(CPGRStereoControlDialog, CDialog)
//{{AFX_MSG_MAP(CPGRStereoControlDialog)
ON_BN_CLICKED(PGRRES_CHECK_STEREO, OnCHECKStereo)
ON_BN_CLICKED(PGRRES_CHECK_EDGE, OnCHECKEdge)
ON_BN_CLICKED(PGRRES_CHECK_TEXTURE, OnCHECKTexture)
ON_BN_CLICKED(PGRRES_CHECK_BACKFORTH, OnCHECKBackForth)
ON_BN_CLICKED(PGRRES_CHECK_UNIQUENESS, OnCHECKUniqueness)
ON_BN_CLICKED(PGRRES_CHECK_SURFACEVALIDATION, OnCHECKSurfaceValidation)
ON_WM_HSCROLL()
ON_BN_CLICKED(PGRRES_CHECK_SUBPIXEL, OnCHECKSubpixel)
ON_BN_CLICKED(PGRRES_CHECK_SMOOTH, OnCHECKSmooth)
ON_CBN_SELCHANGE(PGRRES_IDC_COMBO_RECTIFICATION, OnSELCHANGEIdcComboRectification)
ON_BN_CLICKED(PGRRES_CHECK_STEREO_ALGORITHM, OnCHECKStereoAlgorithm)
ON_BN_CLICKED(PGRRES_RADIO_STEREOALG_2CAMHORZ, OnRadioStereoalg2camhorz)
ON_BN_CLICKED(PGRRES_RADIO_STEREOALG_2CAMVERT, OnRadioStereoalg2camvert)
ON_BN_CLICKED(PGRRES_RADIO_STEREOALG_3CAM, OnRadioStereoalg3cam)
ON_BN_CLICKED(PGRRES_CHECK_DISPLAYHEATMAP, OnCheckDisplayHeatmap)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL 
CPGRStereoControlDialog::OnInitDialog() 
{
   char		  valueText[ 128 ];
   CSliderCtrl*	  pSlider;
   
   // Set up stereo mask slider
   pSlider = (CSliderCtrl*)GetDlgItem( PGRRES_SLIDER_STEREO );
   pSlider->SetRange( 0, 11 );         // 1-23 odd
   sprintf( valueText, "%d", m_nStereoMaskSize );
   GetDlgItem( PGRRES_STATIC_STEREO )->SetWindowText( valueText );
   pSlider->SetPos( (m_nStereoMaskSize - 1) / 2 );
   
   // Set up edge mask slider
   pSlider = (CSliderCtrl*)GetDlgItem( PGRRES_SLIDER_EDGE );
   pSlider->SetRange( 0, 5 );           // 1-11 odd
   sprintf( valueText, "%d", m_nEdgeMaskSize );
   GetDlgItem( PGRRES_STATIC_EDGE )->SetWindowText( valueText );
   pSlider->SetPos( (m_nEdgeMaskSize - 1) / 2 );
   
   // Set up maximum disparity slider
   pSlider = (CSliderCtrl*)GetDlgItem( PGRRES_SLIDER_DISPARITY );
   int maxDisp = getMaxDisparityAllowed( false );
   pSlider->SetRange( 0, maxDisp );     // 0-maxDisp 
   sprintf( valueText, "%d", m_nMaxDisparity );
   GetDlgItem( PGRRES_STATIC_DISPARITY )->SetWindowText( valueText );
   pSlider->SetPos( m_nMaxDisparity );

   // Set up minimum disparity slider
   pSlider = (CSliderCtrl*)GetDlgItem( PGRRES_SLIDER_MINDISPARITY );
   pSlider->SetRange( 0, maxDisp );     // 0-maxDisp
   sprintf( valueText, "%d", m_nMinDisparity );
   GetDlgItem( PGRRES_STATIC_MINDISPARITY )->SetWindowText( valueText );
   pSlider->SetPos( m_nMinDisparity );
   
   // Set up texture validation slider
   pSlider = (CSliderCtrl*)GetDlgItem( PGRRES_SLIDER_TEXTURE );
   pSlider->SetRange( 0, 400 );        // 0-4  (float 1/100) 
   sprintf( valueText, "%5.2f", m_fTextureValidation );
   GetDlgItem( PGRRES_STATIC_TEXTURE )->SetWindowText( valueText );
   pSlider->SetPos( (int)(m_fTextureValidation * 100.0) );
   
   // Set up uniqueness validation slider
   pSlider = (CSliderCtrl*)GetDlgItem( PGRRES_SLIDER_UNIQUENESS );
   pSlider->SetRange( 0, 300 );    // 0-10 (float 1/100)
   sprintf( valueText, "%5.2f", m_fUniquenessValidation );
   GetDlgItem( PGRRES_STATIC_UNIQUENESS )->SetWindowText( valueText );
   pSlider->SetPos( (int)(m_fUniquenessValidation * 100.00) );

   // Set up point size slider
   pSlider = (CSliderCtrl*)GetDlgItem( PGRRES_SLIDER_POINT_SIZE );
   pSlider->SetRange( 0, 200 );    
   sprintf( valueText, "%d%%", m_fPointSize );
   GetDlgItem( PGRRES_STATIC_POINT_SIZE )->SetWindowText( valueText );
   pSlider->SetPos( (int)(m_fPointSize ) );
   
   // Set up surface validation size slider
   pSlider = (CSliderCtrl*)GetDlgItem( PGRRES_SLIDER_SURFACEVALIDATIONSIZE );
   pSlider->SetRange( 0, 400 );          
   sprintf( valueText, "%d", m_nSurfaceValidationSize );
   GetDlgItem( PGRRES_STATIC_SURFACEVALIDATIONSIZE )->SetWindowText( valueText );
   pSlider->SetPos( m_nSurfaceValidationSize );
   
   // Set up surface validation difference slider
   pSlider = (CSliderCtrl*)GetDlgItem( PGRRES_SLIDER_SURFACEVALIDATIONDIFF );
   pSlider->SetRange( 0, 300 );     
   sprintf( valueText, "%3.2f", m_fSurfaceValidationDiff );
   GetDlgItem( PGRRES_STATIC_SURFACEVALIDATIONDIFF )->SetWindowText( valueText );
   pSlider->SetPos( (int)(m_fSurfaceValidationDiff * 100.0) );

   
   CDialog::OnInitDialog();

   // disable the Invalid Colors check box if the heatmap check box 
   // is on
   if ( m_checkHeatmap.GetCheck() == TRUE )
   {
      m_checkInvalidColors.EnableWindow( FALSE );
   }
   
   //
   m_comboBoxRectification.InsertString( 0, "Standard" );
   m_comboBoxRectification.InsertString( 1, "Enhanced 1" );
   m_comboBoxRectification.InsertString( 2, "Enhanced 2" );

   UpdateData( FALSE );

   // Create a tips for the various controls
   m_tipCtrl.Create( this );
   m_tipCtrl.AddWindowTool( &m_checkInvalidColors, 
      _T("Use different colors for each kind of invalid pixel"
	 "(See color icons in validation box)") );
   m_tipCtrl.AddWindowTool( &m_checkHeatmap,
      _T("Display disparity as a 'heatmap' or a grayscale") );
   m_tipCtrl.AddWindowTool( &m_buttonAlg3Cam,
      _T("Use 3 camera stereo - Bumblebee XB3 only") );
   m_tipCtrl.AddWindowTool( &m_buttonAlg2CamV,
      _T("Use 2 camera stereo - Narrow Baseline") );
   m_tipCtrl.AddWindowTool( &m_buttonAlg2CamH,
      _T("Use 2 camera stereo - Wide Baseline") );
   m_tipCtrl.AddWindowTool( &m_sliderSurfaceValidationSize,
      _T("Controls the minimum size that a valid surface must be (in pixels)") );
   m_tipCtrl.AddWindowTool( &m_sliderSurfaceValidationDiff,
      _T("Controls the maximum disparity difference between adjacent pixels for"
      " them to be in the same surface") );
   m_tipCtrl.AddWindowTool( &m_sliderUniqueness,
      _T("Uniqueness threshold - higher values mean less aggressive validation") );
   m_tipCtrl.AddWindowTool( &m_sliderTexture,
      _T("Texture threshold - higher values mean more aggressive validation") );
   m_tipCtrl.AddWindowTool( &m_sliderStereo,
      _T("Controls the size of the stereo correlation mask") );
   m_tipCtrl.AddWindowTool( &m_sliderEdge,
      _T("Controls the size of the edge extraction mask - 7 or 9 is recommended") );
   m_tipCtrl.AddWindowTool( &m_sliderDisparity,
      _T("Controls the maximum disparity searched") );
   m_tipCtrl.AddWindowTool( &m_sliderMinDisparity,
      _T("Controls the minimum disparity searched") );
   m_tipCtrl.AddWindowTool( &m_checkSmooth,
      _T("Should always be left on! - turns pre-smoothing in rectification on or off.") );
   m_tipCtrl.AddWindowTool( &m_checkEdge,
      _T("Indicates whether stereo correlation should be done on edge or greyscale images -"
	 "edge correlation is less sensitive to image brightness differences and is more robust") );
   m_tipCtrl.AddWindowTool( &m_checkStereoButton,
      _T("Turns stereo process on and off") );
   m_tipCtrl.AddWindowTool( &m_checkSubpixel,
      _T("Turns subpixel interpolation on and off - subpixel interpolation on gives more"
      " accurate 3D views") );
   m_tipCtrl.AddWindowTool( &m_checkUniqueness,
      _T("Turns uniqueness validation on and off - recommended to be left OFF") );
   m_tipCtrl.AddWindowTool( &m_checkTexture,
      _T("Turns texture validation on and off - recommended to be left ON") );
   m_tipCtrl.AddWindowTool( &m_checkBackForth,
      _T("Turns back-and-forth validation on and off - only available for 2 camera stereo,"
      " this validation method is very useful") );
   m_tipCtrl.AddWindowTool( &m_checkSurfaceValidation,
      _T("Turns surface validation on or off - recommended to be left ON") );
   m_tipCtrl.AddWindowTool( &m_buttonAlg3Cam,
      _T("") );
	
   return TRUE;
}


void
CPGRStereoControlDialog::PostNcDestroy() 
{
}


void 
CPGRStereoControlDialog::OnCancel()
{
   // do nothing - disable cancel.
}


void 
CPGRStereoControlDialog::OnOK()
{
   // do nothing - disable OK.
}

void 
CPGRStereoControlDialog::OnCHECKStereo() 
{
   TriclopsError error = ::triclopsSetDoStereo( 
      *m_pTriclopsContext, m_checkStereoButton.GetCheck() );

   _VERIFY_TRICLOPS_ERROR;
}

void 
CPGRStereoControlDialog::OnCHECKEdge() 
{
   TriclopsError error = ::triclopsSetEdgeCorrelation( 
      *m_pTriclopsContext, m_checkEdge.GetCheck() );

   _VERIFY_TRICLOPS_ERROR;
}


void 
CPGRStereoControlDialog::OnCHECKSmooth() 
{
   TriclopsError error = ::triclopsSetLowpass( 
      *m_pTriclopsContext, m_checkSmooth.GetCheck() );

   _VERIFY_TRICLOPS_ERROR;
}


void 
CPGRStereoControlDialog::OnCHECKTexture() 
{
   TriclopsError error = ::triclopsSetTextureValidation( 
      *m_pTriclopsContext, m_checkTexture.GetCheck() );

   _VERIFY_TRICLOPS_ERROR;
}


void 
CPGRStereoControlDialog::OnCHECKBackForth() 
{
   TriclopsError error = ::triclopsSetBackForthValidation( 
      *m_pTriclopsContext, m_checkBackForth.GetCheck() );

   _VERIFY_TRICLOPS_ERROR;
}


void 
CPGRStereoControlDialog::OnCHECKUniqueness() 
{
   TriclopsError error = ::triclopsSetUniquenessValidation( 
      *m_pTriclopsContext, m_checkUniqueness.GetCheck() );

   _VERIFY_TRICLOPS_ERROR;
}


void 
CPGRStereoControlDialog::OnCHECKSurfaceValidation() 
{
   TriclopsError error = ::triclopsSetSurfaceValidation( 
      *m_pTriclopsContext, m_checkSurfaceValidation.GetCheck() );

   _VERIFY_TRICLOPS_ERROR;
}


void 
CPGRStereoControlDialog::OnCHECKSubpixel() 
{
   TriclopsError error = ::triclopsSetSubpixelInterpolation( 
      *m_pTriclopsContext, m_checkSubpixel.GetCheck() );

   _VERIFY_TRICLOPS_ERROR;
}

void 
CPGRStereoControlDialog::OnSELCHANGEIdcComboRectification() 
{
   setRectificationQualityToTriclops();
}

void 
CPGRStereoControlDialog::OnCHECKStereoAlgorithm() 
{
   BOOL b = m_checkStereoAlgorithm.GetCheck();
   TriclopsError error = ::triclopsSetStereoQuality( 
      *m_pTriclopsContext, ((b==1) ? TriStereoQlty_ENHANCED : TriStereoQlty_STANDARD ) );

   _VERIFY_TRICLOPS_ERROR;
}

void 
CPGRStereoControlDialog::OnRadioStereoalg2camhorz() 
{

   // set the pan register to 1 (narrow)

   TriclopsError error = ::triclopsSetCameraConfiguration( 
      *m_pTriclopsContext,  TriCfg_2CAM_HORIZONTAL_NARROW );

   _VERIFY_TRICLOPS_ERROR;

   error = ::triclopsSetResolution(
      *m_pTriclopsContext, m_iHeight, m_iWidth );
   
   _VERIFY_TRICLOPS_ERROR;   

   //
   // See if we should adjust the pan register.  Not needed for saved images.
   //
   if( m_pFlycaptureContext )
   {
      flycaptureSetCameraProperty( 
	 *m_pFlycaptureContext, FLYCAPTURE_PAN, 1, 0, FALSE);
   }
   
   m_checkBackForth.EnableWindow( TRUE );
   
   updateDisparitySliders(true);

   UpdateData(FALSE);
}


void 
CPGRStereoControlDialog::OnRadioStereoalg2camvert() 
{

   // set the pan register to 0 (wide)

   TriclopsError error = ::triclopsSetCameraConfiguration(
      *m_pTriclopsContext,  TriCfg_2CAM_HORIZONTAL_WIDE );

   _VERIFY_TRICLOPS_ERROR;  

   error = ::triclopsSetResolution(
      *m_pTriclopsContext, m_iHeight, m_iWidth );
   
   _VERIFY_TRICLOPS_ERROR;   

   //
   // See if we should adjust the pan register.  Not needed for saved images.
   //
   if( m_pFlycaptureContext )
   {
      flycaptureSetCameraProperty( 
	 *m_pFlycaptureContext, FLYCAPTURE_PAN, 0, 0, FALSE);
   }

   m_checkBackForth.EnableWindow( TRUE );
   
   updateDisparitySliders(true);

   UpdateData(FALSE);   
}


void 
CPGRStereoControlDialog::OnRadioStereoalg3cam() 
{
   TriclopsError error = ::triclopsSetCameraConfiguration( 
      *m_pTriclopsContext,  TriCfg_L );

   m_checkBackForth.EnableWindow( FALSE );
   
   updateDisparitySliders(true);

   _VERIFY_TRICLOPS_ERROR;
}

int 
CPGRStereoControlDialog::getMaxDisparityAllowed( bool bSaveAndValidate )
{
   int nrows, ncols;
   int stereoMaskSize;
   int maxDisparityAllowed;
   TriclopsCameraConfiguration config;

   ::triclopsGetResolution( *m_pTriclopsContext, &nrows, &ncols );

   if (bSaveAndValidate) // Get mask size from dialog
   {
      stereoMaskSize = m_sliderStereo.GetPos() * 2 + 1;
   }
   else // Get mask size from triclops (e.g. resolution changed)
   {
      ::triclopsGetStereoMask( *m_pTriclopsContext, &stereoMaskSize );
   }

   ::triclopsGetCameraConfiguration( *m_pTriclopsContext, &config );
   switch( config )
   {
      case TriCfg_L:
	 maxDisparityAllowed = (nrows < ncols ? nrows : ncols) - stereoMaskSize;
	 break;

      case TriCfg_2CAM_HORIZONTAL:
      case TriCfg_2CAM_HORIZONTAL_WIDE:
	 maxDisparityAllowed = ncols - stereoMaskSize;
	 break;

      case TriCfg_2CAM_VERTICAL:
	 maxDisparityAllowed = nrows - stereoMaskSize;
	 break;
	 
      default:
	 // worst case is probably rows (typically less than cols)
	 maxDisparityAllowed = nrows - stereoMaskSize;
	 break;
   }

   return maxDisparityAllowed;
}

void 
CPGRStereoControlDialog::updateDisparitySliders(bool bSaveAndValidate)
{
   int maxDisparityAllowed = getMaxDisparityAllowed( bSaveAndValidate );
   int sliderMaxDisparity = m_sliderDisparity.GetRangeMax();

   // If the current min/max disparity slider ranges have become invalid:
   CSliderCtrl* pSlider;
   char valueText[256];
   int sliderPos;
   if (sliderMaxDisparity != maxDisparityAllowed)
   {
      // Do it for the max slider
      sliderPos = m_sliderDisparity.GetPos();
      pSlider = (CSliderCtrl*)GetDlgItem( PGRRES_SLIDER_DISPARITY );
      pSlider->SetRange( 0, maxDisparityAllowed, TRUE );     
      if (sliderPos > maxDisparityAllowed)
      {
	 m_sliderDisparity.SetPos( maxDisparityAllowed );
	 m_nMaxDisparity = m_sliderDisparity.GetPos();
	 sprintf( valueText, "%d", m_nMaxDisparity );
	 GetDlgItem( PGRRES_STATIC_DISPARITY )->SetWindowText( valueText );
      }
      else
      {
	 // Just reset the slider position -- the current value is within
	 // the new range, so we don't need to reset the window text.
	 m_sliderDisparity.SetPos( sliderPos );
      }

      // Do it for the min slider
      sliderPos = m_sliderMinDisparity.GetPos();
      pSlider = (CSliderCtrl*)GetDlgItem( PGRRES_SLIDER_MINDISPARITY );
      pSlider->SetRange( 0, maxDisparityAllowed, TRUE );     
      if (sliderPos > maxDisparityAllowed)
      {
	 m_sliderMinDisparity.SetPos( maxDisparityAllowed );
	 m_nMinDisparity = m_sliderMinDisparity.GetPos();
	 sprintf( valueText, "%d", m_nMinDisparity );
	 GetDlgItem( PGRRES_STATIC_MINDISPARITY )->SetWindowText( valueText );
      }
      else
      {
	 // Just reset the slider position -- the current value is within
	 // the new range, so we don't need to reset the window text.
	 m_sliderMinDisparity.SetPos( sliderPos );
      }
   }
}

void 
CPGRStereoControlDialog::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
   char	 valueText[5];
   int	 sliderValue;
   int	 staticItemNo = 0;
   int	 otherDisp = 0;

   int	 nControl = pScrollBar->GetDlgCtrlID();
   CSliderCtrl* pControl = (CSliderCtrl*)GetDlgItem( nControl );
   
   if( pControl != NULL )
   {
      sliderValue = pControl->GetPos();
      switch( nControl )
      {
      case PGRRES_SLIDER_STEREO:
	 staticItemNo = PGRRES_STATIC_STEREO;

	 updateDisparitySliders(true);

	 sprintf( 
	    valueText, 
	    "%d", 
	    sliderValue * 2 + 1  );           // make it odd
	 break;

      case PGRRES_SLIDER_EDGE:
	 staticItemNo = PGRRES_STATIC_EDGE;
	 sprintf( 
	    valueText, 
	    "%d", 
	    sliderValue * 2 + 1 );            // make it odd
	 break;

      case PGRRES_SLIDER_DISPARITY:
	 staticItemNo = PGRRES_STATIC_DISPARITY;

	 otherDisp = m_sliderMinDisparity.GetPos();
	 if ((sliderValue-240) > otherDisp)
	 {
	    // Allowable minimum disparities are within 240 of max disp.
	    int newMinDisp = sliderValue-240;
	    m_sliderMinDisparity.SetPos( newMinDisp );
	    newMinDisp = m_sliderMinDisparity.GetPos();
	    sprintf( valueText, "%d",  newMinDisp );   
	    GetDlgItem( PGRRES_STATIC_MINDISPARITY )->SetWindowText( valueText );
	 }

	 // Now do the max disparity case like all the others in this switch
	 sprintf( 
	    valueText, 
	    "%d", 
	    sliderValue );

	 break;

      case PGRRES_SLIDER_MINDISPARITY:
	 staticItemNo = PGRRES_STATIC_MINDISPARITY;

	 otherDisp = m_sliderDisparity.GetPos();
	 if ((otherDisp-240) > sliderValue)
	 {
	    // Allowable minimum disparities are within 240 of max disp.
	    int newMaxDisp = sliderValue+240;
	    m_sliderDisparity.SetPos( newMaxDisp );
	    newMaxDisp = m_sliderDisparity.GetPos();
	    sprintf( valueText, "%d",  newMaxDisp );   
	    GetDlgItem( PGRRES_STATIC_DISPARITY )->SetWindowText( valueText );
	 }

	 // Now do the min disparity case like all the others in this switch
	 sprintf( 
	    valueText, 
	    "%d", 
	    sliderValue );
	 break;

      case PGRRES_SLIDER_SURFACEVALIDATIONSIZE:
	 staticItemNo = PGRRES_STATIC_SURFACEVALIDATIONSIZE;
	 sprintf( 
	    valueText, 
	    "%d", 
	    sliderValue );
	 break;

      case PGRRES_SLIDER_SURFACEVALIDATIONDIFF:
	 staticItemNo = PGRRES_STATIC_SURFACEVALIDATIONDIFF;
	 sprintf( 
	    valueText, 
	    "%3.2f", 
	    (float)sliderValue / 100.0 );
	 break;

      case PGRRES_SLIDER_TEXTURE:
	 staticItemNo = PGRRES_STATIC_TEXTURE;
	 sprintf( 
	    valueText, 
	    "%5.2f", 
	    sliderValue / 100.0 );
	 break;

      case PGRRES_SLIDER_UNIQUENESS:
	 staticItemNo = PGRRES_STATIC_UNIQUENESS;
	 sprintf( 
	    valueText, 
	    "%5.2f", 
	    sliderValue / 100.0 );
	 break;
         
      case PGRRES_SLIDER_POINT_SIZE:
         staticItemNo = PGRRES_STATIC_POINT_SIZE;
         sprintf( 
            valueText, 
            "%d%%", 
            sliderValue );
         break;
      }
      
      GetDlgItem( staticItemNo )->SetWindowText( valueText );
   }

   CDialog::OnHScroll( nSBCode, nPos, pScrollBar );

   CPGRStereoControlDialog::UpdateData( TRUE );
}


TriclopsError
CPGRStereoControlDialog::setContext( TriclopsContext* pTriclopsContext, int iHeight, int iWidth )
{
   if( pTriclopsContext == NULL )
   {
      ASSERT( FALSE );
      return TriclopsErrorInvalidContext;
   }

   m_iHeight = iHeight;
   m_iWidth = iWidth;

   if (m_pTriclopsContext != NULL)
   {
      TriclopsCameraConfiguration	 config;

      // set the new context to have the same camera configuration
      ::triclopsGetCameraConfiguration( *m_pTriclopsContext, &config );
      ::triclopsSetCameraConfiguration( *pTriclopsContext, config );
   } else {

      if (m_bIsXB3)
      { 
	 ::triclopsSetCameraConfiguration( 
	    *pTriclopsContext,  TriCfg_2CAM_HORIZONTAL_WIDE );
      } else {
	 ::triclopsSetCameraConfiguration( 
	    *pTriclopsContext,  TriCfg_2CAM_HORIZONTAL_NARROW );
      }
   
   }

   m_pTriclopsContext = pTriclopsContext;

   if( ::IsWindow( m_hWnd ) )
   {
      UpdateData( FALSE );
   }
   
   return TriclopsErrorOk;
}

TriclopsError
CPGRStereoControlDialog::setFlycaptureContext( FlyCaptureContext* pflycaptureContext )
{
   if( pflycaptureContext == NULL )
   {
      ASSERT( FALSE );
      return TriclopsErrorInvalidContext;
   }
   
   m_pFlycaptureContext = pflycaptureContext;

   flycaptureGetCameraInfo( m_pFlycaptureContext, &m_flycaptureInfo );
   
   if (m_flycaptureInfo.CameraModel != FLYCAPTURE_BUMBLEBEEXB3)
   {
      m_bIsXB3 = true;
      
   } else {
      m_bIsXB3 = false;
   }

   if( ::IsWindow( m_hWnd ) )
   {
      UpdateData( FALSE );
   }
   
   return TriclopsErrorOk;
}

BOOL 
CPGRStereoControlDialog::Create()
{
   if( m_pTriclopsContext == NULL )
   {
      ASSERT( FALSE );
      return false;
   }
   
   return CDialog::Create( CPGRStereoControlDialog::IDD, NULL );
}

void
CPGRStereoControlDialog::setRectificationQualityFromTriclops()
{
   TriclopsRectImgQuality rQuality;
   ::triclopsGetRectImgQuality( *m_pTriclopsContext, &rQuality );

   switch (rQuality)
   {
      case TriRectQlty_STANDARD:
	 m_comboBoxRectification.SetCurSel( 0 );
	 break;
      case TriRectQlty_ENHANCED_1:
	 m_comboBoxRectification.SetCurSel( 1 );
	 break;
      case TriRectQlty_ENHANCED_2:
	 m_comboBoxRectification.SetCurSel( 2 );
	 break;
      default:
	 m_comboBoxRectification.SetCurSel( 0 );
	 break;
   }
}

void
CPGRStereoControlDialog::setRectificationQualityToTriclops() 
{
   int index = m_comboBoxRectification.GetCurSel();
   switch (index)
   {
      case 0:
	 triclopsSetRectImgQuality( *m_pTriclopsContext, TriRectQlty_STANDARD );
	 break;
      case 1:
	 triclopsSetRectImgQuality( *m_pTriclopsContext, TriRectQlty_ENHANCED_1 );
	 break;
      case 2:
	 triclopsSetRectImgQuality( *m_pTriclopsContext, TriRectQlty_ENHANCED_2 );
	 break;
      default:
	 triclopsSetRectImgQuality( *m_pTriclopsContext, TriRectQlty_STANDARD );
	 break;
   }
   
}


void
 CPGRStereoControlDialog::OnCheckDisplayHeatmap() 
{
   if ( m_checkHeatmap.GetCheck() == TRUE )
   {
      m_checkInvalidColors.EnableWindow( FALSE );
   }
   else
   {
      m_checkInvalidColors.EnableWindow( TRUE );
   }
	
}
