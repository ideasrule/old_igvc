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
// $Id: main.cpp,v 1.16 2007/07/04 17:18:46 demos Exp $
//=============================================================================

//=============================================================================
//
// MultipleCameraEx
//
// PGR cameras that are on the same 1394 bus, running at the same frame rate
// automatically synchronize with one another.  This example illustrates how 
// to grab a series of images from multiple cameras, insuring that no images are 
// missed and that all of the images are synchronized.
//
// The example makes use of flycaptureLockNext() and flycaptureUnlock() commands
// to insure that no images are lost.  A call to flycaptureStartLockNext() causes 
// the library to enqueue incoming images to a length not exceeding the number of 
// allocated and available (not 'locked') buffers. Once this limit is hit, the 
// oldest images start to be lost. 
// Calls to flycaptureLockNext return the oldest available image that has yet to 
// be returned.  The user is guaranteed access to this image (they have it 
// 'locked') until they return it to the buffer pool using the flycaptureUnlock() 
// command.
//
// The example illustrates how users can determine if any images have been lost
// as a result of buffer overrun.  This is achieved by simply examining the 
// uiSeqNum field contained in the FlyCaptureImagePlus structure.  Any pair
// of images returned from subsequent calls to flycaptureLockNext() that have 
// uiSeqNum's that differ by more than 1 is an indication that images have been
// dropped.
//
// Although the cameras are always synchronized, it is possible for any pair of 
// cameras set of buffered images to be 'misaligned'.  For example, a call to
// flycaptureStartLockNext() for one camera even a single frame time after the
// same call to a different camera can result in the buffers being misaligned by
// an image.  This issue is resolved with a call to flycaptureSyncForLockNext().
// This command uses a mechanism that clears all of the stored buffers and 
// insures that the buffered images are aligned with respect to time.
//
// This example demonstrates how a user can determine whether or not a pair of 
// images are synchronized based on the FlyCaptureTimestamp structure associated 
// with the image data.  Again, a simple difference calculation based on the
// time stamp is all that is required to determine whether or not the images were
// taken at the same time.
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <assert.h>
#include <stdio.h>
#include <math.h>

//
// math.h doesn't compile cleanly with W4.
//
#pragma warning( disable: 4514 ) // unreferenced inline function has been removed

//=============================================================================
// Project Includes
//=============================================================================
#include "pgrflycapture.h"
#include "pgrflycaptureplus.h"


//=============================================================================
// Macro Definitions 
//=============================================================================
//
// Helper code to handle a FlyCapture error.
//
#define _HANDLE_ERROR( error, function ) \
   if( error != FLYCAPTURE_OK ) \
   { \
      printf( "%s: %s\n", function, flycaptureErrorToString( error ) ); \
      return -1; \
   } \

//
// Video mode and frame rate to use.
//
#define _VIDEOMODE   FLYCAPTURE_VIDEOMODE_ANY
#define _FRAMERATE   FLYCAPTURE_FRAMERATE_ANY

//
// Maximum size of expected (raw) image.  You must adjust this based on
// the above video mode.
//
#define _MAX_IMAGE_SIZE 1600 * 1200

//
// Images to grab.
//
#define _IMAGES 100

//
// Buffers per camera.
//
#define _BUFFERS 40

//
// Maximum cameras on the bus. 
// (the maximum devices allowed on a 1394 bus is 64).
//
#define _MAX_CAMERAS 64

//
// Register defines
// 
#define INITIALIZE         0x000
#define CAMERA_POWER       0x610
#define FRAME_INFO	   0x12F8

//
// Report information for every image
//
#define _VERBOSE 

//=============================================================================
// Global Variables 
//=============================================================================
FlyCaptureContext    g_arContext[ _MAX_CAMERAS ];
FlyCaptureImagePlus  g_arImageplus[ _MAX_CAMERAS ];
FlyCaptureInfoEx     g_arInfo[ _MAX_CAMERAS ];

unsigned int         g_uiCameras = _MAX_CAMERAS;

//
// Image buffers to use.
//
unsigned char**   g_arpBuffers[ _MAX_CAMERAS ];

//=============================================================================
// Function Definitions
//=============================================================================
//
// allocateBuffers()
//
// This function allocates a series of image buffers for every camera on the 
// bus.  These images are later deallocated with a call to deallocateBuffers().
//
void
allocateBuffers()
{
   for( unsigned iCamera = 0; iCamera < g_uiCameras; iCamera++ )
   {
      g_arpBuffers[ iCamera ] = new unsigned char*[ _BUFFERS ];
      
      for( unsigned i = 0; i < _BUFFERS; i++ )
      {
         g_arpBuffers[ iCamera ][ i ] = new unsigned char[ _MAX_IMAGE_SIZE ];
      }
   }
}

