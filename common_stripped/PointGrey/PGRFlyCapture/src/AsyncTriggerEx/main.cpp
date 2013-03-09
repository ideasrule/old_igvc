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
// $Id: main.cpp,v 1.4 2006/11/09 19:37:49 mgibbons Exp $
//=============================================================================

//=============================================================================
//
// AsyncTriggerEx
// 
// The AsyncTriggerEx sample program demonstrates some of the basic 
// asynchronous trigger capabilities of compatible PGR Imaging Products. This 
// program only works with cameras that can be asynchronously triggered, either
// using an external hardware trigger or by using the camera's internal
// software trigger. Specify the way to trigger the camera (software trigger or
// external hardware trigger) by commenting/uncommenting the software trigger
// #define's.
//
// See the camera's Technical Reference or Getting Started manual for a list 
// of supported trigger modes and how to wire the camera for external 
// triggering. Check the PGR IEEE-1394 Register Reference to determine if the 
// camera supports either of the following two software triggering methods:
//
// 1. Using the DCAM 1.31 SOFTWARE_TRIGGER register 0x62Ch and by setting the
//    Trigger_Source to be the SOFTWARE_TRIGGER
// 2. Using the PGR-specific SOFT_ASYNC_TRIGGER register 0x102C
//
// After initializing the camera, the program queries the camera using 
// flycaptureQueryTrigger() to determine whether asynchronous trigger 
// functionality is present. For the purposes of this example we are concerned 
// only with ensuring it is present, and not with the exact trigger modes and 
// accessibility available, so NULLs are passed in for many of the parameters. 
// Once the presence of trigger functionality is confirmed, the program gets 
// the current (or default, since we reinitialize the camera) state of 
// the camera's trigger. The flycaptureGetTrigger() function essentially reads 
// the camera's TRIGGER_MODE register to obtain this information. For most PGR 
// cameras, the default trigger mode is Trigger_Mode_0 and the default trigger 
// source is GPIO0; however, this may not be the case for all cameras.
//
// The program then puts the camera into asynchronous trigger mode 
// using the flycaptureSetTrigger() function. The Trigger_Source that is used,
// however, depends on whether we are software triggering the camera or using
// an external hardware trigger. If we are software triggering the camera using
// the DCAM 1.31 SOFTWARE_TRIGGER register 0x62Ch, we need to set the
// Trigger_Source (we do not need to do this if we are using the PGR 
// SOFT_ASYNC_TRIGGER). 
// 
// It is important to note that the camera is put into trigger mode before it 
// is started with flycaptureStart(). Once the camera is started, it 
// immediately begins isochronously streaming images to the PC and 
// filling the memory buffers allocated by the start call. This will cause the 
// following call(s) to flycaptureGrabImage2() to return immediately with 
// images, even when no trigger has been fired. Putting the camera into trigger 
// mode and polling the camera to make sure it is actually in trigger mode 
// before starting the camera ensures that the images returned by 
// flycaptureGrabImage2() are the result of an actual trigger.
//
// The program then sets a grab timeout period so that calls to 
// flycaptureGrabImage2() will return after TIMEOUT milliseconds if the 
// external trigger has not fired. The flycaptureGrabImage2() call blocks until 
// an image is received; using a timeout ensures that the call grab call 
// eventually falls through and the program is closed correctly by calling 
// flycaptureStop() and flycaptureDestroyContext(). After each successful grab,
// the image is color processed into a 32-bit BGRU image and saved to disk.
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
#include <pgrflycapture.h>

//=============================================================================
// Macro Definitions
//=============================================================================
//
// Grab this many images, then quit.
//
#define IMAGES 5

//
// Set the grab timeout to this many milliseconds.
//
#define TIMEOUT 5000

//
// Software trigger the camera instead of using an external hardware trigger
//
#define SOFTWARE_TRIGGER_CAMERA

//
// By default, use the PGR-specific SOFT_ASYNC_TRIGGER register 0x102C to
// generate the software trigger. Comment this out to use the DCAM 1.31 
// SOFTWARE_TRIGGER register 0x62C as the software trigger (note: this requires
// a DCAM 1.31-compliant camera that implements this functionality).
//
#define USE_SOFT_ASYNC_TRIGGER

//
// Register defines
// 
#define INITIALIZE         0x000
#define TRIGGER_INQ        0x530
#define CAMERA_POWER       0x610
#define SOFTWARE_TRIGGER   0x62C
#define SOFT_ASYNC_TRIGGER 0x102C

