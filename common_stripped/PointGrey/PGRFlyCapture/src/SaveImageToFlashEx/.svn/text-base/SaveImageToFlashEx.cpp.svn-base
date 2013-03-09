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
// $Id: SaveImageToFlashEx.cpp,v 1.2 2007/11/16 21:04:23 soowei Exp $
//=============================================================================

//=============================================================================
//
// SaveImageToFlashEx.cpp
//
// The SaveImageToFlashEx sample program is a simple program designed to report 
// information related to all compatible cameras attached to the host system, 
// capture an image from a single camera, and either save it to the cameras 
// flash memory, or retrieve an image from flash and save it to disk.
//
// The program first creates two FlyCapture variables, one of which is a
// FlyCaptureError variable. In order to reliably debug your application, 
// define a variable of this type to capture meaningful errors returned by API 
// functions. The program then enumerates, or lists, all PGR cameras sitting 
// on the bus and their bus index, starting at zero. This function does not 
// enumerate non-PGR cameras.
//
// A FlyCaptureContext is created, which acts as a handle to the camera, and 
// is required to initialize and start the camera. The program then initializes 
// the camera located at bus index zero and associates it with the camera 
// context. Multiple cameras connected to the FireWire bus are enumerated at 
// bus indices (FireWire nodes) that start at 0. Information related to the 
// initialized camera (model, serial number, DCAM compliance, etc.) is then 
// reported to the user.
// 
// Since the size of the FLASH is typically smaller than the maximum resolution
// of the camera, we query the format7 parameters to get information on the
// maximum image and unit size the camera can support.  Use the DATA_FLASH_CTRL
// register to ensure a FLASH is available, and the size of that flash area.
// Using the available flash size, determine the maximum image which can fit
// in the flash while keeping a 4:3 aspect ration.  The camera can then be started
// in format 7 with the appropriate image size, and an image is grabbed.  Since
// after starting the camera, it takes some time for the auto settings to stabablize
// (such as the gain, exposure, etc), multiple images are taken before the image
// to save is captured.
//
// Use the DATA_FLASH_DATA register to calculate the starting point of the Flash.
// If the parameter passed in is "-capture", the grabbed image is saved into the
// flash area.  If the parameter passed in is "-retrieve", a new FlyCaptureImage
// is created to build up an image to save.  Although the data is coming from
// the flash, the other parameters such as rowInc, iCols, iRows, etc. are needed
// to save the image.
// 
// Finally, the program stops the camera and destroys the camera context. In 
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
#include "pgrflycaptureplus.h"
#include "malloc.h"
#include <cmath>
#include <tchar.h>

//=============================================================================
// Project Includes
//=============================================================================
#include <pgrflycapture.h>

//=============================================================================
// Macro Definitions
//=============================================================================
//
// The number of images to grab before saving an image
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
#define SAVE_FORMAT              FLYCAPTURE_FILEFORMAT_PPM
#define FILENAME_RAW             "image_raw.pgm"
#define FILENAME_RAW_FROM_FLASH  "image_from_flash.pgm"

//
// Register defines
// 
#define INITIALIZE         0x000
#define CAMERA_POWER       0x610
#define DATA_FLASH_CTRL    0x1240
#define DATA_FLASH_DATA    0x1244

#define MODE 0

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
void PrintUsage()
{
   printf("\nUsage:\n\nSaveImageToFlashEx [-capture | -retrieve ]\n");
   printf("\n\twhere \n\n\t-capture  = Used to capture an image in on-camera FLASH\n");
   printf("\t-retrieve = Used to retrieve an image from FLASH and save it to disk.\n\n");
} // end PrintUsage()

void
reportCameraInfo( const FlyCaptureInfoEx* pinfo )
{
   printf( "Serial number: %d\n", pinfo->SerialNumber );
   printf( "Camera model: %s\n", pinfo->pszModelName );
   printf( "Camera vendor: %s\n", pinfo->pszVendorName );
   printf( "Sensor: %s\n", pinfo->pszSensorInfo );
   printf( "DCAM compliance: %1.2f\n", (float)pinfo->iDCAMVer / 100.0 );
   printf( "Bus position: (%d,%d).\n", pinfo->iBusNum, pinfo->iNodeNum );
}

