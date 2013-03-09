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
// $Id: PGRStereoDoc.cpp,v 1.16 2007/10/24 01:37:49 mgriffin Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <sys/timeb.h>
#include <time.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRStereoDoc.h"
#include "PGRImageView.h"
#include "PGROpenGLView.h"
#include "PGRTransformDialog.h"
#include "PGRMainFrm.h"

//=============================================================================
// PGR Includes - order is intentional.
//=============================================================================
//#include <PGRError.h>
#include <pnmutils.h>

//=============================================================================
// Macro Definitions
//=============================================================================
#define PGR_PROJECT_NAME "PGRMFC"
#define PGR_FILE_NAME    "$RCSfile: PGRStereoDoc.cpp,v $"
#define PGR_FILE_VERSION "$Revision: 1.16 $"


IMPLEMENT_DYNCREATE( CPGRStereoDoc, CDocument )

BEGIN_MESSAGE_MAP( CPGRStereoDoc, CDocument )
//{{AFX_MSG_MAP(CPGRStereoDoc)
ON_BN_CLICKED(PGRRES_BUTTON_STEREOPARAMS, OnButtonStereoParams)
ON_BN_CLICKED(PGRRES_BUTTON_TRANSFORMATION, OnButtonTransformation)
ON_COMMAND(PGRRES_CHECK_CAMERA_CONTROL, OnCheckCameraControl)
ON_UPDATE_COMMAND_UI(PGRRES_CHECK_CAMERA_CONTROL, OnUpdateCheckCameraControl)
ON_CBN_SELCHANGE(PGRRES_IDC_COMBO_RESOLUTION, OnComboResolution)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//=============================================================================
// Global constants
//=============================================================================

//=============================================================================
// Local helper functions
//=============================================================================

//
// This function tests the TriclopsContext to determine the "native"
// camera configuration of the camera device.  If the configuration is
// "2 Camera Horizontal" then the context was created from a Bumblebee
//
// returns true if is a bumblebee
//
inline bool
isBumblebee( TriclopsContext triclops )
{
   TriclopsError error;
   TriclopsCameraConfiguration triclopsConfig;
   error = ::triclopsGetDeviceConfiguration( triclops, &triclopsConfig );
   ASSERT( error == TriclopsErrorOk );

   if (( triclopsConfig == TriCfg_2CAM_HORIZONTAL ) || ( triclopsConfig == TriCfg_2CAM_HORIZONTAL_WIDE ))
   {
      return true;
   }
   else
   {
      return false;
   }
}


//
// This function sets a CTransformD object by reading the specified
// Triclops transformation file.
//
// returns true if successful
//
bool
getTransformFromFile( char* pszFileName, CTransformD* pTransform )
{
   TriclopsTransform triTransform;

   if( pTransform == NULL || 
       pszFileName == NULL ||
       triclopsGetTransformFromFile( pszFileName, &triTransform ) !=
          TriclopsErrorOk )
   {
      return false;
   }

   pTransform->setMatrix( triTransform.matrix );
   return true;
}

CPGRStereoDoc::CPGRStereoDoc()
{
   m_bDocumentActive	      = FALSE;
   m_bCameraInitialized	      = FALSE;
   m_bImageAvailable	      = FALSE;
   m_bColorData		      = FALSE;
   m_bStereoDialogActive      = FALSE;
   m_bTransformDialogActive   = FALSE;
   m_bOffline		      = FALSE;
   m_nRectifyColorViews	      = 0;
   m_bIsBumblebee	      = TRUE;

   m_nCurrCols = 0;
   m_nCurrRows = 0;

   m_nMaxCols = 1024;
   m_nMaxRows = 768;

   m_currentStereoResolution = STEREO_640x480;
   m_maxStereoResolution = STEREO_1024x768;
   
   m_triclopsContext	= NULL;
   m_pPointCloud	= NULL;
   m_pStaticPointArray	= NULL;
   m_guicontext		= NULL;

   m_pRedRaw	  = NULL;
   m_pGreenRaw	  = NULL;
   m_pBlueRaw	  = NULL;
   
   m_pRedProc	  = NULL;
   m_pGreenProc	  = NULL;
   m_pBlueProc	  = NULL;
   
   m_nLUTMinDisp     =	99999999;
   m_nLUTMaxDisp     =	99999999;

   m_serialNumber = 0;


   //
   // Initialize framerate to 5Hz.
   // This is only the "reported" frame rate - this has nothing to
   // do with how fast the camera will transmit
   //
   m_dFrameRate	     = 5.0;
      
   //
   // Initialize bitmap array
   //
   for ( int iImage = IMAGE_RIGHT_RAW ; iImage <= IMAGE_DISPARITY; iImage++ )
   {
      m_arpBitmap[ iImage ] = NULL;
   }
}


CPGRStereoDoc::~CPGRStereoDoc()
{   
}

void
CPGRStereoDoc::getCameraTitle( char* buffer, const bool bTerse )
{
   if ( m_bOffline && !bTerse )
   {
      if ( m_bIsBumblebee )
      {
	 sprintf( buffer, "Offline Bumblebee%07d", m_serialNumber );
      }
      else
      {
	 sprintf( buffer, "Unknown device" );
      }
   }
   else
   {
      if ( m_bIsBumblebee )
      {
	 sprintf( buffer, "Bumblebee%07d", m_serialNumber );
      }
      else
      {
	 sprintf( buffer, "Unknown device" );
      }
   }
   return;
}

bool
CPGRStereoDoc::loadTriclopsContextFromFile( TriclopsContext* pTriclops )
{
   TriclopsError error;
   
   static char szFilter[] = 
      "Point Grey Research Calibration File (*.cal)|*.cal||";

   // a file-open dialog
   CFileDialog fileDialog( TRUE, "cal", NULL, NULL, szFilter, NULL );
   
   // display the dialog
   if ( fileDialog.DoModal() == IDOK )
   {
      // user selected a file
      CString fileName = fileDialog.GetPathName();
      // note: length of 200 is just a goodly long enough value
      error = 
	 triclopsGetDefaultContextFromFile( pTriclops, fileName.GetBuffer(200) );
      if ( error != TriclopsErrorOk )
      {
	 char szErrorMessage[1024];
	 sprintf( szErrorMessage,
	    "Error opening calibration file\n"
	    "\tTriclopsError: %s\n",
	    triclopsErrorToString( error ) );
	 AfxMessageBox( szErrorMessage );
	 return false;
      }
      return  true;
   }
   
   // else - user canceled
   return false;
}

BOOL
CPGRStereoDoc::OnFileNewCameraDocument()
{
   //
   // Create the flycapture context
   //
   if( ( ::flycaptureCreateContext( &m_flycaptureContext ) ) != FLYCAPTURE_OK )
   {
      ASSERT( FALSE );
      return FALSE;
   }

   // set the flycapture context so that dialogStereo can modify registers.
   m_dialogStereo.setFlycaptureContext( &m_flycaptureContext );
   
   //
   // Initialize grabbing.
   //
   if( !initGrab() )
   {
      // grab init failed - this is unrecoverable.
      // note: returning false means no document is created
      return FALSE;
   }

   m_bOffline = false;

   // Always default the pan register to 0.  For XB3's, this defaults to the wide
   // baseline.  For Bumblebee and Bumblebee2, this is right/left transmission.
   flycaptureSetCameraProperty( m_flycaptureContext, FLYCAPTURE_PAN, 0, 0, FALSE);

   if ( m_flycaptureInfo.CameraModel == FLYCAPTURE_BUMBLEBEE ||
      m_flycaptureInfo.CameraModel == FLYCAPTURE_BUMBLEBEE2 )
   {      
      unsigned long ulValue;
      flycaptureGetCameraRegister( m_flycaptureContext, 0x1F28, &ulValue );
      
      if ( ( ulValue & 0x2 ) == 0 )
      {
	 // Hi-res
	 m_nMaxCols = 1024; 
	 m_nMaxRows = 768;   
	 m_maxStereoResolution = STEREO_1024x768;
      }
      else
      {
	 // Low-res
	 m_nMaxCols = 640;
	 m_nMaxRows = 480;
	 m_maxStereoResolution = STEREO_640x480;
      }
   }
   else if ( m_flycaptureInfo.CameraModel == FLYCAPTURE_BUMBLEBEEXB3 )   
   {
      m_nMaxRows = 960;
      m_nMaxCols = 1280;
      m_maxStereoResolution = STEREO_1280x960;
   }
   else
   {
      ASSERT( FALSE );
      return FALSE;
   }

   // Get calibration file data from camera
   char* szCalFile = NULL;
   if ( flycaptureGetCalibrationFileFromCamera(
      m_flycaptureContext, &szCalFile ) != FLYCAPTURE_OK )
   {
      AfxMessageBox( "No calibration data information found on camera.\n"
	 "Please load calibration data from a file." );

      if ( !loadTriclopsContextFromFile( &m_triclopsContext ) )
      {
	 AfxMessageBox( "triclopsDemo cannot connect to the camera without the"
	    " associated calibration data.  Camera document creation failed." );
	 return FALSE;
      }
   }
   else
   {
      // Get the camera calibration data
      triclopsGetDefaultContextFromFile( &m_triclopsContext, szCalFile );
   }
   
   return InitDocument();
}

BOOL
CPGRStereoDoc::OnFileNewOfflineDocument()
{
   //
   // work off line!
   // we need to set up all the book keeping parameters so that the StereoDoc 
   // understands there is no camera and the user must load an image
   // and calibration file to do any processing
   //
   m_bOffline = true;
   
   // We can't know what kind of Bumblebee was used to acquire
   // these images so lets assume the largest
   m_nMaxRows  = 960;
   m_nMaxCols  = 1280;
   m_maxStereoResolution = STEREO_1280x960;

   if ( !loadTriclopsContextFromFile( &m_triclopsContext ) )
   {
      return FALSE;
   }
   int nSerialNumber;
   triclopsGetSerialNumber( m_triclopsContext, &nSerialNumber );
   m_serialNumber = nSerialNumber;
   
   // need to gray out required buttons here!

   return InitDocument();
}

BOOL
CPGRStereoDoc::InitDocument()
{   
   m_bIsBumblebee = ::isBumblebee( m_triclopsContext );

   // set the document title.
   char szTitle[50];
   getCameraTitle( szTitle );
   SetTitle( szTitle );

   //
   // change the cursor for the following long operation.
   // this should work - the constructor of the CWaitCursor will create
   // the hourglass icon, and the destructor called when this function 
   // returns should switch it back.
   //
   CWaitCursor wait;
   

   m_nMaximumPoints = m_nMaxRows * m_nMaxCols;
   m_pPointCloud = new CPointList( 0 );
   if( m_pPointCloud == NULL )
   {
      ASSERT( FALSE );
      return FALSE;
   }
   
   m_pStaticPointArray = new C3dColourPointRC[ m_nMaximumPoints ];
   if( m_pStaticPointArray == NULL )
   {
      ASSERT( FALSE );
      return FALSE;
   }
   
   //
   // Set up triclops context
   //
   initTriclopsContext();
   
   initializeBitmaps();

   resetImageResolutions( RES_512x384 );
   
   //
   // Call overridable custom initialization.
   //
   customInit();

   return TRUE;   
}


BOOL
CPGRStereoDoc::ReInitDocument()
{   
   m_bIsBumblebee = ::isBumblebee( m_triclopsContext );

   // set the document title.
   char szTitle[50];
   getCameraTitle( szTitle );
   SetTitle( szTitle );

   //
   // change the cursor for the following long operation.
   // this should work - the constructor of the CWaitCursor will create
   // the hourglass icon, and the destructor called when this function 
   // returns should switch it back.
   //
   CWaitCursor wait;

   //
   // Set up triclops context
   //
   initTriclopsContext();

   // Reset image resolution
   resetImageResolutions( RES_512x384 );
      
   //
   // Call overridable custom initialization.
   //
   customInit();

   return TRUE;   
}

void 
CPGRStereoDoc::OnCloseDocument() 
{
   closeGrab();
   
   if( m_bStereoDialogActive )
   {
      m_dialogStereo.DestroyWindow();
   }
   
   if( m_bTransformDialogActive )
   {
      m_dialogTransform.DestroyWindow();
   }

   destroyTriclopsContext();

   if ( m_pRedRaw != NULL )
   {
      delete m_pRedRaw;
      m_pRedRaw = NULL;
      m_pGreenRaw = NULL;
	  m_pBlueRaw = NULL;
   }
   
   
   if ( m_pRedProc != NULL )
   {
      delete m_pRedProc;
      m_pRedProc = NULL;
      m_pGreenProc = NULL;
      m_pBlueProc = NULL;
   }

   for ( int iImage = IMAGE_RIGHT_RAW ; iImage < IMAGE_TOTAL; iImage++ )
   {
      if( m_arpBitmap[ iImage ] != NULL )
      {
	 delete m_arpBitmap[ iImage ];
	 m_arpBitmap[ iImage ] = NULL;
      }
   }
   
   if( m_pStaticPointArray != NULL )
   {
      delete [] m_pStaticPointArray;
      m_pStaticPointArray = NULL;
   }
   
   if( m_pPointCloud != NULL )
   {
      delete m_pPointCloud;
      m_pPointCloud = NULL;
   }
   
   // if the stereo input was loaded from a file, then
   // destroy it to ensure there are no memory leaks
   if ( m_bImageAvailable && m_bOffline )
   {
      freeInput( &m_tiStereo );
   }

   CDocument::OnCloseDocument();
}


