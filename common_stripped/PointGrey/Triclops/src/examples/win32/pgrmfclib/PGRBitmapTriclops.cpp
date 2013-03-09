//=============================================================================
// Copyright © 2003 Point Grey Research, Inc. All Rights Reserved.
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
// $Id: PGRBitmapTriclops.cpp,v 1.2 2007/05/08 18:12:02 soowei Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <cassert>
#include <cstdio>

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRBitmapTriclops.h"



PGRBitmapTriclops::PGRBitmapTriclops( 
                                     int              iWidth, 
                                     int              iHeight, 
                                     int              iBitsPerPixel, 
				     bool	      bOwnsData,
                                     unsigned char*   pImageData )
{
   assert( iBitsPerPixel % 8 == 0 );

   m_iBitsPerPixel = iBitsPerPixel;
   m_iWidth = iWidth;
   m_iHeight = iHeight;
   m_bOwnsData = bOwnsData;

   if( m_bOwnsData )
   {
      m_pData = new unsigned char[ iWidth * iHeight * ( iBitsPerPixel / 8 ) ];
      memset( m_pData, 0, iWidth * iHeight * ( iBitsPerPixel / 8 ) );
   }
   else
   {
      m_bOwnsData = FALSE;
      m_pData = pImageData;
   }

   initBitmapInfo();
}


PGRBitmapTriclops::~PGRBitmapTriclops()
{
   if( m_pData != NULL && m_bOwnsData == TRUE )
   {
      delete[] m_pData;
      m_pData = NULL;
   }

   if( m_pBitmapInfo != NULL )
   {
      free( m_pBitmapInfo );
      m_pBitmapInfo = NULL;
   }
}


void
PGRBitmapTriclops::initBitmapInfo()
{
   //
   // If the colourdepth is 8 bits or lower, we need a colour palette 
   // embedded in the bitmap info structure.
   //

   if ( m_iBitsPerPixel == 8 )
   {
      // we'll need room for the colour palette.

      m_pBitmapInfo = 
	 (BITMAPINFO*)malloc( sizeof( BITMAPINFO ) + sizeof( RGBQUAD ) * 256 );
      assert( m_pBitmapInfo != NULL );

      //
      // assume that it's greyscale for now.
      //
      for( int i = 0; i < 256; i++ )
      {
	 m_pBitmapInfo->bmiColors[i].rgbBlue     = (unsigned char)i;
	 m_pBitmapInfo->bmiColors[i].rgbGreen    = (unsigned char)i;
	 m_pBitmapInfo->bmiColors[i].rgbRed      = (unsigned char)i;
	 m_pBitmapInfo->bmiColors[i].rgbReserved = (unsigned char)0;
      }
   }
   else
   {
      m_pBitmapInfo = (BITMAPINFO*)malloc( sizeof( BITMAPINFO )  );
      assert( m_pBitmapInfo != NULL );
   }

   m_pBitmapInfo->bmiHeader.biSize           = sizeof( BITMAPINFOHEADER );
   m_pBitmapInfo->bmiHeader.biPlanes         = 1;
   m_pBitmapInfo->bmiHeader.biCompression    = BI_RGB;
   m_pBitmapInfo->bmiHeader.biXPelsPerMeter  = 100;
   m_pBitmapInfo->bmiHeader.biYPelsPerMeter  = 100;
   m_pBitmapInfo->bmiHeader.biClrUsed        = 0;
   m_pBitmapInfo->bmiHeader.biClrImportant   = 0;

   initBitmapInfoHeader();
}


void
PGRBitmapTriclops::initBitmapInfoHeader()
{
   m_pBitmapInfo->bmiHeader.biWidth	  = m_iWidth;
   m_pBitmapInfo->bmiHeader.biHeight	  = -m_iHeight; // top-down bitmap, negative height
   m_pBitmapInfo->bmiHeader.biBitCount	  = (unsigned short)m_iBitsPerPixel;
   m_pBitmapInfo->bmiHeader.biSizeImage	  = ::abs( m_iWidth * m_iHeight );
}


