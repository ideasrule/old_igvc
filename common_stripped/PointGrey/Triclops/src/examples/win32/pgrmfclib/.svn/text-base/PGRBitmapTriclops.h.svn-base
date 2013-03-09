//=============================================================================
// Copyright © 2005 Point Grey Research, Inc. All Rights Reserved.
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
// $Id: PGRBitmapTriclops.h,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================
#ifndef __PGRBITMAPTRICLOPS_H__
#define __PGRBITMAPTRICLOPS_H__

#pragma once

//=============================================================================
// System Includes
//=============================================================================
#include <windows.h>

//=============================================================================
// PGR Includes
//=============================================================================
#include <triclops.h>


/**
 * Colours for creating the colourful disparity image.
 *
 * @note Note that these magic numbers are dependant on the magic numbers in
 *       PGRBitmapTriclops::createDisparityImagePalette().
 */
typedef enum
{
   RED	    = 130,
   GREEN    = 129,
   BLUE	    = 128,
   CYAN	    = 131,
   MAGENTA  = 132,
   YELLOW   = 133,

} enum_CONSTANTS;



/**
 * This class provides bitmap functionality, including the ability to import
 * from a TriclopsInput.
 *
 * $Id: PGRBitmapTriclops.h,v 1.1 2007/03/27 21:36:23 demos Exp $
 */
class PGRBitmapTriclops
{
public:

   /**
    * Default constructor.
    *
    * @param iWidth        The width, in pixels, of the image.
    * @param iHeight       The height, in pixels, of the image.
    * @param iBitsPerPixel The bits per pixel of the image.
    * @param bOwnsData	   If true, this bitmap will allocate and manage it's 
    *                      own internal buffer.
    * @param pImageData    If bOwnsData is false, this is the first chance to
    *			   set the internal buffer from outside.
    */
   PGRBitmapTriclops( 
      int	     iWidth, 
      int	     iHeight, 
      int	     iBitsPerPixel  = 32, 
      bool	     bOwnsData	    = true, 
      unsigned char* pImageData     = NULL );

   /** Default destructor. */
   virtual ~PGRBitmapTriclops();

   /** Returns the current data pointer. */
   unsigned char* getDataPointer() const;

   /**
    * Allows the user to directly set the data pointer.  This should only be
    * if the bitmap does not own its own data.  This has similar functionality
    * to setBitmap().
    */
   BOOL setDataPointer( unsigned char* pBuffer );

   /**
    * Set the image dimensions.  Note that this does not change the buffer
    * size, it only changes the output size.
    */
   int setImageDimensions( int iWidth, int iHeight );

   /**
    * Retrieve the image dimensions.
    */
   void getImageDimensions( int* piWidth, int* piHeight ) const;

   /**
    * Set the bitmap from the specified parameters.  If the current memory
    * is owned by the bitmap, it will be freed.
    */
   void	setBitmap( 
      int iWidth, int iHeight, int iBitsPerPixel, unsigned char* pData );

   /**
    * Paint the current bitmap to a device.
    *
    * @param hDC  The handle to the device context.  Use GetSafeHDC() (or
    *             something) under win32.
    *
    * @param iDestXOrigin  Destination X origin in the window.  Use 0.
    * @param iDestYOrigin  Destination Y origin in the window.  Use 0.
    * @param iDestWidth	   Width to use.  This determintes stretching if 
    *                      necessary.  Don't supply this param.
    * @param iDestHeight   Height to use.  THis determines stretching if 
    *                      necessary.  Don't supply this param.
    */
   int paintToDevice( 
      HDC hDC, 
      int iDestXOrigin, 
      int iDestYOrigin,
      int iDestWidth = -1, 
      int iDestHeight = -1 ) const;

   /**
    * Save image in bitmap format to the specified path and filename.
    * 
    * @note Must be in BGR (24 bit) or BGRU (32 bit) format.
    */
   BOOL saveImageToBMP( const char* pszFilename ) const;

   /**
    * Save image in .ppm format to the specified path and filename.
    *
    * @note Assumes RGB format. (does not munge the bytes before writing to
    *       disk.)
    * @note Assumes 24 bit image.
    */
   BOOL saveImageToPPM( const char* pszFilename ) const;

   /**
    * Save image in .ppm format to the specified path and filename.
    *
    * @note Must be in BGR (24 bit) or BGRU (32 bit) format.
    */
   BOOL saveImageBGRToPPM( const char* pszFilename ) const;

   /**
    * Save an 8-bit image to PGM format.
    *
    * @note Assumes 8 bit image.
    */
   BOOL saveImageToPGM( const char* pszFilename ) const;

   /**
    * Fills the current pointed-to memory with an "attractive" colour ramp.
    * @note Assumes 24 or 32 bits!
    */
   void	fillWithColourRamp();

   /**
    * Fills the current pointed-to memory with an "attractive" b/w ramp.
    * @note Assumes 8 bits!
    *
    */
   void	fillWithBWRamp();

   /**
    * Resets the 8-bit colour palette to the standard colour we use for 
    * disparity images.  This doesn't follow any known standard and uses many 
    * hardcoded values.  Copied directly from the drawimage.* library.
    */
   void	createDisparityImagePalette();
   
   /**
    * Set the bitmap from a triclops input.  If the input is planar (TriInp_RGB)
    * the data is copied into the bitmap.  If it is in 32 bit packed format,
    * the data pointer is modified.
    */
   BOOL	setBitmapFromTriclopsInput( TriclopsInput* pTriclopsInput );

   /**
    * Set the bitmap from a TriclopsImage structure.  For now we assume
    * that the data is 8 bit. (ie, a rectified or 8 bit disparity image.)
    */
   BOOL setBitmapFromTriclopsImage( TriclopsImage* pTriclopsImage );
   
   /**
    * Set the bitmap from a TriclopsPackedColorImage structure.  For now we assume
    * that the data is 32 bit and hopefully BGRU.
    */
   BOOL	setBitmapFromTriclopsImage( TriclopsPackedColorImage* pimage );

   /** Get the rgb value for a given pixel location in the bitmap.
    *  If the bitmap is black and white, all 3 values will be the same.
    */
   BOOL getPixel( 
      unsigned int   nRow, 
      unsigned int   nCol, 
      unsigned char* red, 
      unsigned char* green,
      unsigned char* blue );


private:

   /** Internal helper function. */
   void initBitmapInfo();

   /** Internal helper function. */
   void initBitmapInfoHeader();

   
private:
   
   /**
    * Whether this object owns the data or not (ie, whether it has created it 
    * and will free it upon deletion.
    */
   BOOL	 m_bOwnsData;

   /** Image bpp. */
   int	 m_iBitsPerPixel;

   /** Image cols. */
   int	 m_iWidth;

   /** Image rows. */
   int	 m_iHeight;

   /**
    * The image data, in any format.  For higher colour depths than 8, this
    * is always assumed to be BGR.  But really, it doesn't matter unless you're
    * displaying or saving images.
    */
   unsigned char*  m_pData;
   
   /** Bitmap info structure for painting to a windows device handle. */
   BITMAPINFO* m_pBitmapInfo;
   
};


#endif // #ifndef __PGRBITMAPTRICLOPS_H__
