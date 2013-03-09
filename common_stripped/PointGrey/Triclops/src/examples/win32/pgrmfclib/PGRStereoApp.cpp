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
// $Id: PGRStereoApp.cpp,v 1.1 2007/03/27 21:36:22 demos Exp $
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
#include "PGRStereoApp.h"
#include "PGRStereoDoc.h"


BEGIN_MESSAGE_MAP( CPGRStereoApp, CWinApp )
//{{AFX_MSG_MAP(CPGRStereoApp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CPGRStereoApp::CPGRStereoApp() 
{
}


CPGRStereoApp::~CPGRStereoApp()
{
}


BOOL 
CPGRStereoApp::OnIdle( LONG lCount ) 
{
   //
   // Here we make our attempt at semi-synchronized grabs.  The first
   // call to flycaptureGrab() grabs the frame.  The call to processFrame() 
   // extracts the triclops inputs and does all the rest of the stereo.
   // The theory is this:  the grab call is really thin, so if we do all
   // the grabs from the n documents, they will be "close enough" to each
   // other.  Each document encapsulates a flycapture/triclops lib pair.
   //
   BOOL	bRet;

   POSITION posTemplate = GetFirstDocTemplatePosition();
   
   while ( posTemplate != NULL )
   {
      CDocTemplate* pTemplate = (CDocTemplate*)GetNextDocTemplate( posTemplate );

      POSITION posDoc = pTemplate->GetFirstDocPosition();
      while ( posDoc != NULL )
      {
         CDocument* pDocument;
         if ( (pDocument = pTemplate->GetNextDoc( posDoc )) != NULL)
	 {
	    if ( pDocument->IsKindOf( RUNTIME_CLASS( CPGRStereoDoc )) )
	    {  
	       bRet = ((CPGRStereoDoc*)pDocument)->flycaptureGrab();

	       if ( !bRet )
	       {
		  TRACE( "CPGRStereoApp::OnIdle() - call to flycaptureGrab() failed.\n" );
	       }
	    }
	 }
      }

      posDoc = pTemplate->GetFirstDocPosition();
      while ( posDoc != NULL )
      {
         CDocument* pDocument;
         if ( (pDocument = pTemplate->GetNextDoc( posDoc )) != NULL)
	 {
	    if ( pDocument->IsKindOf( RUNTIME_CLASS( CPGRStereoDoc )) )
	    {  
	       bRet = ((CPGRStereoDoc*)pDocument)->processFrame();

	       if ( !bRet )
	       {
		  TRACE( "CPGRStereoApp::OnIdle() - call to processFrame() failed.\n" );
	       }
	    }
	 }
      }
   }

   // be sure to call the default OnIdle
   CWinApp::OnIdle( lCount );
   
   // force OnIdle to be called again ASAP
   return TRUE;  
}


BOOL 
CPGRStereoApp::isCameraAlreadyOpen( FlyCaptureCameraSerialNumber serial )
{
   POSITION posTemplate = GetFirstDocTemplatePosition();
   
   while( posTemplate != NULL )
   {
      CDocTemplate* pTemplate = (CDocTemplate*)GetNextDocTemplate( posTemplate );
      
      POSITION posDoc = pTemplate->GetFirstDocPosition();
      while( posDoc != NULL )
      {
         CDocument* pDocument;
         if ( ( pDocument = pTemplate->GetNextDoc( posDoc ) ) != NULL )
         {
            if( pDocument->IsKindOf( RUNTIME_CLASS( CPGRStereoDoc ) ) )
            {  
               if( ((CPGRStereoDoc*)pDocument)->isCameraAlreadyOpen( serial ) )
               {
                  return TRUE;
               }
            }
         }
      }
   }

   return FALSE;
}