void
PGRBitmapTriclops::createDisparityImagePalette()
{
   //
   // setup the 128 greyscale ramp.
   //
   int i = 0;
   for ( i = 0; i < 128; i++ )
   {
      m_pBitmapInfo->bmiColors[i].rgbBlue     = (unsigned char)(i * 2);
      m_pBitmapInfo->bmiColors[i].rgbGreen    = (unsigned char)(i * 2);
      m_pBitmapInfo->bmiColors[i].rgbRed      = (unsigned char)(i * 2);
      m_pBitmapInfo->bmiColors[i].rgbReserved = 0;
   }


   //
   // setup the remaining 6 colors
   //
   for( ; i < 134; i++ )
   {
      m_pBitmapInfo->bmiColors[i].rgbBlue     = 0;
      m_pBitmapInfo->bmiColors[i].rgbGreen    = 0;
      m_pBitmapInfo->bmiColors[i].rgbRed      = 0;
      m_pBitmapInfo->bmiColors[i].rgbReserved = 0;
   }
   
   m_pBitmapInfo->bmiColors[BLUE   ].rgbBlue       = 255;
   m_pBitmapInfo->bmiColors[GREEN  ].rgbGreen      = 255;
   m_pBitmapInfo->bmiColors[RED    ].rgbRed        = 255;
   m_pBitmapInfo->bmiColors[CYAN   ].rgbBlue       = 255;
   m_pBitmapInfo->bmiColors[CYAN   ].rgbGreen      = 255;
   m_pBitmapInfo->bmiColors[MAGENTA].rgbBlue       = 255;
   m_pBitmapInfo->bmiColors[MAGENTA].rgbRed        = 255;
   m_pBitmapInfo->bmiColors[YELLOW ].rgbGreen      = 255;
   m_pBitmapInfo->bmiColors[YELLOW ].rgbRed        = 255;
}

void	 
PGRBitmapTriclops::setBitmap( 
                             int	    iWidth, 
                             int	    iHeight, 
                             int	    iBitsPerPixel, 
                             unsigned char* pData )
{
   assert( pData != NULL );
   
   m_iBitsPerPixel   = iBitsPerPixel;   
   m_iWidth	     = iWidth;
   m_iHeight	     = iHeight;
   
   initBitmapInfoHeader();
   
   if( m_bOwnsData )
   {
      delete[] m_pData;
   }
   
   m_pData = (unsigned char*)pData;
   
   m_bOwnsData = FALSE;
}


int
PGRBitmapTriclops::paintToDevice(
                                 HDC  hDC,
                                 int  iDestXOrigin, 
                                 int  iDestYOrigin, 
                                 int  iDestWidth, 
                                 int  iDestHeight ) const
{
   if ( hDC == (HDC)NULL )
   {
      return 0;
   }
   
   if ( iDestWidth == -1 )
   {
      iDestWidth = m_pBitmapInfo->bmiHeader.biWidth;
   }
   if ( iDestHeight == -1 )
   {
      iDestHeight = ::abs( m_pBitmapInfo->bmiHeader.biHeight );
   }
   
   int iAbsHeight = ::abs( m_pBitmapInfo->bmiHeader.biHeight );
   
   if ( iDestWidth == m_pBitmapInfo->bmiHeader.biWidth && iDestHeight == iAbsHeight )
   {
      
      return ::SetDIBitsToDevice(
         hDC,
         iDestXOrigin, 
         iDestYOrigin,
         m_pBitmapInfo->bmiHeader.biWidth, 
         iAbsHeight,
         0, 
         0,
         0, 
         iAbsHeight,
         m_pData,
         m_pBitmapInfo, 
         DIB_RGB_COLORS );
   }
   else
   {
      //
      // Set the stretching mode - the default mode screws up the colour
      // palette.
      //
      ::SetStretchBltMode( hDC, COLORONCOLOR );
      
      return ::StretchDIBits(
         hDC,
         iDestXOrigin,
         iDestYOrigin,
         iDestWidth,
         iDestHeight,
         0,
         0,
         m_pBitmapInfo->bmiHeader.biWidth, 
         iAbsHeight,
         m_pData, 
         m_pBitmapInfo, 
         DIB_RGB_COLORS,
         SRCCOPY );
   }
}


