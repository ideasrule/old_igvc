//=============================================================================
// Copyright © 2005 Point Grey Research, Inc. All Rights Reserved.
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
// $Id: 3dPoint.h,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================
#ifndef __3DPOINT_H__
#define __3DPOINT_H__

#pragma once

//=============================================================================
// System Includes
//=============================================================================
#include <windows.h>  // for RGBQUAD
#include <cassert>

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================



/**
 * This class represents a 3d point in space.  It also keeps track of the 
 * original row and column this point was generated from, so it's useful for
 * Triclops applications.
 */
template<class COORD_TYPE, class RC_TYPE, class COLOUR_TYPE> 
class C3dPoint
{

public:

   C3dPoint()
   {
      m_x      = 0;
      m_y      = 0;
      m_z      = 0;
      m_col    = 0;
      m_row    = 0;      
      ::memset( &m_colour, 0x0, sizeof( COLOUR_TYPE ) );
   };
   
   C3dPoint(
      COORD_TYPE     x,
      COORD_TYPE     y,
      COORD_TYPE     z,
      RC_TYPE        column,
      RC_TYPE        row,
      COLOUR_TYPE    colour )
   {
      m_x      = x;
      m_y      = y;
      m_z      = z;
      m_col    = col;
      m_row    = row;
      m_colour = colour;   
   };


   C3dPoint( COORD_TYPE x, COORD_TYPE y, COORD_TYPE z )
   {
      m_x      = x;
      m_y      = y;
      m_z      = z;
      m_col    = -1;
      m_row    = -1;      
      ::memset( &m_colour, 0x0, sizeof( COLOUR_TYPE ) );
   };


   C3dPoint( const C3dPoint& point )
   {
      m_x      = point.x;
      m_y      = point.y;
      m_z      = point.z;
      m_col    = point.col;
      m_row    = point.row;
      m_colour = point.colour;   
   };


   virtual ~C3dPoint()
   {
   };


   COORD_TYPE x() const 
   { 
      return m_x; 
   };


   COORD_TYPE y() const 
   { 
      return m_y; 
   };


   COORD_TYPE z() const 
   { 
      return m_z; 
   };


   void getXYZ( COORD_TYPE* px, COORD_TYPE* py, COORD_TYPE* pz ) const
   {
      assert( px != NULL && py != NULL && pz != NULL );
      *px = m_x;
      *py = m_y;
      *pz = m_z;
   };


   void setXYZ( COORD_TYPE x, COORD_TYPE y, COORD_TYPE z )
   {
      m_x = x;
      m_y = y;
      m_z = z;      
   };


   RC_TYPE col() const
   {
      return m_col;
   };


   RC_TYPE row() const
   {
      return m_row;
   };


   void getColRow( RC_TYPE* pcol, RC_TYPE* prow ) const
   {
      assert( pcol != NULL && prow != NULL );
      *pcol = m_col;
      *prow = m_row;
   };


   void setColRow( RC_TYPE col, RC_TYPE row )
   {
      m_col = col;
      m_row = row;
   };


   const COLOUR_TYPE* colour() const
   {
      return &m_colour;
   };


   COLOUR_TYPE* colour()
   {
      return &m_colour;
   };


   void setColour( COLOUR_TYPE* pcolour )
   {
      m_colour = *pcolour;
   };
   

protected:

   COORD_TYPE  m_x;
   COORD_TYPE  m_y;
   COORD_TYPE  m_z;

   RC_TYPE     m_col;
   RC_TYPE     m_row;

   COLOUR_TYPE m_colour;

};


/**
 * C3dColourPointRC uses doubles and an RGBQUAD for colour.
 */
typedef C3dPoint< double, int, RGBQUAD > C3dColourPointRC;

/**
 * C3dColourPointRCF uses floats.
 */
typedef C3dPoint< float, int, RGBQUAD > C3dColourPointRCF;


#endif // #ifndef __3DPOINT_H__
