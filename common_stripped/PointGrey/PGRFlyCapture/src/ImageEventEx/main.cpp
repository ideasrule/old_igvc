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
// $Id: main.cpp,v 1.11 2006/10/04 00:05:03 tvlaar Exp $
//=============================================================================

//=============================================================================
//
// ImageEventEx
//   This example illustrates how users can implement partial image event
//   notification.  Partial image event notification is a mechanism that
//   provides the user with access to image data as it arrives in the PC's 
//   memory - before the entire image is available.
//   
//   This functionality is achieved by having the user associate a series of 
//   events at various locations throughout the image.  The events are then
//   signalled as the matching portion of the image arrives.  This allows the 
//   user to start processing the data immediately without having to wait for 
//   image transmission to complete.
//
//   This type of capability is particularly useful in applications requiring
//   extremely low latency.  One example is in applications involving moving 
//   the camera, stopping only to take pictures.  In this case, setting the 
//   earliest event possible is a good method for indicating the end of 
//   integration and that it is safe to move the camera without disrupting 
//   affecting capture.
//
//   This functionality is also available with Custom Image mode however there
//   are some additional things to watch out for when using this mode. 
//   Event notifications must be set on packet boundries so you have to compute
//   the total image size including padding when deciding where to set event
//   sizes.  There will be at most one padded packet transmitted, so simply
//   taking the ceiling of the computed image size divided by the packet size:
//
//    numOfPackets = ceiling(( rows * cols * bytesPerPixel)/bytesPerPacket)
//   
//   will give you the number of packets transmitter per image.
//
//   If the camera has already been started with the chosen bytes per packet,
//   this value can be queried from the format 7 registers. See the entry for
//   PACKET_PER_FRAME_INQ (0x048) in the PGR IEEE-1394 Digital Camera Register
//   Reference.
//
//   NOTE:   
//   Depending on your operating system, in order for this example to work on 
//   your PC, you may have to install a hotfix from Microsoft.  
//
//   Please read the following knowledge base article on our website for more 
//   information.
//
//   http://www.ptgrey.com/support/kb/details.asp?id=153
//   
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <math.h>

//=============================================================================
// Project Includes
//=============================================================================
#include <pgrflycapture.h>
#include <pgrflycaptureplus.h>


//=============================================================================
// Macro Definitions
//=============================================================================
#define _EVENTS         3
#define _BUFFERS        12


#define _COLS           640
#define _ROWS           480
#define _BYTES_PER_PIXEL  1
#define _IMAGE_SIZE     _COLS * _ROWS * _BYTES_PER_PIXEL

 // Only used for standard
#define _FRAMERATE      FLYCAPTURE_FRAMERATE_30
#define _VIDEOMODE      FLYCAPTURE_VIDEOMODE_640x480Y8

 // Only used for custom image
#define _MODE           0
#define _PIXEL_FORMAT   FLYCAPTURE_MONO8

//
// Register defines
// 
#define INITIALIZE         0x000
#define CAMERA_POWER       0x610

//
// Define this to use trigger mode.
//
//#define _USE_TRIGGER

// Define this to use custom image
#define _USE_CUSTOM

#define _CHECK_ERROR( function, error ) \
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
// Function Definitions
//=============================================================================

FlyCaptureError
initializeStandardEventSizes( 
                     FlyCaptureContext    context, 
                     FlyCaptureImageEvent events[] )
{
   FlyCaptureError      error;      
   FlyCapturePacketInfo info;
   
   if( ( error = flycaptureGetPacketInfo( 
      context, _VIDEOMODE, _FRAMERATE, &info ) ) != FLYCAPTURE_OK )
   {
      return error;
   }

   //
   // Initialize 3 events.
   //
   assert( _EVENTS == 3 );

   // 1st event: first packet of the image.
   events[ 0 ].uiSizeBytes = info.uiMaxSizeEventBytes;

   // 2nd event: near the middle of the image.
   events[ 1 ].uiSizeBytes = 
      ( ( _IMAGE_SIZE / 2 ) + ( ( _IMAGE_SIZE / 2 ) % info.uiMaxSizeEventBytes ) ) -
         events[ 0 ].uiSizeBytes;

   // 3rd event: at the end of the image.
   events[ 2 ].uiSizeBytes = 
      _IMAGE_SIZE - ( events[ 0 ].uiSizeBytes + events[ 1 ].uiSizeBytes );
         
   //
   // Make sure the events add up to the image size.
   //
   unsigned int uiTotalBytes = 0;
   for( int i = 0; i < _EVENTS; i++ )
   {
      uiTotalBytes += events[ i ].uiSizeBytes;
   }

   if( uiTotalBytes != _IMAGE_SIZE )
   {
      assert( false );
      return FLYCAPTURE_FAILED;
   }

   return FLYCAPTURE_OK;
}


