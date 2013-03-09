//=============================================================================
// Copyright © 2000 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with PGR.
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: pnmutils.cpp,v 1.16 2007/10/24 01:40:25 mgriffin Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

//=============================================================================
// PGR Includes
//=============================================================================
#include "pgrpnmio.h"

//=============================================================================
// Project Includes
//=============================================================================
#include "pnmutils.h"

//=============================================================================
// Definitions
//=============================================================================
//#define DEBUG

//
// Disable some uncorrectable warnings so this file compiles cleanly with MSDEV
// W4.
//
#ifdef WIN32
#pragma warning (disable: 4514) // unreferenced inline function has been removed
#pragma warning (disable: 4505) // unreferenced local function has been removed
#pragma warning (disable: 4100) // unreferenced formal parameter
#endif

//
// Format definitions for comments in .PPM files.
//
#define TIME_FORMAT " time = %lf"
#define TIME_FORMAT_OUT "time = %.3lf"



//==================== EXPORTED FUNCTIONS ===================================
//
// The following functions are defined in 'pnmutils.h' and are callable
// from other files
//
// see pnmutils.h for function descriptions
//


TriclopsBool
pgmReadToTriclopsImage( const char* 	filename,
			TriclopsImage*	image ) 
{
   int	 nrows;
   int	 ncols;

   unsigned char* data;

   if ( !pgm8Read( filename, NULL, &nrows, &ncols, &data ) )
   {
      return false;
   }

   image->nrows		= nrows;
   image->ncols		= ncols;
   image->rowinc	= ncols;
   image->data		= data;

   return true;
}


TriclopsBool
pgmReadToTriclopsImage16( const char* 		filename,
			  TriclopsImage16*	image )
{
   int	 nrows;
   int	 ncols;

   unsigned short* data;

   if ( !pgm16Read( filename, NULL, &nrows, &ncols, &data ) )
   {
      return false;
   }

   image->nrows		= nrows;
   image->ncols		= ncols;
   image->rowinc	= ncols * 2;  // rowinc in a TriclopsImage16 is in bytes
   image->data		= data;

   return true;
}


TriclopsBool
pgmRead3ToTriclopsInput(const char*    redFileName,
			const char*    greFileName,
			const char*    bluFileName,
			TriclopsInput* pInput )
{
   char           szComment[ 200 ];
   double         dTimestamp = 0;

   int            nrows;
   int            ncols;

   unsigned char* pRedData;
   unsigned char* pGreData;
   unsigned char* pBluData;

   if (!pgm8Read( redFileName, szComment, &nrows, &ncols, &pRedData ) ||
       !pgm8Read( greFileName, szComment, &nrows, &ncols, &pGreData ) ||
       !pgm8Read( bluFileName, szComment, &nrows, &ncols, &pBluData ) )
   {
      return false;
   }

   // try to grab the time stamp from the comment.
   if ( sscanf( szComment, TIME_FORMAT, &dTimestamp ) > 0 )
   {
      pInput->timeStamp.sec   = (long)floor(dTimestamp);
      pInput->timeStamp.u_sec = (long)((dTimestamp - pInput->timeStamp.sec)*1000000.0);
   }
   else
   {
      pInput->timeStamp.sec   = 0;
      pInput->timeStamp.u_sec = 0;
   }

   pInput->inputType   = TriInp_RGB;
   pInput->nrows       = nrows;
   pInput->ncols       = ncols;
   pInput->rowinc      = ncols;
   pInput->u.rgb.red   = pRedData;
   pInput->u.rgb.green = pGreData;
   pInput->u.rgb.blue  = pBluData;

   return true;
}


TriclopsBool
ppmReadToTriclopsInput( const char*	filename,
			TriclopsInput*	input )
{
   int	 nrows;
   int	 ncols;

   unsigned char* 	data;
   char                 szComment[ 200 ];
   double               dTimestamp = 0;

   if ( !ppm8ReadPacked( filename, szComment, &nrows, &ncols, &data ) )
   {
      return false;
   }

   if ( sscanf( szComment, TIME_FORMAT, &dTimestamp ) > 0 )
   {
      input->timeStamp.sec = (long)floor(dTimestamp);
      input->timeStamp.u_sec = (long)((dTimestamp - input->timeStamp.sec)*1000000.0);
   }
   else
   {
      input->timeStamp.sec = 
      input->timeStamp.u_sec = 0;
   }

   input->inputType		= TriInp_RGB_32BIT_PACKED;
   input->nrows			= nrows;
   input->ncols			= ncols;
   input->rowinc		= 4 * ncols;
   input->u.rgb32BitPacked.data	= data;

   return true;
}


