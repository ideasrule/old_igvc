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
// $Id: roistereo.cpp,v 1.6 2007/05/28 22:30:26 soowei Exp $
//=============================================================================
//=============================================================================
// roistereo:
//
// Takes input from the camera, and sets up four regions of interest on 
// which to perform stereo processing; the resultant disparity image is 
// saved.
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <stdlib.h>
#include <stdio.h>

//=============================================================================
// PGR Includes
//=============================================================================
#include "triclops.h"
#include "pgrflycapture.h"
#include "pgrflycapturestereo.h"

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
// Macro to check, report on, and handle Flycapture API error codes.
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
   TriclopsContext    triclops;
   TriclopsImage      disparityImage;
   TriclopsImage      refImage;
   TriclopsInput      triclopsInput;
   TriclopsROI*       pRois;
   int                nMaxRois;
   TriclopsError      te;

   FlyCaptureContext  flycapture; 
   FlyCaptureImage    flycaptureImage;
   FlyCaptureInfoEx pInfo;
   FlyCapturePixelFormat pixelFormat;
   FlyCaptureError    fe;

   int iMaxCols = 0;
   int iMaxRows = 0;
   
   // Create the camera context
   fe = flycaptureCreateContext( &flycapture );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureCreateContext()", fe );

   // Initialize the camera
   fe = flycaptureInitialize( flycapture, 0 );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureInitialize()", fe );

   // Get the camera configuration
   char* szCalFile;
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
 
   // Start grabbing
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
   unsigned char* rowIntColor = 
      new unsigned char[ imageCols * imageRows * iSideBySideImages * 4];
   unsigned char* rowIntMono = 
      new unsigned char[ imageCols * imageRows * iSideBySideImages ];

   // Create a temporary FlyCaptureImage for preparing the stereo image
   FlyCaptureImage tempColorImage;
   FlyCaptureImage tempMonoImage;
   
   tempColorImage.pData = rowIntColor;
   tempMonoImage.pData = rowIntMono;

   // Convert the pixel interleaved raw data to row interleaved format
   fe = flycapturePrepareStereoImage( 
      flycapture, flycaptureImage, &tempMonoImage, &tempColorImage );
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

  
   // Set up some stereo parameters:
   // Set to 640x480 output images
   te = triclopsSetResolution( triclops, 480, 640 );
   _HANDLE_TRICLOPS_ERROR( "triclopsSetResolution()", te );

   // Set disparity range to be quite wide
   te = triclopsSetDisparity( triclops, 0, 200 );
   _HANDLE_TRICLOPS_ERROR( "triclopsSetDisparity()", te );

   // Set subpixel interpolation off - so we know we don't need to use 
   // TriclopsImage16 structures when we access and save the disparity image
   te = triclopsSetSubpixelInterpolation( triclops, 0 );
   _HANDLE_TRICLOPS_ERROR( "triclopsSetSubpixelInterpolation()", te );
   
   
   // Get the pointer to the regions of interest array
   te = triclopsGetROIs( triclops, &pRois, &nMaxRois );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetROIs()", te );
   
   if( nMaxRois >= 4 ) 
   {
      // Set up four regions of interest: 
      
      // Entire upper left quadrant of image
      pRois[0].row   = 0;
      pRois[0].col   = 0;
      pRois[0].nrows = 240;
      pRois[0].ncols = 320;
      
      // Part of the lower right
      pRois[1].row   = 240;
      pRois[1].col   = 320;
      pRois[1].nrows = 180;
      pRois[1].ncols = 240;
      
      // Centered in upper right quadrant
      pRois[2].row   = 60;
      pRois[2].col   = 400;
      pRois[2].nrows = 120;
      pRois[2].ncols = 160;
      
      // Small section of lower left
      pRois[3].row   = 300;
      pRois[3].col   = 30;
      pRois[3].nrows = 80;
      pRois[3].ncols = 80;
      
      // Tell the TriclopsContext how many ROIs we want to process
      te = triclopsSetNumberOfROIs( triclops, 4 );
      _HANDLE_TRICLOPS_ERROR( "triclopsSetNumberOfROIs()", te );
   }
   else
   {
      printf( "Only %d ROIs available in the TriclopsContext "
	      "- this should never happen!\n"
	      "Aborting!\n",
	      nMaxRois );
	      
      // Destroy the Triclops context
      triclopsDestroyContext( triclops ) ;
      
      // Close the camera and destroy the context
      flycaptureStop( flycapture );
      flycaptureDestroyContext( flycapture );
      return 1;
   }
   
   
   // Rectify the images
   te = triclopsRectify( triclops, &triclopsInput );
   _HANDLE_TRICLOPS_ERROR( "triclopsRectify()", te );
   
   // Do stereo processing
   te = triclopsStereo( triclops );
   _HANDLE_TRICLOPS_ERROR( "triclopsStereo()", te );
   
   // Retrieve the disparity image from the Triclops context
   te = triclopsGetImage( triclops, 
			  TriImg_DISPARITY, 
			  TriCam_REFERENCE, 
			  &disparityImage );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetImage()", te );

   // Retrieve the rectified image from the Triclops context
   te = triclopsGetImage( triclops, 
			  TriImg_RECTIFIED, 
			  TriCam_REFERENCE, 
			  &refImage );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetImage()", te );

   // Save the disparity image
   te = triclopsSaveImage( &disparityImage, "disparity.pgm" );
   _HANDLE_TRICLOPS_ERROR( "triclopsSaveImage()", te );

   // Save the rectified image
   te = triclopsSaveImage( &refImage, "rectified.pgm" );
   _HANDLE_TRICLOPS_ERROR( "triclopsSaveImage()", te );

   // Delete the image buffer, it is not needed once the TriclopsInput
   // has been built
   delete rowIntMono;
   redMono = NULL;
   greenMono = NULL;
   blueMono = NULL;
   
   // Destroy the Triclops context
   triclopsDestroyContext( triclops ) ;
   
   // Close the camera and destroy the Flycapture context
   flycaptureStop( flycapture );
   flycaptureDestroyContext( flycapture );
   
   return 0;
}
