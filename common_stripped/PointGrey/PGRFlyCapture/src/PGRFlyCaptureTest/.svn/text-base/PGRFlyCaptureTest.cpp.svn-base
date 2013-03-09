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
// $Id: PGRFlyCaptureTest.cpp,v 1.29 2007/06/05 17:13:58 soowei Exp $
//=============================================================================

//=============================================================================
//
// PGRFlyCaptureTest.cpp
//
// The PGRFlyCaptureTest sample program is a simple program designed to report 
// information related to all compatible cameras attached to the host system, 
// capture a series of images from a single camera, record the amount of time 
// taken to grab these images, then save the last image in the current 
// directory.
//
// The program first creates two FlyCapture variables, one of which is a
// FlyCaptureError variable. In order to reliably debug your application, 
// define a variable of this type to capture meaningful errors returned by API 
// functions. 
//
// The program begins by calling flycaptureBusEnumerateCamerasEx(). This
// enumerates, or lists, all PGR cameras sitting on the bus and their bus index,
// starting at zero. This function does not enumerate non-PGR cameras.
//
// A FlyCaptureContext is created by calling flycaptureCreateContext(). This 
// acts as a handle to the camera, and is required to initialize and start the 
// camera. All camera specific function calls in the FlyCapture library require 
// a valid context in order to succeed. The program then initializes the camera
// located at bus index zero and associates it with the camera context by
// calling flycaptureInitialize().
//
// Multiple cameras connected to the FireWire bus are enumerated at 
// bus indices (FireWire nodes) that start at 0. Information related to the 
// initialized camera (model, serial number, DCAM compliance, etc.) can be
// retrieved at this time by flycaptureGetCameraInfo().
// 
// The initialized camera is then started at the default resolution and image 
// and frame rate of the camera. This function also (silently) allocates four 
// buffers in main memory that are used to hold the images that are streamed in 
// from the camera. Once a camera has been started, it immediately begins 
// capturing and streaming images via Direct Memory Access (DMA) to these 
// memory buffers. Once these buffers are full, they will be overwritten with 
// consecutive images unless they are locked by the user.
// 
// The program then creates a FlyCaptureImage variable. The FlyCaptureImage 
// structure contains the image data, as well as video mode, whether the image 
// is stippled (color) and timestamp information.
// 
// In the grab loop, when a call to flycaptureGrabImage2() is made, a pointer
// to the image buffer (&image) with the newest (latest) complete image is 
// returned. The call to flycaptureGrabImage2() does not involve copying, so 
// it is quite fast. The user will never be given an image that is older than 
// one that has already been seen. Once the pointer to the buffer is returned 
// to the user, this buffer remains locked until flycaptureGrabImage2() is 
// called again. If no buffer contains an image newer than the last returned, 
// then the flycaptureGrabImage2() call will block until a new image is 
// available.
// 
// After the program exits the grab loop, it converts the last image grabbed to 
// a 32-bit per pixel image that can be displayed by Microsoft Windows (which 
// uses the BGRU format) by calling flycaptureConvertImage(). If the camera is
// not a color camera, Y8 and Y16 images are converted to BGRU greyscale.
// flycaptureSaveImage() is then called to save the converted BGRU image as a
// PPM. The raw image is also saved by calling the same function but passing
// in FLYCAPTURE_FILEFORMAT_PGM as the argument.
// 
// Finally, the program stops the camera using flycaptureStop(). This is
// followed by flycaptureDestroyContext(), which destroys the camera context. In 
// order to prevent memory leaks from occurring, this function must be called 
// when the user is finished with the FlyCaptureContext.
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <assert.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <memory.h>

//=============================================================================
// Project Includes
//=============================================================================
#include <pgrflycapture.h>

//=============================================================================
// Macro Definitions
//=============================================================================
//
// The number of images to grab.
//
#define _IMAGES_TO_GRAB 10

//
// The maximum number of cameras on the bus.
//
#define _MAX_CAMS       32

//
// The index of the camera to grab from.
//
#define _CAMERA_INDEX   0

//
// What file format should we save the processed image as?
//
#define SAVE_FORMAT     FLYCAPTURE_FILEFORMAT_PPM
//#define SAVE_FORMAT     FLYCAPTURE_FILEFORMAT_BMP

#define FILENAME_CONVERTED "converted.ppm"
//#define FILENAME_CONVERTED "converted.bmp"
#define FILENAME_RAW       "raw.pgm"

