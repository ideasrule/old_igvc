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
// $Id: customizedstereo.cpp,v 1.3 2005/02/18 02:48:24 mgibbons Exp $
//=============================================================================
//=============================================================================
// customizedstereo:
//
// This example demonstrates the effect of a variety of Triclops library 
// parameters.  The idea is to show by example just some of the many parameters
// you can control to tweak the behaviour of the stereo algorithm.  
//
// The best way to get an idea of how well the stereo is working is to
// run the digiclopsDemo program. This allows you to change all the parameters
// and observe their effects in real time. The purpose of this example is to
// show how easy it is to control there parameters in your own stereo program.
//
// All the images in this series except for the 16-bit subpixel value are 
// scaled for better viewing.  Invalid pixels are black.
// 
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



// Scale a 16 bit image to an 8 bit image. 
//
// note: it is assumed that the TriclopsImage has been pre-allocated
void
scaleImage16ToImage8( TriclopsImage16* 	image16,
		      TriclopsImage* 	image,
		      unsigned char	ucMinOut,
		      unsigned char	ucMaxOut )
{

   double dMinOut = (double) ucMinOut;
   double dMaxOut = (double) ucMaxOut;

   // find the max and minimum disparities
   double dMaxDisp = 0;
   double dMinDisp = 255;
   int    r;

   for ( r = 0; r < image16->nrows; r++ )
   {
      unsigned short* 	pusSrc = image16->data + r*image16->rowinc/sizeof(unsigned short);
      for ( int c = 0; c < image16->ncols; c++ )
      {
	 if ( pusSrc[c] < 0xff00 )
	 {
	    double dDisp = (double) pusSrc[c]/256.0;
	    if ( dMaxDisp < dDisp )
	       dMaxDisp = dDisp;
	    if ( dMinDisp > dDisp )
	       dMinDisp = dDisp;
	 }
      }
   }

   // scale the output to take the disparity values of the input image that fall within
   // dMinDisp to dMaxDisp to fall within ucMinOut and ucMaxOut for the 8 bit output image
   for ( r = 0; r < image16->nrows; r++ )
   {
      unsigned short* 	pusSrc = image16->data + r*image16->rowinc/sizeof(unsigned short);
      unsigned char*	pucDst = image->data + r*image->rowinc;
      for ( int c = 0; c < image16->ncols; c++ )
      {
	 if ( pusSrc[c] < 0xff00 )
	 {
	    double dDisp = (double) pusSrc[c]/256.0;
	    double dOut = (dDisp-dMinDisp)*(dMaxOut-dMinOut)/(dMaxDisp-dMinDisp);
	    dOut += dMinOut;
	    pucDst[c]	= (unsigned char) dOut;
	 }
	 else
	 {
	    pucDst[c]	= 0;
	 }
      }
   }
}

// Scale an 8 bit image 
//
// note: this scaling is done "in place" so it stomps over the current image
void
scaleImage( TriclopsImage* 	image,
	    unsigned char	ucMinOut,
	    unsigned char	ucMaxOut )
{
   int r, c;

   double dMinOut = (double) ucMinOut;
   double dMaxOut = (double) ucMaxOut;

   // find the max and minimum disparities
   double dMaxDisp = 0;
   double dMinDisp = 255;
   for ( r = 0; r < image->nrows; r++ )
   {
      unsigned char* 	pucSrc = image->data + r*image->rowinc;
      for ( c = 0; c < image->ncols; c++ )
      {
	 // note: 240 is the limit of the normal disparity range
	 if ( pucSrc[c] < 240 )
	 {
	    double dDisp = (double) pucSrc[c];
	    if ( dMaxDisp < dDisp )
	       dMaxDisp = dDisp;
	    if ( dMinDisp > dDisp )
	       dMinDisp = dDisp;
	 }
      }
   }

   // scale the output to take the disparity values of the input image that fall within
   // dMinDisp to dMaxDisp to fall within ucMinOut and ucMaxOut for the 8 bit output image
   for ( r = 0; r < image->nrows; r++ )
   {
      unsigned char* 	pucSrc = image->data + r*image->rowinc;
      for ( c = 0; c < image->ncols; c++ )
      {
	 if ( pucSrc[c] < 240 )
	 {
	    double dDisp = (double) pucSrc[c];
	    double dOut = (dDisp-dMinDisp)*(dMaxOut-dMinOut)/(dMaxDisp-dMinDisp);
	    dOut += dMinOut;
	    pucSrc[c]	= (unsigned char) dOut;
	 }
	 else
	 {
	    pucSrc[c]	= 0;
	 }
      }
   }
}