int 
main( int argc, char* argv[] )
{
   FlyCaptureError   error;
   FlyCaptureContext context;

   // check the arguments of the call to make sure the utility is being called properly.
   if (argc != 2){
      PrintUsage();

      printf( "Done!  (hit enter)" );
      getchar();

      return 0;
   } else {
      if (!((strcmp(argv[1], "-retrieve") == 0) || (strcmp(argv[1], "-capture") == 0)))
      {
	 PrintUsage();  

	 printf( "Done!  (hit enter)" );
	 getchar();

	 return 0;
      }
   }

   //
   // Enumerate the cameras on the bus.
   //
   FlyCaptureInfoEx  arInfo[ _MAX_CAMS ];
   unsigned int	     uiSize = _MAX_CAMS;

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
   // Create the context.
   //
   error = flycaptureCreateContext( &context );
   _HANDLE_ERROR( error, "flycaptureCreateContext()" );
   
   //
   // Initialize the camera.
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
   // Report camera info.
   //
   FlyCaptureInfoEx info;
   error = flycaptureGetCameraInfo( context, &info );
   _HANDLE_ERROR( error, "flycaptureGetCameraInfo()" );

   printf( "Camera info:\n" );
   reportCameraInfo( &info );

   //
   // Query and report on the camera's ability to handle custom image modes.
   // We use the maximum and unit values to determine the size of the image
   // which can be saved to flash.
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
   _HANDLE_ERROR( error, "flycaptureQueryCustomImageEx()" );

   //
   // Check to see if Flash Reading/Writing is supported by the camera
   //
   unsigned long ulRegVal;
   error = flycaptureGetCameraRegister(context, DATA_FLASH_CTRL, &ulRegVal);
   printf( "Data control register = %x\n", ulRegVal );

   if((0x80000000 & ulRegVal) == 0)
   {
      printf("This camera does not support the user data area feature. Exiting...\n");
     
      //
      // Destroy the context
      //
      flycaptureDestroyContext(context);

      printf( "Done!  (hit enter)" );
      getchar();

      return 0;
   }

   // Determine the available size of the Flash from the DATA_FLASH_CTRL register
   int iPageSize = (int)pow(2.0, (int)(ulRegVal & 0x00FFF000) >> 12);
   int iNumPages = (int)pow(2.0, (int)(ulRegVal & 0x00000FFF));
   unsigned int uiAvailableFlashSize = iPageSize * iNumPages;

   unsigned int uiHeight = uiMaxImageSizeCols;
   unsigned int uiWidth = uiMaxImageSizeRows;

   // If the Flash is not large enough to hold a full, high-res image, determine the
   // maximum image with a 4:3 aspect ration which can fit in the flash and use those
   // dimensions.
   if (uiMaxImageSizeCols*uiMaxImageSizeRows > uiAvailableFlashSize)
   {
      uiHeight = (int)sqrt(uiAvailableFlashSize*3.0/4);
      uiWidth = uiHeight*4/3;

      uiHeight -= (uiHeight % uiImageUnitSizeVert);
      uiWidth -= (uiWidth % uiImageUnitSizeHorz);
   }

   //
   // Determine the quadlet offset of the Flash area
   //
   unsigned long ulLUTLoc;
   error = flycaptureGetCameraRegister(context, DATA_FLASH_DATA, &ulLUTLoc);
   _HANDLE_ERROR( error, "flycaptureGetCameraRegister()" );   

   //
   // Start grabbing images in the current videomode and framerate.
   //
   printf( "Starting camera.\n\n" );
   error = flycaptureStartCustomImage(context, 0, 0, 0, uiWidth, uiHeight, 40, FLYCAPTURE_MONO8);
   _HANDLE_ERROR( error, "flycaptureStart()" );

   FlyCaptureImage image;
   memset( &image, 0x0, sizeof( FlyCaptureImage ) );

   // Here, we grab 10 images and only look at the last one, since after starting the camera,
   // it takes a few images for the the auto settings to stabilize (ie. exposure, gain, etc)
   for ( int iImage = 0; iImage < _IMAGES_TO_GRAB; iImage++ )
   {
      error = flycaptureGrabImage2( context, &image );
      _HANDLE_ERROR( error, "flycaptureGrabImage2()" );
   }

   // Check to see if we are capturing or retrieving
   if (strcmp(argv[1], "-capture") == 0)
   {

      printf( "Saving raw image to camera FLASH.\n\n" );

      //
      // Write the image to the cameras flash
      //
      error =  flycaptureWriteRegisterBlock(
	 context,
	 0xFFFF,
	 0xF0000000+ulLUTLoc*4,
	 (const unsigned long*)&(image.pData[0]),
	 (image.iRowInc*image.iRows/4));
      _HANDLE_ERROR( error, "flycaptureWriteRegisterBlock()" );

   } else if (strcmp(argv[1], "-retrieve") == 0){ // if we are not capturing (we are retrieving).

      printf( "Grabbing image from FLASH and saving it to disk as %s.\n\n", FILENAME_RAW_FROM_FLASH );

      // Create a flycapture image to save
      FlyCaptureImage savedImage;
      memset( &savedImage, 0x0, sizeof( FlyCaptureImage ) );

      // Fill in the savedImage structure
      savedImage.iCols = image.iCols;
      savedImage.iRows = image.iRows;
      savedImage.iRowInc = image.iRowInc;
      savedImage.videoMode = image.videoMode;
      savedImage.timeStamp = image.timeStamp;
      savedImage.bStippled = image.bStippled;
      savedImage.pixelFormat = image.pixelFormat;
      savedImage.iNumImages = image.iNumImages;
      savedImage.pData = (unsigned char*)malloc(savedImage.iRowInc*savedImage.iRows);

      // Read data from the flash into the SavedImage structure
      error = flycaptureReadRegisterBlock(
	 context,
	 0xFFFF,
	 0xF0000000+ulLUTLoc*4,
	 (unsigned long*)&(savedImage.pData[0]),
	 (savedImage.iRowInc*savedImage.iRows/4));
      _HANDLE_ERROR( error, "flycaptureReadRegisterBlock()" );   

      // Save the image to disk
      error = flycaptureSaveImage(
	 context,
	 &savedImage,
	 FILENAME_RAW_FROM_FLASH,
	 FLYCAPTURE_FILEFORMAT_PGM );
      _HANDLE_ERROR( error, "flycaptureSaveImage()" );

      free(savedImage.pData);

   } else {

      // We should never get here since the check at the top should catch this,
      // but this is here for completeness.
      PrintUsage();

   }

   //
   // Stop the camera
   //
   error = flycaptureStop( context );
   _HANDLE_ERROR( error, "flycaptureStop()" );

   //
   // Destroy the context.
   //
   error = flycaptureDestroyContext( context );
   _HANDLE_ERROR( error, "flycaptureDestroyContext()" );

   printf( "Done!  (hit enter)" );
   getchar();

   return 0;
}