void	 
CPGRStereoDoc::initializeBitmaps()
{
   //
   // Initialize images - some need to be stored within the bitmap object, 
   // some do not, as they will just be read out of the triclops input.
   //
   
   //
   //  Raw Images
   //
   if ( m_bOffline )
   {
      //
      // For offline documents, we need to allocate space for the raw images.  They
      // only need to be 8 bit as we will not be displaying color data.
      //
      m_arpBitmap[ IMAGE_RIGHT_RAW ] = 
	 new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 8, false, (unsigned char*)NULL );
      ASSERT( m_arpBitmap[ IMAGE_RIGHT_RAW ] != NULL );
      
      m_arpBitmap[ IMAGE_CENTER_RAW ] = 
	 new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 8, false, (unsigned char*)NULL );
      ASSERT( m_arpBitmap[ IMAGE_CENTER_RAW ] != NULL );
      
      m_arpBitmap[ IMAGE_LEFT_RAW ] = 
	 new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 8, false, (unsigned char*)NULL );
      ASSERT( m_arpBitmap[ IMAGE_LEFT_RAW ] != NULL );
   }
   else
   {
      // For documents connected to a context, the context will supply
      // the memory - we do not need to allocate it.
      //
      
      m_arpBitmap[ IMAGE_RIGHT_RAW ] = 
	 new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 32, false, (unsigned char*)NULL );
      ASSERT( m_arpBitmap[ IMAGE_RIGHT_RAW ] != NULL );
      
      m_arpBitmap[ IMAGE_CENTER_RAW ] = 
	 new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 32, false, (unsigned char*)NULL );
      ASSERT( m_arpBitmap[ IMAGE_CENTER_RAW ] != NULL );
      
      m_arpBitmap[ IMAGE_LEFT_RAW ] = 
	 new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 32, false, (unsigned char*)NULL );
      ASSERT( m_arpBitmap[ IMAGE_LEFT_RAW ] != NULL );
      
   }
   
   //
   //  Rectified Images.
   //
   m_arpBitmap[ IMAGE_RIGHT_RECTIFIED ] = 
      new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 8 );
   ASSERT( m_arpBitmap[ IMAGE_RIGHT_RECTIFIED ] != NULL );
   
   m_arpBitmap[ IMAGE_CENTER_RECTIFIED ] = 
      new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 8 );
   ASSERT( m_arpBitmap[ IMAGE_CENTER_RECTIFIED ] != NULL );
   
   m_arpBitmap[ IMAGE_LEFT_RECTIFIED ] = 
      new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 8 );
   ASSERT( m_arpBitmap[ IMAGE_LEFT_RECTIFIED ] != NULL );
   
   //
   //  Color rectified Images.
   //
   m_arpBitmap[ IMAGE_RIGHT_COLOR_RECTIFIED ] = 
      new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 32 );
   ASSERT( m_arpBitmap[ IMAGE_RIGHT_COLOR_RECTIFIED ] != NULL );
   
   m_arpBitmap[ IMAGE_CENTER_COLOR_RECTIFIED ] = 
      new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 32 );
   ASSERT( m_arpBitmap[ IMAGE_CENTER_COLOR_RECTIFIED ] != NULL );
   
   m_arpBitmap[ IMAGE_LEFT_COLOR_RECTIFIED ] = 
      new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 32 );
   ASSERT( m_arpBitmap[ IMAGE_LEFT_COLOR_RECTIFIED ] != NULL );

   
   //
   //  Edge Images
   //
   m_arpBitmap[ IMAGE_RIGHT_EDGE ] = 
      new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 8 );
   ASSERT( m_arpBitmap[ IMAGE_RIGHT_EDGE ] != NULL );
   
   m_arpBitmap[ IMAGE_CENTER_EDGE ] = 
      new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 8 );
   ASSERT( m_arpBitmap[ IMAGE_CENTER_EDGE ] != NULL );
   
   m_arpBitmap[ IMAGE_LEFT_EDGE ] = 
      new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 8 );
   ASSERT( m_arpBitmap[ IMAGE_LEFT_EDGE ] != NULL );
   
   //
   // The disparity image
   //
   m_arpBitmap[ IMAGE_DISPARITY ] = new PGRBitmapTriclops( m_nMaxCols, m_nMaxRows, 32 );
   ASSERT( m_arpBitmap[ IMAGE_DISPARITY ] != NULL );
}


BOOL
CPGRStereoDoc::processFrame()
{
   if ( !m_bOffline )
   {
      extractRawStereoImages();
   }

   if ( m_bImageAvailable )
   {      
      //
      // Now that we have some images, do the actual stereo.
      //
      doStereo();

      //
      // Generate stats and update the status bar.
      //
      doFrameStats();
      
      //
      // Call inherited class custom process
      //
      customProcess();
      
      UpdateAllViews( NULL );
   }
   
   return TRUE;
}


BOOL	 
CPGRStereoDoc::flycaptureGrab()
{
   FlyCaptureError fe;
   
   if( m_bStereoDialogActive )
   {
      m_dialogStereo.UpdateData( TRUE );
   }

   //
   // This method is called from App::OnIdle() whether we're grabbing or not,
   // so just return if the user requests that we not grab.
   // Also return true if this document is offline - working with images
   // loaded from a file
   //
   if( !m_dialogStereo.m_bGrab || m_bOffline )
   {
      return TRUE;
   }
   
   // we should be initialized by now.
   if( !m_bCameraInitialized )
   {
      ASSERT( FALSE );
      return FALSE;
   }

   //
   // Grab stereo image
   //
   fe = flycaptureGrabImage2( m_flycaptureContext, &m_flycaptureImage );
   if( fe != FLYCAPTURE_OK )
   {
      ASSERT( FALSE );
      return FALSE;
   }

   bool bImageSizeChanged = false;

   //
   // Compare image sizes. If they are not the same, then we will need to 
   // regenerate the image buffers
   //
   if ( m_nCurrCols != m_flycaptureImage.iCols )
   {
      m_nCurrCols = m_flycaptureImage.iCols;	
      bImageSizeChanged = true;
   }

   if ( m_nCurrRows != m_flycaptureImage.iRows )
   {
      m_nCurrRows = m_flycaptureImage.iRows;
      bImageSizeChanged = true;
   }
   
   //
   // Create the data buffers here
   //
   if ( bImageSizeChanged )
   {
      if ( m_pRedRaw != NULL && m_pGreenRaw != NULL && m_pBlueRaw != NULL )
      {
	 delete m_pRedRaw;
      } 
      
      if ( m_pRedProc != NULL && m_pGreenProc != NULL && m_pBlueProc != NULL )
      {
	 delete m_pRedProc;
      } 
      
      int iDimensions = m_flycaptureImage.iRowInc * m_flycaptureImage.iRows;
      
      m_pRedRaw = new unsigned char[iDimensions];
      m_pGreenRaw = NULL;
      m_pBlueRaw = NULL;
      
      m_pRedProc = new unsigned char[iDimensions * 4];
      m_pGreenProc = NULL;
      m_pBlueProc = NULL;
   }   
   
   return TRUE;
}


void
CPGRStereoDoc::extractRawImagesFromTriclopsInput( TriclopsInput input )
{
   // if images have already been loaded from a file, we should delete
   // them to prevent a memory leak
   if ( m_bImageAvailable )
   {
      freeInput( &m_tiStereo );
   } 
   //
   // set the document's stereo input to the one provided
   // note - the input contains pointers to buffers - obviously care
   // must be taken that this document never deletes those buffers
   // while they are in use
   //
   m_tiStereo = input;

   // 
   // At this point the demo cannot handle loading color images.
   //
   m_bColorData  = FALSE;
   
   if ( input.inputType != TriInp_RGB )
   {
      // note: this is bad - it should be a TriInp_RGB so we can view the
      // raw images easily.
      ASSERT( FALSE );
      return;
   }

   //
   // This mapping will make the stereo raw images available but not color ones.
   //
   // All raw images are set to be the red channel, and it is assumed that
   // it actually holds a side-by-side image set.
   //
   m_arpBitmap[ IMAGE_RIGHT_RAW ]->setBitmap( 
      input.rowinc, input.nrows, 8, (unsigned char*) input.u.rgb.red );
   
   m_arpBitmap[ IMAGE_LEFT_RAW ]->setBitmap( 
      input.rowinc, input.nrows, 8, (unsigned char*) input.u.rgb.red );

   m_arpBitmap[ IMAGE_CENTER_RAW ]->setBitmap( 
      input.rowinc, input.nrows, 8, (unsigned char*) input.u.rgb.red );

   // we know have an image to process
   m_bImageAvailable = TRUE;
   
   return;

}

void	 
CPGRStereoDoc::extractRawStereoImages()
{
   FlyCaptureError fe;
   TriclopsError te;

   // Camera is not initialized - can't do this
   if ( !m_bCameraInitialized )
   {
      return;
   }

   if ( !m_bImageAvailable )
   {
      // this must be the first time calling this - just
      // to be extra safe, lets make sure that flycaptureGrab
      // has been called before we try to extract the image
      if ( !flycaptureGrab() )
      {
	 // can't grab an image yet
	 return;
      }
   }

   //
   // Extract raw images
   //

   FlyCaptureImage pImageMono;
   FlyCaptureImage pImageColor;

   pImageMono.pData = m_pRedRaw;
   pImageColor.pData = m_pRedProc;

   fe = flycapturePrepareStereoImage(m_flycaptureContext, m_flycaptureImage, &pImageMono, &pImageColor );
   
   if (m_flycaptureImage.iNumImages == 2)
   {
      m_pGreenRaw = m_pRedRaw + m_flycaptureImage.iCols;
      m_pBlueRaw = m_pRedRaw + m_flycaptureImage.iCols;
      
      m_pGreenProc = m_pRedProc + m_flycaptureImage.iCols*4;
      m_pBlueProc = m_pRedProc + m_flycaptureImage.iCols*4;
   }
   if (m_flycaptureImage.iNumImages == 3)
   {
      m_pGreenRaw = m_pRedRaw + m_flycaptureImage.iCols;
      m_pBlueRaw = m_pRedRaw + (2*m_flycaptureImage.iCols);
      
      m_pGreenProc = m_pRedProc + m_flycaptureImage.iCols*4;
      m_pBlueProc = m_pRedProc + (2*m_flycaptureImage.iCols*4);
   }

   //
   // Build the TriclopsInputs
   //

   // Use this to grab the actual side-by-side raw image.
   // This has to be done because the IMAGE_RIGHT_RAW and LEFT_RAW can not
   // have the dual images since they feed the color rectification which
   // must show only a single camera.  This way, we store a seperate
   // side by side image which is used to display to the screen when using raw.
   te = triclopsBuildPackedTriclopsInput(
      m_flycaptureImage.iCols*m_flycaptureImage.iNumImages, 
      m_flycaptureImage.iRows, 
      m_flycaptureImage.iCols * 4 * m_flycaptureImage.iNumImages, 
      m_flycaptureImage.timeStamp.ulSeconds, 
      m_flycaptureImage.timeStamp.ulMicroSeconds, 
      m_pRedProc, 
      &m_tiRawColorImages[3] ); 
   
   ASSERT( te == TriclopsErrorOk );
   
   // Save the raw image from the right camera
   te = triclopsBuildPackedTriclopsInput(
      m_flycaptureImage.iCols, 
      m_flycaptureImage.iRows, 
      m_flycaptureImage.iCols * 4 * m_flycaptureImage.iNumImages, 
      m_flycaptureImage.timeStamp.ulSeconds, 
      m_flycaptureImage.timeStamp.ulMicroSeconds, 
      m_pRedProc, 
      &m_tiRawColorImages[IMAGE_RIGHT_RAW] );  

   ASSERT( te == TriclopsErrorOk );

   // Save the raw image from the center camera if using 3cam mode.
   // Note that if using an XB3 in 2Cam mode, the images are still
   // left and right, regardless of the baseline (narrow or wide)
   // that we are using.
   te = triclopsBuildPackedTriclopsInput(
      m_flycaptureImage.iCols, 
      m_flycaptureImage.iRows, 
      m_flycaptureImage.iCols * 4 * m_flycaptureImage.iNumImages, 
      m_flycaptureImage.timeStamp.ulSeconds, 
      m_flycaptureImage.timeStamp.ulMicroSeconds, 
      m_pBlueProc, 
      &m_tiRawColorImages[IMAGE_CENTER_RAW] );   
   
   ASSERT( te == TriclopsErrorOk );
   
   // Save the raw image from the left camera
   te = triclopsBuildPackedTriclopsInput(
	 m_flycaptureImage.iCols, 
	 m_flycaptureImage.iRows, 
	 m_flycaptureImage.iCols * 4 * m_flycaptureImage.iNumImages, 
	 m_flycaptureImage.timeStamp.ulSeconds, 
	 m_flycaptureImage.timeStamp.ulMicroSeconds, 
	 m_pGreenProc, 
	 &m_tiRawColorImages[IMAGE_LEFT_RAW] );  

   ASSERT( te == TriclopsErrorOk );
   
   // Build up a stereo triclops input which stores the the 3 seperate images
   // from the cameras.  Note that m_pRedRaw points to the beginning of the
   // side-by-side images, while m_pGreenRaw and m_pBlueRaw are pointers to the
   // start of their respective images.  These stereo images are used for rectification,
   // edge sensing, and disparity images.
   te = triclopsBuildRGBTriclopsInput(
	 m_flycaptureImage.iCols, 
	 m_flycaptureImage.iRows, 
	 m_flycaptureImage.iRowInc,
	 m_flycaptureImage.timeStamp.ulSeconds, 
	 m_flycaptureImage.timeStamp.ulMicroSeconds, 
	 m_pRedRaw, 
	 m_pGreenRaw, 
	 m_pBlueRaw, 
	 &m_tiStereo );

   ASSERT( te == TriclopsErrorOk );

   // the m_arpBitmap array contains the images which will be drawn to the screen.
   // When looking at Raw images, we want the stereo (side-by-side) images.
   m_arpBitmap[ IMAGE_RIGHT_RAW ]->setBitmapFromTriclopsInput( &m_tiRawColorImages[3] );
   m_arpBitmap[ IMAGE_LEFT_RAW ]->setBitmapFromTriclopsInput( &m_tiRawColorImages[3] );
   m_arpBitmap[ IMAGE_CENTER_RAW ]->setBitmapFromTriclopsInput( &m_tiRawColorImages[3] );

   m_bImageAvailable = TRUE;
}