//
// deallocateBuffers()
//
// This function deallocates the buffers allocated by a previous call to
// allocateBuffers().
//
void
deallocateBuffers()
{
   for( unsigned iCamera = 0; iCamera < g_uiCameras; iCamera++ )
   {      
      if( g_arpBuffers[ iCamera ] != NULL )
      {
         for( unsigned i = 0; i < _BUFFERS; i++ )
         {
            if( g_arpBuffers[ iCamera ][ i ] != NULL )
            {
               delete [] g_arpBuffers[ iCamera ][ i ];
               g_arpBuffers[ iCamera ][ i ] = NULL;
            }
         }      
         
         delete [] g_arpBuffers[ iCamera ];
         g_arpBuffers[ iCamera ] = NULL;
      }
   }
}


//
// doGrabLoop()
//
// This function grabs a series of synchronized images from all of the cameras
// on the bus.  It keeps track of the number of images missed and the number of
// images grabbed that were not synchronized.
//
void
doGrabLoop()
{
   FlyCaptureError error;
   unsigned int   aruiPrevSeqNum[ _MAX_CAMERAS ];
   unsigned int   aruiDelta[ _MAX_CAMERAS ];
   unsigned int   aruiCycles[ _MAX_CAMERAS ];
   unsigned int   uiMissedImages    = 0;
   unsigned int   uiOutOfSyncImages = 0;

   //
   // Grab a series of images from each camera
   //
   printf( "Starting to grab...\n" );
   for( unsigned int uiImage = 0; uiImage < _IMAGES; uiImage++ )
   {
      unsigned int uiCamera = 0;

#ifdef _VERBOSE
      printf( "Image %04u:\n", uiImage );
#else
      printf( ".", uiImage );
#endif

      //
      // Lock images.
      //
      for( uiCamera = 0; uiCamera < g_uiCameras; uiCamera++ )
      {
         error = ::flycaptureLockNext( 
            g_arContext[ uiCamera ], &g_arImageplus[ uiCamera ] );
         if( error != FLYCAPTURE_OK )
         { 
            printf( 
               "flycaptureLockNext(): %s\n", 
               ::flycaptureErrorToString( error ) ); 
            return;
         } 
      }

      //
      // Keep trrack of the difference in image sequence numbers (uiSeqNum)
      // in order to determine if any images have been missed.  A difference
      // greater than 1 indicates that an image has been missed.
      //
      for( uiCamera = 0; uiCamera < g_uiCameras; uiCamera++ )
      {
         if( uiImage == 0 )
         {
            aruiPrevSeqNum[ uiCamera ] = g_arImageplus[ uiCamera ].uiSeqNum;
            aruiDelta[ uiCamera ] = 1;
         }
         else
         {
            aruiDelta[ uiCamera ] = 
               g_arImageplus[ uiCamera ].uiSeqNum - aruiPrevSeqNum[ uiCamera ];
         }
         
         if( aruiDelta[ uiCamera ] != 1 )
         {
            // we have missed an image.
            uiMissedImages += aruiDelta[ uiCamera ] - 1;
            printf( 
               "We have missed an image! (Image %03u, Camera %u)\n", 
               uiImage,
               uiCamera );
         }
         
         aruiPrevSeqNum[ uiCamera ] = g_arImageplus[ uiCamera ].uiSeqNum;

#ifdef _VERBOSE
         printf( 
            "   Camera %02u: sq = %04u, time = %03u.%04u\n",
            uiCamera,
            g_arImageplus[ uiCamera ].uiSeqNum,
            g_arImageplus[ uiCamera ].image.timeStamp.ulCycleSeconds,
            g_arImageplus[ uiCamera ].image.timeStamp.ulCycleCount   );
#endif

	 //
	 // Store the image timestamp
	 //
         aruiCycles[ uiCamera ] = 
            g_arImageplus[ uiCamera ].image.timeStamp.ulCycleSeconds * 8000 +
            g_arImageplus[ uiCamera ].image.timeStamp.ulCycleCount;
      }

      //
      // Determine the difference of the timestamp for every image from the
      // first camera.  If the difference is greater than 1 cycle count, register
      // the camera as being out of synchronization.
      //
      for( uiCamera = 0; uiCamera < g_uiCameras; uiCamera++ )
      {
         int iDeltaFrom0 = abs((long)(aruiCycles[ uiCamera ] - aruiCycles[ 0 ]));
	 
         if( ( iDeltaFrom0 % ( 128 * 8000 - 1 ) ) > 1 )
         {
#ifdef _VERBOSE
	    printf("Camera %d is %d cycle counts out of synchronization "
	       "from camera 0\n",
	       uiCamera, iDeltaFrom0);
#else
	    printf("!");
#endif
	    uiOutOfSyncImages++;
	 }
      }

      //
      // Unlock all of the images effectively handing them back to the buffer pool.
      //
      for( uiCamera = 0; uiCamera < g_uiCameras; uiCamera++ )
      {
         error = ::flycaptureUnlock( 
            g_arContext[ uiCamera ], g_arImageplus[ uiCamera ].uiBufferIndex );
         if( error != FLYCAPTURE_OK )
         { 
            printf( "flycaptureUnlock(): %s\n", ::flycaptureErrorToString( error ) ); 
            return;
         } 
      }
   }

   //
   // Report on the number of images missed and the number of images captured out
   // of synchronization.
   //
   printf( "\nMissed images = %u.\n", uiMissedImages );
   printf( "\nOut of sync images = %u.\n", uiOutOfSyncImages );
}

