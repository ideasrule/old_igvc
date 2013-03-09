//=============================================================================
// Copyright © 2006 Point Grey Research, Inc. All Rights Reserved.
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
// $Id: MessageLoggingEx.cpp,v 1.9 2006/09/07 17:27:12 mgibbons Exp $
//=============================================================================

//=============================================================================
//
// MessageLoggingEx.cpp
//
// The MessageLoggingEx sample program demonstrates how to receive messages
// from each camera and for each bus event.  These messages can be used for 
// troubleshooting device problems or for quick, detailed information on camera
// and bus activity currently taking place.
//
// The camera specific messages that can be received are register reads, 
// register writes, register block reads, register block writes, and grabbed
// images.  The bus event messages that can be received are bus resets, device
// arrivals, and device removals.
//
// The program first creates 4 FlyCapture variables: FlyCaptureContext,
// FlyCaptureError, FlyCaptureInfoEx, and FlyCaptureMessage. The context is
// used to initialize, receive, and close messages.  The error is declared
// In order to reliably debug your application by capturing meaningful errors 
// returned by API functions. The info variable is used to extract the serial
// number from the camera which is then used to initialize, receive, and 
// close messages.  The message variable is used for receiving messages.
//
// Message logging is turned off by default.  However, FlyCap enables message
// logging when it opens, so if FlyCap is running in the background with the 
// same camera index as defined in this example (_CAMERA_INDEX), then messages
// can be logged.
// 
// Messaging is then initialized for the first camera on the bus.  Only one 
// application can initialize messaging for each camera (or the bus events)
//  - only one 'reader' can receive messages at a time.  This is why it is 
// important to close the messaging for a camera (or bus events) when you 
// have finished logging messages.
// 
// In order to have message activity taking place, a camera must be initialized
// and running in a seperate application.  For instance, before running this 
// example, open FlyCapture and select the first camera in the camera selection
// dialog.  Press OK and let FlyCapture run in the background.  This will insure
// that there are messages being posted when we enable message logging.
//
// The most frequent message being sent is the register read message.  This is 
// the time to start receiving messages.  Each message is received in a 
// FlyCaptureMessage structure, and depending on the message type, information 
// is extracted from the message.  Messages are continuously logged until
// a key is pressed.  Using the flycaptureRegisterToString funtion, you can
// obtain a friendly register string from a register number.
//
// Messaging for the bus event is then initialized which can log bus events
// from all buses.  Here, the program tries to receive a bus message (reset, 
// arrival, removal), or it otherwise times out.
//
// Finally, all messaging is closed in order to allow other applications
// to receive messages.  Also, the message variable is deleted in order to 
// prevent memory leaks.
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <stdio.h>
#include "windows.h"  // For OVERLAPPED structure
#include <conio.h>    // For kbhit() function

//=============================================================================
// Project Includes
//=============================================================================
#include <pgrflycapture.h>
#include <pgrflycapturemessaging.h>

//=============================================================================
// Macro Definitions
//=============================================================================

//
// The maximum number of cameras on the bus.
//
#define _MAX_CAMS           32

