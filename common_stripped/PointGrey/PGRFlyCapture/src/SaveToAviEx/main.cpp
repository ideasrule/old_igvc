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
// $Id: main.cpp,v 1.12 2007/12/13 00:52:22 demos Exp $
//=============================================================================

//=============================================================================
//
// SaveToAviEx <destination path of avi>
//
//   This example streams a specified number of raw images into a windows .avi
//   file (without using a compression codec), and then converts the raw .avi
//   into a colour .avi.   
//
//   NOTE: Some media player applications enforce a 2GB limit on the size of 
//   .avi files. 
//
//   To compress the final avi using a compression codec or segment large .avi
//   files, we recommend VirtualDub, http://www.virtualdub.org/.
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

//=============================================================================
// PGR Includes
//=============================================================================
#include <pgrflycapture.h>
#include <pgrflycaptureplus.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRAviFile.h"



//
// Camera bus index to use.  0 = first camera on the bus.
//
#define _CAMERA_INDEX 0

//
// Video mode and frame rate to use.
//
#define _VIDEOMODE   FLYCAPTURE_VIDEOMODE_ANY
#define _FRAMERATE   FLYCAPTURE_FRAMERATE_ANY

//
// Buffers to use for grabbing.  Increase this if you are getting missed frame
// errors.
//
#define _BUFFERS 32

//
// Images to grab, in total.  If this is not a multiple of _BUFFERS, it will
// be rounded up.
//
#define _IMAGES 128

//
// The temporary raw .avi file name.
//
#define _RAW_AVI_FILENAME "raw.avi"

//
// The default destination .avi path.
//
#define _DEFAULT_COLOUR_AVI_FILENAME "colour.avi"

//
// Frame rate to assign to the destination .avi.
//
#define _AVI_FRAMERATE 15.0

//
// Use a compressor on the colour processed images.
//
//#define USE_COMPRESSOR


//
// Global variables.
//
FlyCaptureError      g_error;
FlyCaptureContext    g_context;   
FlyCaptureImagePlus  g_arimageplus[ _BUFFERS / 2 ];
FlyCaptureImage      g_image;
FlyCaptureImage      g_imageConverted;
char                 g_pszDestAviPath[ MAX_PATH ];
char                 g_pszTempAviPath[ MAX_PATH ];
int                  g_iImageSizeBytes = 0;
double               g_dFramerate = _FRAMERATE;

//
// Image buffers to use.
//
unsigned char**   g_arpBuffers = NULL;
unsigned char*    g_pTempFrameBuffer = NULL;


//
// Helper code to handle a FlyCapture error.
//
#define _HANDLE_ERROR( error, function ) \
   if( error != FLYCAPTURE_OK ) \
   { \
      printf( "%s: %s\n", function, flycaptureErrorToString( error ) ); \
      exit( -1 ); \
   } \


void
initBuffers()
{
   assert( _BUFFERS % 2 == 0 );   

   //
   // So the example is general, we are going to start the camera in its
   // current format and framerate.  We then need to grab an image to find out
   // how large of a buffer to allocate.
   //

   printf( "Creating context for determining buffer size.\n" );
   g_error = ::flycaptureCreateContext( &g_context );
   _HANDLE_ERROR( g_error, "flycaptureCreateContext()" );

   printf( "Initializing camera %u.\n", _CAMERA_INDEX );
   g_error = ::flycaptureInitialize( g_context, _CAMERA_INDEX );
   _HANDLE_ERROR( g_error, "flycaptureInitialize()" );

   printf( "Starting camera in lockLatest.\n" );
   g_error = ::flycaptureStart( g_context, _VIDEOMODE, _FRAMERATE );
   _HANDLE_ERROR( g_error, "flycaptureStart()" );

   printf( "Grabbing single image to get image sizes.\n" );
   FlyCaptureImage image;
   g_error = ::flycaptureGrabImage2( g_context, &image );
   _HANDLE_ERROR( g_error, "flycaptureGrabImage2()" );   

   g_iImageSizeBytes = image.iRows * image.iRowInc;
   g_image = image;
   g_image.pData = NULL;

   printf( "Stopping camera.\n" );
   g_error = ::flycaptureStop( g_context );
   _HANDLE_ERROR( g_error, "flycaptureStop()" );

   printf( "Destroying context.\n\n" );
   g_error = ::flycaptureDestroyContext( g_context );
   _HANDLE_ERROR( g_error, "flycaptureDestroyContext()" );

   g_arpBuffers = new unsigned char*[ _BUFFERS ];

   for( unsigned i = 0; i < _BUFFERS; i++ )
   {
      g_arpBuffers[ i ] = new unsigned char[ g_iImageSizeBytes ];
   }

   g_imageConverted.pData = new unsigned char[ g_iImageSizeBytes * 3 ];
   g_pTempFrameBuffer = new unsigned char[ g_iImageSizeBytes ];

   g_imageConverted.pixelFormat = FLYCAPTURE_BGR;
}


