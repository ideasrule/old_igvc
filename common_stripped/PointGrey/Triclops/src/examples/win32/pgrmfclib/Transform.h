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
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: Transform.h,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================
#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#pragma once

//=============================================================================
// System Includes
//=============================================================================
#include <cassert>
#include <cmath>
#include <cstdio>

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================

#define _PI_2  1.57079632679489661923  /* pi/2 */


/**
 * This class implements a 4x4 transformation matrix.  It is a reimplementation
 * of the PGRTransform3d class.
 */
template< class COORD_TYPE > 
class CTransform
{

public:

   /** Default constructor. */
   CTransform();

   /** Set the matrix values from the current transformation parameters */
   void setMatrix();

   /** Set the matrix values as per matrix passed in */
   void setMatrix( COORD_TYPE matrix[4][4] );

   /** Set the transformation parameters from the current matrix values */
   void unsetMatrix();

   /** Return a pointer to the internal data. */
   COORD_TYPE* getData();

   COORD_TYPE rX() const;
   COORD_TYPE rY() const;
   COORD_TYPE rZ() const;

   COORD_TYPE tX() const;
   COORD_TYPE tY() const;
   COORD_TYPE tZ() const;

   /** Apply the transform to the given point. */
   void apply( COORD_TYPE& x, COORD_TYPE& y, COORD_TYPE& z );

   /**
    * Rotations are set following the angle set convention referred to by
    *  Craig (p.443) as Euler Rz'y'x', whereby the rotations are applied in the
    *  order rz-ry-rx about the moving frame (see above).  Note that because
    *  Euler Rz'y'x' convention is equivalent to the Fixed Rxyz fixed-angle
    *  convention, calling this method is equivalent to doing the following:
    *    setRotX( rx );
    *    setRotY( ry );
    *    setRotZ( rz );
    *    setTransX( tx );
    *    setTransY( ty );
    *    setTransZ( tz );
    *    setMatrix();
    * 
    *  Note that the angles passed in here are in x-y-z order, NOT in z-y-x
    *  order as in Craig.
    */
   void setEulerZYX( 
      COORD_TYPE rx,
      COORD_TYPE ry,
      COORD_TYPE rz,
      COORD_TYPE tx,
      COORD_TYPE ty,
      COORD_TYPE tz );

   /**
    *  Use this with caution since there is no corresponding getEulerXYZ() method.
    *  This class uses the Rz'y'x' (or equivalently Rxyz) convention internally,
    *  so if this method is used to set the transformation, it should be understood
    *  that subsequent retrieval use of rotation angles (e.g., using rX(), ...)
    *  will return rotations according to a the convention used internally; i.e.,
    *  angles will not follow EulerXYZ convention.
    *  
    *  This method interprets the rotations according to the angle set convention
    *  referred to by Craig (p.442) as Euler Rx'y'z', whereby the rotations are
    *  applied in the order rx about x, ry about the rotated y, and rz about the
    *  rotated z.  Note that this is equivalent to the fixed-angle Fixed Rzyx
    *  convention.
    */
   void setEulerXYZ( 
      COORD_TYPE rx, 
      COORD_TYPE ry, 
      COORD_TYPE rz,			     
      COORD_TYPE tx, 
      COORD_TYPE ty, 
      COORD_TYPE tz );

   /** 
    * Returns a pointer to a 2d array of floats in the format that OpenGL likes. 
    */
   float* glMatrixf();


protected:

   bool m_bMatrixSet;
   
   COORD_TYPE  m_matrix[ 4 ][ 4 ];

   COORD_TYPE  m_rotX;
   COORD_TYPE  m_rotY;
   COORD_TYPE  m_rotZ;

   COORD_TYPE  m_transX;
   COORD_TYPE  m_transY;
   COORD_TYPE  m_transZ;

   float       m_glMatrix[ 4 ][ 4 ];

};


//
// Define the one and only instantation of this template that we use.
//
typedef CTransform< double > CTransformD;




#define _TEMPLATE template< class COORD_TYPE >
#define _CLASS CTransform< COORD_TYPE >

