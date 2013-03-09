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
// $Id: PGRStereoDialogBar.cpp,v 1.5 2007/07/10 18:01:00 demos Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <afxext.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRStereoDoc.h"
#include "PGRStereoDialogBar.h"

#include "PGRFlyCaptureStereo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


enum
{
   COMBO_NO_MATCH	   = -1,
   COMBO_IMAGE_RAW	   = 0,
   COMBO_IMAGE_RECTIFIED   = 1,
   COMBO_IMAGE_COLOR_RECTIFIED = 2,
   COMBO_IMAGE_DISPARITY   = 3,
   COMBO_IMAGE_EDGE	   = 4,
   COMBO_IMAGE_TOTAL	   = 5,
   COMBO_CAMERA_RIGHT	   = 0,
   COMBO_CAMERA_LEFT	   = 1,
   COMBO_CAMERA_CENTER	   = 2,
   COMBO_CAMERA_TOTAL	   = 3,
   COMBO_RES_160	   = 0,
   COMBO_RES_256	   = 1,
   COMBO_RES_320	   = 2,
   COMBO_RES_400	   = 3,
   COMBO_RES_512	   = 4,
   COMBO_RES_640	   = 5,
   COMBO_RES_800	   = 6,
   COMBO_RES_1024	   = 7,
   COMBO_RES_1280          = 8,
   COMBO_RES_TOTAL	   = 9
};

// Note: these items must be in the same order
// as the COMBO_IMAGE_* types in the enum above
// i.e. "Color Rectified" must be in the 
// COMBO_IMAGE_COLOR_RECTIFIED location
//
const char* imageNames[] =
{
   "Raw",
   "Rectified",
   "Color Rectified",
   "Disparity",
   "Edge"
};

const char* cameraNames[] = 
{
   "Right",
   "Left",
   "Center"
};

const char* resolutionNames[] =
{
   "160x120",
   "256x192",   
   "320x240",
   "400x300",
   "512x384",
   "640x480",
   "800x600",
   "1024x768",
   "1280x960"
};

const int arCameraImageLUT[COMBO_CAMERA_TOTAL][COMBO_IMAGE_TOTAL] =
{
   {IMAGE_RIGHT_RAW, IMAGE_RIGHT_RECTIFIED, IMAGE_RIGHT_COLOR_RECTIFIED, IMAGE_DISPARITY, IMAGE_RIGHT_EDGE},
   {IMAGE_LEFT_RAW,  IMAGE_LEFT_RECTIFIED,  IMAGE_LEFT_COLOR_RECTIFIED,  IMAGE_DISPARITY, IMAGE_LEFT_EDGE},
   {IMAGE_CENTER_RAW,   IMAGE_CENTER_RECTIFIED,   IMAGE_CENTER_COLOR_RECTIFIED,   IMAGE_DISPARITY, IMAGE_CENTER_EDGE}
};

const int arResolutionLUT[COMBO_RES_TOTAL] =
{
   STEREO_160x120,
   STEREO_256x192,
   STEREO_320x240,
   STEREO_400x300,
   STEREO_512x384,
   STEREO_640x480,
   STEREO_800x600,
   STEREO_1024x768,
   STEREO_1280x960,
};

int getListIndex(const char* szIdentifier,
	     const char* arIdentifierList[],
	     int nIdentifiers )
{
   int nIndex = COMBO_NO_MATCH;
   for ( int i = 0; i < nIdentifiers; i++ )
   {
      if ( !strcmp( szIdentifier, arIdentifierList[i] ) )
      {
	 nIndex = i;;
	 break;
      }
   }
   return nIndex;
}

inline int
getImageIndex( const char* szIdentifier )
{
   return getListIndex( szIdentifier, imageNames, COMBO_IMAGE_TOTAL );
}

inline int
getResIndex( const char* szIdentifier )
{
   return getListIndex( szIdentifier, resolutionNames, COMBO_RES_TOTAL );
}

inline int
getCameraIndex( const char* szIdentifier )
{
   return getListIndex( szIdentifier, cameraNames, COMBO_CAMERA_TOTAL );
}




