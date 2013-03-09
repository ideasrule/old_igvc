//=============================================================================
// Copyright © 2003 Point Grey Research, Inc. All Rights Reserved.
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
//
//=============================================================================
//=============================================================================
// $Id: triclopsint.h,v 1.4 2007/06/12 21:51:26 gchow Exp $
//=============================================================================
#ifndef TRICLOPSINT_H
#define TRICLOPSINT_H

//=============================================================================
//
// This file defines the the fixed point API for the Triclops Stereo Vision SDK.
//
//=============================================================================


//=============================================================================
// System Includes  
//=============================================================================
#include "triclops.h"

#ifdef __cplusplus
extern "C"
{
#endif


//
// Name: triclopsFixedRCDToXYZ
//
// Synopsis:
//  This function is the fixed point version of triclopsRCDToXYZ function which
//  converts image coordinates with disparity values that have been mapped 
//  using the disparity mapping function into true 3D points.
//
// Input:
//  context - The stereo context.
//  row_X_2to8     - The row of the input pixel multiplied by 2^8.
//  col_X_2to8     - The column of the input pixel multiplied by 2^8.
//  disp_X_2to8    - The disparity value of the input pixel multiplied by 2^8.	
//
// Output:
//  x_X_2to16 - The x (multiplied by 2^16) coordinate of the point
//      represented by the input row column 
//      disparity in the camera coordinate system.
//  y_X_2to16 - The y (multiplied by 2^16) coordinate of the point
//	represented by the input row column
//      disparity in the camera coordinate system.
//  z_X_2to16 - The z (multiplied by 2^16) coordinate of the point
//	represented by the input row
//      column disparity in the camera coordinate system.
//
// Returns:
//  TriclopsErrorOk  - The operation succeeded.
//  InvalidContext   - The input context was invalid.	
//
// Description:
//  This function takes the 16-bit disparity values (ie. disparity * 2^8) and
//  performed the same RCD to XYZ calculation as in triclopsRCDToXYZ except
//  that all arithmatics are done using fixed point variables.  This function
//  is implemented specifically for the Orbit4 project which uses an Int C64xx
//  DSP which does not directly support floating point arithmatics.
//  Because of the limited precision offered by fixed point variables, there
//  is some round-off errors in comparing the answer provided by this function
//  vs. that from triclopsRCDToXYZ.
//
// Note: It is up to the user to supply valid pixel locations.  Pixels that 
//       have been invalidated may give negative results.
//
// See Also:
//  triclopsRCDFloatToXYZ(), triclopsRCD8ToXYZ(), triclopsRCD16ToXYZ()
//
TriclopsError
triclopsFixedRCDToXYZ( TriclopsContext context, 
		       int             row_X_2to8,  
                       int             col_X_2to8,  
                       unsigned short  disp_X_2to8,
		       int*            x_X_2to16,  
                       int*            y_X_2to16,  
                       int*            z_X_2to16 );


//
// Name: triclopsFixedXYZToRC
//
// Synopsis:
//  This is the fixed point version of triclpsXYZToRC which
//  converts true 3D points into image coordinates. 
//
// Input:
//  context - TriclopsContext set up for desired resolution.
//  x_X_2to16       - X (multiplied by 2^16) value of a point in the Triclops coordinate system.
//  y_X_2to16       - Y (multiplied by 2^16) value of a point in the Triclops coordinate system.
//  z_X_2to16       - Z (multiplied by 2^16) value of a point in the Triclops coordinate system.
//
// Output:
//  row_X_2to8  - The row (multiplied by 2^8) in a disparity image.
//  col_X_2to8  - The column (multiplied by 2^8) in a disparity image.
//
// Returns:
//  TriclopsErrorOk                          - The Operation succeeded.
//  TriclopsErrorInvalidContext - Context is not valid TriclopsContext.
//  TriclopsErrorInvalidRequest - An impossible XYZ value has been provided
//                                (ie: negative Z).
//
// Description:
//  This function takes as input the XYZ position of a point in the Triclops 
//  coordinate system, and determines what row, column, and disparity value 
//  would result from a sensed point at XYZ.  
//
// Note that this function is coded primarily for Censys to figure out how
// to project a transformed 3d point on the right camera of another bumblebee.
// In this case, we do not require the disparity and hence its computation is
// deliberately omitted to save time.  In the rare occassion where the disparity
// is needed, called the floating point version of this function, 
// triclopsXYZToRC().
//
TriclopsError
triclopsFixedXYZToRC( TriclopsContext  context, 
		      int              x_X_2to16,   
                      int              y_X_2to16,   
                      int              z_X_2to16,
		      int*             row_X_2to8, 
                      int*             col_X_2to8 );

//
// Name: triclopsFixedRCDToWorldXYZ
//
// Synopsis:
//  Converts image coordinates and a 16-bit disparity into a world 3D point using fixed
// point arithmatics.
//
// Input:
//  context - The stereo context.
//  row_X_2to8     - The row (multiplied by 2^8) of the input pixel.
//  col_X_2to8     - The column (multiplied by 2^8) of the input pixel.
//  disp_X_2to8    - The disparity value (multiplied by 2^8) of the input pixel.
//
// Output:
//  worldx_X_2to16 - location to return the x coordinate (multiplied by 2^16) 
//		     of the corresponding 3d point in world coordinates	
//  worldy_X_2to16 - location to return the y coordinate (multiplied by 2^16)
//		     of the corresponding 3d point in world coordinates	
//  worldz_X_2to16 - location to return the z coordinate (multiplied by 2^16) 
//		     of the corresponding 3d point in world coordinates	
//
// Returns:
//  TriclopsErrorOk             - The operation succeeded.
//  InvalidContext - The input context was invalid.	
//
// Description:
//  This function takes a 16-bit disparity value and converts it to 
//  XYZ coordinates in world coordinates.  The world coordinates are determined
//  by transforming the point from the Triclops coordinate system to the
//  world coordinate system based on the TriclopsContext transform.
//  All operations are performed using only fixed point arithmatics.
//
// Note: It is up to the user to supply valid disparity values 
//
// See Also:
//  triclopsRCDFloatToXYZ(), triclopsRCDMappedToXYZ(), triclopsRCD8ToXYZ(), 
//  triclopsRCD16ToXYZ(),
//  triclopsSetDisparity(), triclopsSetDisparityMapping()
//  triclopsRCDToWorldXYZ(), triclopsRCDFloatToWorldXYZ(), 
//  triclopsRCD8ToWorldXYZ(), 
//  triclopsSetTriclopsToWorldTransform(), triclopsGetTriclopsToWorldTransform()
//
//
TriclopsError
triclopsFixedRCDToWorldXYZ( TriclopsContext	context,
			    int			row_X_2to8,  
			    int			col_X_2to8,  
			    unsigned short	disp_X_2to8,
			    int*		worldx_X_2to16,  
			    int*		worldy_X_2to16,  
			    int*		worldz_X_2to16 );

//
// Name: triclopsFixedWorldXYZToRC
//
// Synopsis:
//  Converts world 3D points into image coordinates using fixed point arithmatics
//
// Input:
//  context - TriclopsContext set up for desired resolution.
//  worldx_X_2to16 - X value (multiplied by 2^16) of a point in the World coordinate system.
//  worldy_X_2to16 - Y value (multiplied by 2^16) of a point in the World coordinate system.
//  worldz_X_2to16 - Z value (multiplied by 2^16) of a point in the World coordinate system.
//
// Output:
//  row_X_2to8  - The row (multiplied by 2^8) in a disparity image.
//  col_X_2to8  - The column (multiplied by 2^8) in a disparity image.
//
// Returns:
//  TriclopsErrorOk                          - The Operation succeeded.
//  TriclopsErrorInvalidContext - Context is not valid TriclopsContext.
//  TriclopsErrorInvalidRequest - An impossible XYZ value has been provided
//                                (ie: negative Z).
//
// Description:
//  This function takes as input the XYZ position of a point in the World 
//  coordinate system, moves the point to the Triclops coordinate system 
//  (as described by the TriclopsContext transform), and determines what 
//  row, column, and disparity value would result from the resulting point.
//  All operations are performed using only fixed point arithmatics.
//
// See Also:
//  triclopsRCDFloatToXYZ(), triclopsRCDMappedToXYZ(), triclopsRCD8ToXYZ(), 
//  triclopsSetDisparity(), triclopsSetDisparityMapping()
//  triclopsRCDToWorldXYZ(), triclopsRCDFloatToWorldXYZ(), 
//  triclopsRCD8ToWorldXYZ(), triclopsRCD16ToWorldXYZ(), 
//  triclopsXYZToRCD(),
//  triclopsSetTriclopsToWorldTransform(), triclopsGetTriclopsToWorldTransform()
//
TriclopsError
triclopsFixedWorldXYZToRC( TriclopsContext   context,
			    int		     worldx_X_2to16,   
			    int		     worldy_X_2to16,   
			    int		     worldz_X_2to16,
			    int*	     row_X_2to8,      
			    int*	     col_X_2to8 );


//
// Name: triclopsFixedArrayRCDToWorldXYZ
//
// Synopsis:
//  Same as triclopsFixedRCDToWorldXYZ, except the operations will be performed
// on an array of input pixels rather than just a single one. The interface is
// designed specifically for Sensys and is slightly different from that of 
// triclopsFixedRCDToWorldXYZ.
//
// Input:
//  context - The stereo context.
//  numArrayElements - the number of pixels in the array to process
//  rowArray     - The row coordinates of the input array of pixels.
//  colArray     - The column coordinates of the input array of pixels.
//  disp_X_2to8Array - The disparity value (multiplied by 2^8) of the 
//			input array of pixels.
//
// Output:
//  worldx_X_2to10Array - location to return the x coordinates (multiplied by 2^10) 
//		     of the array of corresponding 3d points in world coordinates	
//  worldy_X_2to10Array - location to return the y coordinates (multiplied by 2^10) 
//		     of the array of corresponding 3d points in world coordinates	
//  worldz_X_2to10Array - location to return the z coordinates (multiplied by 2^10) 
//		     of the array of corresponding 3d points in world coordinates	
//
// Returns:
//  TriclopsErrorOk             - The operation succeeded.
//  InvalidContext - The input context was invalid.	
//
// Description:
//  This function takes the 16-bit disparity values for an array of image pixels 
//  and converts them to XYZ coordinates in world coordinates.  
//  The world coordinates are determined by transforming the point from the Triclops 
//  coordinate system to the world coordinate system based on the TriclopsContext transform.
//  All operations are performed using only fixed point arithmatics.
//
// Note: It is up to the user to supply valid disparity values 
//
// See Also:
//  triclopsFixedRCDToWorldXYZ() 
//
TriclopsError
triclopsFixedArrayRCDToWorldXYZ( TriclopsContext   context,
				 unsigned short	   numArrayElements,
				 unsigned short*   rowArray,  
				 unsigned short*   colArray,  
				 unsigned short*   disp_X_2to8Array,
				 unsigned short*   worldx_X_2to10Array,  
				 unsigned short*   worldy_X_2to10Array,  
				 unsigned short*   worldz_X_2to10Array );


//
// Name: triclopsFixedArrayWorldXYZToRC
//
// Synopsis:
//  Same as triclopsFixedWorldXYZToRC, except the operations will be performed
// on an array of input 3d points rather than just a single one. The interface is
// designed specifically for Sensys and is slightly different from that of 
// triclopsFixedWorldXYZToRC.
//
// Input:
//  context - TriclopsContext set up for desired resolution.
//  numArrayElements - the number of 3d points in the array to process
//  worldx_X_2to10Array - X coordinates (multiplied by 2^10) of the input 3d points 
//			   in the World coordinate system.
//  worldy_X_2to10Array - Y coordinates (multiplied by 2^10) of the input 3d points 
//			   in the World coordinate system.
//  worldz_X_2to10Array - Z coordinates (multiplied by 2^10) of the input 3d points 
//			   in the World coordinate system.
//
// Output:
//  row_X_2to8Array  - The row coordinates (multiplied by 2^8) of the input 3d points.
//  col_X_2to8Array  - The column coordinates (multiplied by 2^8) of the input 3d points.
//
// Returns:
//  TriclopsErrorOk                          - The Operation succeeded.
//  TriclopsErrorInvalidContext - Context is not valid TriclopsContext.
//  TriclopsErrorInvalidRequest - An impossible XYZ value has been provided
//                                (ie: negative Z).
//
// Description:
//  This function takes as input the XYZ position of an array of 3d points
//  in the World coordinate system, moves the point to the Triclops coordinate 
//  system (as described by the TriclopsContext transform), and determines what 
//  row, column, and disparity value would result from each resulting point.
//  All operations are performed using only fixed point arithmatics.
//
// See Also:
//  triclopsFixedWorldXYZToRC() 
//
TriclopsError
triclopsFixedArrayWorldXYZToRC( TriclopsContext context,
			        unsigned short  numArrayElements,
				unsigned short* worldx_X_2to10Array,   
				unsigned short* worldy_X_2to10Array,   
				unsigned short* worldz_X_2to10Array,
				int*            row_X_2to10Array,      
				int*            col_X_2to10Array );

//
// Name: triclopsFixedArrayRCDToWorldXYZPacked
//
// Synopsis:
//  Same as triclopsFixedRCDToWorldXYZ, except the operations will be performed
// on an array of input pixels rather than just a single one. The input and 
// output arrays are arranged so that different elements for a pixel are packed
// into consecutive entries in the array (hence named packed).
//
// Input:
//  context - The stereo context.
//  numArrayElements - the number of pixels in the array to process
//  pRCD     - the input RCD array, each pixel will take up 3 entries for its RCD value
//             R and C are non-scaled, where as D is scaled by 2^8 
//             (which is simply the 16bits disparity value as is)
//  pTemp - an array that must be at least as big as the output pXYZ_X_2to16 array, this
//             provide temporary scratch space needed internally to derive the final values.
//
// Output:
//  pXYZ_X_2to16     - The output XYZ array, each pixel will take up 3 entries for 
//             its XYZ value, each is scaled by 2^16.
//
// Returns:
//  TriclopsErrorOk             - The operation succeeded.
//  InvalidContext - The input context was invalid.	
//
// Description:
//  This function takes the 16-bit disparity values for an array of image pixels 
//  and converts them to XYZ coordinates in world coordinates.  
//  The world coordinates are determined by transforming the point from the Triclops 
//  coordinate system to the world coordinate system based on the TriclopsContext transform.
//  All operations are performed using only fixed point arithmatics.
//
// Note: It is up to the user to supply valid disparity values 
//
// See Also:
//  triclopsFixedRCDToWorldXYZ() 
//
// Notes :
// Input : RC - no scaling 2^0
//         D  - scaled by 2^8
// Output : X,Y,Z - scaled by 2^16
extern TriclopsError
triclopsFixedArrayRCDToWorldXYZPacked( TriclopsContext      context,
				 unsigned short       numArrayElements,
                                 unsigned short *     pRCD,
                                 int*                 pXYZ_X_2to16,
                                 int*                 pTemp );

#ifdef __cplusplus
}
#endif

#endif  // #ifndef TRICLOPSINT_H
