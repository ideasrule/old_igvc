//=============================================================================
// Copyright © 2005 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with Point Grey Research Inc.
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//
// Digiclops® is a registered trademark of Point Grey Research Inc.
//=============================================================================
//=============================================================================
// $Id: DemoDoc.h,v 1.1 2007/03/27 21:33:00 demos Exp $
//=============================================================================
#if !defined(AFX_DemoDOC_H__E325D9A1_222A_4A15_A77A_0A2134184A15__INCLUDED_)
#define AFX_DemoDOC_H__E325D9A1_222A_4A15_A77A_0A2134184A15__INCLUDED_

#pragma once

//=============================================================================
// System Includes
//=============================================================================

//=============================================================================
// PGR Includes
//=============================================================================
#include "PGRStereoDoc.h"

//=============================================================================
// Project Includes
//=============================================================================


/**
 * Document object for DigiclopsDemo.  Inherits from CPGRStereoDoc for
 * the stereo stuff.
 */
class CDemoDoc : public CPGRStereoDoc
{
protected: 

   CDemoDoc();
   DECLARE_DYNCREATE( CDemoDoc )
      
   
public:
   virtual ~CDemoDoc();

   /**
    * Make sure that points are being computed (and that this is reflected 
    * in the stereo control dialog.
    */
   void turnOnComputePoints();   

protected:

   /**
    * Override of PGRMFC function in order to call 
    * doModelProcessing() at the right time.
    */
   virtual void	customProcess();
   virtual BOOL OnNewDocument();


protected:
   
   //{{AFX_MSG(CDemoDoc)
   afx_msg void OnFileClose();
   afx_msg void OnFileLoadCal();
   afx_msg void OnFileLoadStereoImage();
   afx_msg void OnFileSaveCurrCal();
   afx_msg void OnFileSaveDefaultCal();
   afx_msg void OnFileSaveDisparity();
   afx_msg void OnFileSaveEdgeLeft();
   afx_msg void OnFileSaveEdgeRight();
   afx_msg void OnFileSaveEdgeCenter();
   afx_msg void OnFileSavePointcloud();
   afx_msg void OnFileSaveRawLeft();
   afx_msg void OnFileSaveRawStereo();
   afx_msg void OnFileSaveRawRight();
   afx_msg void OnFileSaveRawCenter();
   afx_msg void OnFileSaveRectifiedColorLeft();
   afx_msg void OnFileSaveRectifiedColorRight();
   afx_msg void OnFileSaveRectifiedColorCenter();
   afx_msg void OnFileSaveRectifiedLeft();
   afx_msg void OnFileSaveRectifiedRight();
   afx_msg void OnFileSaveRectifiedCenter();
   afx_msg void OnFileSaveStereo();
   afx_msg void OnUpdateFileLoadStereoImage(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFileSavePointcloud(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFileSaveRawRight(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFileSaveRawLeft(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFileSaveRawCenter(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFileSaveRectifiedCenter(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFileSaveRectifiedColorCenter(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFileSaveEdgeCenter(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFileSaveStereo(CCmdUI* pCmdUI);
   afx_msg void OnAppVersionInfo();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   BOOL OnFileNewOfflineDocument();
   BOOL OnFileNewCameraDocument();

};


//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DemoDOC_H__E325D9A1_222A_4A15_A77A_0A2134184A15__INCLUDED_)
