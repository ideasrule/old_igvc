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
// $Id: main.cpp,v 1.10 2006/09/28 16:54:39 demos Exp $
//=============================================================================

//=============================================================================
//
// CustomBuffersEx
// 
// The CustomBuffersEx sample program demonstrates the custom buffer and 
// "locking" functionality available in the PGR FlyCapture library.  The 
// ability to use custom or user allocated buffers is beneficial to users who 
// do not have flexibility with respect to where images are written to in 
// memory.  Enabling the 'Lock Next' funtionality as opposed to the standard 
// 'Lock Latest' method is useful when trying to avoid missing images.  
// In this case, the library hands the user the next image that hasn't been 
// seen as opposed to the latest image.
//
// The program begins by initializing a block of memory that is large enough 
// to accommodate the image data held in all of the image buffers. The size of 
// this block depends on the number of buffers being allocated, the 
// number of bytes in each raw image, and whether the images have been color 
// processed or not.
//
// The program initializes the camera using flycaptureInitializePlus(), which 
// effectively tells the PGR FlyCapture driver to DMA the images to the block 
// of memory pointed to by arpBuffers. 
//
// In this example, we allocate the size of arpBuffers manually, and pass that 
// into flycaptureInitializePlus().  When allocating your own buffers, you must 
// take padding into account.  The maximum amount of padding required is 1 packet,
// which can be up to 4096 bytes for 1394a and 8192 bytes for 1394b.  Adding this 
// padding to the image size will ensure the buffer is large enough to 
// accomodate the image.

// The camera is then started using either flycaptureStartLockNext(), which 
// initializes the library to use the "lock next" functionality, or 
// flycaptureStart(), which initializes the library normally to use only the 
// four buffers automatically allocated by the API.
// 
// The program then enters a grab loop, which will use either 
// flycaptureLockNext() to lock the "next" image that has not been seen, or 
// flycaptureLockLatest() to lock the "latest" image that has not been seen. 
// Using flycaptureLockNext is preferable when the goal is to avoid missed 
// images. The program looks at the sequence numbers of each of the images and 
// determines whether any images have been missed (due to earlier images being 
// overwritten). If specified, it also color processes any images and saves 
// them to disk. Finally, it unlocks the current image.
//
// One thing to note is that the example provides two methods for saving data
// to disk.  The first involves streaming the raw data to a single file.  
// Although it will require a post processing step, this method will provide 
// the best performance in terms of lost images.  The other method involves
// storing individual images to the disk.  This can be somewhat slower and
// may result performance issues when storing large numbers of images.
//
// Once the grab loop is completed, the camera is stopped, the memory 
// allocated at the beginning of the program freed, and the number of missed 
// images reported.
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
#include "pgrflycapture.h"
#include "pgrflycaptureplus.h"

//=============================================================================
// Type Definitions
//=============================================================================
typedef enum _GrabType
{
   LOCK_NEXT,
   LOCK_LATEST,

} GrabType;

//=============================================================================
// Macro Definitions
//=============================================================================
//
// Helper code to handle a FlyCapture error.
//
#define HANDLE_ERROR( error, function ) \
   if( error != FLYCAPTURE_OK ) \
   { \
      printf( "%s: %s\n", function, flycaptureErrorToString( error ) ); \
      return -1; \
   } \

//
// Camera bus index to use.  0 = first camera on the bus.
//
#define CAMERA_INDEX 0

//
// Use the default video mode and frame rate of the camera
//
#define VIDEOMODE   FLYCAPTURE_VIDEOMODE_ANY
#define FRAMERATE   FLYCAPTURE_FRAMERATE_ANY

//
// Maximum size of expected (raw) image.  You must adjust this based on
// the above videomode.  To ensure the buffer is large enough to accomodate
// padding, add 4096 bytes for 1394a or 8192 bytes for 1394b to the image
// size (see comments in header for more details).
//
#define MAX_IMAGE_SIZE 1600*1200

//
// Images to grab.
//
#define IMAGES_TO_GRAB 10

//
// Buffers to use for grabbing.
//
#define BUFFERS 8

//
// Grab type to use for testing
//
#define GRAB_TYPE   LOCK_NEXT

