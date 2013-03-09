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
// $Id: grabstereo.cpp,v 1.7 2007/05/28 22:30:26 soowei Exp $
//=============================================================================
//=============================================================================
// grabstereo
//
// Gets input from the Digiclops/Bumblebee, and performs stereo processing
// to create a disparity image. A rectified image from the reference camera
// and the disparity image are both written out.
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <stdio.h>
#include <stdlib.h>

//=============================================================================
// PGR Includes
//=============================================================================
#include "triclops.h"
#include "pgrflycapture.h"
#include "pgrflycapturestereo.h"
#include "pnmutils.h"

//=============================================================================
// Project Includes
//=============================================================================

//
// Macro to check, report on, and handle Triclops API error codes.
//
#define _HANDLE_TRICLOPS_ERROR( description, error )	\
{ \
   if( error != TriclopsErrorOk ) \
   { \
      printf( \
	 "*** Triclops Error '%s' at line %d :\n\t%s\n", \
	 triclopsErrorToString( error ), \
	 __LINE__, \
	 description );	\
      printf( "Press any key to exit...\n" ); \
      getchar(); \
      exit( 1 ); \
   } \
} \

//
// Macro to check, report on, and handle Digiclops API error codes.
//
#define _HANDLE_FLYCAPTURE_ERROR( description, error )	\
{ \
   if( error != FLYCAPTURE_OK ) \
   { \
      printf( \
	 "*** Flycapture Error '%s' at line %d :\n\t%s\n", \
	 flycaptureErrorToString( error ), \
	 __LINE__, \
	 description );	\
      printf( "Press any key to exit...\n" ); \
      getchar(); \
      exit( 1 ); \
   } \
} \