//
// Error handling macro.
//
#define HANDLE_ERROR( function, error ) \
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
checkSoftwareTriggerPresence( 
                             FlyCaptureContext  context,
                             unsigned int       uiRegister )
{
   FlyCaptureError   error;

   unsigned long     ulValue;
   
   switch( uiRegister )
   {
      case SOFT_ASYNC_TRIGGER:
         error = flycaptureGetCameraRegister( 
            context, SOFT_ASYNC_TRIGGER, &ulValue );
         HANDLE_ERROR( "flycaptureGetCameraRegister()", error );

         //
         // Check the Presence_Inq field of the register; bit 0 == 1 indicates
         // presence of this feature.
         //
         if( ( ulValue & 0x80000000 ) == 0x80000000 )
         {
            return FLYCAPTURE_OK;
         }
         else
         {
            return FLYCAPTURE_NOT_IMPLEMENTED;
         }
      case SOFTWARE_TRIGGER:
         error = flycaptureGetCameraRegister( 
            context, TRIGGER_INQ, &ulValue );
         HANDLE_ERROR( "flycaptureGetCameraRegister()", error );

         //
         // Check the Software_Trigger_Inq field of the register; bit 15 == 1 
         // indicates presence of this feature.
         //
         if( ( ulValue & 0x10000 ) == 0x10000 )
         {
            return FLYCAPTURE_OK;
         }
         else
         {
            return FLYCAPTURE_NOT_IMPLEMENTED;
         }
      default:
         return FLYCAPTURE_INVALID_ARGUMENT;
   }
}

FlyCaptureError
checkTriggerReady(
                  FlyCaptureContext context )
{
   FlyCaptureError   error;

   unsigned long     ulValue;

   // 
   // Do our check to make sure the camera is ready to be triggered
   // by looking at bits 30-31. Any value other than 1 indicates
   // the camera is not ready to be triggered.
   //
   error = flycaptureGetCameraRegister( 
      context, SOFT_ASYNC_TRIGGER, &ulValue );
   HANDLE_ERROR( "flycaptureGetCameraRegister()", error );

   while( ulValue != 0x80000001 )
   {
      error = flycaptureGetCameraRegister( 
         context, SOFT_ASYNC_TRIGGER, &ulValue );
      HANDLE_ERROR( "flycaptureGetCameraRegister()", error );
   }

   return FLYCAPTURE_OK;
}