TriclopsBool
ppmReadToTriclopsInputRGB( const char*	  filename,
			   TriclopsInput* input )
{
   int	 nrows;
   int	 ncols;

   char                 szComment[200];
   double               dTimestamp = 0;

   unsigned char* red;
   unsigned char* green;
   unsigned char* blue;

   if ( !ppm8ReadRGB( filename, 
                      szComment, 
                      &nrows, 
                      &ncols, 
                      &red, 
                      &green, 
                      &blue ) )
   {
      return false;
   }

   // set up timestamp
   if ( sscanf( szComment, TIME_FORMAT, &dTimestamp ) > 0 )
   {
      input->timeStamp.sec = (long)floor(dTimestamp);
      input->timeStamp.u_sec = (long)((dTimestamp - input->timeStamp.sec)*1000000.0);
   }
   else
   {
      input->timeStamp.sec = 0;
      input->timeStamp.u_sec = 0;
   }

   input->inputType	= TriInp_RGB;
   input->nrows		= nrows;
   input->ncols		= ncols;
   input->rowinc	= ncols;
   input->u.rgb.red	= red;
   input->u.rgb.green	= green;
   input->u.rgb.blue	= blue;

   return true;
}


TriclopsBool
pgmReadToTriclopsInput( const char*	filename,
			TriclopsInput*	input )
{
   int	 nrows;
   int	 ncols;

   unsigned char* 	data;
   char                 szComment[ 200 ];
   double               dTimestamp = 0;

   if ( !pgm8Read( filename, szComment, &nrows, &ncols, &data ) )
   {
      //
      // This is a hack to allow loading of side-by-side .ppm images with
      // the same call.  Just strip out the green channel and use it instead
      // of the greyscale input.
      //
      unsigned char* pRed;
      unsigned char* pBlue;
      if( !ppm8ReadRGB(
	 filename, 
	 szComment, 
	 &nrows, 
	 &ncols,
	 &pRed,
	 &data,
	 &pBlue ) )
      {
	 return false;
      }
      free( pRed );
      free( pBlue );
   }

   if ( sscanf( szComment, TIME_FORMAT, &dTimestamp ) > 0 )
   {
      input->timeStamp.sec = (long)floor(dTimestamp);
      input->timeStamp.u_sec = (long)((dTimestamp - input->timeStamp.sec)*1000000.0);
   }
   else
   {
      input->timeStamp.sec = 0;
      input->timeStamp.u_sec = 0;
   }

   // we expect a 4x3 aspect ratio
   int nExpectedCols = nrows*4/3;

   if ( ncols == 2*nExpectedCols )
   {
      // this is a 2-image-side-by-side image
      input->inputType		= TriInp_RGB;
      input->nrows		= nrows;
      input->ncols		= ncols/2;
      input->rowinc		= ncols;

      input->u.rgb.red		= 
	 data;
      input->u.rgb.green	= (unsigned char*)input->u.rgb.red + input->rowinc/2;
      input->u.rgb.blue	 	= (unsigned char*)input->u.rgb.red;
   }
   else if ( ncols == 3*nExpectedCols )
   {
      // this is a 3-image-side-by-side image
      input->inputType		= TriInp_RGB;
      input->nrows		= nrows;
      input->ncols		= ncols/3;
      input->rowinc		= ncols;

      input->u.rgb.red		= 
	 data;
      input->u.rgb.green = (unsigned char*)input->u.rgb.red + input->rowinc/3;
      input->u.rgb.blue = (unsigned char*)input->u.rgb.green + input->rowinc/3;
   }
   else
   {
      // the image does not match a 8x3 or 12x3 aspect ratio so it is not 
      // a row-interleaved 4x3 image
      return false;
   }

   return true;
}


TriclopsBool
freeInput( TriclopsInput* input )
{
   if ( input->inputType == TriInp_RGB )
   {
      //
      // Assume this image is row-interleaved (and mass-allocated)
      // if rowinc > ncols.
      //
      if( input->rowinc / input->ncols > 1 )
      {
	 if( input->u.rgb.red )
	 {
	    free( input->u.rgb.red );
	 }
      }
      else
      {
	 if ( input->u.rgb.red )	
	 {
	    free( input->u.rgb.red );
	 }
	 if ( input->u.rgb.green )
	 {
	    free( input->u.rgb.green );
	 }
	 if ( input->u.rgb.blue )
	 {
	    free( input->u.rgb.blue );
	 }
      }      
   }
   else if ( input->inputType == TriInp_RGB_32BIT_PACKED )
   {
      if ( input->u.rgb32BitPacked.data )	
      {
	 free( input->u.rgb32BitPacked.data );
      }
   }
   else
   {
      return false;
   }
   return true;
}