CPGRStereoDialogBar::CPGRStereoDialogBar()
{
   //{{AFX_DATA_INIT(CPGRStereoDialogBar)
   m_idxCamera = -1;
   m_idxImage = -1;
   m_idxResolution = -1;
   //}}AFX_DATA_INIT
}


void 
CPGRStereoDialogBar::DoDataExchange( CDataExchange* pDX )
{
   CDialogBar::DoDataExchange(pDX);

   //{{AFX_DATA_MAP(CPGRStereoDialogBar)
   DDX_Control(pDX, PGRRES_CHECK_CAMERA_CONTROL, m_buttonCameraControl);
   DDX_Control(pDX, PGRRES_BUTTON_TRANSFORMATION, m_buttonTransformation);
   DDX_Control(pDX, PGRRES_BUTTON_STEREOPARAMS, m_buttonStereoParams);
   DDX_Control(pDX, PGRRES_IDC_COMBO_RESOLUTION, m_comboBoxResolution);
   DDX_Control(pDX, PGRRES_IDC_COMBO_IMAGE, m_comboBoxImage);
   DDX_Control(pDX, PGRRES_IDC_COMBO_CAMERA, m_comboBoxCamera);
   DDX_CBIndex(pDX, PGRRES_IDC_COMBO_CAMERA, m_idxCamera);
   DDX_CBIndex(pDX, PGRRES_IDC_COMBO_IMAGE, m_idxImage);
   DDX_CBIndex(pDX, PGRRES_IDC_COMBO_RESOLUTION, m_idxResolution);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( CPGRStereoDialogBar, CDialogBar )
//{{AFX_MSG_MAP(CPGRStereoDialogBar)
	//}}AFX_MSG_MAP
ON_MESSAGE( WM_INITDIALOG, OnInitDialog )
END_MESSAGE_MAP()


LONG
CPGRStereoDialogBar::OnInitDialog( UINT wParam, LONG lParam )
{
/*
   if( !HandleInitDialog( wParam, lParam ) || !UpdateData( FALSE ) )
   {
      TRACE0( "Warning: UpdateData failed during dialog init.\n" );
      return FALSE;
   }
*/

   //
   // Work around Microsoft bug in MFC 7.1.  (HandleInitDialog() always returns
   // false)  See PGR bug 2378.
   //
   HandleInitDialog( wParam, lParam );
   UpdateData( FALSE );

   // TEMPORARY UNTIL I CHANGE THE RESOURCES!!!
   m_comboBoxImage.ResetContent();
   m_comboBoxImage.AddString( "Rectified" );
   m_comboBoxImage.AddString( "Raw" );
   m_comboBoxImage.AddString( "Disparity" );


   return 1;
}


int 
CPGRStereoDialogBar::getImageType()
{
   UpdateData();

   char  szString[100];
   int	 nComboIndex;

   nComboIndex = m_comboBoxImage.GetCurSel();
   m_comboBoxImage.GetLBText( nComboIndex, (LPTSTR) szString );
   int nImageIndex = ::getImageIndex( szString );

   nComboIndex = m_comboBoxCamera.GetCurSel();
   m_comboBoxCamera.GetLBText( nComboIndex, (LPTSTR) szString );
   int nCameraIndex = ::getCameraIndex( szString );

   if ( nCameraIndex == COMBO_NO_MATCH ||
        nImageIndex == COMBO_NO_MATCH )
   {
      ASSERT( false );
      return -1;
   }

   return arCameraImageLUT[nCameraIndex][nImageIndex];
}


void
CPGRStereoDialogBar::setbyImageType( int nImageType )
{
   ASSERT( nImageType >= 0 && nImageType < IMAGE_TOTAL );

   int nImageIndex = 0;
   int nCameraIndex = 0;

   for ( nCameraIndex = 0; nCameraIndex < COMBO_CAMERA_TOTAL; nCameraIndex++ )
   {
      for ( nImageIndex = 0; nImageIndex < COMBO_IMAGE_TOTAL; nImageIndex++ )
      {
	 if ( arCameraImageLUT[nCameraIndex][nImageIndex] == nImageType )
	 {
	    goto bigbreak;
	 }
      }
   }
bigbreak:

   

   ASSERT( nCameraIndex < COMBO_CAMERA_TOTAL &&
	   nImageIndex < COMBO_IMAGE_TOTAL );

   const char* szImageName = imageNames[nImageIndex];
   const char* szCameraName = cameraNames[nCameraIndex];

   int nComboIndex;
   nComboIndex = m_comboBoxImage.FindStringExact( 0, szImageName );
   ASSERT( nComboIndex != CB_ERR );
   m_comboBoxImage.SetCurSel( nComboIndex );
   nComboIndex = m_comboBoxCamera.FindStringExact( 0, szCameraName );
   ASSERT( nComboIndex != CB_ERR );
   m_comboBoxCamera.SetCurSel( nComboIndex );


   UpdateData();
}


StereoImageResolution 
CPGRStereoDialogBar::getResolution()
{
   UpdateData();

   char szIdentifier[100];
   int nSel = m_comboBoxResolution.GetCurSel();
   m_comboBoxResolution.GetLBText( nSel, szIdentifier );
   int nIndex = ::getResIndex( szIdentifier );

   ASSERT( nIndex >= 0 );

   if ( nIndex < 0 )
      return STEREO_320x240;	 // graceful failure

   return (StereoImageResolution) arResolutionLUT[nIndex];
}


void 
CPGRStereoDialogBar::setResolution( StereoImageResolution resolution )
{
   int nIndex = 0;
   for ( nIndex = 0; nIndex < COMBO_RES_TOTAL; nIndex++ )
   {
      if ( arResolutionLUT[nIndex] == resolution )
	 break;
   }

   ASSERT( nIndex < COMBO_RES_TOTAL );
   if ( nIndex >= COMBO_RES_TOTAL )
      return;  // bail

   const char* szIdentifier = resolutionNames[nIndex];
   nIndex = ::getResIndex( szIdentifier );
   if ( nIndex >= 0 )
      m_comboBoxResolution.SetCurSel( nIndex );


   UpdateData();
}

void
CPGRStereoDialogBar::setComboBoxSelections( BOOL bColor,
					    StereoImageResolution maxResolution,
					    BOOL bBumblebee )
{
   m_comboBoxImage.ResetContent();
   m_comboBoxImage.AddString( imageNames[COMBO_IMAGE_RAW] );
   m_comboBoxImage.AddString( imageNames[COMBO_IMAGE_RECTIFIED] );
   if ( bColor )
   {
      m_comboBoxImage.AddString( imageNames[COMBO_IMAGE_COLOR_RECTIFIED] );
   }
   m_comboBoxImage.AddString( imageNames[COMBO_IMAGE_DISPARITY] );
   m_comboBoxImage.AddString( imageNames[COMBO_IMAGE_EDGE] );

   m_comboBoxCamera.ResetContent();
   m_comboBoxCamera.AddString( cameraNames[COMBO_CAMERA_RIGHT] );
   m_comboBoxCamera.AddString( cameraNames[COMBO_CAMERA_LEFT] );
   if ( !bBumblebee )
   {
      m_comboBoxCamera.AddString( cameraNames[COMBO_CAMERA_CENTER] );
   }
   m_comboBoxResolution.ResetContent();
   m_comboBoxResolution.AddString( resolutionNames[COMBO_RES_160] );
   m_comboBoxResolution.AddString( resolutionNames[COMBO_RES_256] );
   m_comboBoxResolution.AddString( resolutionNames[COMBO_RES_320] );
   m_comboBoxResolution.AddString( resolutionNames[COMBO_RES_400] );
   m_comboBoxResolution.AddString( resolutionNames[COMBO_RES_512] );
   if( maxResolution == STEREO_1024x768 )
   {
      m_comboBoxResolution.AddString( resolutionNames[COMBO_RES_640] );
      m_comboBoxResolution.AddString( resolutionNames[COMBO_RES_800] );
   }
   if( maxResolution == STEREO_1280x960 )
   {
      m_comboBoxResolution.AddString( resolutionNames[COMBO_RES_640] );
      m_comboBoxResolution.AddString( resolutionNames[COMBO_RES_800] );
      m_comboBoxResolution.AddString( resolutionNames[COMBO_RES_1024] );
   }
}