//
// Register defines
// 
#define INITIALIZE         0x000
#define CAMERA_POWER       0x610

//
// Helper code to handle a FlyCapture error.
//
#define _HANDLE_ERROR( error, function ) \
   if( error != FLYCAPTURE_OK ) \
   { \
      printf( "%s: %s\n", function, flycaptureErrorToString( error ) ); \
      return -1; \
   } \

//=============================================================================
// Functions
//=============================================================================
void
reportCameraInfo( const FlyCaptureInfoEx* pinfo )
{
   //
   // Print out camera information. This can be obtained by calling
   // flycaptureGetCameraInfo() anytime after the camera has been initialized.
   //
   printf( "Serial number: %d\n", pinfo->SerialNumber );
   printf( "Camera model: %s\n", pinfo->pszModelName );
   printf( "Camera vendor: %s\n", pinfo->pszVendorName );
   printf( "Sensor: %s\n", pinfo->pszSensorInfo );
   printf( "DCAM compliance: %1.2f\n", (float)pinfo->iDCAMVer / 100.0 );
   printf( "Bus position: (%d,%d).\n", pinfo->iBusNum, pinfo->iNodeNum );
}

int 
main( int /* argc */, char* /* argv[] */ )
{
   // The Flycapture error. This should be assigned to the return value of 
   // most API functions and checked to ensure that the operation was completed
   // successfully.
   FlyCaptureError   error;

   // This acts as a handle to the camera.
   FlyCaptureContext context;   

   // Structure to store various information about the camera such as
   // model, serial number and DCAM compliance.
   FlyCaptureInfoEx info;

   //
   // Enumerate the cameras on the bus.
   //
   FlyCaptureInfoEx  arInfo[ _MAX_CAMS ];
   unsigned int	     uiSize = _MAX_CAMS;

   //
   // This function enumerates all the cameras found on the machine, across
   // all 1394 buses and cards. It fills an array of FlyCaptureInfoEx 
   // structures with all of the pertinent information from the attached
   // cameras. The index of a given FlyCaptureInfoEx structure in the array 
   // parInfo is the device number.
   //
   error = flycaptureBusEnumerateCamerasEx( arInfo, &uiSize );
   _HANDLE_ERROR( error, "flycaptureBusEnumerateCameras()" );

   for( unsigned int uiBusIndex = 0; uiBusIndex < uiSize; uiBusIndex++ )
   {
      FlyCaptureInfoEx* pinfo = &arInfo[ uiBusIndex ];
      printf( 
         "Index %u: %s (%u)\n",
         uiBusIndex,
         pinfo->pszModelName,
         pinfo->SerialNumber );
   }

   //
   // Create the context. This call must be made before any other calls to
   // to the context are made, as a valid context is needed for all
   // camera-specific FlyCapture library calls. This call sets the context
   // to the default settings, but flycaptureInitialize() must be called
   // below to fully initialize the camera for use.
   //
   error = flycaptureCreateContext( &context );
   _HANDLE_ERROR( error, "flycaptureCreateContext()" );
   
   //
   // Initialize the camera. This call initializes one of the cameras on the
   // bus with the FlyCaptureContext that is passed in. This should generally
   // be called right after creating the context but before doing anything 
   // else.
   //
   // This call performs several functions. It sets the camera to communicate 
   // at the proper bus speed, turns on color processing (if available) and 
   // sets the Bayer orientation to the correct setting. Finally, it also 
   // initializes the white balance tables for cameras that support that 
   // function.
   //
   printf( "Initializing camera %u.\n", _CAMERA_INDEX );
   error = flycaptureInitialize( context, _CAMERA_INDEX );
   _HANDLE_ERROR( error, "flycaptureInitialize()" );

   //
   // Reset the camera to default factory settings by asserting bit 0
   //
   error = flycaptureSetCameraRegister( context, INITIALIZE, 0x80000000 );
   _HANDLE_ERROR( error, "flycaptureSetCameraRegister()" );

   //
   // Power-up the camera (for cameras that support this feature)
   //
   error = flycaptureSetCameraRegister( context, CAMERA_POWER, 0x80000000 );
   _HANDLE_ERROR( error, "flycaptureSetCameraRegister()" );

   //
   // Retrieve information about the camera.
   //
   error = flycaptureGetCameraInfo( context, &info );
   _HANDLE_ERROR( error, "flycaptureGetCameraInfo()" );

   printf( "Camera info:\n" );
   reportCameraInfo( &info );

   //
   // Start grabbing images in the current videomode and framerate. Driver
   // level image buffer allocation is performed at this point. After this
   // point, images will be constantly grabbed by the camera and stored
   // in the buffers on the PC.
   //
   printf( "Starting camera.\n\n" );
   error = flycaptureStart( 
      context, 
      FLYCAPTURE_VIDEOMODE_ANY,
      FLYCAPTURE_FRAMERATE_ANY );
   _HANDLE_ERROR( error, "flycaptureStart()" );

   //
   // Record the time taken to grab _IMAGES_TO_GRAB images.
   //
   FlyCaptureImage image;
   memset( &image, 0x0, sizeof( FlyCaptureImage ) );

   struct _timeb   timeStart;
   struct _timeb   timeFinish;

   _ftime( &timeStart );

   printf( "Grabbing images" );
   for ( int iImage = 0; iImage < _IMAGES_TO_GRAB; iImage++ )
   {
      //
      // Grab an image. This obtains an pointer to the latest full
      // image captured by the camera and saved in the image buffer.
      //
      // flycaptureGrabImage2 is used instead of flycaptureGrabImage because
      // it returns a FlyCaptureImage structure, which is generally easier to 
      // work with.
      //
      error = flycaptureGrabImage2( context, &image );
      _HANDLE_ERROR( error, "flycaptureGrabImage2()" );
      
      printf( "." );
   }

   printf( "\n" );

   _ftime( &timeFinish );

#ifdef WIN64
   __time64_t uiTime = 
#else
   unsigned int uiTime = 
#endif
      ( timeFinish.time * 1000 + timeFinish.millitm ) - 
      ( timeStart.time * 1000 + timeStart.millitm );

   printf( 
      "It took %ums to grab %u images. (%ums per image)\n\n", 
      uiTime,
      _IMAGES_TO_GRAB,
      uiTime / _IMAGES_TO_GRAB );
   
   //
   // Convert the last image that was grabbed.
   //
   // First, allocate memory space for the converted image. The size needed is
   // multiplied by 4 because we are converting to FLYCAPTURE_BGRU, which
   // contains 4 bytes (32 bits) of data per pixel. If converting to a 
   // FLYCAPTURE_BGR image, then the size only needs to be multiplied by 3 as
   // each converted pixel will only contain 3 bytes (24 bits) of data.
   // The pixelFormat field should also be set appropriately either to
   // FLYCAPTURE_BGRU or FLYCAPTURE_BGR.
   //
   FlyCaptureImage imageConverted;
   imageConverted.pData = new unsigned char[ image.iCols * image.iRows * 4 ];
   imageConverted.pixelFormat = FLYCAPTURE_BGRU;

   //
   // Now that the converted image's data structure is filled in correctly and
   // sufficient memory space has been allocated, proceed with converting the 
   // raw image into a BGRU image.
   //
   printf( "Converting last image.\n" );
   error = flycaptureConvertImage( context, &image, &imageConverted );
   _HANDLE_ERROR( error, "flycaptureConvertImage()" );

   //
   // Save the converted image to disk.
   //
   printf( "Saving converted image.\n\n" );
   error = flycaptureSaveImage(
      context,
      &imageConverted,
      FILENAME_CONVERTED,
      SAVE_FORMAT );
   _HANDLE_ERROR( error, "flycaptureSaveImage()" );   

   //
   // Save the raw image to disk.
   //
   printf( "Saving raw image.\n\n" );
   error = flycaptureSaveImage(
      context,
      &image,
      FILENAME_RAW,
      FLYCAPTURE_FILEFORMAT_PGM );
   _HANDLE_ERROR( error, "flycaptureSaveImage()" );   

   //
   // Stop the camera. This does not destroy the context. This simply stops
   // the grabbing of images from the camera. This should always be called 
   // prior to calling flycaptureDestroyContext().
   //
   error = flycaptureStop( context );
   _HANDLE_ERROR( error, "flycaptureStop()" );

   //
   // Destroy the context. This should always be called before exiting
   // the application to prevent memory leaks.
   //
   error = flycaptureDestroyContext( context );
   _HANDLE_ERROR( error, "flycaptureDestroyContext()" );

   delete [] imageConverted.pData;

   printf( "Done!  (hit enter)" );
   getchar();

   return 0;
}