//=============================================================================
// Main Program
//=============================================================================
int 
main( int /* argc */, char* /* argv[] */ )
{
   FlyCaptureError	error;
   FlyCaptureContext	context;
   FlyCaptureImage      image;

   printf( "Initializing camera.\n" );

   //
   // Create the context.
   //
   error = flycaptureCreateContext( &context );
   HANDLE_ERROR( "flycaptureCreateContext()", error );
      
   //
   // Initialize the first camera on the bus.
   //
   error = flycaptureInitialize( context, 0 );
   HANDLE_ERROR( "flycaptureInitialize()", error );

   //
   // Reset the camera to default factory settings by asserting bit 0
   //
   error = flycaptureSetCameraRegister( context, INITIALIZE, 0x80000000 );
   HANDLE_ERROR( "flycaptureSetCameraRegister()", error );

   //
   // Power-up the camera (for cameras that support this feature)
   //
   error = flycaptureSetCameraRegister( context, CAMERA_POWER, 0x80000000 );
   HANDLE_ERROR( "flycaptureSetCameraRegister()", error );

   //
   // Determine whether or not the camera supports external trigger mode.
   // If it does, put the camera into external trigger mode and otherwise 
   // exit.
   //
   bool bTriggerPresent;

   error = flycaptureQueryTrigger( 
      context, &bTriggerPresent, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
   HANDLE_ERROR( "flycaptureQueryTrigger()", error );

   if( !bTriggerPresent)
   {
      printf("This camera does not support external trigger... exiting\n");
      return 1;
   }

   int   iPolarity;
   int   iSource;
   int   iRawValue;
   int   iMode;

   error = flycaptureGetTrigger( 
      context, NULL, &iPolarity, &iSource, &iRawValue, &iMode, NULL );
   HANDLE_ERROR( "flycaptureGetTrigger()", error );

   printf( "Going into asynchronous Trigger_Mode_0.\n" );
   //
   // Ensure the camera is in Trigger Mode 0 by explicitly setting it, 
   // as the camera could have a different default trigger mode
   //
#ifdef SOFTWARE_TRIGGER_CAMERA
   //
   // We are triggering the camera using the internal software trigger.
   // If we are using the DCAM SOFTWARE_TRIGGER functionality, we must
   // change the Trigger_Source to reflect the Software Trigger ID = 7.
   //
   error = flycaptureSetTrigger( 
      context, true, iPolarity, 7, 0, 0 );
   HANDLE_ERROR( "flycaptureSetCameraTrigger()", error );
#else
   //
   // We are triggering the camera using an external hardware trigger.
   //
   error = flycaptureSetTrigger( 
      context, true, iPolarity, iSource, 0, 0 );
   HANDLE_ERROR( "flycaptureSetCameraTrigger()", error );

#endif

   // 
   // Poll the camera to make sure the camera is actually in trigger mode
   // before we start it (avoids timeouts due to the trigger not being armed)
   //
   checkTriggerReady( context );

   //
   // Start the camera and grab any excess images that are already in the pipe.
   // Although it is extremely rare for spurious images to occur, it is
   // possible for the grab call to return an image that is not a result of a
   // user-generated trigger. To grab excess images, set a zero-length timeout.
   // A value of zero makes the grab call non-blocking.
   //
   printf( "Checking for any buffered images..." );
   error = flycaptureSetGrabTimeoutEx( context, 0 );
   HANDLE_ERROR( "flycaptureSetGrabTimeoutEx()", error );
      
   error = flycaptureStart(
      context, FLYCAPTURE_VIDEOMODE_ANY, FLYCAPTURE_FRAMERATE_ANY );
   HANDLE_ERROR( "flycaptureStart()", error );

   //
   // Grab the image immediately whether or not trigger present
   //
   error = flycaptureGrabImage2( context, &image );
   if( error == FLYCAPTURE_OK )
   {
      printf( "buffered image found. Flush successful.\n" );
   }
   else if( error == FLYCAPTURE_TIMEOUT )
   {
      printf( "no flush required! (normal behaviour)\n" );
   }
   else
   {
      HANDLE_ERROR( "flycaptureGrabImage2()", error );
   }

   error = flycaptureStop( context );
   HANDLE_ERROR( "flycaptureStop()", error );

   //
   // Start camera.  This is done after setting the trigger so that
   // excess images isochronously streamed from the camera don't fill up 
   // the internal buffers.
   //
   printf( "Starting camera.\n" );
   error = flycaptureStart( 
      context, 
      FLYCAPTURE_VIDEOMODE_ANY,
      FLYCAPTURE_FRAMERATE_ANY );
   HANDLE_ERROR( "flycaptureStart()", error );

   //
   // Set the grab timeout so that calls to flycaptureGrabImage2 will return 
   // after TIMEOUT milliseconds if the trigger hasn't fired.
   //
   error = flycaptureSetGrabTimeoutEx( context, TIMEOUT );
   HANDLE_ERROR( "flycaptureSetGrabTimeoutEx()", error );

   printf( "This program will quit after %d images are grabbed.\n", IMAGES );

#ifndef SOFTWARE_TRIGGER_CAMERA
   printf( "Trigger the camera by sending a trigger pulse to GPIO%d.\n", 
      iSource );
#endif

   for( int iImage = 0; iImage < IMAGES; iImage++ )
   {

#ifdef SOFTWARE_TRIGGER_CAMERA

#ifdef USE_SOFT_ASYNC_TRIGGER
      //
      // Check that the camera actually supports the PGR SOFT_ASYNC_TRIGGER
      // method of software triggering
      //
      error = checkSoftwareTriggerPresence( context, SOFT_ASYNC_TRIGGER );
      if( error == FLYCAPTURE_OK )
      {
         checkTriggerReady( context );
         
         //
         // Camera is now ready to be triggered, so generate software trigger
         // by writing a '0' to bit 31
         //
         printf( "Press the Enter key to initiate a software trigger.\n" );
         getchar();
         error = flycaptureSetCameraRegister( 
            context, SOFT_ASYNC_TRIGGER, 0x80000000 );
         HANDLE_ERROR( "flycaptureSetCameraRegister()", error );
      }
      else
      {
         printf( "SOFT_ASYNC_TRIGGER not implemented! Grab will timeout.\n" );
      }
#else
      //
      // Check that the camera actually supports the DCAM SOFTWARE_TRIGGER
      // method of software triggering    
      //
      error = checkSoftwareTriggerPresence( context, SOFTWARE_TRIGGER );
      if( error == FLYCAPTURE_OK )
      {
         error = checkTriggerReady( context );
         
         //
         // Camera is now ready to be triggered, so generate software trigger
         // by writing a '1' to bit 0
         //
         printf( "Press the Enter key to initiate a software trigger.\n" );
         getchar();
         error = flycaptureSetCameraRegister( 
            context, SOFTWARE_TRIGGER, 0x80000000 );
         HANDLE_ERROR( "flycaptureSetCameraRegister()", error );
      }
      else
      {
         printf( "SOFTWARE_TRIGGER not implemented! Grab will timeout.\n" );
      }
#endif

#endif
      //
      // Do an image grab.  This call will block until the camera
      // is externally triggered.
      //
      error = flycaptureGrabImage2( context, &image );
      if( error == FLYCAPTURE_TIMEOUT )
      {
	 printf( "Grab #%d timed out after %d milliseconds.\n", iImage, TIMEOUT );
      }
      else if( error != FLYCAPTURE_OK )
      {
	 HANDLE_ERROR( "flycaptureGrabImage2()", error );
      }      
      else
      {
	 printf( "Grab %d successful!\n", iImage );	 
      }
   }   

   //
   // Stop the camera and destroy the context.
   //
   flycaptureStop( context );
   flycaptureDestroyContext( context );

   printf( "Done!  (hit enter)" );
   getchar();

   return 0;
}
