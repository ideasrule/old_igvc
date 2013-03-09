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
// $Id: PointList.cpp,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "PointList.h"



CPointList::CPointList( int growBy )
{
   SetSize( 0, growBy );
}

CPointList::CPointList( const CPointList& pointlist )
{
   m_centerOfMass = pointlist.m_centerOfMass;
}


CPointList::~CPointList()
{   
}


CPointList& 
CPointList::operator=( const CPointList& pointlist )
{
   m_centerOfMass = pointlist.m_centerOfMass;
   return *this;
}


void
CPointList::getCenterOfMass( C3dColourPointRC* ppoint )
{
   double sumX = 0;
   double sumY = 0;
   double sumZ = 0;
   
   for ( int i = 0; i < GetSize(); i++ )
   {
      const C3dColourPointRC* ppoint = ElementAt( i );

      sumX += ppoint->x();
      sumY += ppoint->y();
      sumZ += ppoint->z();
   }

   sumX	/= (double)size();
   sumY	/= (double)size();
   sumZ	/= (double)size();
   
   m_centerOfMass.setXYZ( sumX, sumY, sumZ );
   
   ppoint->setXYZ( sumX, sumY, sumZ );
}


int 
CPointList::size() const
{
   return GetSize();
}


void 
CPointList::empty()
{
   RemoveAll();
}


C3dColourPointRC* 
CPointList::elementAt( int iIndex ) const
{
   return GetAt( iIndex );
}


int 
CPointList::insertPoint( C3dColourPointRC* ppoint )
{
   ASSERT( ppoint != NULL );
   return Add( ppoint );
}


bool
CPointList::save( const char* pszFileName )
{
   // save the current point list to the named file
   // - each line contains a single point
   // - each point includes 3d coordinates, rgb color mapping and
   //   the row and column in the reference image where the point originates.
   FILE* pPointFile;
   
   if ( ( pPointFile = fopen( pszFileName, "w" ) ) == NULL )
   {
      return false;
   }
   
   for ( int i = 0; i < GetSize(); i++ )
   {
      const C3dColourPointRC* pPoint = ElementAt( i );
      const RGBQUAD*  pPointColour = pPoint->colour();
	    
      fprintf( 
	 pPointFile,
	 "%lf %lf %lf %d %d %d %lf %lf\n",
	 pPoint->x(),
	 pPoint->y(),
	 pPoint->z(),
	 (int)pPointColour->rgbRed,
	 (int)pPointColour->rgbGreen,
	 (int)pPointColour->rgbBlue,
	 (double) pPoint->row(),
	 (double) pPoint->col() );
     
   }

   fclose( pPointFile );
   
   return true;

}