void	 
CPGRStereoDoc::doStereo()
{
   TriclopsError  te;

   //
   // Since all triclopsStereo() calls will fail until the user resets
   // the ROI, we only want to display the warning once.
   //
   static bool bWarned = false;   
   
   ASSERT( m_triclopsContext != NULL );
   
   if ( m_dialogStereo.m_bPreprocess || m_dialogStereo.m_bStereo )
   {
      te = ::triclopsRectify( m_triclopsContext, &m_tiStereo );
      ASSERT( te == TriclopsErrorOk );
   }
   
   if( m_dialogStereo.m_bStereo )
   {
      //
      // note: order is important here.
      //      
      te = triclopsStereo( m_triclopsContext );

      //
      // (myk) this stereo call might fail with TriclopsErrorInvalidROI, which
      // means that the image size, the max disparity, and the stereo mask size
      // are too great and the kernel has run out of image.  Warn the user
      // once and return silently.
      //
      if( te == TriclopsErrorInvalidROI )
      {
	 if( !bWarned )
	 {
	    MessageBox( 
	       0,
	       "Triclops Stereo Library reported Invalid Region Of Interest. "
		  "Please adjust Max Disparity and/or Mask Size.",
	       "Warning", 
	       0 );

	    bWarned = true;	    
	 }
      }
      else if ( te != TriclopsErrorOk )
      {
	 ASSERT( FALSE );
      }
      else
      {
	 // success.

	 //
	 // We will warn the user again next time.
	 //
	 bWarned = false;

	 //
	 // we need to remap the depth image with nicer, more
	 // easily viewable greyscales and with colors to represent
	 // the invalid pixels
	 //
	 remapDisparityImage();
	 	 
	 // process color data if this is a color camera and either we
	 // are displaying a color view or we are displaying color points
	 if( m_bColorData && 
	    (m_nRectifyColorViews > 0 || 
	     (m_dialogStereo.m_bPoints && m_dialogStereo.m_bDrawColorPoints) ) )
	 {
	    processColorData();
	 }

	 // Process the Recified and Edge images to update the bitmap array
	 // with the latest grabbed image.
	 processRectifiedEdgeImages();
	 
	 if( m_dialogStereo.m_bPoints )
	 {
	    computePoints();
	 }
      }
   }
}


void
CPGRStereoDoc::doFrameStats()
{
   CPGRMainFrame* pFrame = (CPGRMainFrame*)AfxGetApp()->m_pMainWnd;

   char	 pszText[ 256 ];

   //
   // Update framerate.
   //
   static struct _timeb  timeLast;
   static struct _timeb  timeGrab;

   static bool bFirst = true;

   if( bFirst )
   {
      ::_ftime( &timeLast );
      ::_ftime( &timeGrab );

      bFirst = false;
   }
   else
   {
      ::_ftime( &timeGrab );

      double dGrabTime = 
	 ( timeGrab.time * 1000 + timeGrab.millitm ) - 
	 ( timeLast.time * 1000 + timeLast.millitm );

      // this fix ensures that the speeds are both sane and
      // never corrupted by a divide by zero.  Speeds can be
      // very fast when Grab and Stereo check boxes are turned 
      // off in the stereo dialog
      if ( dGrabTime < 1.0 )
	 dGrabTime = 1.0; // minimum 1 millisecond


      //
      // Update the framerate using a running average.
      //
      m_dFrameRate = 0.90 * m_dFrameRate + 100.0 / dGrabTime;
      
      timeLast = timeGrab;


      if ( isDocumentActive() )
      {
	 sprintf( pszText, "%2.1f Hz", m_dFrameRate );
	 pFrame->updateStatusBar( CPGRMainFrame::FRAMERATE, pszText );
      }
   }


   if ( isDocumentActive() )
   {
      if ( m_bOffline )
      {
	 // offline - there is no  1394 bus time or image timestamp
	 pFrame->updateStatusBar( CPGRMainFrame::TIMESTAMP1394, "0" );
	 pFrame->updateStatusBar( CPGRMainFrame::TIMESTAMP, "0" );
      }
      else
      {
	 //
	 // Update status bar
	 //
	 sprintf( 
	    pszText, 
	    "%d,%d", 
	    m_flycaptureImage.timeStamp.ulCycleSeconds,
	    m_flycaptureImage.timeStamp.ulCycleCount );
	 
	 pFrame->updateStatusBar( CPGRMainFrame::TIMESTAMP1394, pszText );
	 
	 time_t timeSeconds = m_tiStereo.timeStamp.sec;
	 const char* pszTimeTemp = ::ctime( &timeSeconds );
	 
	 sprintf( 
	    pszText,
	    "%.19s.%hu %s",
	    pszTimeTemp,
	    m_tiStereo.timeStamp.u_sec,
	    &pszTimeTemp[ 20 ] );
	 
	 pFrame->updateStatusBar( CPGRMainFrame::TIMESTAMP, pszText );
      }
   }

}


void	  
CPGRStereoDoc::customProcess()
{
   // empty - overridden by base class.
}


void	  
CPGRStereoDoc::customInit()
{
   // empty - overridden by base class.
}


void
CPGRStereoDoc::processColorData()
{
   TriclopsError	    te;
   TriclopsPackedColorImage colorImage;
   BOOL			    bError = false;

   te = ::triclopsSetPackedColorImageBuffer( 
      m_triclopsContext,
      TriCam_REFERENCE,
      (TriclopsPackedColorPixel*)m_arpBitmap[ IMAGE_RIGHT_COLOR_RECTIFIED ]->getDataPointer() );
   
   te = ::triclopsRectifyPackedColorImage( 
      m_triclopsContext, 
      TriCam_REFERENCE, 
      &m_tiRawColorImages[IMAGE_RIGHT_RAW], 
      &colorImage );
   
   if ( te != TriclopsErrorOk )
      bError = true;

   te = ::triclopsSetPackedColorImageBuffer( 
      m_triclopsContext,
      TriCam_LEFT,
      (TriclopsPackedColorPixel*)m_arpBitmap[ IMAGE_LEFT_COLOR_RECTIFIED ]->getDataPointer() );
   
   te = ::triclopsRectifyPackedColorImage( 
      m_triclopsContext, 
      TriCam_LEFT, 
      &m_tiRawColorImages[IMAGE_LEFT_RAW], 
      &colorImage );
   
   if ( !m_bIsBumblebee )
   {
      if ( te != TriclopsErrorOk )
	 bError = true;
      
      te = ::triclopsSetPackedColorImageBuffer( 
	 m_triclopsContext,
	 TriCam_TOP,
	 (TriclopsPackedColorPixel*)m_arpBitmap[ IMAGE_CENTER_COLOR_RECTIFIED ]->getDataPointer() );
      
      te = ::triclopsRectifyPackedColorImage( 
	 m_triclopsContext, TriCam_TOP, &m_tiRawColorImages[IMAGE_CENTER_RAW], &colorImage );
      
      if ( te != TriclopsErrorOk )
	 bError = true;
   }

   if( bError )
   {
      static bool bWarnedOnce = false;
      if ( !bWarnedOnce )
      {
	 MessageBox( 0, "Error rectifying color images!\n"
	    "Color images\n"
	    "will not be available\n",
	    "Warning", 0 );
	 bWarnedOnce   = true;
      }
   }

}


void 
CPGRStereoDoc::remapDisparityImage()
{
   TriclopsError     te;
   TriclopsBool	     bSubpixelOn;

   te = triclopsGetSubpixelInterpolation( m_triclopsContext, &bSubpixelOn );
   ASSERT( te == TriclopsErrorOk );

   // if mapping parameters have not changed, this function returns
   // without regenerating the tables
   generateLookupTables();


   if ( bSubpixelOn )
   {
      TriclopsImage16   imageDisparity;
      // get the 16 bit depth image
      te = triclopsGetImage16( 
         m_triclopsContext, TriImg16_DISPARITY, TriCam_REFERENCE, &imageDisparity );
      if ( te != TriclopsErrorOk )
      {
	 ASSERT( false );
         return;
      }
   
      unsigned short* pRow    = imageDisparity.data;
      int iPixelInc	      = imageDisparity.rowinc/2;
      
      unsigned char*  pucDisp = m_arpBitmap[ IMAGE_DISPARITY ]->getDataPointer();
      
      for ( int r = 0, i = 0; r < imageDisparity.nrows; r++ )
      {
	 for ( int c = 0; c < imageDisparity.ncols; c++ )
	 {
	    if (pRow[c] >= 0xFF00)
	    {
	       // for an invalid pixel, only take the bottom 8 bits
	       int index = 0x00FF & pRow[c];
	       
	       pucDisp[ i++ ] = m_ucInvalidDisparityMapLUT[index][0]; // blue
	       pucDisp[ i++ ] = m_ucInvalidDisparityMapLUT[index][1]; // green
	       pucDisp[ i++ ] = m_ucInvalidDisparityMapLUT[index][2]; // red
	       i++; // unused
	    }
	    else
	    {
	       int index = pRow[c] >> DISPARITY_LUT_SHIFT_BITS;
	       pucDisp[ i++ ] = m_ucValidDisparityMapLUT[index][0]; // blue
	       pucDisp[ i++ ] = m_ucValidDisparityMapLUT[index][1]; // green
	       pucDisp[ i++ ] = m_ucValidDisparityMapLUT[index][2]; // red
	       i++; // unused
	    }
	 }
	 pRow   += iPixelInc;
      }
   }
   else // if ( bSubpixelOn )
   {
      // subpixel is not on - lets remap with the 8 bit disparity image
      TriclopsImage   imageDisparity;
      // get the 8 bit depth image
      te = triclopsGetImage( 
         m_triclopsContext, TriImg_DISPARITY, TriCam_REFERENCE, &imageDisparity );
      if ( te != TriclopsErrorOk )
      {
	 ASSERT( false );
         return;
      }
   
      unsigned char* pRow = imageDisparity.data;
      int iPixelInc	  = imageDisparity.rowinc;
 
      unsigned char*  pucDisp = m_arpBitmap[ IMAGE_DISPARITY ]->getDataPointer();
      for ( int r = 0, i = 0; r < imageDisparity.nrows; r++ )
      {
	 for ( int c = 0; c < imageDisparity.ncols; c++ )
	 {
	    if (pRow[c] > 239)
	    {
	       // invalid pixel
	       int index = pRow[c];
	       pucDisp[ i++ ] = m_ucInvalidDisparityMapLUT[index][0]; // red
	       pucDisp[ i++ ] = m_ucInvalidDisparityMapLUT[index][1]; // green
	       pucDisp[ i++ ] = m_ucInvalidDisparityMapLUT[index][2]; // blue
	       i++; // unused
	    }
	    else
	    {
	       // valid pixel
	       int index = pRow[c] << (8 - DISPARITY_LUT_SHIFT_BITS);
	       pucDisp[ i++ ] = m_ucValidDisparityMapLUT[index][0]; // red
	       pucDisp[ i++ ] = m_ucValidDisparityMapLUT[index][1]; // green
	       pucDisp[ i++ ] = m_ucValidDisparityMapLUT[index][2]; // blue
	       i++; // unused
	    }
	 }
	 pRow   += iPixelInc;
      }
 
   }
}