BOOL
PGRBitmapTriclops::saveImageToPPM( const char* pszPpmFileName ) const
{
   assert( m_iBitsPerPixel == 24 );

   if( pszPpmFileName == NULL )
   {
      return FALSE;
   }

   int iWidth = m_pBitmapInfo->bmiHeader.biWidth;
   int iHeight = ::abs( m_pBitmapInfo->bmiHeader.biHeight );

   FILE* pfile = fopen( pszPpmFileName, "wb" );
   if( pfile )
   {
      ::fprintf( pfile, "P6\n%d %d\n255\n", iWidth, iHeight );
      ::fwrite( ((RGBTRIPLE*)m_pData), iWidth * iHeight * sizeof( RGBTRIPLE ), 1, pfile );
      ::fclose( pfile );

      return TRUE;
   }

   return FALSE;
}


BOOL 
PGRBitmapTriclops::saveImageBGRToPPM( const char* pszFilename ) const
{
   if( pszFilename == NULL )
   {
      return FALSE;
   }

   if( m_iBitsPerPixel != 24 && m_iBitsPerPixel != 32 )
   {
      assert( false );
      return FALSE;
   }

   const int iWidth = m_pBitmapInfo->bmiHeader.biWidth;
   const int iHeight = ::abs( m_pBitmapInfo->bmiHeader.biHeight );
   const int iPixels = iWidth * iHeight;
   assert( iPixels > 0 );

   FILE* pfile;

   if( ( pfile = ::fopen( pszFilename, "wb" ) ) == NULL )
   {
      assert( false );
      return FALSE;
   }
   
   ::fprintf( pfile, "P6\n%d %d\n255\n", iWidth, iHeight  );
   
   if( m_iBitsPerPixel == 24 )
   {
      //
      // Write out bytes in RGB order for .ppm.
      //
      for( int iPixel = 0; iPixel < iPixels; iPixel++ )
      {
	 const unsigned char* pPixel = &m_pData[ iPixel * 3 ];
	 
	 // 
	 // Write BGR pixel in the correct order (RGB) to .PPM.
	 //
	 ::fwrite( pPixel + 2, 1, 1, pfile );	
	 ::fwrite( pPixel + 1, 1, 1, pfile );	
	 ::fwrite( pPixel + 0, 1, 1, pfile );
      }
   }
   else if( m_iBitsPerPixel == 32 )
   {
      //
      // Write out bytes in RGB order for .ppm.
      //
      for( int iPixel = 0; iPixel < iPixels; iPixel++ )
      {
	 const unsigned char* pPixel = &m_pData[ iPixel * 4 ];
	 
	 // 
	 // Write BGR pixel in the correct order (RGB) to .PPM.
	 //
	 ::fwrite( pPixel + 2, 1, 1, pfile );	
	 ::fwrite( pPixel + 1, 1, 1, pfile );	
	 ::fwrite( pPixel + 0, 1, 1, pfile );
      }
   }
   else
   {
      assert( false );
   }  
   
   ::fclose( pfile );

   return TRUE;
}


BOOL 
PGRBitmapTriclops::saveImageToPGM( const char* pszFilename ) const
{
   assert( m_iBitsPerPixel == 8 );

   if( pszFilename == NULL )
   {
      return FALSE;
   }

   const int iWidth = m_pBitmapInfo->bmiHeader.biWidth;
   const int iHeight = ::abs( m_pBitmapInfo->bmiHeader.biHeight );

   FILE* pfile;

   if( ( pfile = ::fopen( pszFilename, "wb" ) ) == NULL )
   {
      assert( false );
      return FALSE;
   }

   ::fprintf( 
      pfile, 
      "P5\n%d %d\n255\n",
      iWidth,
      iHeight  );

   ::fwrite( m_pData, iWidth * iHeight, 1, pfile );
   ::fclose( pfile );

   return TRUE;
}