//=============================================================================
// Main Program
//=============================================================================
int 
main( int /* argc */, char* /* argv[] */ )
{
   FlyCaptureError error;
   unsigned int    uiCamera;

   //
   // Enumerate the bus.
   //
   error = ::flycaptureBusEnumerateCamerasEx( g_arInfo, &g_uiCameras );
   _HANDLE_ERROR( error, "flycaptureBusEnumerateCamerasEx()" );
   if( g_uiCameras == 0 )
   {
      printf( "No cameras found!\n" );
      assert( false );
      return -1;
   }

   //
   // Allocate a series of image buffers for every camera.
   //
   allocateBuffers();

   //
   // Create a context for and initialize every camera on the bus.
   //
   for( uiCamera = 0; uiCamera < g_uiCameras; uiCamera++ )
   {
      error = ::flycaptureCreateContext( &g_arContext[ uiCamera ] );
      _HANDLE_ERROR( error, "flycaptureCreateContext()" );
      
      printf( "Initializing camera %u.\n", uiCamera );
      error = ::flycaptureInitializePlus( 
         g_arContext[ uiCamera ], 
         uiCamera,
         _BUFFERS,
         g_arpBuffers[ uiCamera ] );
      _HANDLE_ERROR( error, "flycaptureInitializePlus()" );

      //
      // Reset the camera to default factory settings by asserting bit 0
      //
      error = flycaptureSetCameraRegister( g_arContext[ uiCamera ], INITIALIZE, 0x80000000 );
      _HANDLE_ERROR( error, "flycaptureSetCameraRegister()" );

      //
      // Power-up the camera (for cameras that support this feature)
      //
      error = flycaptureSetCameraRegister( g_arContext[ uiCamera ], CAMERA_POWER, 0x80000000 );
      _HANDLE_ERROR( error, "flycaptureSetCameraRegister()" );

      //
      // Turn on Timestamp (0x01) bit in FRAME_INFO to attach the image timestamp to the image header.
      //
      error = flycaptureSetCameraRegister( g_arContext[ uiCamera ], FRAME_INFO, 0x80000001 );
      _HANDLE_ERROR( error, "flycaptureSetCameraRegister()" );
   }

   //
   // Start all of the cameras grabbing
   //
   for( uiCamera = 0; uiCamera < g_uiCameras; uiCamera++ )
   {    
      printf( "Starting camera.\n\n" );
      error = ::flycaptureStartLockNext( 
         g_arContext[ uiCamera ], _VIDEOMODE, _FRAMERATE );
      _HANDLE_ERROR( error, "flycaptureStart()" );
   }

   //
   // Having started all of the cameras synchronize all of their buffers.
   // Please note that cameras running at the same frame rate on the same
   // bus will automatically synchronize to each other.  This call is for
   // purposes of synchronizing the buffers.
   //
   error = ::flycaptureSyncForLockNext( g_arContext, g_uiCameras );
   if( error != FLYCAPTURE_OK )
   {
      printf( 
         "flycaptureSyncForLockNext() failed with %s!\n", 
         ::flycaptureErrorToString( error ) );
   }

   //
   // Grab a series of images from all of the cameras
   //
   doGrabLoop();
   
   //
   // Stop all cameras from grabbing and destroy their contexts.
   //
   for( uiCamera = 0; uiCamera < g_uiCameras; uiCamera++ )
   {
      error = ::flycaptureStop( g_arContext[ uiCamera ] );
      _HANDLE_ERROR( error, "flycaptureStop()" );
      
      error = ::flycaptureDestroyContext( g_arContext[ uiCamera ] );
      _HANDLE_ERROR( error, "flycaptureBusEnumerateCameras()" );
   }

   //
   // Deallocate all of the image buffers that had been created previously.
   //
   deallocateBuffers();

   printf( "Done!  (hit enter)" );
   getchar();

   return 0;
}