void	  
CPGRStereoDoc::computePoints()
{
   TriclopsError	err;
   C3dColourPointRC*	pPoint;
   
   int	 iRow;
   int	 iCol;
   float fX;
   float fY;
   float fZ;

   // empty our point cloud for this computation
   ASSERT( m_pPointCloud != NULL );
   m_pPointCloud->empty();
   
   int iNumberOfPoints = 0;
   
   TriclopsBool   bSubpixelInterpolation;
   triclopsGetSubpixelInterpolation( m_triclopsContext, &bSubpixelInterpolation );
   
   // For speed, loops below are duplicated for colour/b&w cases
   if( m_bColorData )
   {
      // retrieve the colour rectified image for point colour assignment
      RGBQUAD* pImage = (RGBQUAD*)m_arpBitmap[ IMAGE_RIGHT_COLOR_RECTIFIED ]->getDataPointer();

      if( bSubpixelInterpolation )
      {
	 // retrieve the interpolated depth image from the context
	 TriclopsImage16   imageDepth;
	 triclopsGetImage16( 
	    m_triclopsContext, TriImg16_DISPARITY, TriCam_REFERENCE, &imageDepth );
	 
	 unsigned short* pRow = imageDepth.data;
	 int iPixelInc = imageDepth.rowinc / 2;
	 
	 for ( iRow = 0; iRow < imageDepth.nrows; iRow++ )
	 {          
	    for ( iCol = 0; iCol < imageDepth.ncols; iCol++ )
	    {
	       pPoint = &m_pStaticPointArray[ iNumberOfPoints++ ];
	       if ( pRow[ iCol ] < 0xFF00 ) 
	       {
		  err = triclopsRCD16ToXYZ( m_triclopsContext, iRow, iCol, pRow[ iCol ], 
					    &fX, &fY, &fZ );	 
		  if( err == TriclopsErrorOk )
		  {
		     pPoint->setXYZ( fX, fY, fZ );

                     pPoint->setColour( pImage );
		     		     
                     pPoint->setColRow( iCol, iRow );
		     m_pPointCloud->insertPoint( pPoint );
		  }
		  else
		  {
		     // set invalid point (for triangulation)
                     pPoint->setColRow( -1, -1 );
		  }
	       }
	       else
	       {
		  // set invalid point (for triangulation)
                  pPoint->setColRow( -1, -1 );
	       }
	       
	       // increment rectified colour image pointer to the next RGBQUAD.
	       pImage++;
	    }
	    pRow += iPixelInc;
	 }
      }  // if( bSubpixelInterpolation )
      else
      {
	 TriclopsImage  imageDepth;
	 unsigned char* pRow;
	 
	 // retrieve the interpolated depth image from the context
	 triclopsGetImage( m_triclopsContext, TriImg_DISPARITY, TriCam_REFERENCE, &imageDepth );
	 
	 pRow = imageDepth.data;
	 
	 for ( int iRow = 0; iRow < imageDepth.nrows; iRow++ )
	 {          
	    for ( int iCol = 0; iCol < imageDepth.ncols; iCol++ )
	    {
	       pPoint = &m_pStaticPointArray[ iNumberOfPoints++ ];
	       if ( pRow[ iCol ] < 127)
	       {
		  err = triclopsRCD8ToXYZ( m_triclopsContext, iRow, iCol, pRow[ iCol ], &fX, &fY, &fZ );
		  if( err == TriclopsErrorOk )
		  {
		     pPoint->setXYZ( fX, fY, fZ );
		     
                     pPoint->setColour( pImage );
		     
                     pPoint->setColRow( iCol, iRow );
		     m_pPointCloud->insertPoint( pPoint );
		  }
		  else
		  {
		     // set invalid point (for triangulation)
                     pPoint->setColRow( -1, -1 );
		  }
	       }
	       else
	       {
		  // set invalid point (for triangulation)
                  pPoint->setColRow( -1, -1 );
	       }
	       // increment rectified colour image pointer to the next RGBQUAD.
	       pImage++;
	    }
	    pRow += imageDepth.rowinc;
	 }
      } // if !( bSubpixelInterpolation )
   }
   else
   {
      unsigned char* pucImage =
	 (unsigned char*)m_arpBitmap[ IMAGE_RIGHT_RECTIFIED ]->getDataPointer();

      if( bSubpixelInterpolation )
      {
	 // retrieve the interpolated depth image from the context
	 TriclopsImage16   imageDepth;
	 triclopsGetImage16( m_triclopsContext, TriImg16_DISPARITY, TriCam_REFERENCE, &imageDepth );

	 unsigned short* pRow = imageDepth.data;
	 int iPixelInc = imageDepth.rowinc / 2;
	 
	 for ( iRow = 0; iRow < imageDepth.nrows; iRow++ )
	 {          
	    for ( iCol = 0; iCol < imageDepth.ncols; iCol++ )
	    {
	       pPoint = &m_pStaticPointArray[ iNumberOfPoints++ ];
	       if ( pRow[ iCol ] < 0xFF00)
	       {
		  err = triclopsRCD16ToXYZ( m_triclopsContext, iRow, iCol, pRow[ iCol ], 
					    &fX, &fY, &fZ );	 
		  if( err == TriclopsErrorOk )
		  {
		     pPoint->setXYZ( fX, fY, fZ );

                     RGBQUAD colour;
                     colour.rgbBlue = *pucImage;
                     colour.rgbGreen = *pucImage;
                     colour.rgbRed = *pucImage;

                     pPoint->setColour( &colour );
		     
                     pPoint->setColRow( iCol, iRow );
		     m_pPointCloud->insertPoint( pPoint );
		  }
		  else
		  {
		     // set invalid point (for triangulation)
		     pPoint->setColRow( -1, -1 );
		  }
	       }
	       else
	       {
		  // set invalid point (for triangulation)
                  pPoint->setColRow( -1, -1 );
	       }
	       // increment rectified colour image pointer to the next RGBQUAD.
	       pucImage++;
	    }
	    pRow += iPixelInc;
	 }
      }  // if( bSubpixelInterpolation )
      else
      {
	 TriclopsImage  imageDepth;
	 unsigned char* pRow;
	 
	 // retrieve the interpolated depth image from the context
	 triclopsGetImage( m_triclopsContext, TriImg_DISPARITY, TriCam_REFERENCE, &imageDepth );
	 
	 pRow = imageDepth.data;
	 
	 for ( int iRow = 0; iRow < imageDepth.nrows; iRow++ )
	 {          
	    for ( int iCol = 0; iCol < imageDepth.ncols; iCol++ )
	    {
	       pPoint = &m_pStaticPointArray[ iNumberOfPoints++ ];
	       if ( pRow[ iCol ] < 127)
	       {
		  err = triclopsRCD8ToXYZ( m_triclopsContext, iRow, iCol, pRow[ iCol ], &fX, &fY, &fZ );
		  if( err == TriclopsErrorOk )
		  {
		     pPoint->setXYZ( fX, fY, fZ );
		     
                     RGBQUAD colour;
                     colour.rgbBlue = *pucImage;
                     colour.rgbGreen = *pucImage;
                     colour.rgbRed = *pucImage;

                     pPoint->setColour( &colour );
		     
                     pPoint->setColRow( iCol, iRow );
		     m_pPointCloud->insertPoint( pPoint );
		  }
		  else
		  {
		     // set invalid point (for triangulation)
                     pPoint->setColRow( -1, -1 );
		  }
	       }
	       else
	       {
		  // set invalid point (for triangulation)
		  pPoint->setColRow( -1, -1 );
	       }
	       // increment rectified colour image pointer to the next RGBQUAD.
	       pucImage++;
	    }
	    pRow += imageDepth.rowinc;
	 }
      } // if !( bSubpixelInterpolation )
   }
}


BOOL
CPGRStereoDoc::resetImageResolutions( enum_RESOLUTION resolution )
{
   TriclopsError  te;
   
   //
   // Handle image buffers - we don't need to resize them since they're already
   // set at the maximum size (1280*960) but we do need to tell them they're
   // a different size so they can be drawn correctly..
   //
   int iWidth, iHeight;

   switch( resolution )
   {
   case RES_160x120:
      m_currentStereoResolution = STEREO_160x120;
      iWidth = 160;
      iHeight = 120;
      break;

   case RES_256x192:
      m_currentStereoResolution = STEREO_256x192;
      iWidth = 256;
      iHeight = 192;
      break;

   case RES_320x240:
      m_currentStereoResolution = STEREO_320x240;
      iWidth = 320;
      iHeight = 240;
      break;

   case RES_400x300:
      m_currentStereoResolution = STEREO_400x300;
      iWidth = 400;
      iHeight = 300;
      break;

   case RES_512x384:
      m_currentStereoResolution = STEREO_512x384;
      iWidth = 512;
      iHeight = 384;
      break;

   case RES_640x480:
      m_currentStereoResolution = STEREO_640x480;
      iWidth = 640;
      iHeight = 480;
      break;

   case RES_800x600:
      m_currentStereoResolution = STEREO_800x600;
      iWidth = 800;
      iHeight = 600;
      break;

   case RES_1024x768:
      m_currentStereoResolution = STEREO_1024x768;
      iWidth = 1024;
      iHeight = 768;
      break;
      
   case RES_1280x960:
      m_currentStereoResolution = STEREO_1280x960;
      iWidth = 1280;
      iHeight = 960;
      break;
      
   default:
      ASSERT( FALSE );
      m_currentStereoResolution = STEREO_640x480;
      iWidth = 640;
      iHeight = 480;
      break;
      
   }
   
   //
   // Set new resolutions for all of our images
   //
   // myk note - the PGRBitmap resolution of the raw images is set by
   // extractRawImages() or something like that.
   //
   for( int iImage = IMAGE_RIGHT_RECTIFIED; iImage < IMAGE_TOTAL; iImage++ )
   {
      m_arpBitmap[ iImage ]->setImageDimensions( iWidth, iHeight );
   }

   //
   // Set images for current context now.
   //
   switch( resolution )
   {
   case RES_160x120:
      te = triclopsSetResolution( m_triclopsContext, 120, 160 );
      break;

   case RES_256x192:
      te = triclopsSetResolution( m_triclopsContext, 192, 256 );
      break;
      
   case RES_320x240:
      te = triclopsSetResolution( m_triclopsContext, 240, 320 );
      break;

   case RES_400x300:
      te = triclopsSetResolution( m_triclopsContext, 300, 400 );
      break;

   case RES_512x384:
      te = triclopsSetResolution( m_triclopsContext, 384, 512 );
      break;
      
   case RES_640x480:
      te = triclopsSetResolution( m_triclopsContext, 480, 640 );
      break;

   case RES_800x600:
      te = triclopsSetResolution( m_triclopsContext, 600, 800 );
      break;
      
   case RES_1024x768:
      te = triclopsSetResolution( m_triclopsContext, 768, 1024 );
      break;
      
   case RES_1280x960:
      te = triclopsSetResolution( m_triclopsContext, 960, 1280 );
      break;
      
   default:
      ASSERT( FALSE );
      te = triclopsSetResolution( m_triclopsContext, 480, 640 );
      break;      
   }
   
   //
   // Set context for modification in dialog
   //      
   te = m_dialogStereo.setContext( &m_triclopsContext, iHeight, iWidth );

   if( te != TriclopsErrorOk )
   {
      ASSERT( FALSE );
   }

   //
   // Resize all views that are currently open
   //
   resizeAllViews();
   
   return TRUE;
}

void	  
CPGRStereoDoc::processRectifiedEdgeImages()
{

   TriclopsError te;


   //
   // Unset all the image buffers the rectified and edge image buffers
   //
   if( m_triclopsContext != NULL )
   {      
      // rectified image buffers
      te = triclopsUnsetImageBuffer( m_triclopsContext, TriImg_RECTIFIED, TriCam_RIGHT );
      te = triclopsUnsetImageBuffer( m_triclopsContext, TriImg_RECTIFIED, TriCam_TOP );
      te = triclopsUnsetImageBuffer( m_triclopsContext, TriImg_RECTIFIED, TriCam_LEFT );
      
      // edge image buffers
      te = triclopsUnsetImageBuffer( m_triclopsContext, TriImg_EDGE, TriCam_RIGHT );
      te = triclopsUnsetImageBuffer( m_triclopsContext, TriImg_EDGE, TriCam_TOP );
      te = triclopsUnsetImageBuffer( m_triclopsContext, TriImg_EDGE, TriCam_LEFT );      
   }

   //
   // We don't check errors here because we might be getting InvalidCamera
   // errors for bumblebees.
   //
   
   // rectified image buffers
   te = triclopsSetImageBuffer( 
      m_triclopsContext, 
      m_arpBitmap[ IMAGE_RIGHT_RECTIFIED ]->getDataPointer(),
      TriImg_RECTIFIED, 
      TriCam_RIGHT );
   
   te = triclopsSetImageBuffer( 
      m_triclopsContext, 
      m_arpBitmap[ IMAGE_CENTER_RECTIFIED ]->getDataPointer(),
      TriImg_RECTIFIED, 
      TriCam_TOP );
   
   te = triclopsSetImageBuffer( 
      m_triclopsContext, 
      m_arpBitmap[ IMAGE_LEFT_RECTIFIED ]->getDataPointer(),
      TriImg_RECTIFIED, 
      TriCam_LEFT );
   
   //  no colour rectified buffer - it's generated from the planar image
   //  from rectifyColourImage().  
   
   
   // edge image buffer
   te = triclopsSetImageBuffer( 
      m_triclopsContext, 
      m_arpBitmap[ IMAGE_RIGHT_EDGE ]->getDataPointer(),
      TriImg_EDGE, 
      TriCam_RIGHT );
   
   te = triclopsSetImageBuffer( 
      m_triclopsContext, 
      m_arpBitmap[ IMAGE_CENTER_EDGE ]->getDataPointer(),
      TriImg_EDGE, 
      TriCam_TOP );
   
   te = triclopsSetImageBuffer( 
      m_triclopsContext, 
      m_arpBitmap[ IMAGE_LEFT_EDGE ]->getDataPointer(),
      TriImg_EDGE, 
      TriCam_LEFT );
}