//
// The index of the camera to grab from.
//
#define _CAMERA_INDEX       0

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
reportMessageInfo( FlyCaptureMessage* pgrMsg )
{
   //
   // Determine the message type and print it to the screen.
   //
   switch( pgrMsg->msgType )
   {
   case FLYCAPTURE_REGISTER_READ:
      printf( "REGISTER READ\n" );
      break;
   case FLYCAPTURE_REGISTER_WRITE:      
      printf( "REGISTER WRITE\n" );
      break;
   case FLYCAPTURE_REGISTER_READ_BLOCK:
      printf( "REGISTER READ BLOCK\n" );
      break;
   case FLYCAPTURE_REGISTER_WRITE_BLOCK:      
      printf( "REGISTER WRITE BLOCK\n" );
      break;
   case FLYCAPTURE_BUS_RESET:
      printf( "BUS RESET\n" );
      break;
   case FLYCAPTURE_DEVICE_REMOVAL:
      printf( "DEVICE REMOVAL\n" );
      break;
   case FLYCAPTURE_DEVICE_ARRIVAL:
      printf( "DEVICE ARRIVAL\n" );
      break;
   case FLYCAPTURE_BUS_ERROR:
      printf( "BUS ERROR\n" );
      break;
   default:
      break;
   }

   //
   // Depending on the message type, print specific details.
   //
   switch( pgrMsg->msgType )
   {
   case FLYCAPTURE_REGISTER_READ:
   case FLYCAPTURE_REGISTER_WRITE:      
      printf( "Model Name: %s\n", pgrMsg->Msg.Register.szDevice );
      printf( "Serial Number: %lu\n", pgrMsg->Msg.Register.ulSerialNumber );
      printf( "Bus Number: %d\n", pgrMsg->Msg.Register.iBusNumber );
      printf( "Node Number: %d\n", pgrMsg->Msg.Register.iNodeNumber );
      printf( "Register: 0x%08X\n", pgrMsg->Msg.Register.ulRegister );
      printf( "Register Name: %s\n", 
         flycaptureRegisterToString( pgrMsg->Msg.Register.ulRegister ) );
      printf( "Value: 0x%08X\n", pgrMsg->Msg.Register.ulValue );
      printf( "Error: %s\n\n", pgrMsg->Msg.Register.szError );
      break;
   case FLYCAPTURE_REGISTER_READ_BLOCK:
   case FLYCAPTURE_REGISTER_WRITE_BLOCK:
      printf( "Model Name: %s\n", pgrMsg->Msg.RegisterBlock.szDevice );
      printf( "Serial Number: %lu\n", 
         pgrMsg->Msg.RegisterBlock.ulSerialNumber );
      printf( "Bus Number: %d\n", pgrMsg->Msg.RegisterBlock.iBusNumber );
      printf( "Node Number: %d\n", pgrMsg->Msg.RegisterBlock.iNodeNumber );
      printf( "Register: 0x%08X\n", pgrMsg->Msg.RegisterBlock.ulRegister );
      printf( "Register Name: %s\n", 
         flycaptureRegisterToString( pgrMsg->Msg.Register.ulRegister ) );
      printf( "Value: 0x%08X\n", 
         pgrMsg->Msg.RegisterBlock.ulNumberOfQuadlets );   
      printf( "Error: %s\n\n", pgrMsg->Msg.RegisterBlock.szError );   
      break;
   case FLYCAPTURE_GRABBED_IMAGE:
      printf( "Model Name: %s\n", pgrMsg->Msg.Image.szDevice );
      printf( "Serial Number: %lu\n", pgrMsg->Msg.Image.ulSerialNumber );
      printf( "Bus Number: %d\n", pgrMsg->Msg.Image.iBusNumber );
      printf( "Node Number: %d\n", pgrMsg->Msg.Image.iNodeNumber );
      printf( "Sequence: %lu\n", pgrMsg->Msg.Image.ulSequence );
      printf( "Bytes: %lu\n", pgrMsg->Msg.Image.ulBytes );
      printf( "TimeStamp: %02d:%02d:%02d.%03d\n\n", 
         pgrMsg->Msg.Image.stTimeStamp.usHour,
         pgrMsg->Msg.Image.stTimeStamp.usMinute,
         pgrMsg->Msg.Image.stTimeStamp.usSecond,
         pgrMsg->Msg.Image.stTimeStamp.usMilliseconds ); 
      break;
   case FLYCAPTURE_BUS_RESET:
      printf( "Bus Number: %d\n", pgrMsg->Msg.Reset.iBusNumber );
      printf( "Time: %02d:%02d:%02d.%03d\n\n", 
         pgrMsg->Msg.Reset.stTimeStamp.usHour,
         pgrMsg->Msg.Reset.stTimeStamp.usMinute,
         pgrMsg->Msg.Reset.stTimeStamp.usSecond,
         pgrMsg->Msg.Reset.stTimeStamp.usMilliseconds );         
      break;
   case FLYCAPTURE_DEVICE_REMOVAL:
      printf( "Model Name: %s\n", pgrMsg->Msg.Removal.szDevice );
      printf( "Serial Number: %lu\n", pgrMsg->Msg.Removal.ulSerialNumber );
      printf( "Bus Number: %d\n", pgrMsg->Msg.Removal.iBusNumber );
      printf( "Node Number: %d\n", pgrMsg->Msg.Removal.iNodeNumber );
      printf( "Time: %02d:%02d:%02d.%03d\n\n", 
         pgrMsg->Msg.Removal.stTimeStamp.usHour,
         pgrMsg->Msg.Removal.stTimeStamp.usMinute,
         pgrMsg->Msg.Removal.stTimeStamp.usSecond,
         pgrMsg->Msg.Removal.stTimeStamp.usMilliseconds );         
      break;
   case FLYCAPTURE_DEVICE_ARRIVAL:
      printf( "Model Name: %s\n", pgrMsg->Msg.Arrival.szDevice );
      printf( "Serial Number: %lu\n", pgrMsg->Msg.Arrival.ulSerialNumber );
      printf( "Bus Number: %d\n", pgrMsg->Msg.Arrival.iBusNumber );
      printf( "Node Number: %d\n", pgrMsg->Msg.Arrival.iNodeNumber );
      printf( "Time: %02d:%02d:%02d.%03d\n\n", 
         pgrMsg->Msg.Arrival.stTimeStamp.usHour,
         pgrMsg->Msg.Arrival.stTimeStamp.usMinute,
         pgrMsg->Msg.Arrival.stTimeStamp.usSecond,
         pgrMsg->Msg.Arrival.stTimeStamp.usMilliseconds );          
      break;
   case FLYCAPTURE_BUS_ERROR:
      printf( "Model Name: %s\n", pgrMsg->Msg.BusError.szDevice );
      printf( "Serial Number: %lu\n", 
         pgrMsg->Msg.BusError.ulSerialNumber );
      printf( "Bus Number: %d\n", pgrMsg->Msg.BusError.iBusNumber );
      printf( "Node Number: %d\n", pgrMsg->Msg.BusError.iNodeNumber );
      printf( "Error Code: %lu\n", pgrMsg->Msg.BusError.ulErrorCode );
      printf( "Error: %s\n", 
         flycaptureBusErrorToString( pgrMsg->Msg.BusError.ulErrorCode ) );
      printf( "Time: %02d:%02d:%02d.%03d\n\n", 
         pgrMsg->Msg.BusError.stTimeStamp.usHour,
         pgrMsg->Msg.BusError.stTimeStamp.usMinute,
         pgrMsg->Msg.BusError.stTimeStamp.usSecond,
         pgrMsg->Msg.BusError.stTimeStamp.usMilliseconds );         
      break;
   default:
      break;
   }
}

