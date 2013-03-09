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
//
//=============================================================================
//=============================================================================
// $Id: main.cpp,v 1.11 2006/10/10 23:27:16 mgibbons Exp $
//=============================================================================

//=============================================================================
//
// ExtendedShutterEx
// 
// The ExtendedShutterEx sample program demonstrates how to enable and 
// calculate extended integration times for applicable PGR Imaging Products. 
// The way this is done can differ between cameras.
//
// Many applications require extended shutter (integration) times up to several 
// seconds long. Most PGR Imaging Products implement extended shutter 
// functionality in one of two ways:
// 1.	By turning off the FRAME_RATE register 0x83C. This effectively stops 
// the camera from transmitting images at fixed frame intervals; the frame 
// rate becomes dependent on the shutter time. Consult the PGR IEEE-1394 
// Digital Camera Register Reference for more information.
// 2.	By enabling extended shutter via the EXTENDED_SHUTTER register 0x1028.
//
// The program begins by initializing the first camera on the bus and uses 
// flycaptureGetCameraPropertyRangeEx () to determine if it implements the 
// FRAME_RATE register. If it does, it turns the frame rate off. If the camera 
// does not implement this register, the program then checks to see if the 
// camera implements the EXTENDED_SHUTTER register. If it does, it accesses 
// this register to put the camera into extended shutter mode. Otherwise, 
// the user is notified that the camera does not implement extended shutter 
// and the program exits.
//
// Once the camera is in extended shutter mode, it is started in the default 
// mode and frame rate. A series of images are grabbed, and their timestamps 
// printed as a way of verifying that the extended shutter is working. The last
// image is then converted (color processed) and saved to disk.
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <assert.h>
#include <stdio.h>

//=============================================================================
// Project Includes
//=============================================================================
#include <pgrflycapture.h>

#define REGISTER_EXTENDED_GRAB_MODE    0x1028 
#define VALUE_GRAB_MODE_SLOW	       0x80020000

//
// shutter time, in milliseconds
//
#define	SHUTTER_TIME	  3000

//
// Images to grab.
//
#define	IMAGES		5

//
// Register defines
// 
#define INITIALIZE         0x000
#define CAMERA_POWER       0x610

//
// Helper code to handle a FlyCapture error.
//
#define HANDLE_ERROR( function, error ) \
{ \
   if( error != FLYCAPTURE_OK ) \
   { \
   printf( \
   "ERROR: %s reported %s\n", \
   function, \
   ::flycaptureErrorToString( error ) ); \
   return -1; \
   } \
} \
   
int
cleanUpCamera( FlyCaptureContext context )
{
   FlyCaptureError error;
   
   //
   // Stop the camera from transmitting images
   //
   error = ::flycaptureStop( context );
   HANDLE_ERROR( "flycaptureStop()", error );
   
   //
   // destroy the context - note that this might generate a 'could not stop
   // camera'-type error.
   //
   error = ::flycaptureDestroyContext( context );
   HANDLE_ERROR( "flycaptureCreateContext()", error );
   
   return 0;
}

