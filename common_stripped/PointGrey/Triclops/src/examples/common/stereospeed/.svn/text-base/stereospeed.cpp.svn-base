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
// $Id: stereospeed.cpp,v 1.3 2005/02/18 02:48:40 mgibbons Exp $
//=============================================================================
//=============================================================================
// stereospeed:
//
// This example is a profiling program that shows how fast the stereo kernel can
// run on your PC.  It uses a dummy image, but swaps between two copies of the 
// dummy image each iteration.  This is important to make sure that the stereo
// kernel is having to load different memory into its cache each run.
//
// The timing is typically the same whether you swap images or not, but on 
// systems with very large cache this may not be true.
//
// The purpose of this example is to give the user an idea of their maximum
// disparity-pixels/second benchmark.
//
// Note, because the timer is using computer time, rather than  "wall clock" 
// time, the 10 second profiling run may take longer on your system if it is
// being slowed by other programs.
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef LINUX
#include <sys/resource.h>
#include <sys/time.h>
#else
#include <windows.h>
#endif

//=============================================================================
// PGR Includes
//=============================================================================
#include "triclops.h"
#include "pnmutils.h"

//=============================================================================
// Project Includes
//=============================================================================




// Print error and quit program
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


class SimpleTimer
{
   public:
      // This implementation assumes that "clock()" is available on your system
      clock_t	m_nStartTime;
      clock_t 	m_nEndTime;
      clock_t 	m_nElapsedTime;

      SimpleTimer::SimpleTimer()
      {
	 m_nStartTime = 0;
	 m_nEndTime = 0;
	 m_nElapsedTime = 0;
      }

      void start()
      {
	 m_nElapsedTime	= 0;
	 m_nStartTime	= clock();
      };

      // returns number of seconds clocked so far
      double pause()
      {
	 m_nEndTime	= clock();
	 m_nElapsedTime += m_nEndTime - m_nStartTime;
	 return (double) m_nElapsedTime / CLOCKS_PER_SEC;
      }

      // return the current value of the timer without pausing or stopping it
      double getTime()
      {
	 clock_t m_nCurrentTime = clock();
	 return (double)(m_nElapsedTime + m_nCurrentTime-m_nStartTime)/ CLOCKS_PER_SEC;
      };

      void unpause()
      {
	 m_nStartTime	= clock();
      }

      double stop()
      {
	 return SimpleTimer::pause();
      }
};

//=============================================================================
// raisePriority()
//
// This function raises the priority of the process - hopefully preventing it
// from being swapped out!
// 
//=============================================================================
void
raisePriority()
{
#ifdef LINUX
   // crank it up to the most priority ( -20... valid values range
   // from -20 to 20 with -20 as the best priority
   
   // note: according to the man page "only the super use may lower priorities"
   // so you need to run the program as root for this to take effect
   setpriority( PRIO_PROCESS, 0, -20 );
#else
   HANDLE hProcess = GetCurrentProcess();
   BOOL bResult = SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS);

   HANDLE hThread = GetCurrentThread();
   bResult = SetThreadPriority(hThread,THREAD_PRIORITY_TIME_CRITICAL);
#endif
}

//=============================================================================
// lowerPriority()
//
// This function lowers the priority of the process back to its normal state.
// 
//=============================================================================
void
lowerPriority()
{
#ifdef LINUX
   setpriority( PRIO_PROCESS, 0, 0 );
#else
   HANDLE hProcess = GetCurrentProcess();
   BOOL bResult = SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);
   
   HANDLE hThread = GetCurrentThread();
   bResult = SetThreadPriority(hThread,THREAD_PRIORITY_NORMAL);
#endif
}