BOOL 
PGRBitmapTriclops::saveImageToBMP( const char* pszFilename ) const
{
   if( pszFilename == NULL )
   {
      return FALSE;
   }
   
   if( m_iBitsPerPixel != 24 && m_iBitsPerPixel != 32 )
   {
      assert( false );
      return FALSE;
   }
   
   BITMAPFILEHEADER bfh;
   BITMAPINFOHEADER bih;
   
   bfh.bfType = (WORD)(('M'<<8) + 'B');
   bfh.bfReserved1 = 0;
   bfh.bfReserved2 = 0;

   //
   // The wierd %4 stuff here is to pad things out for windows BMP
   // NOTE: THIS MAKES NO SENSE BUT WORKS!!!
   //
   bfh.bfSize =
      sizeof( BITMAPFILEHEADER )
      + sizeof( BITMAPINFOHEADER ) 
      + ::abs((m_pBitmapInfo->bmiHeader.biHeight+(m_pBitmapInfo->bmiHeader.biWidth%4))*m_pBitmapInfo->bmiHeader.biWidth) 
      * sizeof( RGBTRIPLE );
   
   bfh.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
   
   bih = m_pBitmapInfo->bmiHeader;
   bih.biHeight = ::abs( bih.biHeight ); // Positive height
   
   FILE *outfile = fopen( pszFilename, "wb" );
   if( outfile == NULL )
   {
      return FALSE;
   }
   
   ::fwrite( &bfh, sizeof( BITMAPFILEHEADER ), 1, outfile );
   ::fwrite( &bih, sizeof( BITMAPINFOHEADER ), 1, outfile );
   

   if( bih.biBitCount == 24 )
   {
      for( int i = bih.biHeight - 1; i >= 0; i-- )
      {
	 ::fwrite(
	    &( (RGBTRIPLE*)m_pData )[ i * bih.biWidth ],
	    bih.biWidth * sizeof( RGBTRIPLE ),
	    1,
	    outfile );
	 //
	 // The wierd %4 stuff here is to pad things out for windows BMP
	 // NOTE: THIS MAKES NO SENSE BUT WORKS!!!
	 //
	 ::fwrite( m_pData, ( bih.biWidth % 4 ), 1, outfile );
      }      
   }      
   else if( bih.biBitCount == 32 )
   {
      for( int i = bih.biHeight - 1; i >= 0; i-- )
      {
	 ::fwrite(
	    &( (RGBQUAD*)m_pData )[ i * bih.biWidth ],
	    bih.biWidth * sizeof( RGBQUAD ),
	    1,
	    outfile );
	 //
	 // The wierd %4 stuff here is to pad things out for windows BMP
	 // NOTE: THIS MAKES NO SENSE BUT WORKS!!!
	 //
	 ::fwrite( m_pData, ( bih.biWidth % 4 ), 1, outfile );
      }      
   }
   else
   {
      assert( false );
   }  
   
   ::fclose( outfile );
   
   return TRUE;
}


unsigned char*
PGRBitmapTriclops::getDataPointer() const
{
   return m_pData;
}


BOOL 
PGRBitmapTriclops::setDataPointer( unsigned char* pBuffer )
{
   if( pBuffer == NULL )
   {
      assert( false );
      return FALSE;
   }

   if( m_bOwnsData )
   {
      assert( false );
      return FALSE;
   }

   m_pData = pBuffer;

   return TRUE;
}


