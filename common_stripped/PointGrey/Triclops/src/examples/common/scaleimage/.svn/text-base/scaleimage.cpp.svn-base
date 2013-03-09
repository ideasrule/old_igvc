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
// $Id: scaleimage.cpp,v 1.2 2005/02/28 17:24:21 donm Exp $
//=============================================================================
//=============================================================================
// convertimage16:
//
// Usage: convertimage16 [<input image> <output image>]
//
// Default input image = "disparity-subpixel.pgm"
// Default output image = "disparity-scaled.pgm"
//
// This example shows how to convert a 16 bit disparity image into an 8 bit 
// disparity image for viewing purposes.  It can also be used to convert 16-bit
// images that have been saved by your own processing into an easily viewed
// 8-bit image.  
//
// The PGM file format readily supports 16-bits per image.  The problem is that
// almost all viewers for displaying these images either assume that all images
// are 8-bit (such as PaintShop Pro) or throw away the bottom 8 bits (such as
// the Linux image viewer 'xv').
// 
// This example converts a 16-bit image to an 8-bit image, and also remaps the
// image so that it has more contrast and is easier to view than the original
// image.  It remaps the input image into the 90 to 255 greyscale range.  This
// will generally be both brightening and stretching the range of the input 
// image.
//
// The example image, "disparity-subpixel.pgm" was obtained from the 
// customizedstereo.cpp example in the stereo directory.
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//=============================================================================
// PGR Includes
//=============================================================================
#include "triclops.h"
#include "pnmutils.h"

//=============================================================================
// Project Includes
//=============================================================================


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


int
main( int argc, char** argv )
{
   char* 		szInputFile 	= "disparity.pgm";
   char* 		szOutputFile 	= "disparity-scaled.pgm";
   unsigned char	ucMinOut	= 90;
   unsigned char	ucMaxOut	= 255;

   switch ( argc )
   {
      case 5:
	 ucMinOut 	= (unsigned char) atoi( argv[3] );
	 ucMaxOut 	= (unsigned char) atoi( argv[4] );
	 // deliberately fall through to 3-parm case
      case 3:
	 szInputFile	= argv[1];
	 szOutputFile	= argv[2];
	 break;
      case 1:
	 // use default parameters
	 break;
      default:
	 printf( "Usage: scaleimage <input pgm> <output pgm> [<min out> <max out>]\n" );
	 return 1;
   }

   // Load the bit image from file 
   TriclopsImage image;
   if ( !pgmReadToTriclopsImage( szInputFile,  &image ) )
   {
      printf( "pgmReadToTriclopsImage() failed. Can't read '%s'\n", szInputFile );
      return 1;
   }

   // scale the output image
   scaleImage( &image, ucMinOut, ucMaxOut );

   triclopsSaveImage( &image, szOutputFile );
   
   // clean up memory allocated in context
   freeImage( &image );
   
   return 0;
}