TriclopsBool
freeImage( TriclopsImage* pimage )
{
   if( pimage->data != NULL )
   {
      free( pimage->data );
      pimage->data = NULL;
   }

   return true;
}


TriclopsBool
freeImage16( TriclopsImage16* pimage )
{
   if( pimage->data != NULL )
   {
      free( pimage->data );
      pimage->data = NULL;
   }

   return true;
}


TriclopsBool
pgmWriteFromTriclopsImage( const char*     filename,
			   TriclopsImage*  image)
{
   // open pgm file for writing
   FILE * stream	= fopen( filename, "wb" );
   if ( !stream )
   {
      return false;
   }

   fprintf( stream, "P5\n" );		// PGM magic id value
   fprintf( stream,			// size of pgm image 
	    "%d %d\n",
	    image->ncols, image->nrows );
   fprintf( stream, "255\n" );		// maximum value of an unsigned char

   for ( int row = 0; row < image->nrows; row++ )
   {
      // find the pointer to the beginning of the row
      unsigned char * rowP	= image->data + (row * image->rowinc);
      // write the row to the image file
      fwrite( rowP, image->ncols, 1, stream );
   }
   
   // close the file handle
   fclose( stream );
   return true;
}


TriclopsBool
pgmWrite3FromTriclopsInput(const char*	  redFilename,
			   const char*	  greFilename,
			   const char*	  bluFilename,
			   TriclopsInput* input )
{
   return pgmWrite3FromTriclopsInputWithComment( 
      redFilename, greFilename, bluFilename, "", input );
}


TriclopsBool
pgmWrite3FromTriclopsInputWithComment(const char*     redFilename,
				      const char*     greFilename,
				      const char*     bluFilename,
				      const char*     comment,
				      TriclopsInput*  input )
{
   // open pgm files for writing
   FILE* redstream	= fopen( redFilename, "wb" );
   if ( !redstream )
   {
      return false;
   }
   
   FILE* grestream	= fopen( greFilename, "wb" );
   if ( !grestream )
   {
      fclose( redstream );
      return false;
   }
   
   FILE* blustream	= fopen( bluFilename, "wb" );
   if ( !blustream )
   {
      fclose( redstream );
      fclose( grestream );
      return false;
   }

   // write headers to all images...
   fprintf( redstream, "P5\n" );	  // PGM magic id value
   if ( comment != NULL )
   {
      fprintf( redstream, "# %s\n", comment );         // comment
   }
   fprintf( redstream,			  // size of pgm image 
      "%d %d\n",
      input->ncols, input->nrows );
   fprintf( redstream, "255\n" );	  // maximum value of an unsigned char

   fprintf( grestream, "P5\n" );	  // PGM magic id value
   if ( comment != NULL )
   {
      fprintf( grestream, "# %s\n", comment );         // comment
   }
   fprintf( grestream,			  // size of pgm image 
      "%d %d\n",
      input->ncols, input->nrows );
   fprintf( grestream, "255\n" );	  // maximum value of an unsigned char

   fprintf( blustream, "P5\n" );	  // PGM magic id value
   if ( comment != NULL )
   {
      fprintf( blustream, "# %s\n", comment );         // comment
   }
   fprintf( blustream,			  // size of pgm image 
      "\n%d %d\n",
      input->ncols, input->nrows );
   fprintf( blustream, "255\n" );	  // maximum value of an unsigned char

   int i;
   int r;
   int c;

   switch( input->inputType )
   {
   case TriInp_RGB_32BIT_PACKED:
      for( i = 0, r = 0; r < input->nrows; r++ )
      {
	 for ( c = 0; c < input->ncols; c++ )
	 {
	    fwrite(((unsigned char *)input->u.rgb32BitPacked.data)+i++, 
	       1, 1, blustream );
	    fwrite(((unsigned char *)input->u.rgb32BitPacked.data)+i++, 
	       1, 1, grestream );
	    fwrite(((unsigned char *)input->u.rgb32BitPacked.data)+i++, 
	       1, 1, redstream );
	    i++;
	 }
	 i+=input->rowinc;
      }
	 break;
   case TriInp_RGB:
      for( i = 0; i < input->nrows * input->rowinc; i += input->rowinc )
      {
	 fwrite( (unsigned char *)input->u.rgb.red+i,   1, input->ncols, redstream );
	 fwrite( (unsigned char *)input->u.rgb.green+i, 1, input->ncols, grestream );
	 fwrite( (unsigned char *)input->u.rgb.blue+i,  1, input->ncols, blustream );
      }
      break;

   default:
      fclose( redstream );
      fclose( grestream );
      fclose( blustream );
      return false;
   }
   
   // close the file handle
   fclose( redstream );
   fclose( grestream );
   fclose( blustream );
   return true;
}


