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
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: FrameRate.cpp,v 1.1 2004/04/21 22:28:17 mwhite Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <cassert>

//=============================================================================
// ProjectIncludes
//=============================================================================
#include "FrameRate.h"



FrameRate::FrameRate( double dHistory )
{
   assert( dHistory >= 0.0 && dHistory <= 1.0 );

   m_dHistory     = dHistory;
   m_dFrameRate   = 0.0;

   ::QueryPerformanceFrequency( (LARGE_INTEGER*)&m_nFrequency );
   ::QueryPerformanceCounter( (LARGE_INTEGER*)&m_nLastTime );
}

FrameRate::~FrameRate()
{
}


double 
FrameRate::getFrameRate() const
{
   return m_dFrameRate;
}


void 
FrameRate::setFrameRate( double dFrameRate )
{
   m_dFrameRate = dFrameRate;
}


void 
FrameRate::newFrame()
{
   __int64 timeCurr;

   ::QueryPerformanceCounter( (LARGE_INTEGER*)&timeCurr );
   
   __int64 i64Diff = timeCurr - m_nLastTime;
   if( i64Diff != 0 )
   {
      double dMicroseconds = 
         (double)i64Diff / ((double)m_nFrequency / 1000000.0 );
      
      m_dFrameRate = m_dHistory * m_dFrameRate + 
         ( 1.0 - m_dHistory ) * ( 1000000.0 / dMicroseconds );
      
      m_nLastTime = timeCurr;
   }
}