_TEMPLATE
_CLASS::CTransform()
{
   m_rotX = 0;
   m_rotY = 0;
   m_rotZ = 0;
   m_transX = 0;
   m_transY = 0;
   m_transZ = 0;

   m_bMatrixSet = false;
}


_TEMPLATE
void 
_CLASS::unsetMatrix()
{   
   // based on "Introduction to Robotics" - John Craig
   // page 47
   
   // recall that set matrix does this:
   //matrix[0][0] = cz*cy;
   //matrix[0][1] = cz*sy*sx - sz*cx;
   //matrix[1][0] =  sz*cy;
   //matrix[1][1] =  sz*sy*sx + cz*cx;
   //matrix[2][0] = -sy;
   //matrix[2][1] = cy*sx;
   //matrix[2][2] = cy*cx;
   
   // expensive, 'more stable' method for determining rotY
   m_rotY = atan2( 
      -m_matrix[2][0], 
      sqrt( m_matrix[0][0]*m_matrix[0][0] + m_matrix[1][0]*m_matrix[1][0] ) );
   
   // special case in which rotX & rotZ become unsolvable
   if( m_rotY == _PI_2 || m_rotY == -_PI_2 )
   {
      m_rotX = atan2( m_matrix[0][1], m_matrix[1][1] );
      m_rotZ = 0.0;
   }
   else
   {
      if( m_rotY > -_PI_2 && m_rotY < _PI_2 )
      {
         m_rotX	= atan2( m_matrix[2][1], m_matrix[2][2] );
         m_rotZ	= atan2( m_matrix[1][0], m_matrix[0][0] );
      }
      else
      {
         // account for the division by cos(rotY) in Craig's equation
         //    - which just change which quardrant the tangent result is
         //      to be reported in
         m_rotX	= atan2( -m_matrix[2][1], -m_matrix[2][2] );
         m_rotZ	= atan2( -m_matrix[1][0], -m_matrix[0][0] );
      }
   }
   
   m_transX = m_matrix[0][3];
   m_transY = m_matrix[1][3];
   m_transZ = m_matrix[2][3];
}


_TEMPLATE
void
_CLASS::setMatrix()
{
   if( m_bMatrixSet )
   {
      return;
   }

   m_bMatrixSet = true;

   COORD_TYPE cosX = cos( m_rotX );		
   COORD_TYPE sinX = sin( m_rotX );
   COORD_TYPE cosY = cos( m_rotY );		
   COORD_TYPE sinY = sin( m_rotY );
   COORD_TYPE cosZ = cos( m_rotZ );		
   COORD_TYPE sinZ = sin( m_rotZ );

   // translation portion of transform
   m_matrix[0][3] = m_transX;
   m_matrix[1][3] = m_transY;
   m_matrix[2][3] = m_transZ;
	
   // rotation portion of transform
   // from Craig, page 443, Rxyz()

   // cz*cy;
   m_matrix[0][0] = cosZ * cosY; 
   // cz*sy*sx - sz*cx;
   m_matrix[0][1] = cosZ * sinY * sinX - sinZ * cosX; 
   // cz*sy*cx + sz*sx;
   m_matrix[0][2] = cosZ * sinY * cosX + sinZ * sinX; 

   // sz*cy;
   m_matrix[1][0] = sinZ * cosY; 
   // sz*sy*sx + cz*cx;
   m_matrix[1][1] = sinZ * sinY * sinX + cosZ * cosX; 
   // sz*sy*cx - cz*sx;
   m_matrix[1][2] = sinZ * sinY * cosX - cosZ * sinX; 

   //-sy;
   m_matrix[2][0] = -sinY; 
   //cy*sx;
   m_matrix[2][1] = cosY * sinX; 
   //cy*cx;
   m_matrix[2][2] = cosY * cosX; 

   // bottom row, always the same
   m_matrix[3][0] = 0.0;
   m_matrix[3][1] = 0.0;
   m_matrix[3][2] = 0.0;
   m_matrix[3][3] = 1.0;
}


_TEMPLATE
void
_CLASS::setMatrix( COORD_TYPE matrix[4][4] )
{
   m_bMatrixSet = true;

   for( int i = 0; i < 4; i++ )
   {
      for( int j = 0; j < 4; j++ )
      {
         m_matrix[i][j] = matrix[i][j];
      }
   }

   unsetMatrix();
}


