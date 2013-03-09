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
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: main.cpp,v 1.14 2006/08/31 21:48:35 mgibbons Exp $
//=============================================================================

//=============================================================================
//
// CustomImageEx
// 
// The CustomImageEx sample program demonstrates how to configure a PGR Imaging 
// Product to output custom sized images - the PGRFlycapture equivalent of the
// DCAM specifications 'Format 7'. Custom image modes are often useful for 
// achieving faster frame rates, reducing the resolution of an image, and 
// allowing more cameras to run on a single bus by reducing bandwidth 
// requirements.
//
// The program creates a context and initializes the first camera on the 1394 
// bus. It then queries the camera to determine the custom image modes, 
// resolution sizes, unit sizes and pixel formats the camera supports. The 
// information returned by flycaptureQueryCustomImageEx() is the same kind of 
// information you would see in FlyCap using the Custom Image tab.
//
// The program then starts the camera in custom image mode using parameters 
// defined at the beginning of the code. Calling flycaptureStartCustomImage() 
// with these parameters is essentially the same thing as setting these 
// parameters in FlyCap and clicking "Set". A number of images are grabbed in 
// this custom image mode and image timestamp information is printed out. 
// The program also calculates the time difference between consecutive images, 
// then uses the total amount of time taken to calculate the actual frame rate 
// of the camera given the current custom image configuration. The final image 
// is then saved in an 8-bit format to disk.
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "pgrflycapture.h"
#include "pgrflycaptureplus.h"

//=============================================================================
// Macro Defintions
//=============================================================================
#define IMAGES_TO_GRAB	200

#define MODE		0
#define START_COL	32
#define START_ROW	32
#define COLS		256
#define	ROWS		256
#define	SPEED		100.0
#define PIXEL_FORMAT    FLYCAPTURE_MONO8

//
// Register defines
// 
#define INITIALIZE         0x000
#define CAMERA_POWER       0x610

//
// Small macro to help handle error checking
//
#define CHECK_ERROR( function, error ) \
{ \
   if( error != FLYCAPTURE_OK ) \
   { \
      printf( \
	 "ERROR: %s returned \"%s\" (%d).\n", \
	 function, \
	 ::flycaptureErrorToString( error ), \
	 error ); \
      \
      ::exit( 1 ); \
   } \
} \

//=============================================================================
// Functions
//=============================================================================
//
// saveFinalImage()
//   This function is responsible for saving the last image to disk.  If the 
//   image is an unprocessed color image, this command processes it before
//   saving it to disk.  Otherwise, it simply stores it to disk.
//
FlyCaptureError
saveFinalImage(FlyCaptureContext context, FlyCaptureImage *pImage)
{

   FlyCaptureError error = FLYCAPTURE_OK;

   if(pImage->bStippled)
   {
      FlyCaptureImage colorImage;
      colorImage.pData       = new unsigned char[pImage->iRows*pImage->iCols*3];
      colorImage.pixelFormat = FLYCAPTURE_BGR;
      
      error = flycaptureConvertImage(context,
	 pImage,
	 &colorImage);
      if(error != FLYCAPTURE_OK)
      {
         delete [] colorImage.pData;	 
	 return error;
      }
      
      error = flycaptureSaveImage(
	 context,
	 &colorImage,
	 "format7image.ppm",
	 FLYCAPTURE_FILEFORMAT_PPM);
      CHECK_ERROR( "flycaptureSaveImage", error );

      delete [] colorImage.pData;      

      if(error != FLYCAPTURE_OK)
      {
	 return error;
      }
   }
   else
   {
      error = flycaptureSaveImage(
	 context,
	 pImage,
	 "format7image.pgm",
	 FLYCAPTURE_FILEFORMAT_PGM );
      if(error != FLYCAPTURE_OK)
      {
	 return error;
      }

   }
   
   return error;
}