int
main( int /* argc */, char** /* argv */ )
{
   TriclopsContext   triclops;
   TriclopsImage     disparityImage;
   TriclopsImage     refImage;
   TriclopsInput     triclopsInput;

   FlyCaptureContext	   flycapture;
   FlyCaptureImage	   flycaptureImage;
   FlyCaptureInfoEx	   pInfo;
   FlyCapturePixelFormat   pixelFormat;

   TriclopsError     te;
   FlyCaptureError   fe;
  
   int iMaxCols = 0;
   int iMaxRows = 0;

   char* szCalFile;

   // Open the camera
   fe = flycaptureCreateContext( &flycapture );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureCreateContext()", fe );

   // Initialize the Flycapture context
   fe = flycaptureInitialize( flycapture, 0 );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureInitialize()", fe );

   // Save the camera's calibration file, and return the path 
   fe = flycaptureGetCalibrationFileFromCamera( flycapture, &szCalFile );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureGetCalibrationFileFromCamera()", fe );
   
   // Create a Triclops context from the cameras calibration file
   te = triclopsGetDefaultContextFromFile( &triclops, szCalFile );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetDefaultContextFromFile()", te );
 
   // Get camera information
   fe = flycaptureGetCameraInfo( flycapture, &pInfo );
   _HANDLE_FLYCAPTURE_ERROR( "flycatpureGetCameraInfo()", fe );  
   
   if (pInfo.CameraType == FLYCAPTURE_COLOR)
   {
      pixelFormat = FLYCAPTURE_RAW16;
   } 
   else 
   {
      pixelFormat = FLYCAPTURE_MONO16;
   }

   switch (pInfo.CameraModel)
   {
   case FLYCAPTURE_BUMBLEBEE2:
      {
	 unsigned long ulValue;
	 flycaptureGetCameraRegister( flycapture, 0x1F28, &ulValue );
	 
	 if ( ( ulValue & 0x2 ) == 0 )
	 {
	    // Hi-res BB2
	    iMaxCols = 1024; 
	    iMaxRows = 768;   
	 }
	 else
	 {
	    // Low-res BB2
	    iMaxCols = 640;
	    iMaxRows = 480;
	 }
      }
      break;
      
   case FLYCAPTURE_BUMBLEBEEXB3:
      iMaxCols = 1280;
      iMaxRows = 960;
      break;
      
   default:
      te = TriclopsErrorInvalidCamera;
      _HANDLE_TRICLOPS_ERROR( "triclopsCheckCameraModel()", te );
      break;
   }

   // Start transferring images from the camera to the computer
   fe = flycaptureStartCustomImage( 
      flycapture, 3, 0, 0, iMaxCols, iMaxRows, 100, pixelFormat);
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureStart()", fe );

   // Grab an image from the camera
   fe = flycaptureGrabImage2( flycapture, &flycaptureImage );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureGrabImage()", fe );

   // Extract information from the FlycaptureImage
   int imageCols = flycaptureImage.iCols;
   int imageRows = flycaptureImage.iRows;
   int imageRowInc = flycaptureImage.iRowInc;
   int iSideBySideImages = flycaptureImage.iNumImages;
   unsigned long timeStampSeconds = flycaptureImage.timeStamp.ulSeconds;
   unsigned long timeStampMicroSeconds = flycaptureImage.timeStamp.ulMicroSeconds;

   // Create buffers for holding the mono images
   unsigned char* rowIntMono = 
      new unsigned char[ imageCols * imageRows * iSideBySideImages ];

   // Create a temporary FlyCaptureImage for preparing the stereo image
   FlyCaptureImage tempImage;
   tempImage.pData = rowIntMono;

   // Convert the pixel interleaved raw data to row interleaved format
   fe = flycapturePrepareStereoImage( flycapture, flycaptureImage, &tempImage, NULL);
   _HANDLE_FLYCAPTURE_ERROR( "flycapturePrepareStereoImage()", fe );

   // Pointers to positions in the mono buffer that correspond to the beginning
   // of the red, green and blue sections
   unsigned char* redMono = NULL;
   unsigned char* greenMono = NULL;
   unsigned char* blueMono = NULL;

   redMono = rowIntMono;
   if (flycaptureImage.iNumImages == 2)
   {
	   greenMono = redMono + imageCols;
	   blueMono = redMono + imageCols;
   }
   if (flycaptureImage.iNumImages == 3)
   {
	   greenMono = redMono + imageCols;
	   blueMono = redMono + ( 2 * imageCols );
   }
   
   // Use the row interleaved images to build up an RGB TriclopsInput.  
   // An RGB triclops input will contain the 3 raw images (1 from each camera).
   te = triclopsBuildRGBTriclopsInput(
      imageCols, 
      imageRows, 
      imageRowInc,  
      timeStampSeconds, 
      timeStampMicroSeconds, 
      redMono, 
      greenMono, 
      blueMono, 
      &triclopsInput);
   _HANDLE_TRICLOPS_ERROR( "triclopsBuildRGBTriclopsInput()", te );

   // Rectify the images
   te = triclopsRectify( triclops, &triclopsInput );
   _HANDLE_TRICLOPS_ERROR( "triclopsRectify()", te );
   
   // Do stereo processing
   te = triclopsStereo( triclops );
   _HANDLE_TRICLOPS_ERROR( "triclopsStereo()", te );
   
   // Retrieve the disparity image from the triclops context
   te = triclopsGetImage( triclops, TriImg_DISPARITY, TriCam_REFERENCE, &disparityImage );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetImage()", te );

   // Retrieve the rectified image from the triclops context
   te = triclopsGetImage( triclops, TriImg_RECTIFIED, TriCam_REFERENCE, &refImage );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetImage()", te );
   
   // Save the disparity and reference images
   te = triclopsSaveImage( &disparityImage, "disparity.pgm" );
   _HANDLE_TRICLOPS_ERROR( "triclopsSaveImage()", te );

   te = triclopsSaveImage( &refImage, "reference.pgm" );
   _HANDLE_TRICLOPS_ERROR( "triclopsSaveImage()", te );
   
   // Close the camera
   fe = flycaptureStop( flycapture );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureStop()", fe );

   // Delete the image buffer
   delete rowIntMono;
   redMono = NULL;
   greenMono = NULL;
   blueMono = NULL;

   fe = flycaptureDestroyContext( flycapture );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureDestroyContext()", fe );
   
   // Destroy the Triclops context
   te = triclopsDestroyContext( triclops ) ;
   _HANDLE_TRICLOPS_ERROR( "triclopsDestroyContext()", te );
   
   return 0;
}
