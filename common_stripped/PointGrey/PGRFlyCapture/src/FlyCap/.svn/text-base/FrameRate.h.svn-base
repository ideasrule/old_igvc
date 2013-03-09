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
// $Id: FrameRate.h,v 1.1 2004/04/21 22:28:17 mwhite Exp $
//=============================================================================
#ifndef __FRAMERATE_H__
#define __FRAMERATE_H__

//=============================================================================
// System Includes
//=============================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>




/**
 * Calculate and store camera frame rates.  Uses the high-resolution 
 * performance counter.  Will not work on non-NT operating systems.
 *
 * @bug Implement pause.
 */
class FrameRate  
{
public:

   /** 
    * Default constructor. 
    *
    * @param dHistory The percentage of previous frame rate incorporated with
    *                 each new calculated frame rate.
    */
   FrameRate( double dHistory = 0.50 );

   /** Default destructor */
   virtual ~FrameRate();

   /** Returns the current frame rate. */
   double getFrameRate() const;

   /** Sets the current frame rate. */
   void setFrameRate( double dFrameRate );

   /** Call when there is a new frame event. */
   void newFrame();


protected:
   
   double m_dHistory;
   double m_dFrameRate;

   __int64  m_nFrequency;
   __int64  m_nLastTime;
   
};



#endif // #ifndef __FRAMERATE_H__