BOOL
CPGRStereoDoc::initGrab()
{
   FlyCaptureError fe;
   CameraGUIError    guierror;   
   

   BOOL	 bSuccess = true;
   
   m_bCameraInitialized = FALSE;

   unsigned long  ulSerialNumber;
   INT_PTR	  iDialogStatus;
      
   guierror = ::pgrcamguiCreateContext( &m_guicontext );
   if( guierror != PGRCAMGUI_OK )
   {
      // Remove for now until we can include PGRError.h
//      PGR_ERROR_MESSAGE( "Error creating GUI context." );
      bSuccess = FALSE;
      goto Failed;
   }

   guierror = ::pgrcamguiShowCameraSelectionModal(
      m_guicontext, m_flycaptureContext, &ulSerialNumber, &iDialogStatus );
   if( guierror != PGRCAMGUI_OK )
   {
      // Remove for now until we can include PGRError.h
      //PGR_ERROR_MESSAGE( "Error showing camera selection dialog." );
      bSuccess = FALSE;
      goto Failed;
   }

   if( ulSerialNumber != 0 )
   {
      //
      // Check if any other documents are using the selected camera.
      //
      if( ((CPGRStereoApp*)AfxGetApp())->isCameraAlreadyOpen( ulSerialNumber ) )
      {
	 // Remove for now until we can include PGRError.h
	 /*
         CString csMsg;
         csMsg.Format( 
            "Camera with serial number %d is already in use.", ulSerialNumber );
         PGR_ERROR_MESSAGE( csMsg );
	 */
         bSuccess = FALSE;
         goto Failed;
      }

      //
      // Attempt to initialize the selected camera.
      //
      fe = ::flycaptureInitializeFromSerialNumber( 
	 m_flycaptureContext, ulSerialNumber );
      if ( fe != FLYCAPTURE_OK )
      {
	 // Remove for now until we can include PGRError.h
	 /*
	 char  pszError[ 256 ];
	 sprintf( 
	    pszError,
	    "flycaptureInitializeFromSerialNumber() reported \"%s\"", 
	    ::flycaptureErrorToString( fe ) );
	 PGR_ERROR_MESSAGE3( pszError, fe, CRITICAL_MESSAGES );
	 */
	 
	 bSuccess = FALSE;
	 goto Failed;
      }
   }
   else
   {
      //
      // User hit 'cancel' in the dialog - this can be checked with
      // iDialogStatus.
      //
      bSuccess = FALSE;
      goto Failed;
   }

   m_serialNumber = ulSerialNumber;

   //
   // Create settings dialog
   //
   guierror = pgrcamguiInitializeSettingsDialog(
      m_guicontext,
      (GenericCameraContext)m_flycaptureContext);
   if( guierror != PGRCAMGUI_OK )
   {
      // Remove for now until we can include PGRError.h
      //PGR_ERROR_MESSAGE( "Error creating settings dialog." );
   }

   //
   // Find out if we're using a colour or b/w camera
   //
   fe = flycaptureGetCameraInfo( m_flycaptureContext, &m_flycaptureInfo );
   if( fe != FLYCAPTURE_OK )
   {
      static bool bWarnedOnce = false;
      if ( !bWarnedOnce )
      { 
	 AfxMessageBox( "Could not read camera information." );
	 bWarnedOnce = true; 
      }
      bSuccess = FALSE;
      goto Failed;
   }

   if ( m_flycaptureInfo.CameraType == FLYCAPTURE_COLOR )
   {
      m_bColorData = TRUE;
   }
   else
   {
      m_bColorData = FALSE;
   }   

   FlyCaptureVideoMode  pVideoMode;
   FlyCaptureFrameRate  pFrameRate;

   if ( flycaptureGetCurrentVideoMode( m_flycaptureContext, &pVideoMode, &pFrameRate ) != FLYCAPTURE_OK )
   {
      static bool bWarnedOnce = false;
      if ( !bWarnedOnce )
      { 
	 AfxMessageBox( "Could not get videomode and frame rate!" ); 
	 bWarnedOnce = true; 
      }
      bSuccess = FALSE;
      goto Failed;
   }
   
   //
   // Depending on color or B&W camera, we need to start in the right mode
   //
   FlyCapturePixelFormat pPixelFormat;

   if ( m_flycaptureInfo.CameraModel == FLYCAPTURE_BUMBLEBEE ||
      m_flycaptureInfo.CameraModel ==  FLYCAPTURE_BUMBLEBEE2 )
   {
      // Bumblebee and Bumblebee2
      m_bColorData ? pPixelFormat = FLYCAPTURE_RAW16 : pPixelFormat = FLYCAPTURE_MONO16;
      
      unsigned long ulValue;
      flycaptureGetCameraRegister( m_flycaptureContext, 0x1F28, &ulValue );
      
      if ( ( ulValue & 0x2 ) == 0 )
      {
	 // Hi-res
	 fe = flycaptureStartCustomImage( m_flycaptureContext, 3, 0, 0, 1024, 768, 100.0, pPixelFormat );
      }
      else
      {
	 // Low-res
	 fe = flycaptureStartCustomImage( m_flycaptureContext, 3, 0, 0, 640, 480, 100.0, pPixelFormat );
      }
   }
   else
   {
      // Bumblebee XB3
      m_bColorData ? pPixelFormat = FLYCAPTURE_RAW16 : pPixelFormat = FLYCAPTURE_MONO16;
      fe = flycaptureStartCustomImage( m_flycaptureContext, 3, 0, 0, 1280, 960, 100.0, pPixelFormat );
   }

   if( fe != FLYCAPTURE_OK )
   {
      static bool bWarnedOnce = false;
      if ( !bWarnedOnce )
      { 
	 AfxMessageBox( "Could not start camera" );
	 bWarnedOnce = true;
      }
      bSuccess = FALSE;
      goto Failed;
   }

   m_bCameraInitialized = TRUE;
   
Failed:

   return bSuccess;
}


void
CPGRStereoDoc::closeGrab()
{
   //
   // Destroy gui context.
   //
   if ( m_guicontext != NULL )
   {
      ::pgrcamguiDestroyContext( m_guicontext );
   }
   
   if ( m_bCameraInitialized )
   {
      flycaptureStop( m_flycaptureContext );
      flycaptureDestroyContext( m_flycaptureContext );
      m_bCameraInitialized = FALSE;
   }
}


void 
CPGRStereoDoc::resizeAllViews()
{
   POSITION pos = GetFirstViewPosition();
   
   while (pos != NULL)
   {
      CView* pView = STATIC_DOWNCAST( CView, GetNextView( pos ) );
      ASSERT_VALID( pView );
      
      if( pView->IsKindOf( RUNTIME_CLASS( CPGRImageView )) )
      {
	 ((CPGRImageView*)pView)->changeDisplay();
      }
   }
}


void 
CPGRStereoDoc::OnButtonStereoParams() 
{
   m_dialogStereo.AssertValid();
   
   if( m_bStereoDialogActive )
   {
      m_dialogStereo.DestroyWindow();
   }
   else
   {
      m_dialogStereo.Create();
      char szTitle[ 100 ];
      sprintf( szTitle, "Stereo Parameters: " );
      getCameraTitle( &szTitle[ strlen(szTitle) ] );
      m_dialogStereo.SetWindowText( szTitle );

      int iWidth = -1;
      int iHeight = -1;
      getResolution( getResolution(), &iHeight, &iWidth );
      m_dialogStereo.setContext( &m_triclopsContext, iHeight, iWidth );
   }
   
   m_bStereoDialogActive = !m_bStereoDialogActive;
}


void 
CPGRStereoDoc::OnButtonTransformation() 
{
   m_dialogTransform.AssertValid();
   
   if( m_bTransformDialogActive )
   {
      m_dialogTransform.DestroyWindow();
   }
   else
   {
      m_dialogTransform.Create();
      char szTitle[ 100 ];
      // set the title on the transformation control
      sprintf( szTitle, "Transformation: " );
      getCameraTitle( &szTitle[ strlen(szTitle) ] );

      m_dialogTransform.SetWindowText( szTitle );      
   }
   
   m_bTransformDialogActive = !m_bTransformDialogActive;
}


void 
CPGRStereoDoc::OnCheckCameraControl() 
{
   if ( m_guicontext )
   {
      ::pgrcamguiToggleSettingsWindowState(
	 m_guicontext, ::AfxGetApp()->m_pMainWnd->GetSafeHwnd() );
   }
}

void 
CPGRStereoDoc::OnUpdateCheckCameraControl(CCmdUI* pCmdUI) 
{
   BOOL bShowing;
   
   pgrcamguiGetSettingsWindowState( m_guicontext, &bShowing );
   pCmdUI->SetCheck( bShowing ? 1 : 0 );
}


void 
CPGRStereoDoc::OnComboResolution() 
{
   CPGRMainFrame* pFrame = (CPGRMainFrame*)AfxGetMainWnd();
   ASSERT_VALID( pFrame );
   pFrame->m_dialogBarStereo.AssertValid();

   // if the user has selected high-res (1024x768 or 1280x960) and it's not a 
   // high-res system, then reset the size to the current resolution.
   if ( ( pFrame->m_dialogBarStereo.getResolution() == STEREO_1024x768 || 
	  pFrame->m_dialogBarStereo.getResolution() == STEREO_1280x960 ) &&
	( m_maxStereoResolution != STEREO_1024x768 &&
	  m_maxStereoResolution != STEREO_1280x960 ) )
   {
      pFrame->m_dialogBarStereo.setResolution( m_currentStereoResolution );
      return;
   }
   
   m_currentStereoResolution = pFrame->m_dialogBarStereo.getResolution();
   
   switch( m_currentStereoResolution )
   {
   case STEREO_160x120:
      resetImageResolutions( RES_160x120 );
      break;

   case STEREO_256x192:
      resetImageResolutions( RES_256x192 );
      break;
      
   case STEREO_320x240:
      resetImageResolutions( RES_320x240 );
      break;

   case STEREO_400x300:
      resetImageResolutions( RES_400x300 );
      break;

   case STEREO_512x384:
      resetImageResolutions( RES_512x384 );
      break;
      
   case STEREO_640x480:
      resetImageResolutions( RES_640x480 );
      break;
      
   case STEREO_800x600:
      resetImageResolutions( RES_800x600 );
      break;

   case STEREO_1024x768:
      resetImageResolutions( RES_1024x768 );
      break;
      
   case STEREO_1280x960:
      resetImageResolutions( RES_1280x960 );
      break;

   default:
      ASSERT( FALSE );
      resetImageResolutions( RES_640x480 );      
   }   
}

inline void
disparityToTemperature(TriclopsContext,	  // may want to use this in the future...
		       unsigned int uiDisp16Bit, 
		       double dMinDisp,
		       double dMaxDisp,
		       double *pdRed,
		       double *pdGreen,
		       double *pdBlue )
{
   // note: context not used now but maybe later
   double dDisp = (double) uiDisp16Bit/256.0;

   if ( dDisp < dMinDisp )
   {
      dDisp = dMinDisp;
   }
   if ( dDisp > dMaxDisp )
   {
      dDisp = dMaxDisp;
   }

   double dRange = dMaxDisp - dMinDisp;


   // This code is a little more complicated than it should be, but it
   // does allow one to easily tweak the sizes of the different color
   // zones.
   // you can change the width of the red zone, blue zone and green zone 
   // by tweaking the dRed, dGreen and dBlue values
   double dRed	     = 1;
   double dGreen     = 1;
   double dBlue	     = 1;
   double dTotal     = dRed + dGreen + dBlue;
   double dNormDisp  = (dDisp-dMinDisp)/dRange;
   double dBGThresh  = dBlue/dTotal;
   double dGRThresh  = (dBlue+dGreen)/dTotal;

   if ( dNormDisp < dBGThresh  )
   {
      double dInBand = dNormDisp/dBGThresh;
      *pdRed   = 0;
      *pdGreen = 255*dInBand;
      *pdBlue  = 255;
   }
   else if ( dNormDisp < dGRThresh )
   {
      double dInBand = (dNormDisp-dBGThresh)/(dGRThresh-dBGThresh);
      *pdRed   = 255*dInBand;
      *pdGreen = 255;
      *pdBlue  = 255*(1-dInBand);
   }
   else
   {
      double dInBand = (dNormDisp-dGRThresh)/(1-dGRThresh);
      *pdRed   = 255;
      *pdGreen = 255*(1-dInBand);
      *pdBlue  = 0;
   }

   
   return;
}