int
main( int /* argc */, char* /* * argv */ )
{
   TriclopsContext	triclops;
   TriclopsError       	error;
   TriclopsInput 	triclopsInput1;
   TriclopsInput	triclopsInput2;
   char* 		szInputFile 	= "input.ppm";
   char* 		szCalFile 	= "input.cal";



   //===================================================================================
   // *** Start of configuration section ***
   // Triclops settings that affect performance
   // Edit to match the configuration you are trying to profile

   int			nRows			= 240;
   int			nCols			= nRows*4/3;
   int			nDisparities		= 64;
   bool			bSubpixel		= true;
   bool			bTextureValidationOn	= false;
   bool			bUniquenessValidationOn	= false;
   bool			bSurfaceValidationOn	= false;
   
   // choose either "L" - Digiclops, or "2CAM_HORIZONTAL" - Bumblebee
   //TriclopsCameraConfiguration	cameraConfig = TriCfg_L;
   TriclopsCameraConfiguration	cameraConfig = TriCfg_2CAM_HORIZONTAL;
   
   // *** Endof configuration section ***
   // you shouldn't need to edit below this point
   //===================================================================================


   // Get the camera calibration data
   error = triclopsGetDefaultContextFromFile( &triclops, szCalFile );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetDefaultContextFromFile(): "
			   "Can't open calibration file", 
			   error );

   //===================================================================================
   //  update the TriclopsContext with the selected configuration
   triclopsSetCameraConfiguration( triclops, cameraConfig );
   triclopsSetResolution( triclops, nRows, nCols );
   triclopsSetDisparity( triclops, 0, nDisparities-1 );
   triclopsSetSubpixelInterpolation( triclops, bSubpixel );
   triclopsSetTextureValidation( triclops, bTextureValidationOn );
   triclopsSetUniquenessValidation( triclops, bUniquenessValidationOn );
   triclopsSetSurfaceValidation( triclops, bSurfaceValidationOn );

   //===================================================================================
   
   
   

   // Load images from file
   if ( !ppmReadToTriclopsInput( szInputFile,  &triclopsInput1 ) )
   {
      printf( "ppmReadToTriclopsInput() failed. Can't read '%s'\n", szInputFile );
      return 1;
   }

   // Load images from file
   if ( !ppmReadToTriclopsInput( szInputFile,  &triclopsInput2 ) )
   {
      printf( "ppmReadToTriclopsInput() failed. Can't read '%s'\n", szInputFile );
      return 1;
   }
   
   // Do processing once - this makes sure all necessarly lookup tables, etc are
   // build - these are one-time only constructions that make the first call to these
   // functions take longer

   // Rectify the images
   error = triclopsRectify( triclops, &triclopsInput1 );
   _HANDLE_TRICLOPS_ERROR( "triclopsRectify()", error );
     
   // Do stereo processing
   error =  triclopsStereo( triclops );
   _HANDLE_TRICLOPS_ERROR( "triclopsStereo()", error );

   //===================================================================================
   //  Do the time testing

   int nFrames = 0;
   SimpleTimer	stereoTimer;
   SimpleTimer	rectificationTimer;
   SimpleTimer	totalTimer;

   printf( "Starting 10 second profiling run\n" );

   // raise the priority of this process - note under Linux this only works if the
   // process is run by the super-user
   raisePriority();


   stereoTimer.start();
   stereoTimer.pause();
   rectificationTimer.start();
   rectificationTimer.pause();
   totalTimer.start();
   // run for 10 seconds
   while( totalTimer.getTime() < 10.0 )
   {
      TriclopsInput* pTriclopsInput = &triclopsInput1;
      if ( nFrames % 2 )
	 pTriclopsInput = &triclopsInput2;

      // Rectify the images
      rectificationTimer.unpause();
      error = triclopsRectify( triclops, pTriclopsInput );
      _HANDLE_TRICLOPS_ERROR( "triclopsRectify()", error );
      rectificationTimer.pause();
      
      // Do stereo processing
      stereoTimer.unpause();
      error =  triclopsStereo( triclops );
      _HANDLE_TRICLOPS_ERROR( "triclopsStereo()", error );
      stereoTimer.pause();

      nFrames++;
   }
   double dSecs 	= totalTimer.stop();
   double dRectSecs 	= rectificationTimer.stop();
   double dStereoSecs 	= stereoTimer.stop();
   
   // return priority of process to normal
   lowerPriority();


   printf( "\n==========\n" );
   printf( "Stereo resolution: %d x %d\n", nCols, nRows );
   printf( "Number of disparities: %d\n", nDisparities );
   printf( "Subpixel: %s\n", bSubpixel ? "on"  : "off" );
   printf( "Texture validation: %s\n", bTextureValidationOn ? "on"  : "off" );
   printf( "Uniqueness validation: %s\n", bUniquenessValidationOn ? "on"  : "off" );
   printf( "Surface validation: %s\n", bSurfaceValidationOn ? "on"  : "off" );

   printf( "\n==========\n" );
   printf( "Time spent on rectification was %f seconds\n", dRectSecs );
   printf( "Time spent on stereo was %f seconds\n", dStereoSecs );
   double dFps = (double) nFrames/dSecs;
   printf( "Processed %d frames in %f seconds\n",
	   nFrames, dSecs );
   printf( "Frame rate of %f frames/sec\n", dFps );
   double dPixDispPerSec;
   dPixDispPerSec = nRows * nCols * nDisparities * dFps;
   printf( "Disparity-pixels/sec = %f\n", dPixDispPerSec );
   printf( "\n==========\n" );

   //===================================================================================

   // clean up memory allocated in context
   freeInput( &triclopsInput1 );
   freeInput( &triclopsInput2 );
   error = triclopsDestroyContext( triclops );
   _HANDLE_TRICLOPS_ERROR( "triclopsDestroyContext()", error );
   
   return 0;
}