void
initCamera()
{
   printf( "Creating context for capturing the AVI.\n" );
   g_error = ::flycaptureCreateContext( &g_context );
   _HANDLE_ERROR( g_error, "flycaptureCreateContext()" );
   
   printf( "Initializing camera %u with %u allocated buffers.\n", _CAMERA_INDEX, _BUFFERS );
   g_error = ::flycaptureInitializePlus( 
      g_context, 
      _CAMERA_INDEX,
      _BUFFERS,
      g_arpBuffers );
   _HANDLE_ERROR( g_error, "flycaptureInitializePlus()" );

   printf( "Starting camera in lockNext.\n" );
   g_error = ::flycaptureStartLockNext( g_context, _VIDEOMODE, _FRAMERATE );
   _HANDLE_ERROR( g_error, "flycaptureStartLockNext()" );
}


void
deinitBuffers()
{
   if( g_arpBuffers != NULL )
   {
      for( unsigned i = 0; i < _BUFFERS; i++ )
      {
         if( g_arpBuffers[ i ] != NULL )
         {
            delete [] g_arpBuffers[ i ];
            g_arpBuffers[ i ] = NULL;
         }
      }      

      delete [] g_arpBuffers;
      g_arpBuffers = NULL;
   }

   if( g_imageConverted.pData != NULL )
   {
      delete [] g_imageConverted.pData;
      g_imageConverted.pData = NULL;
   }

   if( g_pTempFrameBuffer != NULL )
   {
      delete [] g_pTempFrameBuffer;
      g_pTempFrameBuffer = NULL;
   }
}


void
deinitCamera()
{
   printf( "Stopping camera.\n" );
   g_error = ::flycaptureStop( g_context );
   _HANDLE_ERROR( g_error, "flycaptureStop()" );

   printf( "Destroying context.\n\n" );
   g_error = ::flycaptureDestroyContext( g_context );
   _HANDLE_ERROR( g_error, "flycaptureDestroyContext()" );
}


void
doGrabLoop()
{
   int iImagesCompleted = 0;

   PGRAviFile avifile;

   bool bRet = avifile.open( 
      g_pszTempAviPath, g_image.iCols, g_image.iRows, (g_image.iRowInc/g_image.iCols)*8, _AVI_FRAMERATE );
   
   if( !bRet )
   {
      printf( 
         "Error initializing avi library or opening %s.\n", g_pszTempAviPath );
      printf( "Press any key.\n" );
      getch();
      exit( -1 );
   }

   printf( "Grabbing...\n" );

   while( iImagesCompleted < _IMAGES )
   {
      unsigned int uiPrevSeqNum = 0;   
      unsigned int uiDelta;
      int iImage;
      const int iChunkSize = _BUFFERS / 2;
      
      //
      // Capture half of the available buffers.
      //
      for( iImage = 0; iImage < iChunkSize; iImage++ )
      {
         FlyCaptureImagePlus* pimage = &g_arimageplus[ iImage ];
         
         g_error = ::flycaptureLockNext( g_context, pimage );
         if( g_error != FLYCAPTURE_OK )
         { 
            printf( 
               "flycaptureLockNext(): %s\n", 
               ::flycaptureErrorToString( g_error ) ); 
            return;
         } 
         
         assert( 
            pimage->image.iRows * pimage->image.iRowInc <= g_iImageSizeBytes );
         
         if( iImage == 0 )
         {
            uiPrevSeqNum = pimage->uiSeqNum;
            uiDelta = 1;
         }
         else
         {
            uiDelta = pimage->uiSeqNum - uiPrevSeqNum;
         }
         
         if( uiDelta != 1 )
         {
            printf( "We have missed an image!\n" );
         }

         printf( 
            "Image %04u: buffer = %02u, sequence = %04u, delta = %u\n", 
            iImagesCompleted,
            pimage->uiBufferIndex, 
            pimage->uiSeqNum, 
            uiDelta );         
         
         uiPrevSeqNum = pimage->uiSeqNum;

         iImagesCompleted++;
      }

      //
      // While the rest of the buffers are filling up inside the library, 
      // save the locked buffers.
      //
      for( iImage = 0; iImage < iChunkSize; iImage++ )
      {
         const FlyCaptureImagePlus* pimage = &g_arimageplus[ iImage ];

         bool bRet = avifile.appendFrame( pimage->image.pData, false );
         if( !bRet )
         {
            printf( "Error writing to avi!\n" );
         }

         g_error = ::flycaptureUnlock( g_context, pimage->uiBufferIndex );
         if( g_error != FLYCAPTURE_OK )
         { 
            printf( 
               "flycaptureUnlock: %s\n", ::flycaptureErrorToString( g_error ) ); 
            return;
         } 
      }
   }

   avifile.close();
}