int 
main( int /* argc */, char** /* argv */ )
{
   FlyCaptureError	error;
   FlyCaptureContext	context; 
   FlyCaptureImage	image;
   
   unsigned long ulValue;

   bool	 bPresent;
   bool	 bOnOff;
   bool	 bOnePush;
   bool	 bAuto;
   int	 iValueA;
   int	 iValueB;
   long  lValueA;
   long  lValueB;
   
   error = ::flycaptureCreateContext( &context );
   HANDLE_ERROR( "flycaptureCreateContext()", error );
   
   error = ::flycaptureInitialize( context, 0 );
   HANDLE_ERROR( "flycaptureInitialize()", error );

   //
   // Reset the camera to default factory settings by asserting bit 0
   //
   error = ::flycaptureSetCameraRegister( context, INITIALIZE, 0x80000000 );
   HANDLE_ERROR( "flycaptureSetCameraRegister()", error );

   //
   // Power-up the camera (for cameras that support this feature)
   //
   error = ::flycaptureSetCameraRegister( context, CAMERA_POWER, 0x80000000 );
   HANDLE_ERROR( "flycaptureSetCameraRegister()", error );
   
   //
   // Check if the camera implements the FRAME_RATE register
   //
   printf( "Checking for extended shutter via FRAME_RATE register 83Ch... " );
   error = ::flycaptureGetCameraPropertyRangeEx( 
                                                context,
                                                FLYCAPTURE_FRAME_RATE,
                                                &bPresent,
                                                NULL,
                                                NULL,
                                                &bOnOff,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL );
   HANDLE_ERROR( "flycaptureGetCameraPropertyRangeEx()", error );
   
   if( bPresent )
   {
      //
      // Camera implements the FRAME_RATE register so turn it off
      printf( "supported.\nTurning off frame rate to enable extended shutter.\n" );
      
      error = ::flycaptureGetCameraPropertyEx( 
                                                context,
                                                FLYCAPTURE_FRAME_RATE,
                                                &bOnePush,
                                                &bOnOff,
                                                &bAuto,
                                                &iValueA,
                                                &iValueB );
      HANDLE_ERROR( "flycaptureGetCameraPropertyEx()", error );
      
      // 
      // Turn frame rate off
      //
      error = ::flycaptureSetCameraPropertyEx( 
                                               context,
                                               FLYCAPTURE_FRAME_RATE,
                                               bOnePush,
                                               false,
                                               bAuto,
                                               iValueA,
                                               iValueB );
      HANDLE_ERROR( "flycaptureSetCameraPropertyEx()", error );   
   }
   //
   // Camera doesn't use FRAME_RATE - check if it implements
   // the EXTENDED_SHUTTER register
   //
   else
   {
      error = ::flycaptureGetCameraRegister( 
                                             context,
                                             REGISTER_EXTENDED_GRAB_MODE,
                                             &ulValue );
      HANDLE_ERROR( "flycaptureGetCameraRegister()", error );

      // 
      // Check if the extended shutter mode is present on this camera
      //
      if( ( ulValue & 0x80000000 ) == 0x80000000 )
      {
         //
         // Go into extended shutter mode
         //
         printf( "using EXTENDED_SHUTTER register 1028h.\n" );
         error = ::flycaptureSetCameraRegister( 
                                                context,
                                                REGISTER_EXTENDED_GRAB_MODE,
                                                VALUE_GRAB_MODE_SLOW );
         HANDLE_ERROR( "flycaptureSetCameraRegister()", error );
      }
      else
      {
         printf( "extended shutter not available with this camera.\n" );
         if( cleanUpCamera( context ) != 0 )
         {
            printf( "Failed to clean up camera.\n" );
         }
         return 0;
      }
   }
   
   error = ::flycaptureGetCameraProperty(
                                          context,
                                          FLYCAPTURE_SHUTTER,
                                          &lValueA,
                                          &lValueB,
                                          &bAuto );
   HANDLE_ERROR( "flycaptureGetCameraProperty()", error );

   error = ::flycaptureSetCameraProperty( 
                                          context,
                                          FLYCAPTURE_SHUTTER,
                                          lValueA,
                                          lValueB,
                                          false );
   HANDLE_ERROR( "flycaptureSetCameraProperty()", error );

   printf( "Using shutter time %dms\n", SHUTTER_TIME );
   
   error = ::flycaptureSetCameraAbsProperty( 
                                             context,
                                             FLYCAPTURE_SHUTTER,
                                             SHUTTER_TIME );
   HANDLE_ERROR( "flycaptureSetCameraAbsProperty()", error );
   
   //
   // The maximum shutter time for DCAM 1.31 cameras will vary according
   // to the frame rate.
   //
   error = ::flycaptureStart( 
                              context, 
                              FLYCAPTURE_VIDEOMODE_ANY, 
                              FLYCAPTURE_FRAMERATE_ANY );
   HANDLE_ERROR( "flycaptureStart()", error );
   
   //
   // Do an image grab.
   //
   error = ::flycaptureGrabImage2( context, &image );
   HANDLE_ERROR( "flycaptureGrabImage2()", error );
   
   //
   // Grab a few more images and print out their timestamps
   //
   for( int iImage = 0; iImage < IMAGES; iImage++ )
   {
      error = ::flycaptureGrabImage2( context, &image );
      HANDLE_ERROR( "flycaptureGrabImage2()", error );
      
      printf( 
         "Image %03d - system timestamp = %d.%d\n",
         iImage,
         image.timeStamp.ulSeconds,
         image.timeStamp.ulMicroSeconds	  );
   }
   
   //
   // Convert the last image and save it
   //
   FlyCaptureImage imageConverted;
   imageConverted.pData = new unsigned char[ image.iCols * image.iRows * 4 ];
   imageConverted.pixelFormat = FLYCAPTURE_BGRU;

   printf( "Converting last image.\n" );
   error = flycaptureConvertImage( context, &image, &imageConverted );
  
   if( error == FLYCAPTURE_OK )
   {
      //
      // Write out the image in PPM format.
      //
      printf( "Saving converted image.\n\n" );
      error = flycaptureSaveImage( 
                                   context,
                                   &imageConverted,
                                   "extendedshutterdemoimage.ppm",
                                   FLYCAPTURE_FILEFORMAT_PPM );
      HANDLE_ERROR( "flycaptureSaveImage()", error );
   }

   HANDLE_ERROR( "flycaptureConvertImage()", error );
  
   if( cleanUpCamera( context ) != 0 )
   {
      printf( "Failed to clean up camera.\n" );
   }

   delete imageConverted.pData;
   
   return 0;
}
