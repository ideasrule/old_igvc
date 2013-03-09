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
// $Id: printconfig.cpp,v 1.1 2007/03/22 17:20:11 soowei Exp $
//=============================================================================
//=============================================================================
// printconfig:
//
// Prints the Triclops library version as well as camera information.
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
// Macro to check, report on, and handle Flycapture API error codes.
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
   TriclopsError     te;
   FlyCaptureError   fe;
   TriclopsContext   triclops;
   FlyCaptureContext flycapture;

   float             baseline;
   int		     nrows;
   int               ncols;
   float             focalLength;

   TriclopsCameraConfiguration triclopsConfig;
   
   // Create the Flycapture context
   fe = flycaptureCreateContext( &flycapture );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureCreateContext()", fe );
   
   // Initialize the camera
   fe = flycaptureInitialize( flycapture, 0 );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureInitialize()", fe );
   
   // Get the camera configuration
   char* szCalFile;
   fe = flycaptureGetCalibrationFileFromCamera( flycapture, &szCalFile );
   _HANDLE_FLYCAPTURE_ERROR( "flycaptureGetCalibrationFileFromCamera()", fe );
   
   // Create a Triclops context from the cameras calibration file
   te = triclopsGetDefaultContextFromFile( &triclops, szCalFile );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetDefaultContextFromFile()", te );

   // Destroy the Flycapture context - this program just displays information contained
   // in the TriclopsContext
   flycaptureDestroyContext( flycapture );  

   printf( "Triclops Version  : %s\n", triclopsVersion() );
   
   // get the camera configuration 
   te = triclopsGetCameraConfiguration( triclops, &triclopsConfig );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetCameraConfiguration()", te );
   switch( triclopsConfig )
   {
      case TriCfg_L:
	 printf( "Configuration   : 3 Camera\n" );                   
	 break;
      case TriCfg_2CAM_HORIZONTAL:
	 printf( "Configuration   : 2 Camera horizontal\n" );                   
	 break;
      case TriCfg_2CAM_VERTICAL:
	 printf( "Configuration   : 2 Camera vertical\n" );                   
	 break;
      default:
	 printf( "Unrecognized configuration: %d\n", triclopsConfig ); 
   }
   
   // Get the baseline
   te = triclopsGetBaseline( triclops, &baseline );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetBaseline()", te );
   printf( "Baseline        : %f cm\n", baseline*100.0 );
   
   // Get the focal length
   te = triclopsGetFocalLength( triclops, &focalLength );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetFocalLength()", te );
   te = triclopsGetResolution( triclops, &nrows, &ncols );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetResolution()", te );
   
   printf( "Focal Length    : %f pixels for a %d x %d image\n", 
	   focalLength, 
	   ncols, 
	   nrows ) ;

   int   nRows, nCols;
   te = triclopsGetResolution( triclops, &nRows, &nCols );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetResolution()", te );

   float fCenterRow, fCenterCol;
   te = triclopsGetImageCenter( triclops, &fCenterRow, &fCenterCol );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetImageCenter()", te );

   printf( "The default image resolution for stereo processing is %d x %d\n",
	   nCols, nRows );
   printf( "For this resolution, the 'image center' or 'principal point' "
	   "is:\n" );
   printf( "Center Row = %f\n", fCenterRow );
   printf( "Center Col = %f\n", fCenterCol );

   
   // Destroy the Triclops context
   triclopsDestroyContext( triclops ) ;
   
   return 0;
}