void
colourProcessAvi()
{
   printf( 
      "Converting %s to %s...\n", g_pszTempAviPath, g_pszDestAviPath );

   bool bRet;

   PGRAviFile avifileRaw;
   PGRAviFile avifileColour;

#ifdef USE_COMPRESSOR

   //
   // Retreive the number of compressors.
   //
   int iNumCompressors = avifileColour.enumerateCompressors(
      g_image.iRows,
      g_image.iCols,
      24,
      NULL,
      0 );

   if( iNumCompressors )
   {
      ICINFO* picinfo = new ICINFO[ iNumCompressors ];

      //
      // Retreive information about these compressors.
      //
      avifileColour.enumerateCompressors(
         g_image.iRows,
         g_image.iCols,
         24,
         picinfo,
         iNumCompressors );      
   
      // 
      // Set the compressor to the first one found.
      //
      avifileColour.setCompressor( picinfo[ 0 ] );

      delete [] picinfo;
   }   

#endif

   FlyCaptureImage image;
   image = g_image;
   image.pData = g_pTempFrameBuffer;

   int iFrame = 0;

   bRet = avifileRaw.open( g_pszTempAviPath );
   assert( bRet );
   bRet = avifileColour.open( 
      g_pszDestAviPath, g_image.iCols, g_image.iRows, 24, _AVI_FRAMERATE );
   assert( bRet );

   while( avifileRaw.readNextFrame( g_pTempFrameBuffer, false ) )
   {
      g_error = flycaptureConvertImage( g_context, &image, &g_imageConverted );
      assert( g_error == FLYCAPTURE_OK );      
      
      bRet = avifileColour.appendFrame( g_imageConverted.pData, true );
      assert( bRet );
      
      printf( "Converted frame %d.\n", iFrame++ );
   }

   avifileColour.close();
   avifileRaw.close();   
}


int 
main( int argc, char* argv[] )
{
   //
   // Handle arguments.
   //
   switch( argc )
   {
   case 2:
      // Store destination avi path argument
      strncpy( g_pszDestAviPath, argv[ 1 ], MAX_PATH );

      //
      // Parse out the directory, and use the hardcoded raw avi filename
      // for the temp avi path.
      //
      char pszDrive[ _MAX_DRIVE ];
      char pszDir[ _MAX_DIR ];
      _splitpath( g_pszDestAviPath, pszDrive, pszDir, NULL, NULL );

      sprintf( g_pszTempAviPath, "%s%s\\%s", pszDrive, pszDir, _RAW_AVI_FILENAME );
      break;

   default:
      _snprintf( 
         g_pszDestAviPath, MAX_PATH, ".\\%s", _DEFAULT_COLOUR_AVI_FILENAME );
      _snprintf( 
         g_pszTempAviPath, MAX_PATH, ".\\%s", _RAW_AVI_FILENAME);
      break;
   }

   printf( "%s starting...\n", argv[ 0 ] );
   printf( "Destination avi path: %s\n", g_pszDestAviPath );
   printf( "Temp avi path: %s\n\n", g_pszTempAviPath );

   initBuffers();
   initCamera();

   doGrabLoop();   
   colourProcessAvi();

   deinitCamera();
   deinitBuffers();


   printf( "Done!  (hit enter)" );
   getch();

   return 0;
}


// eof.
