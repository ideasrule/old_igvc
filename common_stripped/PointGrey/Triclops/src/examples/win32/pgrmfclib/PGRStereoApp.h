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
// $Id: PGRStereoApp.h,v 1.1 2007/03/27 21:36:23 demos Exp $
//=============================================================================
#ifndef __PGRSTEREOAPP_H__
#define __PGRSTEREOAPP_H__

#pragma once

//=============================================================================
// System Includes
//=============================================================================
#include <afxwin.h>

//=============================================================================
// PGR Includes
//=============================================================================
#include <PGRFlyCaptureStereo.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "PointList.h"


typedef CList< CPointList, CPointList > PointListList;


/**
 * This is the PGRMFC version of the MFC CWinApp.  It inherits from CWinApp
 * and only overrides the OnIdle() method, where it loops through the 
 * documents and triggers grabbing, stereo processing, and displaying each
 * frame.
 */
class CPGRStereoApp : public CWinApp  
{
public:

   CPGRStereoApp();
   virtual ~CPGRStereoApp();
   
   //{{AFX_VIRTUAL(CPGRStereoApp)
public:
   virtual BOOL OnIdle(LONG lCount);
   //}}AFX_VIRTUAL
   
   //{{AFX_MSG(CPGRStereoApp)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()


public:

   /** @return TRUE if the indicated serial number is already in use by
    *  another document. */
   BOOL isCameraAlreadyOpen( FlyCaptureCameraSerialNumber serial );

   
protected:
   

};

#endif // #ifdef __PGRSTEREOAPP_H__