BOOL
PGRBitmapTriclops::setImageDimensions( int iWidth, int iHeight )
{
   if( iWidth < 0 || iHeight < 0 )
   {
      assert( FALSE );
      return FALSE;
   }
   
   m_iWidth = iWidth;
   m_iHeight = iHeight;

   // New dimentions; set infoheader information
   
   m_pBitmapInfo->bmiHeader.biWidth	  = iWidth;
   m_pBitmapInfo->bmiHeader.biHeight	  = -iHeight; // top-down bitmap, negative height
   m_pBitmapInfo->bmiHeader.biSizeImage	  = ::abs( iWidth * iHeight );

   return TRUE;
}


void
PGRBitmapTriclops::getImageDimensions( int* piWidth, int* piHeight ) const
{
   *piWidth = m_pBitmapInfo->bmiHeader.biWidth;
   *piHeight = ::abs( m_pBitmapInfo->bmiHeader.biHeight );
}


void	 
PGRBitmapTriclops::fillWithColourRamp()
{
   if( m_iBitsPerPixel != 24 && m_iBitsPerPixel != 32 )
   {
      assert( false );
      return;
   }

   if ( m_pData == NULL )
   {
      assert( FALSE );
      return;
   }

   if( m_iBitsPerPixel == 24 )
   {
      for( int iRow = 0; iRow < m_iHeight; iRow++ )
      {
	 unsigned char ucPixel = (unsigned char)( iRow % 256 );
	 
	 for( int iCol = 0; iCol < m_iWidth; iCol++ )
	 {
	    m_pData[ 3 * (iRow * m_iWidth + iCol) ] = (unsigned char)iCol;
	    m_pData[ 3 * (iRow * m_iWidth + iCol) + 1 ] = 0x0;
	    m_pData[ 3 * (iRow * m_iWidth + iCol) + 2 ] = ucPixel;
	 }
      }
   }
   else if ( m_iBitsPerPixel == 32 )
   {
      for( int iRow = 0; iRow < m_iHeight; iRow++ )
      {
	 unsigned char ucPixel = (unsigned char)( iRow % 256 );
	 
	 for( int iCol = 0; iCol < m_iWidth; iCol++ )
	 {
	    m_pData[ 4 * (iRow * m_iWidth + iCol) ] = (unsigned char)iCol;
	    m_pData[ 4 * (iRow * m_iWidth + iCol) + 1 ] = 0x0;
	    m_pData[ 4 * (iRow * m_iWidth + iCol) + 2 ] = ucPixel;
	 }
      }
   }
   else
   {
      assert( false );
   }
}


void	 
PGRBitmapTriclops::fillWithBWRamp()
{
   if( m_iBitsPerPixel != 8  )
   {
      assert( false );
      return;
   }

   if ( m_pData == NULL )
   {
      assert( FALSE );
      return;
   }

   int iMaxValue = 256;
   
   for( int iRow = 0; iRow < m_iHeight; iRow++ )
   {
      unsigned char ucPixel = (unsigned char)(iRow % iMaxValue );
      
      for( int iCol = 0; iCol < m_iWidth; iCol++ )
      {
	 m_pData[ iRow * m_iWidth + iCol ] = ucPixel;
      }
   }
}