inline void
makeColoredInvalidLUT(unsigned char ucLUT[DISPARITY_INVALID_LUT_ENTRIES][3],
		      unsigned char ucInvalidTexture,
		      unsigned char ucInvalidUniqueness,
		      unsigned char ucInvalidSurface,
		      unsigned char ucInvalidBackForth,
		      unsigned char ucInvalidSubpixel )
{
   for ( int i = 0; i < DISPARITY_INVALID_LUT_ENTRIES; i++)
   {
      if ( i == ucInvalidTexture )
      {
	 // red
	 ucLUT[i][0] = 0;
	 ucLUT[i][1] = 0;
	 ucLUT[i][2] = 255;
      }
      else if (i == ucInvalidUniqueness )
      {
	 // green
	 ucLUT[i][0] = 0;
	 ucLUT[i][1] = 255;
	 ucLUT[i][2] = 0;
      }
      else if ( i == ucInvalidSurface )
      {
	 // blue
	 ucLUT[i][0] = 255;
	 ucLUT[i][1] = 0;
	 ucLUT[i][2] = 0;
      }
      else if ( i == ucInvalidBackForth )
      {
	 // yellow
	 ucLUT[i][0] = 0;
	 ucLUT[i][1] = 255;
	 ucLUT[i][2] = 255;
      }
      else if ( i == ucInvalidSubpixel )
      {
	 // ugh, magenta
	 ucLUT[i][0] = 255;
	 ucLUT[i][1] = 0;
	 ucLUT[i][2] = 255;
      }
      else
      {
	 // these pixel should never appear
	 // ugh, cyan
	 ucLUT[i][0] = 255;
	 ucLUT[i][1] = 255;
	 ucLUT[i][2] = 0;
      }
   }
}

void
CPGRStereoDoc::generateLookupTables()
{
   static bool	  bCalledBefore	   = false;
   static bool	  bLastPseudoColor = false;
   static bool	  bLastColorInvalid= false;

   TriclopsError  error;
   int            nMinDisparity;
   int            nMaxDisparity;
   int            nDisparityOffset;
   int		  i;


   // get convenience variables from dialog
   // do we want to display as temperature?
   bool		  bPseudoColor	   = (m_dialogStereo.m_bHeatmap == TRUE);
   bool		  bColorInvalid	   = (m_dialogStereo.m_bInvalidColors == TRUE);
      
   error = triclopsGetDisparity( m_triclopsContext, &nMinDisparity, &nMaxDisparity );
   ASSERT( error == TriclopsErrorOk );
   error = triclopsGetDisparityOffset( m_triclopsContext, &nDisparityOffset );
   ASSERT( error == TriclopsErrorOk );


   // do not generate table if nothing has changed.
   if(( nMinDisparity-nDisparityOffset == m_nLUTMinDisp ) &&
      ( nMaxDisparity-nDisparityOffset == m_nLUTMaxDisp ) &&
      ( bLastPseudoColor == bPseudoColor ) &&
      ( bLastColorInvalid == bColorInvalid ) &&
      bCalledBefore )
   {
      return;
   }

   if ( bPseudoColor )
   {
      // we enforce that the coloring of invalid pixels doesn't happen 
      // if we are using the heatmap disparity display
      bColorInvalid = false;
   }

   bCalledBefore     = true;
   bLastPseudoColor  = bPseudoColor;
   bLastColorInvalid = bColorInvalid;


   // get the invalid pixel mappings
   unsigned char ucInvalidTexture;
   unsigned char ucInvalidUniqueness;
   unsigned char ucInvalidSurface;
   unsigned char ucInvalidBackForth;
   unsigned char ucInvalidSubpixel;
   
   triclopsGetTextureValidationMapping(    m_triclopsContext, &ucInvalidTexture );
   triclopsGetUniquenessValidationMapping( m_triclopsContext, &ucInvalidUniqueness );
   triclopsGetSurfaceValidationMapping(    m_triclopsContext, &ucInvalidSurface );
   triclopsGetSubpixelValidationMapping(   m_triclopsContext, &ucInvalidSubpixel );
   triclopsGetBackForthValidationMapping(  m_triclopsContext, &ucInvalidBackForth );

   if ( bPseudoColor )
   {
      
      // store values that we're dealing with so we don't compute next time.
      m_nLUTMinDisp     = nMinDisparity-nDisparityOffset;
      m_nLUTMaxDisp     = nMaxDisparity-nDisparityOffset;
      
      
      // handle valid pixels
      for( i = 0; i < DISPARITY_VALID_LUT_ENTRIES; i++ )
      {
	 unsigned int uiDisp16Bit = i << DISPARITY_LUT_SHIFT_BITS;
	 if( uiDisp16Bit <= 0xFF00 )
	 {

	    double dRed, dGreen, dBlue;
	    disparityToTemperature( m_triclopsContext, 
				    uiDisp16Bit, 
				    (double) m_nLUTMinDisp,
				    (double) m_nLUTMaxDisp,
				    &dRed, 
				    &dGreen, 
				    &dBlue );
	    m_ucValidDisparityMapLUT[i][2] = (unsigned char) dRed;
	    m_ucValidDisparityMapLUT[i][1] = (unsigned char) dGreen;
	    m_ucValidDisparityMapLUT[i][0] = (unsigned char) dBlue;
	 }
      }
      
      if ( !bColorInvalid )
      {
	 for (i = 0; i < DISPARITY_INVALID_LUT_ENTRIES; i++)
	 {
   	    // make them all grey
	    m_ucInvalidDisparityMapLUT[i][0] = 127;
	    m_ucInvalidDisparityMapLUT[i][1] = 127;
	    m_ucInvalidDisparityMapLUT[i][2] = 127;
	 }
      }
      else
      {
	 makeColoredInvalidLUT(	 m_ucInvalidDisparityMapLUT,
				 ucInvalidTexture,
			   	 ucInvalidUniqueness,
				 ucInvalidSurface,
				 ucInvalidBackForth,
				 ucInvalidSubpixel );
      }
   }
   else // if ( bPseudoColor )
   {
      
      // store values that we're dealing with so we don't compute next time.
      m_nLUTMinDisp     = nMinDisparity-nDisparityOffset;
      m_nLUTMaxDisp     = nMaxDisparity-nDisparityOffset;
      
      
      
      // These parameters control the mapping of disparity values to
      // the palette of the disparity display window. We want to scale
      // the current disparity range between 127 and 255 for "easy viewing"
      const unsigned char g_ucLUTMinDispMap = 127;
      const unsigned char g_ucLUTMaxDispMap = 255;
      
      float fScale = ((float)(g_ucLUTMaxDispMap - g_ucLUTMinDispMap))/
	 ((float)(m_nLUTMaxDisp-m_nLUTMinDisp));
      
      // handle valid pixels
      for( i = 0; i < DISPARITY_VALID_LUT_ENTRIES; i++ )
      {
	 unsigned int uiDisp16Bit = i << DISPARITY_LUT_SHIFT_BITS;
	 if( uiDisp16Bit <= 0xFF00 )
	 {
	    double dGreyValue = (
	       ((double)( uiDisp16Bit - (m_nLUTMinDisp<<8)))
	       *fScale + (double)(g_ucLUTMinDispMap<<8))/256.0;
	    m_ucValidDisparityMapLUT[i][0] = (unsigned char) dGreyValue;
	    m_ucValidDisparityMapLUT[i][1] = (unsigned char) dGreyValue;
	    m_ucValidDisparityMapLUT[i][2] = (unsigned char) dGreyValue;
	 }
      }
      
      if ( !bColorInvalid )
      {
	 for (i = 0; i < DISPARITY_INVALID_LUT_ENTRIES; i++)
	 {
   	    // make them all blue
	    m_ucInvalidDisparityMapLUT[i][0] = 255;
	    m_ucInvalidDisparityMapLUT[i][1] = 0;
	    m_ucInvalidDisparityMapLUT[i][2] = 0;
	 }
      }
      else
      {
	 makeColoredInvalidLUT(	 m_ucInvalidDisparityMapLUT,
				 ucInvalidTexture,
			   	 ucInvalidUniqueness,
				 ucInvalidSurface,
				 ucInvalidBackForth,
				 ucInvalidSubpixel );
      }
   }
}


CTransformD* 
CPGRStereoDoc::getTransform()
{
   return &m_dialogTransform.m_transform;
}


StereoImageResolution
CPGRStereoDoc::getResolution()
{
   return m_currentStereoResolution;   
}

void CPGRStereoDoc::getResolution( StereoImageResolution resType, int* iRows, int* iCols )
{
   switch ( resType )
   {  
   case STEREO_160x120:
      *iCols = 160;
      *iRows = 120;
      break;

   case STEREO_256x192:
      *iCols = 256;
      *iRows = 192;
      break;

   case STEREO_320x240:
      *iCols = 320;
      *iRows = 240;
      break;

   case STEREO_400x300:
      *iCols = 400;
      *iRows = 300;
      break;

   case STEREO_512x384:
      *iCols = 512;
      *iRows = 384;
      break;
   
   case STEREO_640x480:
      *iCols = 640;
      *iRows = 480;
      break;

   case STEREO_800x600:
      *iCols = 800;
      *iRows = 600;
      break;

   case STEREO_1024x768:
      *iCols = 1024;
      *iRows = 768;
      break;

   case STEREO_1280x960:
      *iCols = 1280;
      *iRows = 960;
      break;

   default:
      *iCols = 320;
      *iRows = 240;
   }

   return;
}


StereoImageResolution 
CPGRStereoDoc::getMaxResolution()
{
   return m_maxStereoResolution;
}


BOOL  
CPGRStereoDoc::isStereoDialogActive()
{
   return m_bStereoDialogActive;
}


BOOL
CPGRStereoDoc::isTransformDialogActive()
{
   return m_bTransformDialogActive;
}


BOOL
CPGRStereoDoc::drawingPoints()
{
   return m_dialogStereo.m_bDrawPoints;
}


BOOL
CPGRStereoDoc::drawingColorPoints()
{
   return m_dialogStereo.m_bDrawColorPoints;
}


BOOL
CPGRStereoDoc::drawingTriangles()
{
   return m_dialogStereo.m_bDrawSurfaces;
}


BOOL
CPGRStereoDoc::drawingTexture()
{
   return m_dialogStereo.m_bDrawTextureMap;
}

float
CPGRStereoDoc::getPointSize()
{
   return m_dialogStereo.m_fPointSize;
}


BOOL	  
CPGRStereoDoc::getPointData( 
			    const int	  iRow, 
			    const int	  iCol,
			    double*	  pdDisparity,
			    double*	  pdX,
			    double*	  pdY,
			    double*	  pdZ )
{
   if( iRow < 0 || iRow >= m_nMaxRows || iCol < 0 || iCol >= m_nMaxCols )
   {
      ASSERT( FALSE );
      return FALSE;
   }

   // if there are no images - do nothing
   if ( !m_bImageAvailable )
   {
      return FALSE;
   }

   // if stereo is not turned on - do nothing
   if( !m_dialogStereo.m_bStereo )
   {
      return FALSE;
   }

   TriclopsBool bSubpixelOn;
   TriclopsError te;

   te = triclopsGetSubpixelInterpolation( m_triclopsContext, &bSubpixelOn );
   ASSERT( te == TriclopsErrorOk );

   float fX, fY, fZ;
   if ( bSubpixelOn )
   {
      TriclopsImage16 image;
      te = triclopsGetImage16( m_triclopsContext, TriImg16_DISPARITY, TriCam_REFERENCE, &image );
      ASSERT( te == TriclopsErrorOk && &image != NULL );

      // If we get an pointer to invalid image data back, 
      // do not attempt any more processing
      if ( image.data == NULL )
      {
	 return FALSE;
      }
      
      unsigned short usDisparity = *(image.data + iRow * image.rowinc/2 + iCol);
   
      if ( usDisparity >= 0xff00 )
      {
	 // invalid pixel
	 return FALSE;
      }
      *pdDisparity = (double) usDisparity / 256.0;
      if ( usDisparity == 0 )
      {
	 // special case - point at infinity, can't represent
	 fX = fY = fZ = 0;
      }
      else
      {
	 te = triclopsRCD16ToXYZ( m_triclopsContext, iRow, iCol, usDisparity,
				 &fX, &fY, &fZ );
	 ASSERT( te == TriclopsErrorOk );
      }
   }
   else  // if (bSubpixelOn)
   {
      TriclopsImage image;
      te = triclopsGetImage( m_triclopsContext, TriImg_DISPARITY, TriCam_REFERENCE, &image );
      ASSERT( te == TriclopsErrorOk );
      unsigned char ucDisparity = *(image.data + iRow * image.rowinc + iCol);
   
      if ( ucDisparity > 239 )
      {
	 // invalid pixel
	 return FALSE;
      }
      *pdDisparity = (double) ucDisparity;
      if ( ucDisparity == 0 )
      {
	 fX = fY = fZ = 0;
      }
      else
      {
         te = triclopsRCD8ToXYZ( m_triclopsContext, iRow, iCol, ucDisparity,
	    			 &fX, &fY, &fZ );
	 ASSERT( te == TriclopsErrorOk );
      }
      
   }

   *pdX	 = fX;
   *pdY	 = fY;
   *pdZ	 = fZ;
   return (te == TriclopsErrorOk );
}


