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
// $Id: timeSliceDoc.cpp,v 1.49 2007/11/16 18:58:21 soowei Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include "stdafx.h"
#include <process.h>
#include <sys/timeb.h>
#include <time.h>

//
// MS's STL implementation does not compile cleanly under W4.
//
#pragma warning( push, 2 )
#include <list>
#pragma warning( pop )
#pragma warning( disable:4146 )

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "timeSliceDemo.h"
#include "timeSliceDoc.h"
#include "timesliceview.h"
#include "dialogconfig.h"
#include "pgravifile.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define _CHECK_ERROR( function, error ) \
{ \
   if ( error != FLYCAPTURE_OK ) \
   { \
      CString  csMessage; \
      csMessage.Format( \
	 "%s reported \"%s\"", \
	 function, \
	 ::flycaptureErrorToString( error ) ); \
      ::AfxMessageBox( csMessage ); \
      \
      return FALSE; \
   } \
} \



IMPLEMENT_DYNCREATE( CTimeSliceDoc, CDocument )

BEGIN_MESSAGE_MAP( CTimeSliceDoc, CDocument )
//{{AFX_MSG_MAP(CTimeSliceDoc)
ON_COMMAND(ID_CONFIG, OnConfig)
ON_COMMAND(ID_EDIT_SAVE_ANIMATION, OnEditSaveAnimation)
ON_COMMAND(ID_BUTTON_SCROLL_MODE, OnButtonScrollMode)
ON_UPDATE_COMMAND_UI(ID_BUTTON_SCROLL_MODE, OnUpdateButtonScrollMode)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CTimeSliceDoc::CTimeSliceDoc()
{
   m_uiNumCameras	      = 0;
   m_iTotalExtraImageBuffers  = 0;

   m_iFramesPerCameraPanning  = 0;
   m_iFramesForEndCamera      = 0;
   m_iFramesAtEnd	      = 0;
   m_iNumberOfPanCycles	      = 0;
   m_iExtraBuffers	      = 0;

   m_uiMissedImages           = 0;
   m_uiOutOfSyncImages        = 0;

   m_dFrameRate		      = 0.0;

   m_eventGrabFinished	      = NULL;
   m_bContinueGrab	      = false;
   m_bSliceEvent	      = false;
   m_stateSlice               = RUNNING;
   m_bDrawAlignCross	      = false;

   m_iInitialBufferedImages   = 0;   
   m_iInitialBufferIndex      = 0;

   m_arpImageBuffers          = NULL;
   m_arpInitialBuffers[ 0 ]   = NULL;
   m_arpInitialBuffers[ 1 ]   = NULL;

   m_arpScrollModeBuffers        = NULL;
   m_iScrollModeCamera           = 0;
   m_iScrollModeTimeSliceBuffer  = 0;
   m_iScrollModeLoc              = 0;
   m_bScrollMode                 = false;

   m_iMostNumberOfCameras        = 0;

   m_iBufferSize                 = 0;
}


CTimeSliceDoc::~CTimeSliceDoc()
{
}