//
// Register defines
// 
#define INITIALIZE         0x000
#define CAMERA_POWER       0x610

//=============================================================================
// Optional Macro Defintions
//=============================================================================
//
// Should we color process the image?  Assumes a stippled image!
//
#define COLOR_PROCESS

// 
// Defining this parameter will cause the software to stream the raw images to
// a single file.  This will provide the best performance when trying to write
// large numbers of files to disk.
//
// Increasing BUFFERS and using GRAB_TYPE = LOCK_NEXT will help reduce the
// the chance of images being lost.
//
#define STREAM_RAW_TO_DISK

//
// Defining this parameter will result in individual images being stored to 
// disk as either PPMs or PGMS.
//
//#define SAVE_IMAGES

//
// Filename to stream the raw images to.
//
#define STREAM_FILENAME ".\\stream.pgr"


//=============================================================================
// Function Definitions
//=============================================================================
//
// allocateImageBuffers()
//
// This function allocates a series of image buffers.
//
void
allocateImageBuffers(unsigned char ***arpBuffers)
{
   *arpBuffers = new unsigned char*[ BUFFERS ];

   for( unsigned i = 0; i < BUFFERS; i++ )
   {
      (*arpBuffers)[ i ] = new unsigned char[ MAX_IMAGE_SIZE ];
   }
}

//
// deallocateImageBuffers()
//
// This function destroys previously allocated image buffers.
//
void
deallocateImageBuffers(unsigned char ***arpBuffers)
{
   if( *arpBuffers != NULL )
   {
      for( unsigned i = 0; i < BUFFERS; i++ )
      {
         if( (*arpBuffers)[ i ] != NULL )
         {
            delete [] (*arpBuffers)[ i ];
            (*arpBuffers)[ i ] = NULL;
         }
      }      

      delete [] *arpBuffers;
      *arpBuffers = NULL;
   }
}


//
// doGrabLoop()
//
// This function performs all of the grabbing, processing and writing to disk
// functionality of the software.
//
int
doGrabLoop(FlyCaptureContext context)
{
   FlyCaptureError      error           = FLYCAPTURE_OK;
   FlyCaptureImagePlus  imagePlus       = {0};
   FlyCaptureImageFileFormat fileFormat = FLYCAPTURE_FILEFORMAT_PGM;
   FlyCaptureImage      *imageToWrite   = &imagePlus.image;
   char                 *szFileSuffix   = ".pgm";
   unsigned int         uiPrevSeqNum    = 0;
   unsigned int         uiDelta;
   unsigned int         uiMissedImages  = 0;

#ifdef COLOR_PROCESS
   FlyCaptureImage	   colorImage;
   colorImage.pixelFormat = FLYCAPTURE_BGR;
   colorImage.pData       = new unsigned char[ MAX_IMAGE_SIZE * 3 ];
#endif

#ifdef STREAM_RAW_TO_DISK
   FILE           *pFile = ::fopen( STREAM_FILENAME, "wb" );
   assert( pFile != NULL );
#endif

   printf( "Starting to grab...\n" );
   for( unsigned int uiImage = 0; uiImage < IMAGES_TO_GRAB; uiImage++ )
   {
      switch( GRAB_TYPE )
      {
      case LOCK_NEXT:
         error = ::flycaptureLockNext( context, &imagePlus );
         break;

      case LOCK_LATEST:
         error = ::flycaptureLockLatest( context, &imagePlus );
         break;

      default:
	 error = FLYCAPTURE_FAILED;
         assert( false );
      }

      HANDLE_ERROR(error,"flycaptureLock*()");

      assert( imagePlus.image.iRows * imagePlus.image.iRowInc <= MAX_IMAGE_SIZE );

      if( uiImage == 0 )
      {
         uiPrevSeqNum = imagePlus.uiSeqNum;
         uiDelta = 1;
      }
      else
      {
         uiDelta = imagePlus.uiSeqNum - uiPrevSeqNum;
      }

      //
      // Perform color processing on the image and stream and/or save images 
      // to disk as defined by the macros.
      //
#ifdef COLOR_PROCESS
      error = ::flycaptureConvertImage(
         context, &imagePlus.image, &colorImage);
      HANDLE_ERROR( error, "flycaptureConvertImage()" );

      imageToWrite = &colorImage;
      fileFormat   = FLYCAPTURE_FILEFORMAT_PPM;
      szFileSuffix = ".ppm";
#endif

#ifdef STREAM_RAW_TO_DISK
      size_t length = ::fwrite( 
         imagePlus.image.pData, 
         imagePlus.image.iRows * imagePlus.image.iRowInc,
         1,
         pFile );

      assert( length == 1 );
#endif
      
#ifdef SAVE_IMAGES
      char szFileName[100];
      sprintf(szFileName,"%d%s",uiImage,szFileSuffix);
      error = ::flycaptureSaveImage(context,
	 imageToWrite,
	 szFileName,
	 fileFormat);
      HANDLE_ERROR( error, "flycaptureSaveImage()");
#endif

      //
      // Display the image number, the sequence number and the change in 
      // the sequence number since the last image.
      //
      printf( 
         "Image %04u: sq = %03u, d = %u\n", 
         uiImage, 
         imagePlus.uiSeqNum,
         uiDelta );

      if( uiDelta != 1 )
      {
         // we have missed an image.
         uiMissedImages += uiDelta - 1;
      }

      uiPrevSeqNum = imagePlus.uiSeqNum;

      //
      // Release the image as we have finished with it.
      //

      error = ::flycaptureUnlock( context, imagePlus.uiBufferIndex );
      HANDLE_ERROR(error, "flycaptureUnlock()");
   }

   printf( "\nMissed images = %u.\n", uiMissedImages );

   //
   // clean up as required by the defined macros.
   //
#ifdef COLOR_PROCESS
   if( colorImage.pData != NULL )
   {
      delete [] colorImage.pData;
   }
#endif

#ifdef STREAM_RAW_TO_DISK
   ::fclose( pFile );
#endif

   return 0;

}