bool 
CPGRStereoDoc::updatePointOfRotation( double dX, double dY, double dZ )
{
   //
   // Find the OpenGL views and update them.
   //
   POSITION pos = GetFirstViewPosition();
   
   while( pos != NULL )
   {
      CView* pView = STATIC_DOWNCAST( CView, GetNextView( pos ) );
      ASSERT_VALID( pView );
      
      if( pView->IsKindOf( RUNTIME_CLASS( CPGROpenGLView )) )
      {
	 CPGROpenGLView* pGLView = ((CPGROpenGLView*)pView);

         pGLView->setRotationPoint( C3dColourPointRC( dX, dY, dZ ) );
	 pGLView->resetTransformations();
      }
   }

   UpdateAllViews( NULL );

   return true;
}


PGRBitmapTriclops* 
CPGRStereoDoc::getBitmap( enum_IMAGETYPE imageType )
{
   return m_arpBitmap[ imageType ];
}


BOOL 
CPGRStereoDoc::hasColorData()
{
   return m_bColorData;
}


CPointList*
CPGRStereoDoc::getPointCloud()
{
   return m_pPointCloud;
}


C3dColourPointRC*
CPGRStereoDoc::getPointArray()
{
   return m_pStaticPointArray;
}


void 
CPGRStereoDoc::saveRawImage( const char* szDefaultFilename ) 
{
   
   if ( !m_bCameraInitialized )
   {
      AfxMessageBox( "Camera has not been initialized" );
      return;
   }
   static char szFilter[] = 
      "Portable Pixel Map (*.ppm)|*.ppm|Portable Grey Map (*.pgm)|*.pgm||";
   
   CFileDialog fileDialog( FALSE, "ppm", szDefaultFilename, NULL, szFilter, NULL );
   
   if ( fileDialog.DoModal() == IDOK )
   {
      FlyCaptureImage tempImage;
      int nFileNameLength = fileDialog.GetPathName().GetLength() - 4;
      char szRedFileName[ MAX_PATH ];
      
      tempImage.iCols = m_flycaptureImage.iCols*m_flycaptureImage.iNumImages;
      tempImage.iRows = m_flycaptureImage.iRows;
	 
      if ( fileDialog.GetFileExt() == "ppm" )
      { 
	 tempImage.pixelFormat = FLYCAPTURE_BGRU;	 
	 tempImage.iRowInc = 4 * tempImage.iCols;
	 tempImage.pData = m_pRedProc;      

	 sprintf( 
	    szRedFileName, 
	    "%s_stereo.ppm", 
	    fileDialog.GetPathName().GetBufferSetLength( nFileNameLength ) );

	 flycaptureSaveImage( m_flycaptureContext, &tempImage, szRedFileName, FLYCAPTURE_FILEFORMAT_PPM );
      }
      else if ( fileDialog.GetFileExt() == "pgm" )
      {
	 tempImage.iRowInc = tempImage.iCols;
	 tempImage.pixelFormat = FLYCAPTURE_RAW8;
	 tempImage.pData = m_pRedRaw;

	 sprintf(
	    szRedFileName,
	    "%s_stereo.pgm",
	    fileDialog.GetPathName().GetBufferSetLength( nFileNameLength ) );

	 flycaptureSaveImage( m_flycaptureContext, &tempImage, szRedFileName, FLYCAPTURE_FILEFORMAT_PGM );
      }
      else
      {
	 AfxMessageBox( 
	    "Unable to determine image format because an "
	    "unsupported file extension was used.",
	    MB_ICONERROR );
      }
   }
}


void 
CPGRStereoDoc::saveCalAs( bool bCurrent ) 
{
   TriclopsError te;
   int nSerial;
   te = ::triclopsGetSerialNumber( m_triclopsContext, &nSerial );
   ASSERT( te == TriclopsErrorOk );

   // get the device configuration to determine if this is a Digiclops or Bumblebee
   TriclopsCameraConfiguration triclopsConfig;
   te = ::triclopsGetDeviceConfiguration( m_triclopsContext, &triclopsConfig );
   ASSERT( te == TriclopsErrorOk );

   char szFilename[1024];

   sprintf( szFilename, 
	    "%s%7.7d-%s.cal",
	    (triclopsConfig == TriCfg_2CAM_HORIZONTAL || 
	     triclopsConfig == TriCfg_2CAM_HORIZONTAL_WIDE ) ? "bumblebee" : "digiclops",
	    nSerial,
	    (bCurrent) ? "current" : "default" );

   static char szFilter[] = 
      "Point Grey Research Cal File (*.cal)|*.cal||";

   CFileDialog fileDialog( FALSE, "cal", szFilename, NULL, szFilter, NULL );
   
   if ( fileDialog.DoModal() == IDOK )
   {
      {
	 if ( bCurrent )
	 {
	    te = ::triclopsWriteCurrentContextToFile( 
	       m_triclopsContext, 
	       (LPSTR)((LPCTSTR)fileDialog.GetPathName()) );
	    ASSERT( te == TriclopsErrorOk );
	 }
	 else
	 {
	    te = ::triclopsWriteDefaultContextToFile( 
	       m_triclopsContext, 
	       (LPSTR)((LPCTSTR)fileDialog.GetPathName()) );
	    ASSERT( te == TriclopsErrorOk );
	 }
      }
   }
}


void 
CPGRStereoDoc::OnFileSaveCurrCal() 
{
   saveCalAs( true );
}


void 
CPGRStereoDoc::OnFileSaveDefaultCal() 
{
   saveCalAs( false );
}


void 
CPGRStereoDoc::OnFileSaveDisparity() 
{
   if ( !m_bImageAvailable )
   {
      AfxMessageBox( "No image available" );
      return;
   }

   if ( !m_dialogStereo.m_bStereo )
   {
      AfxMessageBox( "Stereo check box must be checked to enable saving disparity images" );
      return;
   }

   {
      static bool bWarned = false;
      if( !bWarned )
      {
	 MessageBox( 
	    0,
	    "Note - the disparity image saves the raw disparity values.  The image does "
	    "not appear as it does in the triclopsDemo display, as this image has "
	    "been modified for better viewing.  Disparity images will tend to appear very "
	    "dark due to the nature of disparity readings.\n\nThis warning will only appear once.",
	    "Warning", 
	    0 );
	 bWarned = true;
      }
   }

   TriclopsBool bSubpixel;
   TriclopsError error;
   error = triclopsGetSubpixelInterpolation( m_triclopsContext, &bSubpixel );
   ASSERT( error == TriclopsErrorOk );

   char szFilename[1024];
   if ( bSubpixel )
   {
      static bool bWarned = false;
      if( !bWarned )
      {
	 MessageBox( 
	    0,
	    "16-bit (subpixel) disparity images are not readable by most "
	    "image viewing packages.  If you wish to save the image for display, "
	    "the 8-bit disparity image is more suitable.  To save an 8-bit image, "
	    "please uncheck the Subpixel check box.\n\nThis warning will only appear once.",
	    "Warning", 
	    0 );
	 bWarned = true;
      }
      // 16 bit
      strcpy( szFilename, "disp16.pgm" );
   }
   else
   {
      // 8 bit
      strcpy( szFilename, "disp.pgm" );
   }

   static char szFilter[] = 
      "Portable Gray Map Image File (*.pgm)|*.pgm||";

   CFileDialog fileDialog( FALSE, "pgm", szFilename, NULL, szFilter, NULL );
   
   if ( fileDialog.DoModal() == IDOK )
   {
      if ( bSubpixel )
      {
	 TriclopsImage16 image;
	 error = triclopsGetImage16( m_triclopsContext, 
				     TriImg16_DISPARITY, 
				     TriCam_REFERENCE, 
				     &image );
	 ASSERT( error == TriclopsErrorOk );
	 error = triclopsSaveImage16( &image, (LPSTR)((LPCTSTR) fileDialog.GetPathName()) );
	 ASSERT( error == TriclopsErrorOk );
      }
      else
      {
	 TriclopsImage image;
	 error = triclopsGetImage( m_triclopsContext, 
				   TriImg_DISPARITY, 
				   TriCam_REFERENCE, 
				   &image );
	 ASSERT( error == TriclopsErrorOk );
	 error = triclopsSaveImage( &image, (LPSTR)((LPCTSTR) fileDialog.GetPathName()) );
	 ASSERT( error == TriclopsErrorOk );
   
      }
   }
}


void inline
saveTriclopsGreyscaleImage( TriclopsContext     triclops,
		            TriclopsImageType   imageType,
			    TriclopsCamera      camera,
			    char*	        szFilename )
{
   TriclopsError error;   
   static char szFilter[] = 
      "Portable Gray Map Image File (*.pgm)|*.pgm||";

   CFileDialog fileDialog( FALSE, "pgm", szFilename, NULL, szFilter, NULL );
   
   if ( fileDialog.DoModal() == IDOK )
   {
      TriclopsImage image;
      error = triclopsGetImage( triclops,
				imageType,
				camera,
				&image );
      ASSERT( error == TriclopsErrorOk );
      error = triclopsSaveImage( &image, (LPSTR)((LPCTSTR) fileDialog.GetPathName()) );
      ASSERT( error == TriclopsErrorOk );
   }

}

void
CPGRStereoDoc::OnFileSaveEdgeLeft() 
{
   if ( !m_bImageAvailable )
   {
      AfxMessageBox( "No image available" );
      return;
   }
   saveTriclopsGreyscaleImage( m_triclopsContext,
			       TriImg_EDGE,
			       TriCam_LEFT,
			       "edge-left.pgm" );
}


void 
CPGRStereoDoc::OnFileSaveEdgeRight() 
{   
   if ( !m_bImageAvailable )
   {
      AfxMessageBox( "No image available" );
      return;
   }
   saveTriclopsGreyscaleImage( m_triclopsContext,
			       TriImg_EDGE,
			       TriCam_RIGHT,
			       "edge-right.pgm" );
}


void 
CPGRStereoDoc::OnFileSaveEdgeCenter() 
{  
   if ( !m_bIsBumblebee )
   {
         
      if ( !m_bImageAvailable )
      {
	 AfxMessageBox( "No image available" );
	 return;
      }
      saveTriclopsGreyscaleImage( m_triclopsContext,
				  TriImg_EDGE,
				  TriCam_TOP,
				  "edge-center.pgm" );
   }
   else
   {
      AfxMessageBox( "This camera does not have a center image" );
   }
}


void 
CPGRStereoDoc::OnFileSaveRawLeft() 
{
   if ( m_bOffline )
   {
      // should really have menu item greyed out
      AfxMessageBox( "Cannot save raw images in offline-mode" );
      return;
   }

   saveRawImage("raw-stereo" );

}

void 
CPGRStereoDoc::OnFileSaveRawStereo() 
{      
   if ( m_bOffline )
   {
      // should really have menu item greyed out
      AfxMessageBox( "Cannot save raw images in offline-mode" );
      return;
   }

   saveRawImage("raw-stereo" );
}

void 
CPGRStereoDoc::OnFileSaveRawRight() 
{
   if ( m_bOffline )
   {
      // should really have menu item greyed out
      AfxMessageBox( "Cannot save raw images in offline-mode" );
      return;
   }
   
   saveRawImage("raw-stereo" );

}


void 
CPGRStereoDoc::OnFileSaveRawCenter() 
{
   if ( !m_bIsBumblebee )
   {
      if ( m_bOffline )
      {
	 // should really have menu item greyed out
	 AfxMessageBox( "Cannot save raw images in offline-mode" );
	 return;
      }
      
      saveRawImage("raw-stereo" );

   }
   else
   {
      AfxMessageBox( "This camera does not have a center image" );
   }   
}


inline void
saveRectifiedColorImage( TriclopsContext triclops,
			 TriclopsCamera  camera,
			 TriclopsInput*  input,
			 char*	         szFilename )
{

   TriclopsError error;   
   static char szFilter[] = 
      "Portable Pix-Map Image File (*.ppm)|*.ppm||";

   CFileDialog fileDialog( FALSE, "ppm", szFilename, NULL, szFilter, NULL );
   
   if ( fileDialog.DoModal() == IDOK )
   {
      TriclopsColorImage image;
      error = triclopsRectifyColorImage( triclops, 
					 camera, 
					 input,
					 &image );
      ASSERT( error == TriclopsErrorOk );
      error = triclopsSaveColorImage( &image, (LPSTR)((LPCTSTR) fileDialog.GetPathName()) );
      ASSERT( error == TriclopsErrorOk );
   }
}



void 
CPGRStereoDoc::OnFileSaveRectifiedColorLeft() 
{   
   if ( !m_bImageAvailable )
   {
      AfxMessageBox( "No image available" );
      return;
   }
   saveRectifiedColorImage( m_triclopsContext,
			    TriCam_LEFT,
			    &m_tiRawColorImages[IMAGE_LEFT_RAW],
			    "rect-left.ppm" );
}