void
doRectifyStereoSave( TriclopsContext 	triclops,
		     TriclopsInput*	pTriclopsInput,
		     const char*	szDisparityBase,
		     const char*     	szNameModifier )
{
   TriclopsError error;
   // Rectify the images
   error = triclopsRectify( triclops, pTriclopsInput );
   _HANDLE_TRICLOPS_ERROR( "triclopsRectify()", error );

   // Do stereo processing
   error =  triclopsStereo( triclops );
   _HANDLE_TRICLOPS_ERROR( "triclopsStereo()", error );
   
   // test whether we are doing subpixel stereo
   TriclopsBool bSubpixel;
   error =  triclopsGetSubpixelInterpolation( triclops, &bSubpixel );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetSubpixelInterpolation()", error );

   char szDisparityFile[1024];
   if ( bSubpixel )
   {
      // Retrieve the disparity image from the context
      TriclopsImage16 triclopsImage16;
      error = triclopsGetImage16( triclops, 
				  TriImg16_DISPARITY, 
				  TriCam_REFERENCE, 
				  &triclopsImage16 );
      _HANDLE_TRICLOPS_ERROR( "triclopsGetImage16(): Failed to get disparity image", 
			      error );
      
      // create the disparity name
      sprintf( szDisparityFile, "%s-%s.pgm", szDisparityBase, szNameModifier );

      // Save the disparity 16-bit image
      error = triclopsSaveImage16( &triclopsImage16, szDisparityFile );
      _HANDLE_TRICLOPS_ERROR( "triclopsSaveImage16(): Failed to save disparity image", error );
      printf( "Wrote 16-bit disparity image to '%s'\n", szDisparityFile );

      // scale it into an 8 bit image and save that as well
      TriclopsImage triclopsImage;
      // need to allocate the 8-bit TriclopsImage to scale this one into
      triclopsImage.nrows 	= triclopsImage16.nrows;
      triclopsImage.ncols 	= triclopsImage16.ncols;
      triclopsImage.rowinc 	= triclopsImage16.ncols;
      triclopsImage.data	= (unsigned char*) 
	 malloc( triclopsImage.nrows*triclopsImage.ncols );

      scaleImage16ToImage8( &triclopsImage16, &triclopsImage, 90, 255 );

      // create the disparity name
      sprintf( szDisparityFile, "%s-%s-scaled.pgm", szDisparityBase, szNameModifier );
      triclopsSaveImage( &triclopsImage, szDisparityFile );
      printf( "Wrote disparity image to '%s'\n", szDisparityFile );
      
      free( triclopsImage.data );

   }	
   else
   {
      // Retrieve the disparity image from the context
      TriclopsImage triclopsImage;
      error = triclopsGetImage( triclops, TriImg_DISPARITY, TriCam_REFERENCE, &triclopsImage );
      _HANDLE_TRICLOPS_ERROR( "triclopsGetImage(): Failed to get disparity image", 
			      error );

      // scale the image so it is easier to see
      scaleImage( &triclopsImage, 90, 255 );
      
      // create the disparity name
      sprintf( szDisparityFile, "%s-%s.pgm", szDisparityBase, szNameModifier );
      
      // Save the disparity image
      error = triclopsSaveImage( &triclopsImage, szDisparityFile );
      _HANDLE_TRICLOPS_ERROR( "triclopsSaveImage(): Failed to save disparity image", error );
      printf( "Wrote disparity image to '%s'\n", szDisparityFile );
   }
   return;
}


int
main( int /* argc */, char* /* * argv */ )
{
   TriclopsContext	triclops;
   TriclopsError       	error;
   TriclopsInput 	triclopsInput;
   char* 		szInputFile 	= "input.ppm";
   char* 		szCalFile 	= "input.cal";
   char* 		szDisparityBase	= "disparity";


   // Get the camera calibration data
   error = triclopsGetDefaultContextFromFile( &triclops, szCalFile );
   _HANDLE_TRICLOPS_ERROR( "triclopsGetDefaultContextFromFile(): "
			   "Can't open calibration file", 
			   error );
   
   // Load images from file
   if ( !ppmReadToTriclopsInput( szInputFile,  &triclopsInput ) )
   {
      printf( "ppmReadToTriclopsInput() failed. Can't read '%s'\n", szInputFile );
      return 1;
   }
   
   doRectifyStereoSave( triclops, &triclopsInput, szDisparityBase, "default" );

   // try doing stereo with a big stereo mask
   triclopsSetStereoMask( triclops, 21 );
   doRectifyStereoSave( triclops, &triclopsInput, szDisparityBase, "bigmask" );

   // try doing stereo with a small stereo mask
   triclopsSetStereoMask( triclops, 5 );
   doRectifyStereoSave( triclops, &triclopsInput, szDisparityBase, "smallmask" );

   // set mask to a neutral value
   triclopsSetStereoMask( triclops, 11 );

   // try doing stereo without any validation
   triclopsSetSurfaceValidation( triclops, 0 );
   triclopsSetUniquenessValidation( triclops, 0 );
   triclopsSetTextureValidation( triclops, 0 );
   doRectifyStereoSave( triclops, &triclopsInput, szDisparityBase, "novalidation" );

   // try doing stereo with only texture and surface
   triclopsSetSurfaceValidation( triclops, 1 );
   triclopsSetTextureValidation( triclops, 1 );
   doRectifyStereoSave( triclops, &triclopsInput, szDisparityBase, "surf-tex-val" );


   // try doing stereo in 2 camera mode with back and forth validation
   triclopsSetCameraConfiguration( triclops, TriCfg_2CAM_HORIZONTAL );
   triclopsSetBackForthValidation( triclops, 1 );
   doRectifyStereoSave( triclops, &triclopsInput, szDisparityBase, "backforth" );

   // try doing stereo in subpixel mode to compare the results of subpixel with
   // whole pixel - try running "convertimage16" example to convert this image to
   // one that is more easily viewed
   triclopsSetSubpixelInterpolation( triclops, 1 );
   doRectifyStereoSave( triclops, &triclopsInput, szDisparityBase, "subpixel" );


   
   // clean up memory allocated in context
   freeInput( &triclopsInput );
   error = triclopsDestroyContext( triclops );
   _HANDLE_TRICLOPS_ERROR( "triclopsDestroyContext()", error );
   
   return 0;
}
