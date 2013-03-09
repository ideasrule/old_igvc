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
// $Id: grabrectifiedcolor.cpp,v 1.7 2007/05/28 22:30:26 soowei Exp $
//=============================================================================
//=============================================================================
// grabrectifiedcolor
//
// Takes input from a Color Digiclops, extracts the color image from the 
// right camera, rectifies this image, and saves it to a ppm file.
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
#define _HANDLE_TRICLOPS_ERROR( function, error ) \
{ \
   if( error != TriclopsErrorOk ) \
   { \
      printf( \
	 "ERROR: %s reported %s.\n", \
	 function, \
	 triclopsErrorToString( error ) ); \
      exit( 1 ); \
   } \
} \

//
// Macro to check, report on, and handle Flycapture API error codes.
//
#define _HANDLE_FLYCAPTURE_ERROR( function, error ) \
{ \
   if( error != FLYCAPTURE_OK ) \
   { \
      printf( \
	 "ERROR: %s reported %s.\n", \
	 function, \
	 flycaptureErrorToString( error ) ); \
      exit( 1 ); \
   } \
} \


int
main( int /* argc */, char** /* argv */ )
{
   TriclopsInput       colorInput;
   TriclopsPackedColorImage  colorImage;
   TriclopsContext     triclops;

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
   
   // set rectified resolution to 320x240 
   te = triclopsSetResolution( triclops, 240, 320 );
   _HANDLE_TRICLOPS_ERROR( "triclopsSetResolution()", te );
   
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
   
   // Create buffers for holding the color and mono images
   unsigned char* rowIntColor = 
      new unsigned char[ imageCols * imageRows * iSideBySideImages * 4 ];

   // Create a temporary FlyCaptureImage for preparing the stereo image
   FlyCaptureImage tempImage;
   tempImage.pData = rowIntColor;
   
   // Convert the pixel interleaved raw data to row interleaved format
   fe = flycapturePrepareStereoImage( flycapture, flycaptureImage, NULL, &tempImage );
   _HANDLE_FLYCAPTURE_ERROR( "flycapturePrepareStereoImage()", fe );
   
   // Pointers to positions in the color buffer that correspond to the beginning
   // of the red, green and blue sections
   unsigned char* redColor = NULL;
   unsigned char* greenColor = NULL;
   unsigned char* blueColor = NULL;
   
   redColor = rowIntColor;
   if (flycaptureImage.iNumImages == 2)
   {
      greenColor = redColor + ( 4 * imageCols );
      blueColor = redColor + ( 4 * imageCols );
   }
   if (flycaptureImage.iNumImages == 3)
   {
      greenColor = redColor + ( 4 * imageCols );
      blueColor = redColor + ( 2 * 4 * imageCols );
   }
   
   // Use the row interleaved images to build up a packed TriclopsInput.
   // A packed triclops input will contain a single image with 32 bpp.
   te = triclopsBuildPackedTriclopsInput(
      imageCols,
      imageRows,
      imageRowInc * 4,
      timeStampSeconds,
      timeStampMicroSeconds,
      redColor,
      &colorInput );
   _HANDLE_TRICLOPS_ERROR( "triclopsBuildPackedTriclopsInput()", te );
   
   // rectify the color image
   te = triclopsRectifyPackedColorImage( triclops, 
				   TriCam_REFERENCE, 
				   &colorInput, 
				   &colorImage );
   _HANDLE_TRICLOPS_ERROR( "triclopsRectifyPackedColorImage()", te );
   
   // Save the color rectified image to file
   triclopsSavePackedColorImage(&colorImage, "right-rectified.ppm");

   // Close the camera
   fe = flycaptureStop( flycapture );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureStop()", fe );

   // Delete the image buffer.
   delete rowIntColor;
   redColor = NULL;
   greenColor = NULL;
   blueColor = NULL;   

   fe = flycaptureDestroyContext( flycapture );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureDestroyContext()", fe );
   
   // Destroy the Triclops context
   te = triclopsDestroyContext( triclops ) ;
   _HANDLE_TRICLOPS_ERROR( "triclopsDestroyContext()", te );
         
   return 0;
   
}