//=============================================================================
// Main Program
//=============================================================================
int 
main( int /* argc */, char* /* argv[] */ )
{
   FlyCaptureContext context;
   FlyCaptureError   error;
   unsigned char**   arpBuffers = NULL;

   //
   // Allocate all of the necessary image buffers.
   //
   allocateImageBuffers( &arpBuffers );

   //
   // Create and initialize the camera context
   //
   error = ::flycaptureCreateContext( &context );
   HANDLE_ERROR( error, "flycaptureCreateContext()" );
   
   printf( "Initializing camera %u.\n", CAMERA_INDEX );
   error = ::flycaptureInitializePlus( 
      context, 
      CAMERA_INDEX,
      BUFFERS,
      arpBuffers );
   HANDLE_ERROR( error, "flycaptureInitializePlus()" );

   //
   // Reset the camera to default factory settings by asserting bit 0
   //
   error = flycaptureSetCameraRegister( context, INITIALIZE, 0x80000000 );
   HANDLE_ERROR( error, "flycaptureSetCameraRegister()" );

   //
   // Power-up the camera (for cameras that support this feature)
   //
   error = flycaptureSetCameraRegister( context, CAMERA_POWER, 0x80000000 );
   HANDLE_ERROR( error, "flycaptureSetCameraRegister()" );

   //
   // Start the camera in either LockNext or standard mode.
   //
   printf( "Starting camera.\n\n" );
   switch( GRAB_TYPE )
   {
   case LOCK_NEXT:
      error = ::flycaptureStartLockNext( context, VIDEOMODE, FRAMERATE );
      break;
      
   case LOCK_LATEST:
      error = ::flycaptureStart( context, VIDEOMODE, FRAMERATE );
      break;
      
   default:
      assert( false );
   }
   HANDLE_ERROR( error, "flycaptureStart*()" );

   //
   // Do grabbing
   //
   doGrabLoop(context);
   
   //
   // Stop the camera and destroy the context.
   //
   error = ::flycaptureStop( context );
   HANDLE_ERROR( error, "flycaptureStop()" );

   error = ::flycaptureDestroyContext( context );
   HANDLE_ERROR( error, "flycaptureBusEnumerateCameras()" );

   //
   // Deallocate the image buffers.
   //
   deallocateImageBuffers( &arpBuffers );

   //
   // Exit the program
   //
   printf( "Done!  (hit enter)" );
   getchar();
   return 0;
}