TriclopsBool
pgmWriteFromTriclopsImage16(const char*	filename,
			    TriclopsImage16*	image )
{  
   // open pgm file for writing
   FILE * stream	= fopen( filename, "wb" );
   if ( !stream )
   {
      return false;
   }

   fprintf( stream, "P5\n" );		// PGM magic id value
   fprintf( stream,			// size of pgm image 
	    "%d %d\n",
	    image->ncols, image->nrows );
   fprintf( stream, "%d\n", 0xffff );	// maximum value of an unsigned short

   // figure out how many unsigned shorts there are between rows
   // (rowinc = row increment in 'bytes' not in 'unsigned short' units)
   int	pixelinc	= image->rowinc/sizeof(unsigned short);
   for ( int row = 0; row < image->nrows; row++ )
   {
      // find the pointer to the beginning of the row
      unsigned short * rowP	= image->data + (row * pixelinc);
      // write the row to the image file
      fwrite( rowP, image->ncols, sizeof(unsigned short), stream );
   }
   
   // close the file handle
   fclose( stream );
   return true;
}


TriclopsBool
ppmWriteFromTriclopsInput(const char*	  filename,
			  TriclopsInput*  input )
{
   return ppmWriteFromTriclopsInputWithComment( filename, NULL, input );
}


TriclopsBool
ppmWriteFromTriclopsInputWithComment(const char*      filename,
				     const char*      comment,
				     TriclopsInput*   input )
{
   // open pgm file for writing
   FILE * stream	= fopen( filename, "wb" );
   if ( !stream )
   {
      return false;
   }

   fprintf( stream, "P6\n" );		// PPM magic id value
   if ( comment != NULL )
   {
      fprintf( stream, "# %s\n", comment );          // comment
   }
   else
   {
      fprintf( stream, "#" TIME_FORMAT_OUT "\n",
	 (double)(input->timeStamp.sec+input->timeStamp.u_sec/1000000.0 ));
   }

   fprintf( stream,			// size of ppm image 
	    "%d %d\n",
	    input->ncols, input->nrows );
   fprintf( stream, "%d\n", 0xff );	// maximum value of an unsigned char

   if ( input->inputType == TriInp_RGB )
   {
      unsigned char*	red	= (unsigned char*)input->u.rgb.red;
      unsigned char*	green	= (unsigned char*)input->u.rgb.green;
      unsigned char*	blue	= (unsigned char*)input->u.rgb.blue;
            
      // ppm image expects rgb to be interleaved
      for( int i = 0; i < input->nrows * input->rowinc; i += input->rowinc )
      {
	 for( int col = 0; col < input->ncols; col++ )
	 {
	    fwrite( red+i+col,   1, 1, stream );
	    fwrite( green+i+col, 1, 1, stream );
	    fwrite( blue+i+col,  1, 1, stream );
	 }
      }
   }
   else if ( input->inputType == TriInp_RGB_32BIT_PACKED )
   {
      // RGB 32bit packed is in the format [B G R U] [B G R U]...
      unsigned char* data = (unsigned char*)input->u.rgb32BitPacked.data;

      for ( int row = 0; row < input->nrows; row++ )
      {
	 for ( int col = 0; col < input->ncols; col++ )
	 {
	    fwrite( data+2, 1, 1, stream );	// write red pixel
	    fwrite( data+1, 1, 1, stream );	// write green pixel
	    fwrite( data+0, 1, 1, stream );	// write blue pixel
	    data	+= 4;			// advance one quadlet
	 }
	 data += input->rowinc - 4*input->ncols;
      }
   }
   else
   {
      return false;
   }

   // close the file handle
   fclose( stream );
   return true;
}