//=============================================================================
// Main Function
//=============================================================================
int 
main( int /* argc */, char* /* argv[] */ )
{
   FlyCaptureError   error;
   FlyCaptureContext context;   

   bool	             bOn;

   unsigned int      uiCurTime     = 0;
   unsigned int      uiLastTime    = 0;
   unsigned int      uiTotalTime   = 0;
   unsigned int      uiSeconds     = 0;
   unsigned int      uiCount	   = 0;
   unsigned int      uiOffset      = 0;

   double            dGrabTime     = 0.0;

   //
   // Create context
   //
   error = ::flycaptureCreateContext( &context );
   CHECK_ERROR( "flycaptureCreateContext()", error );

   //
   // Initialize first camera on the bus.
   //
   error = ::flycaptureInitialize( context, 0 );
   CHECK_ERROR( "flycaptureInitialize()", error );

   //
   // Reset the camera to default factory settings by asserting bit 0
   //
   error = flycaptureSetCameraRegister( context, INITIALIZE, 0x80000000 );
   CHECK_ERROR( "flycaptureSetCameraRegister()", error );

   //
   // Power-up the camera (for cameras that support this feature)
   //
   error = flycaptureSetCameraRegister( context, CAMERA_POWER, 0x80000000 );
   CHECK_ERROR( "flycaptureSetCameraRegister()", error );

   //
   // Enable image timestamping
   //
   error = ::flycaptureGetImageTimestamping( context, &bOn );
   CHECK_ERROR( "flycaptureGetImageTimestamping()", error );

   if( !bOn )
   {
      error = ::flycaptureSetImageTimestamping( context, true );
      CHECK_ERROR( "flycaptureSetImageTimestamping()", error );
   }

   //
   // Query and report on the camera's ability to handle custom image modes.
   //
   bool		  bAvailable;
   unsigned int	  uiMaxImageSizeCols;
   unsigned int	  uiMaxImageSizeRows;
   unsigned int	  uiImageUnitSizeHorz;
   unsigned int	  uiImageUnitSizeVert;
   unsigned int   uiOffsetUnitSizeHorz;
   unsigned int   uiOffsetUnitSizeVert;
   unsigned int   uiPixelFormats;

   error = ::flycaptureQueryCustomImageEx(
      context,
      MODE,
      &bAvailable,
      &uiMaxImageSizeCols,
      &uiMaxImageSizeRows,
      &uiImageUnitSizeHorz,
      &uiImageUnitSizeVert,
      &uiOffsetUnitSizeHorz,
      &uiOffsetUnitSizeVert,
      &uiPixelFormats );
   CHECK_ERROR( "flycaptureQueryCustomImage()", error );

   if( !bAvailable )
   {
      printf( 
         "Warning!  Camera reports that mode %u is not available.\n",
         MODE );
   }

   printf( 
      "Max image pizels: (%u, %u)\n"
      "Image Unit size: (%u, %u)\n"
      "Offset Unit size: (%u, %u)\n"
      "Pixel format bitfield: 0x%08x\n",
      uiMaxImageSizeCols,
      uiMaxImageSizeRows,
      uiImageUnitSizeHorz,
      uiImageUnitSizeVert,
      uiOffsetUnitSizeHorz,
      uiOffsetUnitSizeVert,
      uiPixelFormats );

   if( ( PIXEL_FORMAT & uiPixelFormats ) == 0 )
   {
      printf( 
         "Warning!  "
         "Camera reports that the requested pixel format is not supported!.\n",
         MODE );
   }

   //
   // Start camera using custom image size mode.
   //
   error = ::flycaptureStartCustomImage(
      context, 
      MODE, 
      START_COL, 
      START_ROW, 
      COLS, 
      ROWS, 
      SPEED, 
      PIXEL_FORMAT );
   CHECK_ERROR( "flycaptureStartCustomImage()", error );


   //
   // Grab a series of images, computing the time difference
   // between consecutive images.
   //
   FlyCaptureImage image = { 0 };
   for( int iImage = 0; iImage < IMAGES_TO_GRAB; iImage++ )
   {
      //
      // Grab an image
      //
      error = ::flycaptureGrabImage2( context, &image );
      CHECK_ERROR( "flycaptureGrabImage2()", error );

      //
      // Calculate the time difference between current and last image
      // in order to calculate actual frame rate
      //
      error = ::flycaptureParseImageTimestamp( context,
					       image.pData,
					       &uiSeconds,
					       &uiCount,
					       &uiOffset );
      CHECK_ERROR( "flycaptureParseImageTimestamp()", error );
      
      uiCurTime = (uiSeconds * 8000) + uiCount;
      
      if( iImage == 0 )
      {
	 uiLastTime = uiCurTime;
	 uiTotalTime = 0;
      }
      else
      {
	 uiTotalTime = uiTotalTime + (uiCurTime - uiLastTime);
	 uiLastTime = uiCurTime;
      }
      
      //
      // Print info.
      //
      printf(
	 "Image %03d: %d x %d %d %d %d %d\n",
	 iImage,
	 image.iCols,
	 image.iRows,
	 image.timeStamp.ulSeconds,
	 image.timeStamp.ulMicroSeconds,
	 image.timeStamp.ulCycleSeconds,
	 image.timeStamp.ulCycleCount  );
   }

   // 
   // Convert to a frames per second number
   //
   dGrabTime = (double)(1 / ( ((double)uiTotalTime / (double)8000) 
	       / IMAGES_TO_GRAB ));
   printf("Frame rate: %lfHz\n", dGrabTime );

   //
   // Save the last image to disk
   //
   printf( "Saving last image..." );
   saveFinalImage(context, &image);
   printf( "done\n" );

   //
   // stop the camera and destroy the context.
   //
   ::flycaptureStop( context );
   ::flycaptureDestroyContext( context );

   return 0;
}