FlyCaptureError
initializeCustomEventSizes( 
                     FlyCaptureContext    context, 
                     FlyCaptureImageEvent events[],
                     int*                 piTotalSize )
{
   FlyCaptureError      error;      
   FlyCapturePacketInfo info;     
   
   if( ( error = flycaptureGetCustomImagePacketInfo( 
      context, 
      _MODE,
      _COLS,
      _ROWS,
      _PIXEL_FORMAT,
      &info ) ) != FLYCAPTURE_OK )
   {
      return error;
   }

   //
   // Use the maximum packet size.
   //
   unsigned long ulPacketSize = info.uiMaxSizeBytes;

   //
   // Calculate the number of packets in the image
   //
   unsigned long ulPacketsPerFrame = 
      (unsigned long)ceil(_IMAGE_SIZE / (double)ulPacketSize );

   //
   // Calculate the total image size with padding
   //
   unsigned long ulTotalSizeBytes = ulPacketSize * ulPacketsPerFrame;

   //
   // Initialize 3 events.
   //
   assert( _EVENTS == 3 );

   // 1st event: first packet of the image.
   events[ 0 ].uiSizeBytes = ulPacketSize;

   // 2nd event: near the middle of the image.
   events[ 1 ].uiSizeBytes = ( ulPacketsPerFrame / 2 ) * ulPacketSize;

   // 3rd event: at the end of the image.
   events[ 2 ].uiSizeBytes = 
      ulTotalSizeBytes - events[ 0 ].uiSizeBytes - events[ 1 ].uiSizeBytes;
         
   //
   // Make sure the events add up to the image size.
   //
   unsigned long ulTotalBytes = 0;
   for( int i = 0; i < _EVENTS; i++ )
   {
      ulTotalBytes += events[ i ].uiSizeBytes;
   }

   if( ulTotalBytes != ulTotalSizeBytes )
   {
      assert( false );
      return FLYCAPTURE_FAILED;
   }

   *piTotalSize = ulTotalSizeBytes;
   return FLYCAPTURE_OK;
}