BOOL 
CTimeSliceDoc::OnNewDocument()
{
   if ( !CDocument::OnNewDocument() )
   {
      return FALSE;
   }

   m_eventGrabFinished = ::CreateEvent( NULL, FALSE, FALSE, NULL );

   FlyCaptureError   error;
   unsigned int      uiCamera;

   //
   // Initialize all the cameras on the bus in order of serial number.
   //
   FlyCaptureInfoEx arCameras[ MAX_CAMERAS ];
   m_uiNumCameras = MAX_CAMERAS;
   error = ::flycaptureBusEnumerateCamerasEx( arCameras, &m_uiNumCameras );
   _CHECK_ERROR( "flycaptureBusEnumerateCamerasEx()", error );

   if( m_uiNumCameras == 0 )
   {
      AfxMessageBox( "No cameras were detected on the bus, exiting" );
      return FALSE;
   }

   //
   // Sort the serial numbers, remembering the bus order.
   //
   std::list< unsigned long > listCameras;
   for( uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
   {
      listCameras.push_back( arCameras[ uiCamera ].SerialNumber );
   }

   listCameras.sort();

   //
   // Initialize the cameras in serial-number order.
   //
   for( uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
   {
      CameraInfo&  camera = m_arCameraInfo[ uiCamera ];

      error = ::flycaptureCreateContext( &camera.m_context );
      _CHECK_ERROR( "flycaptureCreateContext()", error );

      unsigned int uiBusIndex;
      for( uiBusIndex = 0; uiBusIndex < m_uiNumCameras; uiBusIndex++ )
      {
         if( arCameras[ uiBusIndex ].SerialNumber == listCameras.front() )
         {
            break;
         }
      }
      ASSERT( uiBusIndex != m_uiNumCameras );

      error = ::flycaptureInitializePlus(
         camera.m_context, uiBusIndex, NUM_BUFFERS, NULL );
      _CHECK_ERROR( "flycaptureInitializePlus()", error );

      listCameras.pop_front();

      error = ::flycaptureGetCameraInfo(
	 camera.m_context, &camera.m_info   );
      _CHECK_ERROR( "flycaptureGetCameraInfo()", error );

      camera.m_uiPrevSeqNum = 0;

      //
      // Set a default colour processing method.
      //
      error = ::flycaptureSetColorProcessingMethod(
	 camera.m_context, COLOUR_PROCESSING_METHOD );
      _CHECK_ERROR( "flycaptureSetColorProcessingMethod()", error );
   }

   //
   // Configure grab and pan values based on camera speed.
   //
   if( m_uiNumCameras <= 4 )   
   {
      //
      // 30 Hz. 
      //
      m_iFramesPerCameraPanning  = 4;
      m_iFramesForEndCamera      = 4;
      m_iFramesAtEnd		 = 50;
      m_iNumberOfPanCycles	 = 2;
      m_iInitialBufferedImages	 = 15;  // half a second.
   }
   else
   {
      //
      // 15 Hz.
      //
      m_iFramesPerCameraPanning  = 2;
      m_iFramesForEndCamera      = 2;
      m_iFramesAtEnd		 = 20;
      m_iNumberOfPanCycles	 = 2;
      m_iInitialBufferedImages	 = 7;   // half a second.
   }

   m_iExtraBuffers = m_iFramesForEndCamera + m_iFramesAtEnd;

   //
   // Create extra view windows
   //
   ((CTimeSliceApp*)AfxGetApp())->setViews( m_uiNumCameras );

   //
   // Initialize the GUI contexts for all of the cameras.
   //
   for( uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
   {
      CameraGUIError guierror;
      CameraInfo&    cameraInfo = m_arCameraInfo[ uiCamera ];

      guierror = ::pgrcamguiCreateContext( &cameraInfo.m_guicontext );
      ASSERT( error == PGRCAMGUI_OK );

      guierror = ::pgrcamguiCreateSettingsDialog(
	 cameraInfo.m_guicontext,
	 PGRCAMGUI_TYPE_PGRFLYCAPTURE,
	 (GenericCameraContext)cameraInfo.m_context );
      ASSERT( error == PGRCAMGUI_OK );
   }

   //
   // Call helper functions.
   //
   initBitmapInfo();
   initTempImages();
   initProcessedImages();
   resizeExtraImageBuffers( MIN_BGR_BUFFER_SIZE );
   mostNumberOfCamerasOnASingleBus();
   readTweakValues();

   m_cameraInfoTimeSlice = m_arCameraInfo[ 0 ];

   VERIFY( m_dialogScrollMode.Create( IDD_DIALOG_SCROLLMODE ) );
   m_dialogScrollMode.m_pDoc = this;
   allocateScrollModeBuffers( MIN_BGR_BUFFER_SIZE );

   //
   // Start up grab thread.
   //
   m_bContinueGrab = true;
   ::_beginthread( threadGrab, 0, this  );

   return TRUE;
}

void 
CTimeSliceDoc::OnCloseDocument() 
{
   writeTweakValues();

   //
   // Wait for the thread to stop.
   //
   if( m_bContinueGrab )
   {
      m_bContinueGrab = false;
      ::WaitForSingleObject( m_eventGrabFinished, 10000 );
   }

   for( unsigned int uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
   {
      CameraInfo&  camera = m_arCameraInfo[ uiCamera ];

      //
      // Destroy flycapture contexts.
      //
      ::flycaptureStop( camera.m_context );
      ::flycaptureDestroyContext( camera.m_context );

      //
      // Destroy GUI contexts.
      //
      ::pgrcamguiDestroyContext( camera.m_guicontext );

      //
      // Free bitmap info memory.
      //
      if( camera.m_pbitmapinfo32 != NULL )
      {
	 free( camera.m_pbitmapinfo32 );
	 camera.m_pbitmapinfo32 = NULL;
      }

      if( camera.m_imageProcessed.pData != NULL )
      {
         delete [] camera.m_imageProcessed.pData;
         camera.m_imageProcessed.pData = NULL;
      }
   }

   dealocateExtraImageBuffers();

   deallocateScrollModeBuffers();

   ::CloseHandle( m_eventGrabFinished );

   CDocument::OnCloseDocument();
}


void 
CTimeSliceDoc::OnConfig() 
{
   CDialogConfig  dialog;

   int*	 arpiTweaks[] = 
   {
      &dialog.m_iTweak0,
      &dialog.m_iTweak1,
      &dialog.m_iTweak2,
      &dialog.m_iTweak3,
      &dialog.m_iTweak4,
      &dialog.m_iTweak5,
      &dialog.m_iTweak6,
      &dialog.m_iTweak7,
      &dialog.m_iTweak8,
      &dialog.m_iTweak9,
      &dialog.m_iTweak10,
      &dialog.m_iTweak11,
      &dialog.m_iTweak12,
      &dialog.m_iTweak13,
   };

   unsigned int uiCamera;

   //
   // Initialize the tweak values.
   //
   for( uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
   {
      *arpiTweaks[ uiCamera ] = m_arCameraInfo[ uiCamera ].m_iVerticalTweak;
   }

   //
   // Display the dialog.
   //
   INT_PTR iRet = dialog.DoModal();

   //
   // Store the new tweak values.
   //
   if( iRet == IDOK )
   {
      for( uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
      {
	 m_arCameraInfo[ uiCamera ].m_iVerticalTweak = *arpiTweaks[ uiCamera ];
      }
   }
}

void 
CTimeSliceDoc::OnEditSaveAnimation() 
{
   CWaitCursor waitcursor;

   if( m_listSavingCameraInfo.size() == 0 )
   {
      ::AfxMessageBox( "No images to save!" );
      return;
   }

   //
   // Assemble saving filename.  Use the current time instead of the capture
   // time so we don't get filename conflicts when accidentally saving
   // multiple times.
   //
   time_t timeCurr;
   ::time( &timeCurr );
   struct tm* timeStruct = ::localtime( &timeCurr );

   char pszFilename[ MAX_PATH ];
   sprintf( 
      pszFilename, 
      "timeslice %04d%02d%02d %02d%02d%02d.avi", 
      timeStruct->tm_year + 1900,
      timeStruct->tm_mon + 1,
      timeStruct->tm_mday,
      timeStruct->tm_hour,
      timeStruct->tm_min,
      timeStruct->tm_sec );


   PGRAviFile  avifile;

   int	 iFramerate;
   if( m_uiNumCameras <= 4 )   
   {
      // assume 30 Hz
      iFramerate = 30;
   }
   else
   {
      // assume 15 Hz
      iFramerate = 15;
   }


   if( !avifile.open( pszFilename, 640, 480 - MAX_VERTICAL_TWEAK, 32, iFramerate ) )
   {
      CString  csMessage;
      csMessage.Format( "Could not open file %s", pszFilename );
      ::AfxMessageBox( csMessage );
      return;
   }

   unsigned char* pTempImageBuffer = new unsigned char[ m_iBufferSize ];
   unsigned int uiFrame;
   size_t stSize = m_listSavingCameraInfo.size();
   
   for( uiFrame = 0; uiFrame < stSize; uiFrame++ )
   {
      CameraInfo& info = m_listSavingCameraInfo.at( uiFrame ); 

      FlyCaptureImage imageTemp = info.m_image.image;
      
      imageTemp.pData = info.m_pTimesliceImageBuffer;

      //
      // Readjust for vertical tweak.
      //
      unsigned char* pTweaked = imageTemp.pData + 
	 ( info.m_iVerticalTweak * imageTemp.iCols * 3 );

      //
      // Save it to the .avi file
      //
      avifile.appendFrame( pTweaked );
   }

   avifile.close();

   CString csMessage;
   csMessage.Format( 
      "Saved %d frames to %s!\n"
      "Would you like to view the .avi?", 
      uiFrame, 
      pszFilename );

   if( ::AfxMessageBox( csMessage, MB_YESNO ) == IDYES )
   {
      INT_PTR ipError = (INT_PTR)::ShellExecute(
	 0, "open", pszFilename, NULL, NULL, SW_SHOWNORMAL );

      if( ipError < 33 ) 
      {
	 CString csMessage;
	 csMessage.Format( "Error openning %s", pszFilename );
	 ::AfxMessageBox( csMessage );
      }
   }

   delete [] pTempImageBuffer;
}


void 
CTimeSliceDoc::OnButtonScrollMode() 
{
   if( checkScrollModeCompat() )
   {
      m_bScrollMode = !m_bScrollMode;
      
      if( m_bScrollMode )
      {
         m_dialogScrollMode.ShowWindow( SW_SHOW );
      }
      else
      {
         m_dialogScrollMode.ShowWindow( SW_HIDE );
      }
   }
}


void 
CTimeSliceDoc::OnUpdateButtonScrollMode( CCmdUI* pCmdUI ) 
{
   pCmdUI->SetCheck( m_bScrollMode ? 1 : 0 );
}


void 
CTimeSliceDoc::Serialize( CArchive& ar )
{
   if ( ar.IsStoring() )
   {
   }
   else
   {
   }
}


#ifdef _DEBUG
void 
CTimeSliceDoc::AssertValid() const
{
   CDocument::AssertValid();
}

void 
CTimeSliceDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif //_DEBUG


void 
CTimeSliceDoc::initAllViews()
{
   POSITION	     pos = GetFirstViewPosition();
   CTimeSliceView*   pView = NULL;

   unsigned int   uiCameraToView = 0;
   
   while ( pos != NULL )
   {
      pView = (CTimeSliceView*)GetNextView( pos );
      
      if( uiCameraToView >= m_uiNumCameras )
      {
	 //
	 // Special case - the last view is the panning view.
	 //
	 pView->setViewType( PANNING );	    
      }
      else
      {
	 pView->setCamera( uiCameraToView );
	 pView->setViewType( SINGLE_CAMERA );
      }
      
      uiCameraToView++;
   }  
}


void
CTimeSliceDoc::initBitmapInfo()
{ 
   for( unsigned int uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
   {
      CameraInfo& camera = m_arCameraInfo[ uiCamera ];

      //
      // init the 32 bit bitmapinfo.
      //
      camera.m_pbitmapinfo32 = (BITMAPINFO*)calloc( 1, sizeof( BITMAPINFO ) );

      camera.m_pbitmapinfo32->bmiHeader.biSize		 = sizeof( BITMAPINFOHEADER );
      camera.m_pbitmapinfo32->bmiHeader.biPlanes	 = 1;
      camera.m_pbitmapinfo32->bmiHeader.biCompression	 = BI_RGB;
      camera.m_pbitmapinfo32->bmiHeader.biXPelsPerMeter	 = 100;
      camera.m_pbitmapinfo32->bmiHeader.biYPelsPerMeter	 = 100;
      camera.m_pbitmapinfo32->bmiHeader.biClrUsed	 = 0;
      camera.m_pbitmapinfo32->bmiHeader.biClrImportant	 = 0;
      camera.m_pbitmapinfo32->bmiHeader.biBitCount	 = 32;

      camera.m_pbitmapinfo32->bmiHeader.biHeight	 = -100;
      camera.m_pbitmapinfo32->bmiHeader.biWidth		 = 100;
   }     
}

void
CTimeSliceDoc::resizeExtraImageBuffers( int iSizeBytes )
{
   if( iSizeBytes < MIN_BGR_BUFFER_SIZE )
   {
      iSizeBytes = MIN_BGR_BUFFER_SIZE;
   }

   ASSERT( iSizeBytes <= MAX_BGR_BUFFER_SIZE );

   //
   // Extra image buffers
   //
   m_iTotalExtraImageBuffers =  
      ( m_uiNumCameras * m_iFramesPerCameraPanning ) * 
      m_iNumberOfPanCycles + m_iExtraBuffers + 1;

   dealocateExtraImageBuffers();   
   
   //
   // Initialize extra image buffers.
   //
   m_arpImageBuffers = new unsigned char*[ m_iTotalExtraImageBuffers ];

   int i;
   for( i = 0; i < m_iTotalExtraImageBuffers; i++ )
   {
      m_arpImageBuffers[ i ] = new unsigned char[ iSizeBytes ];
   }

   //
   // Initialize image buffers.
   //
   m_arpInitialBuffers[ 0 ] = new unsigned char*[ m_iInitialBufferedImages ];
   m_arpInitialBuffers[ 1 ] = new unsigned char*[ m_iInitialBufferedImages ];

   for( i = 0; i < m_iInitialBufferedImages; i++ )
   {
      m_arpInitialBuffers[ 0 ][ i ] = new unsigned char[ iSizeBytes ];
      m_arpInitialBuffers[ 1 ][ i ] = new unsigned char[ iSizeBytes ];
   }

   m_iInitialBufferIndex = 0; // point the index to the first image for now.

   for( unsigned int uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
   {
      CameraInfo& camera = m_arCameraInfo[ uiCamera ];

      if( camera.m_imageProcessed.pData != NULL )
      {
         delete [] camera.m_imageProcessed.pData;
         camera.m_imageProcessed.pData = NULL;
      }

      camera.m_imageProcessed.pData = new unsigned char[ iSizeBytes ];            
      memset( camera.m_imageProcessed.pData, 0x0, iSizeBytes );
   }
}

void
CTimeSliceDoc::dealocateExtraImageBuffers()
{
   //
   // Free extra image buffers.
   //
   int i;
   for( i = 0; i < m_iTotalExtraImageBuffers; i++ )
   {
      if( m_arpImageBuffers != NULL )
      {
         delete [] m_arpImageBuffers[ i ];
         m_arpImageBuffers[ i ] = NULL;
      }
   }

   //
   // Free the ring buffers that will hold the images before the tigger
   // for saving the .avi file.
   //
   for( i = 0; i < m_iInitialBufferedImages; i++ )
   {
      if( m_arpInitialBuffers[ 0 ] != NULL )
      {
         delete [] m_arpInitialBuffers[ 0 ][ i ];
         m_arpInitialBuffers[ 0 ][ i ] = NULL;
      }

      if( m_arpInitialBuffers[ 1 ] != NULL )
      {
         delete [] m_arpInitialBuffers[ 1 ][ i ];
         m_arpInitialBuffers[ 1 ][ i ] = NULL;
      }
   }

   delete [] m_arpInitialBuffers[ 0 ];
   delete [] m_arpInitialBuffers[ 1 ];
}

void
CTimeSliceDoc::initTempImages()
{
   for( unsigned int uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
   {
      CameraInfo& camera = m_arCameraInfo[ uiCamera ];
      camera.m_pTimesliceImageBuffer = NULL;
   }
}

void
CTimeSliceDoc::initProcessedImages()
{
   for( unsigned int uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
   {
      CameraInfo&  cameraInfo = m_arCameraInfo[ uiCamera ];

      memset( &cameraInfo.m_imageProcessed, 0x0, 
         sizeof( cameraInfo.m_imageProcessed ) );
   }
}


void	 
CTimeSliceDoc::readTweakValues()
{
   for( unsigned int uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
   {
      CString csKey;
      csKey.Format( "Camera%u", uiCamera );

      m_arCameraInfo[ uiCamera ].m_iVerticalTweak = 
	 ::AfxGetApp()->GetProfileInt( "VerticalTweak", csKey, 0 );        
   }
}


void	 
CTimeSliceDoc::writeTweakValues()
{
   for( unsigned int uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
   {
      CString csKey;
      csKey.Format( "Camera%d", uiCamera );

      ::AfxGetApp()->WriteProfileInt( 
	 "VerticalTweak", 
	 csKey, 
	 m_arCameraInfo[ uiCamera ].m_iVerticalTweak );
   }
}


void 
CTimeSliceDoc::spaceBarEvent()
{
   if( m_bSliceEvent )
   {
      if( m_stateSlice == DISPLAYING_TIMESLICE )
      {
	 m_bSliceEvent = !m_bSliceEvent;
      }
      
      if( m_stateSlice == SCROLLMODE_SCROLLING )
      {
         m_bSliceEvent = !m_bSliceEvent;
      }
   }
   else
   {
      m_bSliceEvent = !m_bSliceEvent;
   }
}


bool 
CTimeSliceDoc::checkScrollModeCompat()
{
#if 0
   if( m_uiNumCameras != 8 )
   {
#if 0
      ::AfxMessageBox( 
         "Sorry, scroll mode is only available for the \"8 Dragonlfy\" "
         "camera configuration.\n" );
#endif
      return false;
   }
#endif
   return true;
}


void 
CTimeSliceDoc::allocateScrollModeBuffers( int iSizeBytes )
{
   if( !checkScrollModeCompat() )
   {
      return;
   }

   ASSERT( m_arpScrollModeBuffers == NULL );

   m_arpScrollModeBuffers = new unsigned char**[ m_uiNumCameras ];

   for( unsigned iCamera = 0; iCamera < m_uiNumCameras; iCamera++ )
   {      
      m_arpScrollModeBuffers[ iCamera ] = new unsigned char*[ SCROLL_MODE_BUFFERS ];

      for( unsigned iBuffer = 0; iBuffer < SCROLL_MODE_BUFFERS; iBuffer++ )
      {
         m_arpScrollModeBuffers[ iCamera ][ iBuffer ] = new unsigned char[ iSizeBytes ];
      }
   }   
}


void 
CTimeSliceDoc::deallocateScrollModeBuffers()
{
   if( m_arpScrollModeBuffers == NULL )
   {
      return;
   }

   for( unsigned iCamera = 0; iCamera < m_uiNumCameras; iCamera++ )
   {      
      for( unsigned iBuffer = 0; iBuffer < SCROLL_MODE_BUFFERS; iBuffer++ )
      {
         delete [] m_arpScrollModeBuffers[ iCamera ][ iBuffer ];
         m_arpScrollModeBuffers[ iCamera ][ iBuffer ] = NULL;
      }

      delete [] m_arpScrollModeBuffers[ iCamera ];
      m_arpScrollModeBuffers[ iCamera ] = NULL;
   }   

   delete [] m_arpScrollModeBuffers;
   m_arpScrollModeBuffers = NULL;
}


void 
CTimeSliceDoc::scrollModifyFrame( int iMod )
{
   if( iMod >= 0 )
   {
      m_iScrollModeLoc = ( m_iScrollModeLoc + iMod ) % SCROLL_MODE_BUFFERS;
   }
   else
   {
      m_iScrollModeLoc = 
         ( ( m_iScrollModeLoc + iMod + SCROLL_MODE_BUFFERS ) % SCROLL_MODE_BUFFERS );
   }
}


void 
CTimeSliceDoc::scrollModifyCamera( int iMod )
{
   if( iMod >= 0 )
   {
      m_iScrollModeCamera = ( m_iScrollModeCamera + iMod ) % m_uiNumCameras;
   }
   else
   {
      m_iScrollModeCamera = 
         ( ( m_iScrollModeCamera + iMod + m_uiNumCameras ) % m_uiNumCameras );
   }
}


void 
CTimeSliceDoc::updateViews() const
{
   POSITION          pos = GetFirstViewPosition();      
   CTimeSliceView*   pView = NULL;
   
   while( pos != NULL )
   {
      pView = (CTimeSliceView*)GetNextView( pos );	 
      ::InvalidateRect( pView->GetSafeHwnd(), NULL, FALSE );
   }
}


BOOL 
CTimeSliceDoc::startSync() 
{
   FlyCaptureContext arContexts[ MAX_CAMERAS ];

   bool		  bAvailable;
   unsigned int	  uiMaxImagePixelsWidth;
   unsigned int	  uiMaxImagePixelsHeight;
   unsigned int	  uiPixelUnitHorz;
   unsigned int	  uiPixelUnitVert;
   unsigned int   uiPixelFormats;
   unsigned int   uiPacketSize;
   int            iImageSize;
   
   FlyCapturePixelFormat pixelFormat = FLYCAPTURE_MONO8;
   FlyCapturePacketInfo  packetInfo;

   FlyCaptureError error;

   for( unsigned int uiContext = 0; uiContext < m_uiNumCameras; uiContext++ )
   {
      arContexts[ uiContext ] = m_arCameraInfo[ uiContext ].m_context;

      //
      // Retrieve the available pixel format, width and height.
      //
      error = flycaptureQueryCustomImage(
         arContexts[ uiContext ],
         0,
         &bAvailable,
         &uiMaxImagePixelsWidth,
         &uiMaxImagePixelsHeight,
         &uiPixelUnitHorz,
         &uiPixelUnitVert,
         &uiPixelFormats );
      _CHECK_ERROR( "flycaptureQueryCustomImage()", error );

      if( !bAvailable )
      {
         ::AfxMessageBox( 
            "Warning!  Camera reports that mode 0 is not available." );
      }

      if( FLYCAPTURE_RAW8 & uiPixelFormats )
      {
         pixelFormat = FLYCAPTURE_RAW8;
      }
      else if( FLYCAPTURE_MONO8 & uiPixelFormats )
      {
         pixelFormat = FLYCAPTURE_MONO8;
      }
      else
      {
         ::AfxMessageBox( "8bit pixel formats not supported with this camera" );
      }      

      //
      // Retrieve packet information.
      //
      error = flycaptureGetCustomImagePacketInfo(
         arContexts[ uiContext ],
         0,
         uiMaxImagePixelsWidth,
         uiMaxImagePixelsHeight,
         pixelFormat,
         &packetInfo );
       _CHECK_ERROR( "flycaptureGetCustomImagePacketInfo()", error );

       iImageSize = uiMaxImagePixelsWidth * uiMaxImagePixelsHeight * 4;
       
       if( m_iBufferSize != iImageSize )
       {
          m_iBufferSize = iImageSize;
          
          //
          // Make sure the destination buffers are big enough for the resultant
          // 32 bit image.
          //
          resizeExtraImageBuffers( m_iBufferSize );
          
          deallocateScrollModeBuffers();
          allocateScrollModeBuffers( m_iBufferSize );    
       }

      //
      // Setup the maximum packet size available.
      //
      uiPacketSize = packetInfo.uiMaxSizeBytes;
      if( (uiPacketSize * m_iMostNumberOfCameras) > MAX_BANDWIDTH_BYTES )
      {
         uiPacketSize = 
            (unsigned int)(MAX_BANDWIDTH_BYTES / m_iMostNumberOfCameras);
         uiPacketSize = 
            uiPacketSize - (uiPacketSize % packetInfo.uiMinSizeBytes);
      }

      ASSERT( uiPacketSize >= packetInfo.uiMinSizeBytes );

      error = flycaptureStartLockNextCustomImagePacket(
         arContexts[ uiContext ],
         0,
         0,
         0,
         uiMaxImagePixelsWidth,
         uiMaxImagePixelsHeight,
         uiPacketSize,
         pixelFormat );
      _CHECK_ERROR( "flycaptureStartLockNextCustomImagePacket()", error );

   }

   error = ::flycaptureSyncForLockNext( 
      arContexts, m_uiNumCameras );
   if( error != FLYCAPTURE_OK )
   {
      ::AfxMessageBox( 
         "Warning: camera syncronization failed.  "
         "Images will not necessarily be synched." );
   }

   return TRUE;
}

void
CTimeSliceDoc::mostNumberOfCamerasOnASingleBus()
{
   int iaNumberOfCamerasOnBus[ MAX_NUM_1394_CARDS ];
   int i;
   for( i = 0; i < MAX_NUM_1394_CARDS; i++ )
   {
      iaNumberOfCamerasOnBus[ i ] = 0;
   }
   
   for( unsigned int uiCamera = 0; uiCamera < m_uiNumCameras; uiCamera++ )
   {
      CameraInfo& camera = m_arCameraInfo[ uiCamera ];

      iaNumberOfCamerasOnBus[ camera.m_info.iBusNum ]++;
   }

   for( i = 0; i < MAX_NUM_1394_CARDS; i++ )
   {
      if( iaNumberOfCamerasOnBus[ i ] > m_iMostNumberOfCameras )
      {
         m_iMostNumberOfCameras = iaNumberOfCamerasOnBus[ i ];
      }
   }
}


void 
CTimeSliceDoc::grabAndCheckSync()
{
   unsigned nCamera;

   //
   // Grab an image from each camera.
   //
   for( nCamera = 0; nCamera < m_uiNumCameras; nCamera++ )
   {
      CameraInfo& camera = m_arCameraInfo[ nCamera ];
      
      camera.m_error = ::flycaptureLockNext( camera.m_context, &camera.m_image );
      if( camera.m_error != FLYCAPTURE_OK )
      {
         TRACE( 
            "flycaptureLockNext() failed with %s", 
            ::flycaptureErrorToString( camera.m_error ) );
      }
      
      camera.m_imageProcessed.pixelFormat = FLYCAPTURE_BGRU;
      
      camera.m_error = flycaptureConvertImage( 
         camera.m_context, 
         &camera.m_image.image, 
         &camera.m_imageProcessed );
      if( camera.m_error != FLYCAPTURE_OK )
      {
         TRACE( 
            "flycaptureConvertImage() returned an error (\"%s\")\n",
            flycaptureErrorToString( camera.m_error ) );
         continue;
      }
      
   }
   
   //
   // Check if the images are synchronized.
   //
#ifdef COUNT_OUT_OF_SYNC
   for( nCamera = 1; nCamera < m_uiNumCameras; nCamera++ )
   {
      int iDeltaFrom0 =      
         abs( (int)( (  
            m_arCameraInfo[ nCamera ].m_image.image.timeStamp.ulCycleSeconds * 8000 +
            m_arCameraInfo[ nCamera ].m_image.image.timeStamp.ulCycleCount ) -
         (  m_arCameraInfo[ 0       ].m_image.image.timeStamp.ulCycleSeconds * 8000 +
            m_arCameraInfo[ 0       ].m_image.image.timeStamp.ulCycleCount ) ) );

      if( ( iDeltaFrom0 % ( 128 * 8000 - 1 ) ) > SYNC_TOLERANCE )
      {
         m_uiOutOfSyncImages++;
         break;
      }
   }
#endif

   //
   // Check the sequence numbers to see if we have missed an image.
   //
   for( nCamera = 0; nCamera < m_uiNumCameras; nCamera++ )
   {
      CameraInfo& camera = m_arCameraInfo[ nCamera ];
      
      if( camera.m_uiPrevSeqNum != 0 )
      {
         int iDelta = camera.m_image.uiSeqNum - camera.m_uiPrevSeqNum;
         //ASSERT( iDelta > 0 );
         
         if( iDelta > 1 )
         {
            m_uiMissedImages += ( iDelta - 1 );
         }
      }
      
      camera.m_uiPrevSeqNum = camera.m_image.uiSeqNum;
   }
}


void 
CTimeSliceDoc::updateBitmaps()
{
   //
   // Update both bitmap info headers for each camera.  Here's where we
   // reduce the height of each image for the vertical tweak.
   //
   for( unsigned nCamera = 0; nCamera < m_uiNumCameras; nCamera++ )
   {
      CameraInfo& camera = m_arCameraInfo[ nCamera ];

      BITMAPINFOHEADER* pbmiHeader32 = &camera.m_pbitmapinfo32->bmiHeader;
      
      pbmiHeader32->biWidth     = camera.m_image.image.iCols;
      pbmiHeader32->biHeight    = -( camera.m_image.image.iRows - MAX_VERTICAL_TWEAK );	 
      pbmiHeader32->biSizeImage = 
         pbmiHeader32->biWidth * 
         pbmiHeader32->biHeight * 
         ( pbmiHeader32->biBitCount / 8 );
   }
}


void 
CTimeSliceDoc::grabLoop()
{
   //
   // For framerate calculation
   //
   struct _timeb timeLast;
   struct _timeb timeGrab;

   ::_ftime( &timeLast );

   //
   // List of camerainfo objects for our panning pattern
   //
   std::deque< CameraInfo > queueCamInfoDisplay;

   int	 iFreeBufferIndex  = 0;
   int	 iFramesAtEnd	   = m_iFramesAtEnd;

   int   iScrollModeBuffersLeft = -1;

   //
   // The set of ring buffers we are using. 0 or 1.
   //
   int	 iInitialBufferSet = 0;

   int	 iDisplayingTimesliceIndex = 0;
   

   startSync();

   //
   // The main grab loop.
   //
   while( m_bContinueGrab )
   {
      grabAndCheckSync();
      updateBitmaps();      

      if( !m_bContinueGrab )
      {
	 break;
      }
      
      unsigned nCamera;
      unsigned nImage;


      if( !m_bSliceEvent )
      {
         //
         // We are not in a slice event, just grabbing images.
         //
         m_stateSlice = RUNNING;

         ASSERT( queueCamInfoDisplay.size() == 0 );

         if( !m_bScrollMode )
         {
            //
            // Copy the image into one of our buffer slots just in case we need 
            // it later for saving.
            //
            m_iInitialBufferIndex = 
               ( m_iInitialBufferIndex + 1 ) % m_iInitialBufferedImages;

            //
            // just get the panning view to display camera 0
            //
            CameraInfo cameraInfo = m_arCameraInfo[ 0 ];
            
            ::memcpy( 
               m_arpInitialBuffers[ iInitialBufferSet ][ m_iInitialBufferIndex ],
               cameraInfo.m_imageProcessed.pData,
               cameraInfo.m_imageProcessed.iRowInc * 
               cameraInfo.m_imageProcessed.iRows );
         
            cameraInfo.m_pTimesliceImageBuffer = 
               m_arpInitialBuffers[ iInitialBufferSet ][ m_iInitialBufferIndex ];

            //
            // push the image onto the list (stack?) - this will immediately be 
            // popped off when the image is about to be displayed.
            //
            queueCamInfoDisplay.push_back( cameraInfo );
         }
         else
         {
            //
            // Scroll mode grabbing.  Buffer images from all cameras.
            //
            ASSERT( m_arpScrollModeBuffers != NULL );
            
            m_iScrollModeTimeSliceBuffer = 
               ( m_iScrollModeTimeSliceBuffer + 1 ) % SCROLL_MODE_BUFFERS;

            for( nCamera = 0; nCamera < m_uiNumCameras; nCamera++ )
            {               
               ::memcpy(
                  m_arpScrollModeBuffers[ nCamera ][ m_iScrollModeTimeSliceBuffer ],
                  m_arCameraInfo[ nCamera ].m_imageProcessed.pData,
                  m_arCameraInfo[ nCamera ].m_imageProcessed.iRowInc * 
                  m_arCameraInfo[ nCamera ].m_imageProcessed.iRows );               
            }

            CameraInfo info = m_arCameraInfo[ m_iScrollModeCamera ];
            info.m_pTimesliceImageBuffer = 
               m_arpScrollModeBuffers[ m_iScrollModeCamera ][ m_iScrollModeTimeSliceBuffer ];
            queueCamInfoDisplay.push_back( info );
         }
      }
      else
      {
         //
         // In timeslice mode. 
         //
	 switch( m_stateSlice )
	 {
	 case RUNNING:
	    {
               if( !m_bScrollMode )
               {                  
                  m_stateSlice = GRABBING_TIMESLICE;

                  CameraInfo cameraInfo = m_arCameraInfo[ 0 ];
                  cameraInfo.m_pTimesliceImageBuffer = cameraInfo.m_imageProcessed.pData;
                  queueCamInfoDisplay.push_back( cameraInfo );
                  
                  //
                  // Clear the last bunch of saved images
                  //
                  m_listSavingCameraInfo.clear();
                  
                  //
                  // Save the buffered images - just copy everything besides the
                  // image pointer from the current image - this is a bit of a cheat.
                  //
                  CameraInfo info = m_arCameraInfo[ 0 ];
                  
                  for( int i = 0; i < m_iInitialBufferedImages; i++ )
                  {
                     int iIndex = 
                        ( m_iInitialBufferIndex + i + 1 ) % m_iInitialBufferedImages;
                     
                     info.m_pTimesliceImageBuffer = 
                        m_arpInitialBuffers[ iInitialBufferSet ][ iIndex ];
                     
                     m_listSavingCameraInfo.push_back( info );
                  }
                  
                  //
                  // Use the other initial buffer set now.
                  //
                  iInitialBufferSet = ( iInitialBufferSet + 1 ) % 2;
               }
               else
               {
                  // Scroll mode.
                  ASSERT( m_arpScrollModeBuffers != NULL );

                  //
                  // Buffer this image too.
                  //
                  m_iScrollModeTimeSliceBuffer = 
                     ( m_iScrollModeTimeSliceBuffer + 1 ) % SCROLL_MODE_BUFFERS;
                  
                  for( nCamera = 0; nCamera < m_uiNumCameras; nCamera++ )
                  {
                     ::memcpy(
                        m_arpScrollModeBuffers[ nCamera ][ m_iScrollModeTimeSliceBuffer ],
                        m_arCameraInfo[ nCamera ].m_imageProcessed.pData,
                        m_arCameraInfo[ nCamera ].m_imageProcessed.iRowInc * 
                        m_arCameraInfo[ nCamera ].m_imageProcessed.iRows );                 
                  }

                  //
                  // Continue to display the selected camera.
                  //
                  CameraInfo info = m_arCameraInfo[ m_iScrollModeCamera ];
                  info.m_pTimesliceImageBuffer = 
                     m_arpScrollModeBuffers[ m_iScrollModeCamera ][ m_iScrollModeTimeSliceBuffer ];
                  queueCamInfoDisplay.push_back( info );

                  //
                  // Set up for next step.
                  //
                  m_iScrollModeLoc = m_iScrollModeTimeSliceBuffer;                  
                  m_stateSlice = SCROLLMODE_GRABBING;                  
                  iScrollModeBuffersLeft = SCROLL_MODE_BUFFERS / 2;
               }
	    }
	    break;
	    

	 case GRABBING_TIMESLICE:
	    {
	       CameraInfo cameraInfo;
	       
	       for( int iPan = 0; iPan < m_iNumberOfPanCycles; iPan++ )
	       {
		  //
		  // forward pan
		  //
		  for( nCamera = 0; nCamera < m_uiNumCameras; nCamera++ )
		  {
		     cameraInfo = m_arCameraInfo[ nCamera ];
		     
		     ::memcpy( 
			m_arpImageBuffers[ iFreeBufferIndex ],
			cameraInfo.m_imageProcessed.pData,
                        cameraInfo.m_imageProcessed.iRowInc * 
                        cameraInfo.m_imageProcessed.iRows );
		     
		     cameraInfo.m_pTimesliceImageBuffer = 
			m_arpImageBuffers[ iFreeBufferIndex ];
		     
		     iFreeBufferIndex++;
		     ASSERT( iFreeBufferIndex < m_iTotalExtraImageBuffers );
		     
		     for( nImage = 0; nImage < (unsigned)m_iFramesPerCameraPanning; nImage++ )
		     {		     
			queueCamInfoDisplay.push_back( cameraInfo );

			m_listSavingCameraInfo.push_back( cameraInfo );
		     }
		  }	    
		  
		  //
		  // pause at end
		  //
		  for( nImage = 0; nImage < (unsigned)m_iFramesForEndCamera; nImage++ )
		  {
		     queueCamInfoDisplay.push_back( cameraInfo );

		     m_listSavingCameraInfo.push_back( cameraInfo );
		  }
		  
		  //
		  // backward pan
		  //
		  for( int iCamera = m_uiNumCameras - 1; iCamera >= 0; iCamera-- )
		  {
		     cameraInfo = m_arCameraInfo[ iCamera ];
		     
		     ::memcpy( 
			m_arpImageBuffers[ iFreeBufferIndex ],
                        cameraInfo.m_imageProcessed.pData,
                        cameraInfo.m_imageProcessed.iRowInc * 
                        cameraInfo.m_imageProcessed.iRows );
		     
		     cameraInfo.m_pTimesliceImageBuffer = 
			m_arpImageBuffers[ iFreeBufferIndex ];
		     
		     iFreeBufferIndex++;
		     ASSERT( iFreeBufferIndex < m_iTotalExtraImageBuffers );
		     
		     for( nImage = 0; nImage < (unsigned)m_iFramesPerCameraPanning; nImage++ )
		     {
			queueCamInfoDisplay.push_back( cameraInfo );
			m_listSavingCameraInfo.push_back( cameraInfo );
		     }
		  }
	       }
	       
	       m_stateSlice = GRABBING_DELAYED;
	       
	       break;
	    }


	 case GRABBING_DELAYED:
	    {	       
	       CameraInfo  cameraInfo = m_arCameraInfo[ 0 ];

	       ASSERT( iFreeBufferIndex < m_iTotalExtraImageBuffers );
	       
	       ::memcpy( 
		  m_arpImageBuffers[ iFreeBufferIndex ],
                  cameraInfo.m_imageProcessed.pData,
                  cameraInfo.m_imageProcessed.iRowInc * 
                  cameraInfo.m_imageProcessed.iRows );
	       
	       cameraInfo.m_pTimesliceImageBuffer = 
		  m_arpImageBuffers[ iFreeBufferIndex ];
	       
	       iFreeBufferIndex++;	       
	       
	       queueCamInfoDisplay.push_back( cameraInfo );

	       m_listSavingCameraInfo.push_back( cameraInfo );
	       
	       iFramesAtEnd--;
	       
	       if( iFramesAtEnd == 0 )
	       {
		  m_stateSlice = DISPLAYING_DELAYED;
	       }
	       
	       break;
	    }


	 case DISPLAYING_DELAYED:

	    if( queueCamInfoDisplay.size() == 1 )
	    {
	       //
	       // Reset to starting state.
	       //
	       iFreeBufferIndex     = 0;
	       iFramesAtEnd	    = m_iFramesAtEnd;

	       
	       m_stateSlice	  = DISPLAYING_TIMESLICE;
	       iDisplayingTimesliceIndex  = 0;
	    }

	    break;

	 case DISPLAYING_TIMESLICE:
	    {
	       ASSERT( m_listSavingCameraInfo.size() > 0 );

	       iDisplayingTimesliceIndex = 
		  ( iDisplayingTimesliceIndex + 1 ) % (int)m_listSavingCameraInfo.size();

	       CameraInfo info = 
		  m_listSavingCameraInfo.at( iDisplayingTimesliceIndex );

	       queueCamInfoDisplay.push_back( info );
	    }

	    break;


         case SCROLLMODE_GRABBING:
            {
               m_iScrollModeTimeSliceBuffer = 
                  ( m_iScrollModeTimeSliceBuffer + 1 ) % SCROLL_MODE_BUFFERS;

               for( nCamera = 0; nCamera < m_uiNumCameras; nCamera++ )
               {
                  ::memcpy(
                     m_arpScrollModeBuffers[ nCamera ][ m_iScrollModeTimeSliceBuffer ],
                     m_arCameraInfo[ nCamera ].m_imageProcessed.pData,
                     m_arCameraInfo[ nCamera ].m_imageProcessed.iRowInc * 
                     m_arCameraInfo[ nCamera ].m_imageProcessed.iRows );                
               }

               iScrollModeBuffersLeft--;

               if( iScrollModeBuffersLeft == 0 )
               {
                  m_stateSlice = SCROLLMODE_SCROLLING;
               }

               //
               // Just display the images we're grabbing.
               //
               CameraInfo info = m_arCameraInfo[ m_iScrollModeCamera ];
               info.m_pTimesliceImageBuffer = 
                  m_arpScrollModeBuffers[ m_iScrollModeCamera ][ m_iScrollModeTimeSliceBuffer ];
               queueCamInfoDisplay.push_back( info );
            }
            break;


         case SCROLLMODE_SCROLLING:
            {               
               ASSERT( iScrollModeBuffersLeft == 0 );
               ASSERT( m_iScrollModeLoc >= 0 );
               ASSERT( m_iScrollModeLoc < SCROLL_MODE_BUFFERS );
               ASSERT( m_iScrollModeCamera >= 0 );
               ASSERT( m_iScrollModeCamera < (signed)m_uiNumCameras );
               ASSERT( m_arpScrollModeBuffers[ m_iScrollModeCamera ][
                  m_iScrollModeLoc ] != NULL );
               
               CameraInfo info = m_arCameraInfo[ m_iScrollModeCamera ];
               info.m_pTimesliceImageBuffer = 
                  m_arpScrollModeBuffers[ m_iScrollModeCamera ][ m_iScrollModeLoc ];
               queueCamInfoDisplay.push_back( info );
            }
            break;

	 default:
	    ASSERT( FALSE );
	 }

      }

      //
      // Unlock this set of images.
      //
      for( nCamera = 0; nCamera < m_uiNumCameras; nCamera++ )
      {
         CameraInfo& camera = m_arCameraInfo[ nCamera ];

         camera.m_error = ::flycaptureUnlock(
            camera.m_context, camera.m_image.uiBufferIndex );
      }



      ASSERT( queueCamInfoDisplay.size() > 0 );
      
      m_cameraInfoTimeSlice = queueCamInfoDisplay.front();
      queueCamInfoDisplay.pop_front();



      //
      // Update frame rate.
      //
      ::_ftime( &timeGrab );

      double dGrabTime = 
	    (double)( timeGrab.time * 1000 + timeGrab.millitm ) - 
	            ( timeLast.time * 1000 + timeLast.millitm );

      if( dGrabTime != 0 )
      {	 
	 // Calculate our framerate using a running average.
	 m_dFrameRate = 0.95 * m_dFrameRate + 0.05 * ( 1000.0 / dGrabTime );
	 
	 timeLast = timeGrab;
      }

      updateViews();
   }
   
   ::SetEvent( m_eventGrabFinished );
}


void
CTimeSliceDoc::threadGrab( void* pparam )
{
   CTimeSliceDoc* pDoc = (CTimeSliceDoc*)pparam;

   pDoc->grabLoop();
}


// eof.