TriclopsBool
pgmWriteFromTriclopsInput(const char*	  	filename,
			  TriclopsInput*  	input,
			  int 			nCameras )
{
   return pgmWriteFromTriclopsInputWithComment( filename, NULL, input, nCameras );
}


TriclopsBool
pgmWriteFromTriclopsInputWithComment(const char*      	filename,
				     const char*      	comment,
				     TriclopsInput*   	input,
				     int 		nCameras )
{
   // open pgm file for writing
   FILE * stream	= fopen( filename, "wb" );
   if ( !stream )
   {
      return false;
   }

   fprintf( stream, "P5\n" );		// PPM magic id value
   if ( comment != NULL )
   {
      fprintf( stream, "# %s\n", comment);	// comment
   }
   else 
   {
      fprintf( stream, "# " TIME_FORMAT_OUT "\n",
	 (double)(input->timeStamp.sec+input->timeStamp.u_sec/1000000.0 ));
   }

   fprintf( stream,			// size of ppm image 
	    "%d %d\n",
	    nCameras*input->ncols, input->nrows );
   fprintf( stream, "%d\n", 0xff );	// maximum value of an unsigned char

   if ( input->inputType == TriInp_RGB )
   {
      
      // write out as row interleaved
      for ( int row = 0; row < input->nrows; row++ )
      {
	 unsigned char*	red	= (unsigned char*)input->u.rgb.red + row*input->rowinc;
	 unsigned char*	green	= (unsigned char*)input->u.rgb.green + row*input->rowinc;
	 unsigned char*	blue	= (unsigned char*)input->u.rgb.blue + row*input->rowinc;
	 int col;

	 // write out red row
	 for ( col = 0; col < input->ncols; col++ )
	 {
	    fwrite( red + col,   1, 1, stream );	// write red pixel
	 }
	 // write out green row
	 for ( col = 0; col < input->ncols; col++ )
	 {
	    fwrite( green + col,   1, 1, stream );	// write red pixel
	 }
	 if ( nCameras == 3 )
	 {
	    // write out green row
	    for ( col = 0; col < input->ncols; col++ )
	    {
	       fwrite( blue + col,   1, 1, stream );	// write red pixel
	    }
	 }

      }
   }
   else if ( input->inputType == TriInp_RGB_32BIT_PACKED )
   {
      // RGB 32bit packed is in the format [R G B U] [R G B U]...

      for ( int row = 0; row < input->nrows; row++ )
      {

	 unsigned char*	pRow	= (unsigned char*)input->u.rgb32BitPacked.data + row*input->rowinc;
	 int col;

	 // write out red row
	 for ( col = 0; col < 4*input->ncols; col += 4 )
	 {
	    fwrite( pRow + col + 0,   1, 1, stream );	// write red pixel
	 }
	 // write out green row
	 for ( col = 0; col < 4*input->ncols; col += 4 )
	 {
	    fwrite( pRow + col + 1,   1, 1, stream );	// write red pixel
	 }
	 if ( nCameras == 3 )
	 {
	    // write out green row
	    for ( col = 0; col < 4*input->ncols; col += 4 )
	    {
	       fwrite( pRow + col + 2,   1, 1, stream );	// write red pixel
	    }
	 }
      }
   }
   else
   {
      return false;
   }

   // close the file handle
   fclose( stream );
   return true;
}


TriclopsBool
ppmWriteFromTriclopsColorImage( const char*	      filename,
				TriclopsColorImage*   image )
{
   // open pgm file for writing
   FILE* stream	= fopen( filename, "wb" );
   if ( !stream )
   {
      return false;
   }

   fprintf( stream, "P6\n" );		// PPM magic id value
   fprintf( stream,			// size of ppm image 
	    "%d %d\n",
	    image->ncols, image->nrows );
   fprintf( stream, "%d\n", 0xff );	// maximum value of an unsigned char

   // ppm image expect rgb to be interleaved
   for ( int row = 0; row < image->nrows; row++ )
   {
      unsigned char*	red	= image->red + image->rowinc * row;
      unsigned char*	green	= image->green + image->rowinc * row;
      unsigned char*	blue	= image->blue + image->rowinc * row;

      for ( int col = 0; col < image->ncols; col++ )
      {
	 fwrite( red++,   1, 1, stream );	// write red pixel
	 fwrite( green++, 1, 1, stream );	// write green pixel
	 fwrite( blue++,  1, 1, stream );	// write blue pixel
      }
   }

   // close the file handle
   fclose( stream );
   return true;
}