int 
runStandard()
{
   FlyCaptureError	error;
   FlyCaptureContext	context;   
   int                  iBuffers = _BUFFERS;
   unsigned char**      arpBuffers;
   int                  iEvents = _EVENTS;
   FlyCaptureImageEvent arEvents[ _EVENTS ];

   //
   // Initialize a number of image buffers
   //
   arpBuffers = new unsigned char*[ iBuffers ];
   for( int iBuffer = 0; iBuffer < iBuffers; iBuffer++ )
   {
      arpBuffers[ iBuffer ] = new unsigned char[ _IMAGE_SIZE ];
   }

   //
   // Create the camera context.
   //
   error = flycaptureCreateContext( &context );
   _CHECK_ERROR( "flycaptureCreateContext()", error );

   //
   // Initialize first camera on the bus.
   //
   error = flycaptureInitializePlus( context, 0, iBuffers, arpBuffers );
   _CHECK_ERROR( "flycaptureInitialize()", error );

   //
   // Reset the camera to default factory settings by asserting bit 0
   //
   error = flycaptureSetCameraRegister( context, INITIALIZE, 0x80000000 );
   _CHECK_ERROR( "flycaptureSetCameraRegister()", error );

   //
   // Power-up the camera (for cameras that support this feature)
   //
   error = flycaptureSetCameraRegister( context, CAMERA_POWER, 0x80000000 );
   _CHECK_ERROR( "flycaptureSetCameraRegister()", error );

   //
   // Initialize the events
   //
   error = initializeStandardEventSizes( context, arEvents );
   _CHECK_ERROR( "initializeEventSizes()", error );

   //
   // Initialize the event notification mechanism.
   //
   error = flycaptureInitializeNotify( context, iEvents, arEvents );
   _CHECK_ERROR( "flycaptureInitializeNotify()", error );

#ifdef _USE_TRIGGER
   error = flycaptureSetTrigger( context, true, 0, 0, 0, 0 );
   _CHECK_ERROR( "flycaptureSetTrigger()", error );
#endif

   //
   // Start the camera
   //
   error = flycaptureStartLockNext( context, _VIDEOMODE, _FRAMERATE );
   _CHECK_ERROR( "flycaptureStartLockNext()", error );

   
   //
   // Do the main grab loop.
   //
   int iImage = 0;
   while( _kbhit() == 0 )
   {
      FlyCaptureImage image;

      //
      // Request the next set of events.  This function will not block.
      // This does not need to be the same set of events that was passed in
      // to flycaptureInitializeNotify(), and it can be a different set of
      // events each grab (if you want to retain ownership of the images, 
      // for example.)
      //
      error = flycaptureLockNextEvent( context, &image, arEvents );
      _CHECK_ERROR( "flycaptureLockNextEvent()", error );

      //
      // Print out event information.
      //
	  int iEvent;
      for( iEvent = 0; iEvent < iEvents; iEvent++ )
      {
         FlyCaptureImageEvent* pevent = &arEvents[ iEvent ];

         printf( 
            "buffer = %p, idx = %u, size = %u, cols = %d, rows = %d\n",
            pevent->pBuffer,
            pevent->uiBufferIndex,
            pevent->uiSizeBytes,
            image.iCols,
            image.iRows );
      }

      //
      // Wait for each event to trigger, indicating that part of the image
      // is complete.
      //
      for( iEvent = 0; iEvent < iEvents; iEvent++ )
      {
         FlyCaptureImageEvent* pevent = &arEvents[ iEvent ];

         error = flycaptureWaitForImageEvent( 
            context, pevent, FLYCAPTURE_INFINITE );
         _CHECK_ERROR( "flycaptureWaitForImageEvent()", error );

         printf( 
            "Image part index %d completed! seqnum = %u, size = %u\n", 
            iEvent,
            pevent->uiSeqNum,
            pevent->uiSizeBytes );
      }


      //
      // Save some random images along the way.
      //
      if( iImage == 42 || iImage == 100 || iImage == 120 || iImage == 140 )
      {
         char pszFilename[ MAX_PATH ];
         sprintf( pszFilename, "events-%03d.pgm", iImage );

         flycaptureSaveImage( 
            context, &image, pszFilename, FLYCAPTURE_FILEFORMAT_PGM );
      }

      error = flycaptureUnlockEvent( context, arEvents );
      _CHECK_ERROR( "flycaptureUnlockEvent()", error );

      printf( "Press any key to quit.\n" );
      printf( "\n" );
      iImage++;
   }

   _getch();

   printf( "Got %d images.\n", iImage );
  
   
   //
   // Stop grabbing images.
   //
   printf( "Stopping camera.\n" );
   flycaptureStop( context );

   //
   // Destroy the camera context
   //
   flycaptureDestroyContext( context );

   printf( "\nHit any key..." );
   _getch();

   return 0;
}