void 
CPGRStereoDoc::OnFileSaveRectifiedColorRight() 
{   
   if ( !m_bImageAvailable )
   {
      AfxMessageBox( "No image available" );
      return;
   }
   saveRectifiedColorImage( m_triclopsContext,
			    TriCam_RIGHT,
			    &m_tiRawColorImages[IMAGE_RIGHT_RAW],
			    "rect-right.ppm" );
}


void 
CPGRStereoDoc::OnFileSaveRectifiedColorCenter() 
{
   if ( !m_bIsBumblebee )
   {   
      if ( !m_bImageAvailable )
      {
	 AfxMessageBox( "No image available" );
	 return;
      }
      saveRectifiedColorImage( m_triclopsContext,
			       TriCam_TOP,
			       &m_tiRawColorImages[IMAGE_CENTER_RAW],
			       "rect-center.ppm" );

   }
   else
   {
      AfxMessageBox( "This camera does not have a center image" );
   }   
}


void 
CPGRStereoDoc::OnFileSaveRectifiedLeft() 
{   
   if ( !m_bImageAvailable )
   {
      AfxMessageBox( "No image available" );
      return;
   }   
   saveTriclopsGreyscaleImage( m_triclopsContext,
			       TriImg_RECTIFIED,
			       TriCam_LEFT,
			       "rect-left.pgm" );
}


void 
CPGRStereoDoc::OnFileSaveRectifiedRight() 
{   
   if ( !m_bImageAvailable )
   {
      AfxMessageBox( "No image available" );
      return;
   }
   saveTriclopsGreyscaleImage( m_triclopsContext,
			       TriImg_RECTIFIED,
			       TriCam_RIGHT,
			       "rect-right.pgm" );
}


void 
CPGRStereoDoc::OnFileSaveRectifiedCenter() 
{
   if ( !m_bIsBumblebee )
   {   
      if ( !m_bImageAvailable )
      {
	 AfxMessageBox( "No image available" );
	 return;
      }
      saveTriclopsGreyscaleImage( m_triclopsContext,
				  TriImg_RECTIFIED,
				  TriCam_TOP,
				  "rect-center.pgm" );
   }
   else
   {
      AfxMessageBox( "This camera does not have a center image" );
   }   

}


void 
CPGRStereoDoc::OnFileSaveStereo() 
{
   if ( m_bOffline )
   {
      // should really have menu item greyed out
      AfxMessageBox( "Cannot save stereo input in offline-mode" );
      return;
   }
   if( !m_bImageAvailable )
   {
      AfxMessageBox( "There is currently no image available to save." );
      return;
   }
 
   static char szFilter[] = 
      "Portable Grey Map (*.pgm)|*.pgm||";
   
   CFileDialog fileDialog( FALSE, "pgm", "stereo", NULL, szFilter, NULL );
   
   if ( fileDialog.DoModal() == IDOK )
   {
      if( !pgmWriteFromTriclopsInput( 
	 fileDialog.GetFileName(), 
	 &m_tiStereo, 
	 m_flycaptureImage.iNumImages ) )
      {
	 AfxMessageBox( "There was an error writing the image file." );
	 return;
      }
   }
}


void 
CPGRStereoDoc::OnFileSavePointcloud() 
{
      
   if ( !m_bImageAvailable )
   {
      AfxMessageBox( "No image available" );
      return;
   }
  
   
   char	 pszDefaultPointCloudFilename[] = "pointcloud.pts";

   CFileDialog fileDialog( 
      FALSE, "pts", pszDefaultPointCloudFilename, NULL, NULL, NULL );
   
   if ( fileDialog.DoModal() == IDOK )
   {
      CPointList* pointList = getPointCloud();

      if( !pointList->save( fileDialog.GetPathName() ) )
      {
	 AfxMessageBox( "Failed to save point cloud file " );
      }
   }
}

void
CPGRStereoDoc::destroyTriclopsContext()
{
   triclopsDestroyContext( m_triclopsContext );
}

void
CPGRStereoDoc::initTriclopsContext()
{
   //
   // Initialize triclops library
   //

   // Always rectify.
   ::triclopsSetRectify( m_triclopsContext, 1 );

   // Turn on low-pass filtering
   ::triclopsSetLowpass( m_triclopsContext, 1 );

   // Turn on subpixel by default.
   ::triclopsSetSubpixelInterpolation( m_triclopsContext, 1 );

   // Make sure strict subpixel validations is always off (bug 850)
   ::triclopsSetStrictSubpixelValidation( m_triclopsContext, 0 );

   // Set the validation mappings
   ::triclopsSetTextureValidationMapping( m_triclopsContext, 255 );
   ::triclopsSetUniquenessValidationMapping( m_triclopsContext, 254 );
   ::triclopsSetSubpixelValidationMapping( m_triclopsContext, 253 );
   ::triclopsSetSurfaceValidationMapping( m_triclopsContext, 252 );
   ::triclopsSetBackForthValidationMapping( m_triclopsContext, 251 );

   // Set uniqueness off by default - its not a good default setting 
   // and should be off by default but most config files have it on
   ::triclopsSetUniquenessValidation( m_triclopsContext, 0 );

   // These defaults should be eventually migrated to defaults for
   // the stereo library but for now we put some hard coded Triclops
   // parameters here
   ::triclopsSetTextureValidation( m_triclopsContext, 1 );
   ::triclopsSetTextureValidationThreshold( m_triclopsContext, (float) 0.2 );

   // Get transformation from file.  If fails, m_dialogTransform will simply
   // remain as identity matrix.
   CTransformD* pTransTemp = getTransform();
   char szTitle[50];
   getCameraTitle( szTitle, true );
   if( getTransformFromFile( szTitle, pTransTemp ) )
   {
      m_dialogTransform.m_dX	 = pTransTemp->tX();
      m_dialogTransform.m_dY	 = pTransTemp->tY();
      m_dialogTransform.m_dZ	 = pTransTemp->tZ();
      m_dialogTransform.m_dRoll	 = pTransTemp->rX();
      m_dialogTransform.m_dPitch = pTransTemp->rY();
      m_dialogTransform.m_dYaw	 = pTransTemp->rZ();
   }

   //
   // Initialize the triclops context by loading each resolution ahead of time
   // If the camera is an XB3, initialize both the narrow and wide baselines
   // for each resolution.
   //
   TriclopsError te;
   bool bIsXB3;

   //
   // Check if the camera is a XB3
   //
   if ( m_flycaptureInfo.CameraModel == FLYCAPTURE_BUMBLEBEEXB3 )
   {
      bIsXB3 = true;
   }
   else
   {
      bIsXB3 = false;
   }

   //
   // Prepare narrow baseline
   // This is applicable to all Bumblebees
   //
   te = ::triclopsSetCameraConfiguration( 
	 m_triclopsContext, TriCfg_2CAM_HORIZONTAL_NARROW );

   switch( m_nMaxCols )
   {
   case 1280:
      ASSERT( te == TriclopsErrorOk );
      te = ::triclopsSetResolutionAndPrepare( 
         m_triclopsContext, 960, 1280, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );

   case 1024:
      te = ::triclopsSetResolutionAndPrepare( 
         m_triclopsContext, 768, 1024, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );

   case 800:
      te = ::triclopsSetResolutionAndPrepare( 
         m_triclopsContext, 600, 800, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
      
   case 640:
      te = ::triclopsSetResolutionAndPrepare( 
	 m_triclopsContext, 480, 640, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );

   case 512:
      te = ::triclopsSetResolutionAndPrepare( 
         m_triclopsContext, 384, 512, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );

   case 400:
      te = ::triclopsSetResolutionAndPrepare( 
         m_triclopsContext, 300, 400, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
      
   case 320:
      te = ::triclopsSetResolutionAndPrepare( 
	 m_triclopsContext, 240, 320, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
      
   case 256:
      te = ::triclopsSetResolutionAndPrepare( 
         m_triclopsContext, 192, 256, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
      
   case 160:
      te = ::triclopsSetResolutionAndPrepare( 
	 m_triclopsContext, 120, 160, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
   }

   //
   // If the camera is NOT a XB3, we return here since we do not need to
   // prepare the wide baseline
   //
   if ( bIsXB3 != true )
   {
      return;
   }

   //
   // Prepare wide baseline
   // This is applicable to XB3 only
   //
   te = ::triclopsSetCameraConfiguration( 
      m_triclopsContext, TriCfg_2CAM_HORIZONTAL_WIDE );
   ASSERT( te == TriclopsErrorOk );
   
   switch( m_nMaxCols )
   {
   case 1280:
      ASSERT( te == TriclopsErrorOk );
      te = ::triclopsSetResolutionAndPrepare( 
         m_triclopsContext, 960, 1280, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
      
   case 1024:
      te = ::triclopsSetResolutionAndPrepare( 
         m_triclopsContext, 768, 1024, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
      
   case 800:
      te = ::triclopsSetResolutionAndPrepare( 
         m_triclopsContext, 600, 800, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
      
   case 640:
      te = ::triclopsSetResolutionAndPrepare( 
	 m_triclopsContext, 480, 640, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
      
   case 512:
      te = ::triclopsSetResolutionAndPrepare( 
         m_triclopsContext, 384, 512, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
      
   case 400:
      te = ::triclopsSetResolutionAndPrepare( 
         m_triclopsContext, 300, 400, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
      
   case 320:
      te = ::triclopsSetResolutionAndPrepare( 
	 m_triclopsContext, 240, 320, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
      
   case 256:
      te = ::triclopsSetResolutionAndPrepare( 
         m_triclopsContext, 192, 256, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
      
   case 160:
      te = ::triclopsSetResolutionAndPrepare( 
	 m_triclopsContext, 120, 160, m_nMaxRows, m_nMaxCols );
      ASSERT( te == TriclopsErrorOk );
   }
}



BOOL 
CPGRStereoDoc::OnFileLoadStereoImage() 
{
   static char szFilter[] = 
      "Portable Grey Map File (*.pgm)|*.pgm|Portable Pixel Map File (*.ppm)|*.ppm||";

   // a file-open dialog
   CFileDialog fileDialog( TRUE, "pgm", NULL, NULL, szFilter, NULL );
   
   // display the dialog
   if ( fileDialog.DoModal() == IDOK )
   {
      // user selected a file
      CString fileName = fileDialog.GetPathName();
      // note: length of 200 is just a goodly long enough value
      TriclopsInput input;      
      if ( !pgmReadToTriclopsInput( fileName.GetBuffer(200), &input ) )
      {
	 AfxMessageBox( "Error opening image file" );
	 return FALSE;
      } 
      extractRawImagesFromTriclopsInput( input );
      return TRUE;
   }
   
   // else - user canceled
   return FALSE;
}

void 
CPGRStereoDoc::OnFileClose() 
{
   OnCloseDocument();
}

void
CPGRStereoDoc::OnFileLoadCal()
{
   static bool bWarnedOnce = false;

   if ( !bWarnedOnce && !m_bOffline )
   {
      MessageBox( 0, 
	 "Note:  Calibration data is normally loaded from "
	 "the connected camera.  After you load a calibration file "
	 "you will need to restart this application "
	 "if you want to retrieve the calibration file from the camera "
	 "again.\n"
	 "This warning will only be displayed once.\n"
	 "",
	 "Warning", 0 );
      bWarnedOnce   = true;
   }

   // 
   // Destroy the current triclops context
   // 
   destroyTriclopsContext();

   if ( !loadTriclopsContextFromFile( &m_triclopsContext ) )
   {
      AfxMessageBox( "Failed to load TriclopsContext from file!" );
      return;
   }

   int nSerialNumber;
   triclopsGetSerialNumber( m_triclopsContext, &nSerialNumber );
   m_serialNumber = nSerialNumber;
   
   ReInitDocument();
   return;
}

BOOL	 
CPGRStereoDoc::isOffline()
{
   return m_bOffline;
}

void 
CPGRStereoDoc::setDocumentActive( BOOL bActive )
{
   m_bDocumentActive = bActive;
   return;
}

BOOL
CPGRStereoDoc::isDocumentActive()
{
   return m_bDocumentActive;
}

BOOL
CPGRStereoDoc::isBumblebee()
{
   return m_bIsBumblebee;
}

void 
CPGRStereoDoc::decrementColorRectifiedViews()
{
   ASSERT( m_nRectifyColorViews > 0 );
   m_nRectifyColorViews--;
}

void 
CPGRStereoDoc::incrementColorRectifiedViews()
{
   m_nRectifyColorViews++;
}

void 
CPGRStereoDoc::OnAppVersionInfo()
{
   if( m_flycaptureContext != NULL && m_guicontext != NULL )
   {
      pgrcamguiShowInfoDlg( 
         m_guicontext, m_flycaptureContext, AfxGetMainWnd()->GetSafeHwnd() );
   }
}

BOOL 
CPGRStereoDoc::isCameraAlreadyOpen( FlyCaptureCameraSerialNumber serial )
{
   if( m_bOffline )
   {
      return FALSE;
   }

   return ( serial == m_serialNumber );
}

