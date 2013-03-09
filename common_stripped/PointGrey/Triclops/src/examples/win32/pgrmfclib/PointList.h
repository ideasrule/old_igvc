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
// $Id: PointList.h,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================
#ifndef __POINTLIST_H__
#define __POINTLIST_H__

#pragma once

//=============================================================================
// System Includes
//=============================================================================
#include <afxtempl.h>

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "3dPoint.h"


//
// Number of points to grow the array by at a time.
//
#define _GROWBY 1000



/**
 * A reimplementation of the internal PGRPointcloud3d class.  This class 
 * inherits from the MFC CArray and contains a list of C3dColourPointRC 
 * pointers which have been allocated somewhere else.  It provides useful 
 * functions like center of mass.
 */
class CPointList : public CArray< C3dColourPointRC*, C3dColourPointRC* >
{
public:

   /**
    * Default constructor.
    *
    * @param growBy The number of elements to grow by if we need more space.
    */
   CPointList( int growBy = _GROWBY );

   /** Default copy constructor. */
   CPointList( const CPointList& pointlist );

   /** Default destructor. */
   virtual ~CPointList();

   CPointList& operator=( const CPointList& pointlist );

   /** Calculates and returns the center of mass of all the points. */
   void getCenterOfMass( C3dColourPointRC* ppoint );

   /** Current one-based size of the point list. */
   int size() const;

   /** Remove all elements of the point list. */
   void empty();

   /** Return the element at the given zero-based index. */
   C3dColourPointRC* elementAt( int iIndex ) const;

   /** Insert the passed pointer into the list. */
   int insertPoint( C3dColourPointRC* ppoint );

   /** save the point cloud to the specified file */
   bool save( const char* pszFileName );

protected:

   /** Storage for the center of mass.  Not used. */
   C3dColourPointRC m_centerOfMass;
};


#endif // #ifndef __POINTLIST_H__