_TEMPLATE
COORD_TYPE*
_CLASS::getData()
{
   setMatrix();
   return &( m_matrix[ 0 ][ 0 ] );
}


_TEMPLATE
COORD_TYPE
_CLASS::rX() const
{
   return m_rotX;
}


_TEMPLATE
COORD_TYPE
_CLASS::rY() const
{
   return m_rotY;
}


_TEMPLATE
COORD_TYPE
_CLASS::rZ() const
{
   return m_rotZ;
}

_TEMPLATE
COORD_TYPE
_CLASS::tX() const
{
   return m_transX;
}


_TEMPLATE
COORD_TYPE
_CLASS::tY() const
{
   return m_transY;
}


_TEMPLATE
COORD_TYPE
_CLASS::tZ() const
{
   return m_transZ;
}


_TEMPLATE
void
_CLASS::setEulerZYX( 
                    COORD_TYPE rx,
                    COORD_TYPE ry,
                    COORD_TYPE rz,
                    COORD_TYPE tx,
                    COORD_TYPE ty,
                    COORD_TYPE tz )
{
   m_bMatrixSet = false;

   m_rotX = rx;
   m_rotY = ry;
   m_rotZ = rz;
   m_transX = tx;
   m_transY = ty;
   m_transZ = tz;

   setMatrix();
}


_TEMPLATE
void
_CLASS::setEulerXYZ( 
                    COORD_TYPE rx,
                    COORD_TYPE ry,
                    COORD_TYPE rz,
                    COORD_TYPE tx,
                    COORD_TYPE ty,
                    COORD_TYPE tz )
{
   m_bMatrixSet	= true;

   double cosX = cos( rx );		
   double sinX = sin( rx );
   double cosY = cos( ry );		
   double sinY = sin( ry );
   double cosZ = cos( rz );		
   double sinZ = sin( rz );

   // translation portion of transform
   m_matrix[0][3] = tx;
   m_matrix[1][3] = ty;
   m_matrix[2][3] = tz;
	
   // rotation portion of transform
   // from Craig, page 442, Euler Rx'y'z'()

   // cy*cz;
   m_matrix[0][0] = cosY * cosZ; 
   // -cy*sz
   m_matrix[0][1] = -cosY * sinZ;
   // sy
   m_matrix[0][2] = sinY;

   // sx*sy*cz + cx*sz
   m_matrix[1][0] = sinX*sinY*cosZ + cosX*sinZ;
   // -sx*sy*sz + cx*cz
   m_matrix[1][1] = -sinX*sinY*sinZ + cosX*cosZ;
   // -sx*cy
   m_matrix[1][2] = -sinX*cosY;

   //-cx*sy*cz + sx*sz
   m_matrix[2][0] = -cosX*sinY*cosZ + sinX*sinZ;
   //cx*sy*sz + sx*cz
   m_matrix[2][1] = cosX*sinY*sinZ + sinX*cosZ;
   //cx*cy
   m_matrix[2][2] = cosX * cosY; 

   // bottom row, always the same
   m_matrix[3][0] = 0.0;
   m_matrix[3][1] = 0.0;
   m_matrix[3][2] = 0.0;
   m_matrix[3][3] = 1.0;
   
   unsetMatrix();
}


_TEMPLATE
float*
_CLASS::glMatrixf()
{
   setMatrix();

   for ( int r = 0; r < 4; r++ )
   {
      for ( int c = 0; c < 4; c++ )
      {
         m_glMatrix[c][r] = (float)m_matrix[r][c];
      }
   }

   return &(m_glMatrix[0][0]);
}


_TEMPLATE
void 
_CLASS::apply( COORD_TYPE& x, COORD_TYPE& y, COORD_TYPE& z )
{
   setMatrix();
   COORD_TYPE outPoint[3];
   
   for( int r = 0; r < 3; r++ )
   {
      outPoint[r] = 
         m_matrix[r][3] 
            + m_matrix[r][0] * x
            + m_matrix[r][1] * y
            + m_matrix[r][2] * z;
   }
   
   x = outPoint[0];
   y = outPoint[1];
   z = outPoint[2];
}



#endif // #ifndef __TRANSFORM_H__