int
runCustom()
{
   FlyCaptureError	error;
   FlyCaptureContext	context;   
   int                  iBuffers = _BUFFERS;
   unsigned char**      arpBuffers;
   int                  iEvents = _EVENTS;
   FlyCaptureImageEvent arEvents[ _EVENTS ];
   int                  iTotalSize;

   //
   // Create the camera context.
   //
   error = flycaptureCreateContext( &context );
   _CHECK_ERROR( "flycaptureCreateContext()", error );

   //
   // Initialize the camera so we can query packet information from it
   //
   error = flycaptureInitialize( context, 0 );
   _CHECK_ERROR( "flycaptureInitialize()", error );

   //
   // Reset the camera to default factory settings by asserting bit 0
   //
   error = flycaptureSetCameraRegister( context, INITIALIZE, 0x80000000 );
   _CHECK_ERROR( "flycaptureSetCameraRegister()", error );

   //
   // Power-up the camera (for cameras that support this feature)
   //
   error = flycaptureSetCameraRegister( context, CAMERA_POWER, 0x80000000 );
   _CHECK_ERROR( "flycaptureSetCameraRegister()", error );

   //
   // Initialize the events
   //
   error = initializeCustomEventSizes( context, arEvents, &iTotalSize );
   _CHECK_ERROR( "initializeEventSizes()", error );

   //
   // Initialize a number of image buffers
   //
   arpBuffers = new unsigned char*[ iBuffers ];
   for( int iBuffer = 0; iBuffer < iBuffers; iBuffer++ )
   {
      arpBuffers[ iBuffer ] = new unsigned char[ iTotalSize ];
   }

   //
   // Now initialize the camera with our buffers
   //
   error = flycaptureInitializePlus( context, 0, iBuffers, arpBuffers );
   _CHECK_ERROR( "flycaptureInitialize()", error );

   //
   // Initialize the event notification mechanism.
   //
   error = flycaptureInitializeNotify( context, iEvents, arEvents );
   _CHECK_ERROR( "flycaptureInitializeNotify()", error );

#ifdef _USE_TRIGGER
   error = flycaptureSetTrigger( context, true, 0, 0, 0, 0 );
   _CHECK_ERROR( "flycaptureSetTrigger()", error );
#endif

   //
   // Start the camera
   //
   error = flycaptureStartLockNextCustomImagePacket( 
      context, 
      _MODE, 
      0, 
      0, 
      _COLS, 
      _ROWS, 
      arEvents[0].uiSizeBytes,
      _PIXEL_FORMAT );
   _CHECK_ERROR( "flycaptureStartLockNextCustomImage()", error );
   
   //
   // Do the main grab loop.
   //
   int iImage = 0;
   while( _kbhit() == 0 )
   {
      FlyCaptureImage image;

      //
      // Request the next set of events.  This function will not block.
      // This does not need to be the same set of events that was passed in
      // to flycaptureInitializeNotify(), and it can be a different set of
      // events each grab (if you want to retain ownership of the images, 
      // for example.)
      //
      error = flycaptureLockNextEvent( context, &image, arEvents );
      _CHECK_ERROR( "flycaptureLockNextEvent()", error );

      //
      // Print out event information.
      //
	  int iEvent;
      for( iEvent = 0; iEvent < iEvents; iEvent++ )
      {
         FlyCaptureImageEvent* pevent = &arEvents[ iEvent ];

         printf( 
            "buffer = %p, idx = %u, size = %u, cols = %d, rows = %d\n",
            pevent->pBuffer,
            pevent->uiBufferIndex,
            pevent->uiSizeBytes,
            image.iCols,
            image.iRows );
      }

      //
      // Wait for each event to trigger, indicating that part of the image
      // is complete.
      //
      for( iEvent = 0; iEvent < iEvents; iEvent++ )
      {
         FlyCaptureImageEvent* pevent = &arEvents[ iEvent ];

         error = flycaptureWaitForImageEvent( 
            context, pevent, FLYCAPTURE_INFINITE );
         _CHECK_ERROR( "flycaptureWaitForImageEvent()", error );

         printf( 
            "Image part index %d completed! seqnum = %u, size = %u\n", 
            iEvent,
            pevent->uiSeqNum,
            pevent->uiSizeBytes );
      }


      //
      // Save some random images along the way.
      //
      if( iImage == 42 || iImage == 100 || iImage == 120 || iImage == 140 )
      {
         char pszFilename[ MAX_PATH ];
         sprintf( pszFilename, "events-%03d.pgm", iImage );

         flycaptureSaveImage( 
            context, &image, pszFilename, FLYCAPTURE_FILEFORMAT_PGM );
      }

      error = flycaptureUnlockEvent( context, arEvents );
      _CHECK_ERROR( "flycaptureUnlockEvent()", error );

      printf( "Press any key to quit.\n" );
      printf( "\n" );
      iImage++;
   }

   _getch();

   printf( "Got %d images.\n", iImage );
  
   
   //
   // Stop grabbing images.
   //
   printf( "Stopping camera.\n" );
   flycaptureStop( context );

   //
   // Destroy the camera context
   //
   flycaptureDestroyContext( context );

   printf( "\nHit any key..." );
   _getch();

   return 0;
}


int
main( int /*argc*/, char** /*argv*/ )
{
   int ch;

   printf( "*******************************************************\n" );
   printf( "* WARNING - ensure you have Windows XP Service Pack 2 *\n" );
   printf( "* or Microsoft Hotfix 94672 (Win2000) or Hotfix 94674 *\n" );
   printf( "* (WinXP) installed on your PC to avoid a potential   *\n" );
   printf( "* system crash. See the following for more info:      *\n" );
   printf( "*                                                     *\n" );
   printf( "* http://ptgrey.com/support/kb/index.asp?a=4&q=153    *\n" );
   printf( "*******************************************************\n" );
   
   printf( "\nHit any key to continue, or the 'X' key to exit...\n" );
   ch = _getch();
   if( ch == 'X' || ch == 'x')
   {
      return 0;
   }

#ifdef _USE_CUSTOM
   return runCustom();
#else
   return runStandard();
#endif   
}