BOOL
PGRBitmapTriclops::setBitmapFromTriclopsInput( TriclopsInput* pTriclopsInput )
{
   //
   // Document the fact that this works on planar input also.
   //
   if( pTriclopsInput == NULL )
   {
      assert( FALSE );
      return FALSE;
   }

   //
   // Store some of these values so we don't have to dereference pointers
   // all the time.
   //
   int	 iImageRows	=  pTriclopsInput->nrows;
   int	 iImageCols	=  pTriclopsInput->ncols;
   int	 iImageRowInc	=  pTriclopsInput->rowinc;
   int	 iIncrement	=  iImageRowInc - iImageCols;
   
   if( iImageRows < 1 || iImageCols < 1 )
   {
      assert( FALSE );
      return FALSE;
   }
   
   //
   // Planar input case
   //
   if( pTriclopsInput->inputType == TriInp_RGB )
   {
      assert( m_bOwnsData );
      unsigned char* pucRed   = (unsigned char*)pTriclopsInput->u.rgb.red;
      unsigned char* pucGreen = (unsigned char*)pTriclopsInput->u.rgb.green;
      unsigned char* pucBlue  = (unsigned char*)pTriclopsInput->u.rgb.blue;
      
      for( int iRow = 0; iRow < iImageRows; iRow++ )
      {
	 for (int iCol = 0; iCol < iImageCols; iCol++ )
	 {
	    ((RGBTRIPLE*)m_pData)[ iRow * iImageCols + iCol ].rgbtRed   = *pucRed;
	    ((RGBTRIPLE*)m_pData)[ iRow * iImageCols + iCol ].rgbtGreen = *pucGreen;
	    ((RGBTRIPLE*)m_pData)[ iRow * iImageCols + iCol ].rgbtBlue  = *pucBlue;
	    
	    pucRed++;
	    pucGreen++;
	    pucBlue++;
	 }
	 
	 pucRed   += iIncrement;
	 pucGreen += iIncrement;
	 pucBlue  += iIncrement;      
      }

      m_iBitsPerPixel = 24;
   }
   //
   // 32 bit (RGBU) case
   //
   else
   {
      assert( pTriclopsInput->inputType == TriInp_RGB_32BIT_PACKED );
      assert( !m_bOwnsData );

      m_pData = (unsigned char*)pTriclopsInput->u.rgb32BitPacked.data;
      assert( m_pData != NULL );
      m_bOwnsData = FALSE;

      m_iBitsPerPixel = 32;
      m_iWidth        = pTriclopsInput->ncols;
      m_iHeight       = pTriclopsInput->nrows;
      
   }

   initBitmapInfoHeader();
   
   return TRUE;
}


BOOL	 
PGRBitmapTriclops::setBitmapFromTriclopsImage( TriclopsImage* pTriclopsImage )
{
   //
   // sanity checking
   //
   assert( m_bOwnsData == FALSE );
   if( pTriclopsImage == NULL || m_iBitsPerPixel != 8 )
   {
      assert( false );
      return FALSE;
   }

   m_iWidth    = pTriclopsImage->ncols;
   m_iHeight   = pTriclopsImage->nrows;

   m_pData = pTriclopsImage->data;
   assert( m_pData != NULL );
   m_bOwnsData = FALSE;

   return TRUE;
}


BOOL	 
PGRBitmapTriclops::setBitmapFromTriclopsImage( TriclopsPackedColorImage* pimage )
{
   //
   // sanity checking
   //
   assert( m_bOwnsData == FALSE );
   if( pimage == NULL || m_iBitsPerPixel != 32 )
   {
      assert( false );
      return FALSE;
   }

   m_iWidth    = pimage->ncols;
   m_iHeight   = pimage->nrows;

   m_pData = (unsigned char*)pimage->data;
   assert( m_pData != NULL );
   m_bOwnsData = FALSE;

   return TRUE;
}


BOOL 
PGRBitmapTriclops::getPixel(
                            unsigned int     nRow, 
			    unsigned int     nCol, 
			    unsigned char*   red, 
			    unsigned char*   green,
			    unsigned char*   blue )
{
   // default to black
   *red     = 0;
   *green   = 0;
   *blue    = 0;

   if ( m_pData == NULL )
   {
      return false;
   }

   // check that the requested pixel is in the image
   if ( (signed int) nRow > m_iHeight || (signed int) nCol > m_iWidth )
   {
      return false;
   }

   int nBytesPerPixel = m_iBitsPerPixel/8;

   unsigned char* pucPixel =
      m_pData + nRow*nBytesPerPixel*m_iWidth + nCol*nBytesPerPixel;

   BOOL	 retval = true;

   switch( m_iBitsPerPixel )
   {
   case 8:
      *red     = pucPixel[0];
      *green   = pucPixel[0];
      *blue    = pucPixel[0];
      break;

   case 24:
   case 32:
      *blue    = pucPixel[0];
      *green   = pucPixel[1];
      *red     = pucPixel[2];
      break;

   default:
      retval   = false;
   }

   return retval;
}


