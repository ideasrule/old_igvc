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
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: writecal.cpp,v 1.4 2007/05/07 18:51:27 soowei Exp $
//=============================================================================
//=============================================================================
// writecal:
//
// Retrieves a .cal calibration file from the camera and writes it to disk.
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <stdio.h>
#include <stdlib.h>

//=============================================================================
// PGR Includes
//=============================================================================
#include "triclops.h"
#include "pgrflycapture.h"
#include "pgrflycapturestereo.h"

//=============================================================================
// Project Includes
//=============================================================================


//
// Macro to check, report on, and handle Triclops API error codes.
//
#define _HANDLE_TRICLOPS_ERROR( description, error )	\
{ \
   if( error != TriclopsErrorOk ) \
   { \
      printf( \
	 "*** Triclops Error '%s' at line %d :\n\t%s\n", \
	 triclopsErrorToString( error ), \
	 __LINE__, \
	 description );	\
      printf( "Press any key to exit...\n" ); \
      getchar(); \
      exit( 1 ); \
   } \
} \


//
// Macro to check, report on, and handle Digiclops API error codes.
//
#define _HANDLE_FLYCAPTURE_ERROR( description, error )	\
{ \
   if( error != FLYCAPTURE_OK ) \
   { \
      printf( \
	 "*** Flycapture Error '%s' at line %d :\n\t%s\n", \
	 flycaptureErrorToString( error ), \
	 __LINE__, \
	 description );	\
      printf( "Press any key to exit...\n" ); \
      getchar(); \
      exit( 1 ); \
   } \
} \


int
main( int /* argc */, char** /* argv */ )
{
   FlyCaptureContext flycapture;
   FlyCaptureInfoEx cameraInfo;   
   FlyCaptureError   fe;
   
   // Create the Flycapture context
   fe = flycaptureCreateContext( &flycapture );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureCreateContext()", fe );
   
   // Initialize the camera
   fe = flycaptureInitialize( flycapture, 0 );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureInitialize()", fe );

   // Get the camera info  
   fe = flycaptureGetCameraInfo( flycapture, &cameraInfo );
   _HANDLE_FLYCAPTURE_ERROR( "flycatpureGetCameraInfo()", fe );

   // A string to hold the calibration file name
   // We will construct a meaningful name for the written calibration file
//   char* szFilename = new char[512];
//   sprintf( szFilename, "bumblebee%7.7u.cal", cameraInfo.SerialNumber );

   // Currently, we can not pass in our own filename to save to when saving
   // the calibration file.  The flycaptureGetCalibrationFileFromCamera function
   // will return the location of the saved file instead.  This will be fixed
   // shortly.
   char* szFilename = NULL;   

   // Get the camera configuration
   fe = flycaptureGetCalibrationFileFromCamera( flycapture, &szFilename );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureGetCalibrationFileFromCamera()", fe );

   printf("Calibration File successfully saved at %s", szFilename);
   
   // Destroy the Flycapture context
   flycaptureDestroyContext( flycapture );  

   if ( szFilename != NULL )
   {
      delete szFilename;
   }
   
   return 0;
}