int 
main( int /* argc */, char* /* argv[] */ )
{   
   FlyCaptureError     error;
   FlyCaptureContext   context;
   FlyCaptureInfoEx    info;
   FlyCaptureMessage*  pgrMsg = new FlyCaptureMessage;

   printf( "*******************************************************\n" );
   printf( "* NOTE - In order to have message activity take place *\n" );
   printf( "* a camera must be initialized and running in a       *\n" );
   printf( "* seperate application.  For instance, before running *\n" );
   printf( "* this example, open FlyCapture and select the first  *\n" );
   printf( "* camera in the camera selection dialog.  Press OK    *\n" );
   printf( "* and let FlyCapture run in the background.  This     *\n" );
   printf( "* will insure that there are messages being posted    *\n" );
   printf( "* when we enable message logging.                     *\n" );
   printf( "*******************************************************\n" );
   
   printf( "\nHit any key to continue...\n" );
   _getch();

   //
   // Create the context.
   //
   error = flycaptureCreateContext( &context );
   _HANDLE_ERROR( error, "flycaptureCreateContext()" );
   
   //
   // Initialize a camera.
   //
   printf( "Initializing camera %u.\n", _CAMERA_INDEX );
   error = flycaptureInitialize( context, _CAMERA_INDEX );
   _HANDLE_ERROR( error, "flycaptureInitialize()" );

   //
   // Retreive information on this camera.
   //
   error = flycaptureGetCameraInfo( context, &info );
   _HANDLE_ERROR( error, "flycaptureGetCameraInfo()" );

   //
   // Initialize the mailslot for this camera.
   //
   if( flycaptureInitializeMessaging( context, info.SerialNumber ) 
      == FLYCAPTURE_OK )
   {
      printf( 
         "Initialized mailslot for %s:%lu\n\n",
         info.pszModelName, 
         info.SerialNumber );
   }
   else
   {
      printf( 
         "Could not initialize mailslot for %s:%lu\n\n",
         info.pszModelName, 
         info.SerialNumber );
   }

   //
   // Initialize the bus mailslot in order to receive bus event messages.
   //
   if( flycaptureInitializeMessaging( context, FLYCAPTURE_BUS_MESSAGE ) 
      == FLYCAPTURE_OK )
   {
      printf( 
         "Initialized mailslot for bus events.\n");
   }
   else
   {
      printf( 
         "Could not initialize mailslot for bus events.\n" );
   }

   //
   // Turn on message logging.  This is turned off by default.  However,
   // if FlyCap is open, then FlyCap turns on message logging. 
   //
   error = flycaptureSetMessageLoggingStatus( context, true );
   _HANDLE_ERROR( error, "flycaptureSetMessageLogging()" );

   //
   // Receive messages until a key is hit.
   //
   printf( "Receiving Messages..." );
   printf( "(Press any key to end)\n" );

   Sleep( 2000 );

   HANDLE hOverlapped = ::CreateEvent( NULL, true, false, NULL );
   OVERLAPPED olRead;
   olRead.hEvent = hOverlapped;

   while( kbhit() == 0  )
   {
      if( flycaptureReceiveMessage( 
         context, info.SerialNumber, pgrMsg, &olRead ) == FLYCAPTURE_OK )
      {
         switch( ::WaitForSingleObject( olRead.hEvent, 3000 ) )
         {
         case WAIT_OBJECT_0:
            printf( "***Received Message***\n" ); 
            reportMessageInfo( pgrMsg ); 
            Sleep(1000);
            break;
         case WAIT_TIMEOUT:
            printf( "***Time out***\n" );
            Sleep(1000);
            break;
         }
      }
   }

   //
   // Receive a bus event message.
   //
   printf( "Receiving Bus Messages...\n" );
   printf( "**Attach or remove a camera now (10 second timeout)**\n\n" );

   if( flycaptureReceiveMessage( 
      context, FLYCAPTURE_BUS_MESSAGE,
      pgrMsg, &olRead ) == FLYCAPTURE_OK )
   {
      switch( ::WaitForSingleObject( olRead.hEvent, 10000 ) )
      {
      case WAIT_OBJECT_0:
         printf( "***Received Message***\n" ); 
         reportMessageInfo( pgrMsg ); 
         Sleep(1000);
         break;
      case WAIT_TIMEOUT:
         printf( "***Time out***\n" );
         break;
      }
   }

   //
   // Delete the allocated message.
   //
   delete pgrMsg;

   //
   // Close the mailslots.
   //
   flycaptureCloseMessaging( context, info.SerialNumber );
   flycaptureCloseMessaging( context, FLYCAPTURE_BUS_MESSAGE ); 

   printf( "Done!  (hit enter)" );
   getchar();
   getchar();

   return 0;
}


