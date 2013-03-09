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
// $Id: FlyCapDoc.cpp,v 1.189 2007/12/13 23:47:17 soowei Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include "stdafx.h"

//=============================================================================
// PGR Includes
//=============================================================================
#include <PGRError.h>

#pragma warning(disable:4097) // disable compiler warnings due to fstream
//=============================================================================
// Project Includes
//=============================================================================
#include "FlyCap.h"
#include "FlyCapDoc.h"
#include "FlyCapView.h"
#include "DlgRecord.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace std;

#ifndef _PGRDIST
#include "htmlhelp.h"
#endif

//=============================================================================
// Macro Definitions
//=============================================================================
#define PGR_PROJECT_NAME "FlyCap"
#define PGR_FILE_NAME    "$RCSfile: FlyCapDoc.cpp,v $"
#define PGR_FILE_VERSION "$Revision: 1.189 $"

#define REGISTRY_KEY_TEXT "SOFTWARE\\Point Grey Research, Inc.\\PGRFlyCapture"
#define REGISTRY_KEY_SIZE 256

//
// Comment this line out if you want to automatically select the first camera 
// on the bus.
//
#define ALLOW_CAMERA_SELECT

//
// Comment this line out if you don't want to enable message logging.
//
#define ENABLE_MESSAGE_LOGGING

//
// The maximum size of the static RGB buffer.  We won't attempt to dynamically
// resize above this.
//
#define _MAX_BGR_BUFFER_SIZE  5000 * 5000 * 4

//
// The minimum size of the BGR image buffer.
//
#define _MIN_BGR_BUFFER_SIZE  640 * 480 * 4

//
// Used throughout to check errors.
//
#define _CHECK( error, function, retval ) \
{ \
   if ( error != FLYCAPTURE_OK ) \
   { \
      CString  csMessage; \
      csMessage.Format( \
         "%s reported \"%s\"", \
         function, \
         flycaptureErrorToString( error ) ); \
      PGR_ERROR_MESSAGE3( csMessage, error, CRITICAL_MESSAGES ); \
      \
      return retval;\
   } \
} \

#define _ENTER_MUTEX( handle, time ) \
   { \
      DWORD dwErrorMutex = ::WaitForSingleObject( handle, time ); \
      ASSERT( dwErrorMutex == WAIT_OBJECT_0 ); \
   } \

#define _EXIT_MUTEX( handle ) ::ReleaseMutex( handle );

#if defined( WIN64 ) && (_MSC_VER >= 1400)
#define REGISTRY_HIVE     KEY_WOW64_64KEY
#elif defined( WIN32 ) && (_MSC_VER >= 1400)
#define REGISTRY_HIVE     KEY_WOW64_32KEY
#else
#define REGISTRY_HIVE     0
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE( CFlyCapDoc, CDocument )

BEGIN_MESSAGE_MAP( CFlyCapDoc, CDocument )
//{{AFX_MSG_MAP(CFlyCapDoc)
ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
ON_COMMAND(ID_VIEW_CAMERACONTROL, OnViewCameracontrol)
ON_UPDATE_COMMAND_UI(ID_VIEW_CAMERACONTROL, OnUpdateViewCameracontrol)
ON_COMMAND(ID_BUTTON_CAMERA_START, OnButtonCameraStart)
ON_UPDATE_COMMAND_UI(ID_BUTTON_CAMERA_START, OnUpdateButtonCameraStart)
ON_COMMAND(ID_FILE_DISPLAYCROSSHAIR, OnFileDisplaycrosshair)
ON_UPDATE_COMMAND_UI(ID_FILE_DISPLAYCROSSHAIR, OnUpdateFileDisplaycrosshair)
ON_COMMAND(ID_FILE_GRAB_AVI, OnFileGrabAvi)
ON_UPDATE_COMMAND_UI(ID_FILE_GRAB_AVI, OnUpdateFileGrabAvi)
ON_COMMAND(ID_BUTTON_START_RECORD, OnButtonStartRecord)
ON_UPDATE_COMMAND_UI(ID_BUTTON_START_RECORD, OnUpdateButtonStartRecord)
ON_COMMAND(ID_BUTTON_HISTOGRAM, OnButtonHistogram)
ON_UPDATE_COMMAND_UI(ID_BUTTON_HISTOGRAM, OnUpdateButtonHistogram)
ON_COMMAND(ID_APP_VERSION_INFO, OnAppVersionInfo)
ON_COMMAND_RANGE(ID_VIEW_TB_REQUESTEDFPS, ID_VIEW_SB_DISPLAYEDFPS, OnChangeInfoMode)

#ifndef _PGRDIST
ON_COMMAND(ID_HELP_TOC, OnHelpContents)
ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
ON_COMMAND(ID_HELP_SEARCH, OnHelpSearch)
#endif 

//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CFlyCapDoc::CFlyCapDoc() : 
   m_framerateGrab( 0.70 ), 
   m_framerateDisplay( 0.70 )   
{
   m_flyCaptureContext  = NULL;
   m_guicontext         = NULL;
   
   m_iProcessedBufferSize  = 0;
   m_CameraInfoMode = MODEL_AND_SERIAL;
   
   m_bContinueGrabThread   = false;   
   m_bViewCrosshair        = false;
   m_bNewImageSize         = true;
   m_bAviKeyWarned         = false;   
   m_heventThreadDone	   = CreateEvent( NULL, FALSE, FALSE, NULL );
   m_hMutexAVI		   = CreateMutex( NULL, FALSE, NULL );
   m_iCameraBusIndex       = -1;
   m_dSaveFrameRate        = 0.0;
   m_iJPGCompressionQuality = 85;

   m_loopmode = NONE;
 
   char pszAviSavePath[MAX_PATH];
   GetAviSavePath(pszAviSavePath);
   m_dlgRecord.m_csPath       = pszAviSavePath;

   m_dlgRecord.m_iFrames      = _DEFAULT_AVI_SAVE_FRAMES;
   m_dlgRecord.m_iLengthTime  = _DEFAULT_AVI_SAVE_TIME;
   m_dlgRecord.m_dFramerate   = 0.0;   

   m_iStreamedImages = 0;

   memset( &m_imageRaw, 0x0, sizeof( m_imageRaw ) );
   memset( &m_imageProcessed, 0x0, sizeof( m_imageProcessed ) );
   
   initBitmapStruct( _DEFAULT_WINDOW_X, _DEFAULT_WINDOW_Y );
   initHelpStrings();
}


CFlyCapDoc::~CFlyCapDoc()
{
   if( m_imageProcessed.pData != NULL )
   {
      delete [] m_imageProcessed.pData;
      m_imageProcessed.pData = NULL;
   }
   CloseHandle( m_heventThreadDone );
   CloseHandle( m_hMutexAVI );
}


void
CFlyCapDoc::initBitmapStruct( int iCols, int iRows )
{
   BITMAPINFOHEADER* pheader = &m_bitmapInfo.bmiHeader;
   
   //
   // Initialize permanent data in the bitmapinfo header.
   //
   pheader->biSize          = sizeof( BITMAPINFOHEADER );
   pheader->biPlanes        = 1;
   pheader->biCompression   = BI_RGB;
   pheader->biXPelsPerMeter = 100;
   pheader->biYPelsPerMeter = 100;
   pheader->biClrUsed       = 0;
   pheader->biClrImportant  = 0;
   
   //
   // Set a default window size.
   // 
   pheader->biWidth    = iCols;
   pheader->biHeight   = -iRows;
   pheader->biBitCount = 32;
   
   m_bitmapInfo.bmiHeader.biSizeImage = 0;
}


void
CFlyCapDoc::initHelpStrings()
{
   HKEY hkey = NULL;
   DWORD dwBufSize;
   char szInstallDir[ _MAX_HELP_STRING ];
   memset( szInstallDir, 0x0, _MAX_HELP_STRING );
   
   ::RegOpenKeyEx(
      HKEY_LOCAL_MACHINE,
      _T( "Software\\Point Grey Research, Inc.\\PGRFlyCapture\\" ),
      0,
      KEY_QUERY_VALUE | REGISTRY_HIVE,
      &hkey );

   if( hkey == NULL )
   {
      sprintf( m_szHelpPath, _DEFAULT_HELP_PATH );
      sprintf( m_szHelpPrefix, _DEFAULT_HELP_PREFIX );      
   }
   else
   {
      dwBufSize = _MAX_HELP_STRING;
      ::RegQueryValueEx( 
         hkey, 
         _T( "InstallDir" ), 
         0,
         NULL,
         (unsigned char*)szInstallDir,
         &dwBufSize );
      ::RegCloseKey( hkey );
      if( strlen( szInstallDir ) != 0 && strlen( szInstallDir ) + strlen( "doc\\FlyCapture SDK Help.chm" ) < _MAX_HELP_STRING )
      {
         sprintf( m_szHelpPath, "%sdoc\\FlyCapture SDK Help.chm", szInstallDir );
         sprintf( m_szHelpPrefix, _DEFAULT_HELP_PREFIX );
      }
      else
      {
         sprintf( m_szHelpPath, _DEFAULT_HELP_PATH );
         sprintf( m_szHelpPrefix, _DEFAULT_HELP_PREFIX );      
      }
   }   
}


void 
CFlyCapDoc::resizeProcessedImage( int iSizeBytes )
{
   if( iSizeBytes < _MIN_BGR_BUFFER_SIZE )
   {
      iSizeBytes = _MIN_BGR_BUFFER_SIZE;
   }
   
   ASSERT( iSizeBytes <= _MAX_BGR_BUFFER_SIZE );
   
   if( iSizeBytes > m_iProcessedBufferSize )
   {
      if( m_imageProcessed.pData != NULL )
      {
         delete [] m_imageProcessed.pData;
      }      
      
      m_imageProcessed.pData = new unsigned char[ iSizeBytes ];            
      memset( m_imageProcessed.pData, 0x0, iSizeBytes );  
      m_iProcessedBufferSize = iSizeBytes;
   }   
}


BOOL 
CFlyCapDoc::OnNewDocument()
{
   FlyCaptureError   error;
   CameraGUIError    guierror; 

   FlyCaptureInfoEx*	arInfo = new FlyCaptureInfoEx[64];
//   FlyCaptureInfoEx  arInfo[ 1024 ];
   unsigned int      uiCamerasOnBus = 64;
   
   if( arInfo == NULL )
   {
      return FALSE;
   }

   if( !CDocument::OnNewDocument() )
   {
      delete [] arInfo;
      return FALSE;
   }

   //
   // Have we already been grabbing?
   //
   if( m_flyCaptureContext != NULL )
   {      
      shutdown();
   }
   
   m_iCameraBusIndex = -1;   

   initBitmapStruct( _DEFAULT_WINDOW_X, _DEFAULT_WINDOW_Y );
   resizeProcessedImage( _MIN_BGR_BUFFER_SIZE );
   memset( m_imageProcessed.pData, 0x0, _MIN_BGR_BUFFER_SIZE );

   //
   // Reset the view.
   //
   POSITION pos = GetFirstViewPosition();
   while( pos != NULL )
   {
      CFlyCapView* pView = (CFlyCapView*)GetNextView( pos );      
      pView->newImageSize();            
      InvalidateRect( pView->GetSafeHwnd(), NULL, FALSE );
   }
   
   error = flycaptureCreateContext( &m_flyCaptureContext );
   _CHECK( error, "flycaptureCreateContext()", FALSE );
   
   guierror = pgrcamguiCreateContext( &m_guicontext );
   if( guierror != PGRCAMGUI_OK )
   {
      PGR_ERROR_MESSAGE( "Error creating GUI context." );
      delete [] arInfo;
      return FALSE;
   }

   //
   // Set up the help prefix to allow context sensitive help
   //
   char szHelpPrefix[ 2*_MAX_HELP_STRING ];
   sprintf( szHelpPrefix, "%s::%s", m_szHelpPath, m_szHelpPrefix );

   guierror = pgrcamguiSetSettingsWindowHelpPrefix(
      m_guicontext,
      szHelpPrefix );
   if( guierror != PGRCAMGUI_OK )
   {
      PGR_ERROR_MESSAGE( 
         "Error setting the context help prefix"
         "for the settings window." );
   }

#ifdef ENABLE_MESSAGE_LOGGING

   //
   // Enable Message Logging.
   //
   error = flycaptureSetMessageLoggingStatus( m_flyCaptureContext, true );
   _CHECK( error, "flycaptureSetMessageLoggingStatus()", FALSE );

#endif

#ifdef ALLOW_CAMERA_SELECT
   {
      //
      // Show the camera selection dialog.
      //
      unsigned long     ulSerialNumber = 0;
      INT_PTR		    ipDialogStatus;
                 
      guierror = pgrcamguiShowCameraSelectionModal(
         m_guicontext,
         m_flyCaptureContext,
         &ulSerialNumber,
         &ipDialogStatus );
      if( guierror != PGRCAMGUI_OK )
      {
         PGR_ERROR_MESSAGE( "Error showing camera selection dialog." );
      }

      //
      // Reenumerate the bus, just incase it changed during the dialog.
      //

      if( ulSerialNumber != 0 )
      {
         error = flycaptureBusEnumerateCamerasEx( arInfo, &uiCamerasOnBus );
         _CHECK( error, "flycaptureBusEnumerateCamerasEx()", FALSE );

         error = flycaptureInitializeFromSerialNumber( 
            m_flyCaptureContext, ulSerialNumber );
         _CHECK( error, "flycaptureInitializeFromSerialNumber()", FALSE  );

         //
         // Determine bus index for camera.  (should do bug 1376 instead.)
         //
         for( unsigned i = 0; i < uiCamerasOnBus; i++ )
         {
            if( arInfo[ i ].SerialNumber == ulSerialNumber )
            {
               m_iCameraBusIndex = i;
            }
         }
      }
      else
      {
         //
         // User hit 'cancel' in the dialog - this can also be checked with
         // iDialogStatus.
         //
         delete [] arInfo;
         return FALSE;
      }
   }
#elif
   {
      //
      // Just initialize the first camera on the bus.
      //
      error = flycaptureInitialize( m_flyCaptureContext, 0 );
      _CHECK( error, "flycaptureInitialize()", FALSE );
      m_iCameraBusIndex = 0;
   }
#endif
   
   //
   // Create settings dialog
   //
   guierror = pgrcamguiInitializeSettingsDialog(
      m_guicontext,
      (GenericCameraContext)m_flyCaptureContext );
   if( guierror != PGRCAMGUI_OK )
   {
      PGR_ERROR_MESSAGE( "Error creating settings dialog." );
   }

   //
   // Create the graph dialog.
   //   
   guierror = pgrcamguiCreateGraphWindow( m_guicontext );
   if( guierror != PGRCAMGUI_OK )
   {
      PGR_ERROR_MESSAGE( "Error creating graph dialog." );
   }

   //
   // Set the bus notification callback so the user-level app can know when
   // the camera is unplugged, etc.
   //
   error = flycaptureModifyCallback( 
      m_flyCaptureContext, busCallback, this, true );
   _CHECK( error, "flycaptureModifyCallback()", FALSE );
 
   
   //
   // Get camera info
   //
   error = flycaptureGetCameraInfo( m_flyCaptureContext, &m_cameraInfo );
   _CHECK( error, "flycaptureGetCameraInfo()", FALSE );

   ASSERT( m_iCameraBusIndex != -1 );

   m_pause = PAUSE_NO;
   m_bContinueGrabThread = true;   
   AfxBeginThread( threadGrabImage, this );
   
   delete [] arInfo;

   return TRUE;
}


void 
CFlyCapDoc::OnCloseDocument() 
{   
   shutdown();   
   CDocument::OnCloseDocument();         
}


void 
CFlyCapDoc::OnAppVersionInfo() 
{
   pgrcamguiShowInfoDlg( 
      m_guicontext, m_flyCaptureContext, AfxGetMainWnd()->GetSafeHwnd(), "PGR FlyCap" );
}


void 
CFlyCapDoc::getImageSize( int* piX, int* piY ) const
{
   *piX = m_bitmapInfo.bmiHeader.biWidth;
   *piY = abs( m_bitmapInfo.bmiHeader.biHeight );
}


void
CFlyCapDoc::busCallback( void* pparam, int iMessage, unsigned long ulParam )
{
   CFlyCapDoc* pDoc = (CFlyCapDoc*)pparam;
   
   ASSERT( pDoc->m_flyCaptureContext != NULL );
   
   const unsigned long ulSerialNumber = ulParam;
   
   CString csMsg;
   
   switch( iMessage )
   {
   case FLYCAPTURE_MESSAGE_BUS_RESET:
      if( ulSerialNumber != pDoc->m_cameraInfo.SerialNumber )
      {
	    ::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), FLYCAPTURE_WINDOWS_MESSAGE_BUS_RESET, NULL, NULL);
      }
      break;      
      
   case FLYCAPTURE_MESSAGE_DEVICE_ARRIVAL:
	  ::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), FLYCAPTURE_WINDOWS_MESSAGE_DEVICE_ARRIVAL, NULL, NULL);
      break;
      
   case FLYCAPTURE_MESSAGE_DEVICE_REMOVAL:
      if( ulSerialNumber == pDoc->m_cameraInfo.SerialNumber )
      {
         //
         // The camera we are currently using has been unplugged.
         //
         pDoc->m_bContinueGrabThread = false;
  	     
  	     ::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), FLYCAPTURE_WINDOWS_MESSAGE_DEVICE_REMOVAL, NULL, NULL);
         
         //
         // Bring up the camera selection dialog again.
         //
  	     ::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_COMMAND, ID_FILE_NEW, NULL);
      }
      else
      {
  	     ::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), FLYCAPTURE_WINDOWS_MESSAGE_DEVICE_REMOVAL, NULL, NULL);
      }
      break;
      
   default:
      PGR_ERROR_MESSAGE2( 
         "CFlyCapDoc::busCallback(): Software version mismatch! "
         "Please reinstall and recompile.",
         CRITICAL_MESSAGES );
      ASSERT( FALSE );
   }
}


void 
CFlyCapDoc::OnFileSaveAs() 
{
   FlyCaptureError error = FLYCAPTURE_OK;

   PauseCondition prevCondition = pauseCamera( PAUSE_YES );
   
   CFileDialog fileDialog( 
      FALSE, 
      "bmp", 
      "image", 
      OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, 
      "JPEG (*.jpg)|*.jpg|"
      "Windows Bitmap (*.bmp)|*.bmp|"
      "Portable Pixelmap (*.ppm)|*.ppm|"
      "Portable Greymap (raw image) (*.pgm)|*.pgm|"
      "Raw data (*.raw)|*.raw|"
      "All Files (*.*)|*.*||", 
      AfxGetMainWnd() );

   if( fileDialog.DoModal() == IDOK )
   {
      CString csExt = fileDialog.GetFileExt();

      if( csExt.CompareNoCase( "ppm" ) == 0 )
      {
         // Save the colour processed image.
         error = flycaptureSaveImage( 
            m_flyCaptureContext, 
            &m_imageProcessed, 
            fileDialog.GetPathName(), 
            FLYCAPTURE_FILEFORMAT_PPM );

      }
      else if( csExt.CompareNoCase( "pgm" ) == 0 )
      {
         // Save the raw image.
         error = flycaptureSaveImage( 
            m_flyCaptureContext, 
            &m_imageRaw, 
            fileDialog.GetPathName(), 
            FLYCAPTURE_FILEFORMAT_PGM );
      }
      else if( csExt.CompareNoCase( "bmp" ) == 0 )
      {
         // Save the colour processed image.
         error = flycaptureSaveImage( 
            m_flyCaptureContext, 
            &m_imageProcessed, 
            fileDialog.GetPathName(), 
            FLYCAPTURE_FILEFORMAT_BMP );
      }
      else if ( csExt.CompareNoCase( "jpg" ) == 0 )
      {
	 // Save the jpg color processed image
	 error = flycaptureSaveImage(
	    m_flyCaptureContext,
	    &m_imageProcessed,
	    fileDialog.GetPathName(),
	    FLYCAPTURE_FILEFORMAT_JPG );
      }
      else if( csExt.CompareNoCase( "raw" ) == 0 )
      {
         // Save the raw image.
         error = flycaptureSaveImage( 
            m_flyCaptureContext, 
            &m_imageRaw, 
            fileDialog.GetPathName(), 
            FLYCAPTURE_FILEFORMAT_RAW );         
      }
      else
      {
         AfxMessageBox( "Invalid file type" );
      }

      if( error != FLYCAPTURE_OK )
      {
         CString  csMsg;
         csMsg.Format(
            "Could not save image: \"%s\"", flycaptureErrorToString( error ) );
         AfxMessageBox( csMsg );
      }
   }
   
   pauseCamera( prevCondition );     
}


void
CFlyCapDoc::OnUpdateFileSaveAs( CCmdUI* pCmdUI )
{
   if( m_iCameraBusIndex < 0 )
   {
      pCmdUI->SetCheck( 0 );
      pCmdUI->Enable( FALSE );
      return;
   }
   pCmdUI->Enable( TRUE );
}


void 
CFlyCapDoc::OnViewCameracontrol() 
{
   pgrcamguiToggleSettingsWindowState(
      m_guicontext, AfxGetApp()->m_pMainWnd->GetSafeHwnd() );
}


void 
CFlyCapDoc::OnUpdateViewCameracontrol( CCmdUI* pCmdUI ) 
{
   if( m_iCameraBusIndex < 0 )
   {
      pCmdUI->SetCheck( 0 );
      pCmdUI->Enable( FALSE );
      return;
   }

   BOOL	 bShowing;
   
   pgrcamguiGetSettingsWindowState( m_guicontext, &bShowing );

   pCmdUI->Enable( TRUE );
   pCmdUI->SetCheck( bShowing ? 1 : 0 );
}


CFlyCapDoc::PauseCondition
CFlyCapDoc::pauseCamera( PauseCondition pauseCondition )
{
   unsigned long ulISOEN = 0;
   FlyCaptureError error;
   error = flycaptureGetCameraRegister(
      m_flyCaptureContext,
      0x614,
      &ulISOEN );
   //
   // If there is an error here, don't inform the user.  This could be a
   // result of a bus reset where the camera is not available.
   //
   if( error != FLYCAPTURE_OK )
   {
      return PAUSE_ERROR;
   }   

   PauseCondition prevCondition;
   if( ( ulISOEN & (0x1 << 31 ) ) != 0 )
   {
      prevCondition = PAUSE_NO;
   }
   else
   {
      prevCondition = PAUSE_YES;
   }

   if( pauseCondition == PAUSE_TOGGLE )
   {
      ulISOEN = ( ( ~( ulISOEN >> 31 ) ) << 31 ) | ( ulISOEN & 0x7FFFFFFF );
   }
   else
   {
      if( pauseCondition == PAUSE_YES )
      {
         ulISOEN = ulISOEN & 0x7FFFFFFF;
      }
      else if( pauseCondition == PAUSE_NO )
      {
         ulISOEN |= ( 0x1 << 31 );
      }
      else
      {
         return PAUSE_ERROR;
      }
   }

   error = flycaptureSetCameraRegister(
      m_flyCaptureContext,
      0x614,
      ulISOEN );
   _CHECK( error, "flycaptureSetCameraRegister()", PAUSE_ERROR );
   return prevCondition;
}


void 
CFlyCapDoc::OnButtonCameraStart() 
{
   m_pause = pauseCamera( PAUSE_TOGGLE );
}


void 
CFlyCapDoc::OnUpdateButtonCameraStart( CCmdUI* pCmdUI ) 
{  
   static unsigned int updateCount = 0;
   if( m_iCameraBusIndex < 0 )
   {
      pCmdUI->SetCheck( 0 );
      pCmdUI->Enable( FALSE );
   }
   else if( ++updateCount == 20 )
   {      
      updateCount = 0;
      pCmdUI->Enable( TRUE );

      unsigned long ulISOEN = 0;
      FlyCaptureError error;
      error = flycaptureGetCameraRegister(
         m_flyCaptureContext,
         0x614,
         &ulISOEN );
      if( error != FLYCAPTURE_OK )
         return;

      if( ( ulISOEN & (0x1 << 31 ) ) != 0 )
      {
         m_pause = PAUSE_YES;
      }
      else
      {
         m_pause = PAUSE_NO;
      }
      pCmdUI->SetCheck( ( m_pause == PAUSE_NO )? 0 : 1 );
   }
}


void 
CFlyCapDoc::OnButtonStartRecord() 
{
   if( m_loopmode == NONE || m_loopmode == FREE_RUNNING )
   {

      int iRows = m_bitmapInfo.bmiHeader.biHeight;
      int iCols = m_bitmapInfo.bmiHeader.biWidth;
      int iBPP  = 32;
      int iNumICInfo;

      iNumICInfo = m_avi.enumerateCompressors( iRows, iCols, iBPP, NULL, 0 );

      ICINFO* picinfo = new ICINFO[ iNumICInfo ];

      m_avi.enumerateCompressors( iRows, iCols, iBPP, picinfo, iNumICInfo );
      
      m_dlgRecord.setAVIMember( &m_avi );
      m_dlgRecord.m_compressorDlg.initialize( picinfo, iNumICInfo );

      if( m_dlgRecord.DoModal() == IDOK )
      {

		 bool bFileExists = false;
		 bool bPathInvalid = false;
		 CString csMessage;
		 fstream infile;


		 // Test the file path for valid directory or for existing file.
		 // Note, by testing the directory, a file is created if not present
		 // and the path is valid, therefore it should be removed.

		 infile.open(m_dlgRecord.m_csPath, ios::in);

		 // no file or bad path
		 if( infile.fail() )
		 {		    
		    infile.clear(ios::failbit);

			// test ability to open as output
   			infile.open(m_dlgRecord.m_csPath, ios_base::out);
			
			// if open, then valid path
			if( infile.is_open() )
			{
				infile.close();
				remove( m_dlgRecord.m_csPath );
			}
			// if not open invalid path
			else
			{
				bPathInvalid = true;
			}

		 }
		 else if( infile.is_open() )
		 {
			infile.close();
			bFileExists = true;
		 }


		 // If dir is invalid, let the user know.
		 if( bPathInvalid )
		 {
			csMessage.Format( 
			   "Specified path %s is invalid.\n"
			   "Please make sure directory exists.\n", 
			   m_dlgRecord.m_csPath );
		    AfxMessageBox( csMessage );
			return;
		 }

		 // If the file exists, check for overwrite.
		 if ( bFileExists )
		 {
			csMessage.Format( 
			   "File %s already exists!\n"
			   "Would you like to Overwrite?\n", 
			   m_dlgRecord.m_csPath );

			if( AfxMessageBox( csMessage, MB_YESNO ) == IDNO )
			{
				return;
			}		
		 }

		 // If we get here, the filename/path is valid
		 // Save the current path to registry for future use.
		 char* pszPathToSave = new char[MAX_PATH];
		 sprintf(pszPathToSave, m_dlgRecord.m_csPath);
		 SaveAviPath(pszPathToSave);

		 if( m_dlgRecord.m_dFramerate != 0.0 )
         {
            m_dSaveFrameRate = m_dlgRecord.m_dFramerate;    	    
         }
         else
         {
            m_dSaveFrameRate = m_framerateGrab.getFrameRate();
         }

	 m_iJPGCompressionQuality = m_dlgRecord.m_iCompressionQuality;
         
         if( !m_bAviKeyWarned )
         {
            // Drive home the fact that F9 needs to be pressed.
            AfxMessageBox( "Press F9 to start capturing." );
            m_bAviKeyWarned = true;
         }    
         
         if( m_dlgRecord.m_saveFormat == CDlgRecord::STREAM )
         {
            m_iStreamedImages = 0;
            m_loopmode = RECORD_PRE_STREAMING;
         }
         else
         {
            m_loopmode = RECORD;
         }
      }
   }
   else if( m_loopmode == RECORD_STREAMING )
   {
      _ENTER_MUTEX( m_hMutexAVI, 5000 );

	  AfxGetMainWnd()->PostMessage(FLYCAPTURE_WINDOWS_MESSAGE_STOPPED_RECORDING, NULL, NULL );

      m_avi.close();
      m_loopmode = FREE_RUNNING;

      _EXIT_MUTEX( m_hMutexAVI );
   }
   else
   {
	  AfxGetMainWnd()->PostMessage(FLYCAPTURE_WINDOWS_MESSAGE_STOPPED_RECORDING, NULL, NULL );

      m_loopmode = FREE_RUNNING;
   }
}


void 
CFlyCapDoc::OnUpdateButtonStartRecord( CCmdUI* pCmdUI ) 
{
   if( m_iCameraBusIndex < 0 )
   {
      pCmdUI->SetCheck( 0 );
      pCmdUI->Enable( FALSE );
      return;
   }

   pCmdUI->Enable( TRUE );

   switch( m_loopmode )
   {  
   case FREE_RUNNING:
   case NONE:
      pCmdUI->SetCheck( 0 );
      break;
   case RECORD_STORING:
   case RECORD:
   case RECORD_STREAMING:
   case RECORD_PRE_STREAMING:
      pCmdUI->SetCheck( 1 );
      break;
   default:
      ASSERT( FALSE );
   }      
}


void
CFlyCapDoc::OnUpdateButtonHistogram( CCmdUI* pCmdUI )
{
   if( m_iCameraBusIndex < 0 )
   {
      pCmdUI->SetCheck( 0 );
      pCmdUI->Enable( FALSE );
      return;
   }

   BOOL	 bShowing;   
   pgrcamguiGetGraphWindowState( m_guicontext, &bShowing );

   pCmdUI->Enable( TRUE );
   pCmdUI->SetCheck( bShowing ? 1 : 0 );
}


void
CFlyCapDoc::OnButtonHistogram()
{     
   pgrcamguiToggleGraphWindowState( 
      m_guicontext, 
      AfxGetApp()->m_pMainWnd->GetSafeHwnd() );
}


void 
CFlyCapDoc::OnFileDisplaycrosshair() 
{
   m_bViewCrosshair = !m_bViewCrosshair;
}


void
CFlyCapDoc::OnUpdateFileDisplaycrosshair( CCmdUI* pCmdUI )
{
   if( m_iCameraBusIndex < 0 )
   {
      pCmdUI->SetCheck( 0 );
      pCmdUI->Enable( FALSE );
      return;
   }

   pCmdUI->Enable( TRUE );
}


void 
CFlyCapDoc::OnFileGrabAvi() 
{
   if( m_loopmode == RECORD )
   {
      m_loopmode = RECORD_STORING;
   }
   else if( m_loopmode == RECORD_PRE_STREAMING )
   {
      // default BPP to use to 32, since by default we save everything as BGRU
      int ibpp = 32;
      
      // if we are using a non-stippled, mono8 image, save as an 8bit avi instead of a 32-bit.
      if ( (m_imageRaw.pixelFormat == FLYCAPTURE_MONO8) && !(m_imageRaw.bStippled) )
      {
	 ibpp = 8;
      }
      
      // If avi files are too large, they can not be played by windows media player.
      // For this reason, the avi's can be split up automatically to allow long
      // streams to be saved as multiple, number appended avi's.
      if( !m_avi.openSizeLimitedAVI( 
         m_dlgRecord.m_csPath, 
         m_imageProcessed.iCols, 
         m_imageProcessed.iRows, 
         ibpp,
         m_dSaveFrameRate ) )
      {
         CString csMsg;
         csMsg.Format( "Error opening %s.", m_dlgRecord.m_csPath );
         AfxMessageBox( csMsg );
      }
      
      m_loopmode = RECORD_STREAMING;
   }

   AfxGetMainWnd()->PostMessage( 
      FLYCAPTURE_WINDOWS_MESSAGE_RECORDING, NULL, NULL );
}


void 
CFlyCapDoc::OnUpdateFileGrabAvi( CCmdUI* pCmdUI ) 
{
   if( m_iCameraBusIndex < 0 )
   {
      pCmdUI->Enable( FALSE );
      pCmdUI->SetCheck( 0 );
      return;
   }

   pCmdUI->Enable( m_loopmode == RECORD 
      || m_loopmode == RECORD_PRE_STREAMING );
}


void 
CFlyCapDoc::shutdown()
{   
   //
   // Check to see if there is a valid camera selected.
   //
   if( m_iCameraBusIndex >= 0 )
   {
      //
      // Make sure the camera is not paused (since the grab
      // timeout defaults to infinite, the grab thread will
      // not die properly if the camera is paused.)
      //
      pauseCamera( PAUSE_NO );

      //
      // Stop the thread
      //      
      m_bContinueGrabThread   = false;      
      
      DWORD dwRet = WaitForSingleObject( m_heventThreadDone, 5000 );
      if( dwRet != WAIT_OBJECT_0 )
      {
	 // Gave up waiting... quit anyway         
      }  
   }

   BOOL bShowing = FALSE;
   pgrcamguiGetSettingsWindowState( m_guicontext, &bShowing );   
   if( bShowing == TRUE )
   {
      pgrcamguiToggleSettingsWindowState(
         m_guicontext, AfxGetApp()->m_pMainWnd->GetSafeHwnd() );
   }

   pgrcamguiGetGraphWindowState( m_guicontext, &bShowing );
   if( bShowing == TRUE )
   {
      pgrcamguiToggleGraphWindowState( 
         m_guicontext, AfxGetApp()->m_pMainWnd->GetSafeHwnd() );
   }
   
   flycaptureDestroyContext( m_flyCaptureContext );
   m_flyCaptureContext = NULL;
   
   pgrcamguiDestroyContext( m_guicontext );
   m_guicontext = NULL;   
}


void
CFlyCapDoc::addCrossHairToImage(
                                FlyCaptureImage* pimage )
{
#define CROSSHAIRRADIUS 20

   if( pimage->pixelFormat != FLYCAPTURE_BGRU )
   {
      ASSERT( false );
      return;
   }

   const int nCols = pimage->iCols;
   const int nRows = pimage->iRows;
   unsigned char* pData = pimage->pData;

   //
   // Setup the area where we are going to draw the cross hair.
   // We want it to be a single unit wide/tall if the image dimensions are
   // odd and 2 units wide if the image dimensions are an even number of units
   // wide.
   const int nHorLineStartX = nCols/2 - CROSSHAIRRADIUS;
   const int nHorLineEndX   = nCols/2 + CROSSHAIRRADIUS;
   const int nHorLineY1     = nRows/2; 
   const int nHorLineY2     = (nRows%2 == 0)?(nRows/2 + 1):(nRows/2);
   
   const int nVerLineStartY = nRows/2 - CROSSHAIRRADIUS;
   const int nVerLineEndY   = nRows/2 + CROSSHAIRRADIUS;
   const int nVerLineX1     = nCols/2; 
   const int nVerLineX2     = (nCols%2 == 0)?(nCols/2 + 1):(nCols/2);
   
   //
   // draw the horizontal line
   //
   int row;
   for(row = nHorLineY1; row<=nHorLineY2; row++)
   {
      for(int col = nHorLineStartX; col<nHorLineEndX; col++)
      {
         pData[(row*nCols+col)*4+0] = 
            (unsigned char)(pData[(row*nCols+col)*4+0]^255);
         pData[(row*nCols+col)*4+1] = 
            (unsigned char)(pData[(row*nCols+col)*4+1]^255);
         pData[(row*nCols+col)*4+2] = 
            (unsigned char)(pData[(row*nCols+col)*4+2]^255);
      }
   }
   
   //
   // draw the vertical line
   //
   for(row = nVerLineStartY; row<nVerLineEndY; row++)
   {
      for(int col = nVerLineX1; col<=nVerLineX2; col++)
      {
         pData[(row*nCols+col)*4+0] = 
            (unsigned char)(pData[(row*nCols+col)*4+0]^255);
         pData[(row*nCols+col)*4+1] = 
            (unsigned char)(pData[(row*nCols+col)*4+1]^255);
         pData[(row*nCols+col)*4+2] = 
            (unsigned char)(pData[(row*nCols+col)*4+2]^255);
      }
   }
}


void 
CFlyCapDoc::saveAVI()
{
   if( m_dlgRecord.m_saveFormat == CDlgRecord::FRAMES )
   {
      if( (signed)m_qImageBuffer.size() != 
         m_dlgRecord.m_iFrames )
      {
         ASSERT( FALSE );
         return;
      }
   }
   else if( m_dlgRecord.m_saveFormat == CDlgRecord::TIME )
   {
      if( (signed)m_qImageBuffer.size() != 
         (int)(m_dlgRecord.m_iLengthTime * m_dSaveFrameRate) )
      {
         ASSERT( FALSE );
         return;
      }
   }
   else
   {
      ASSERT( FALSE );
      return;
   }

   if( m_loopmode != RECORD_STORING )
   {
      ASSERT( FALSE );
      return;
   }

   ::PostMessage( AfxGetApp()->m_pMainWnd->GetSafeHwnd(), FLYCAPTURE_WINDOWS_MESSAGE_SAVING, NULL, NULL );

   size_t stSize = m_qImageBuffer.size();
   FlyCaptureImagePlus& image = m_qImageBuffer.at( 0 );


   const int iRawFrameSize = image.image.iCols * image.image.iRows * image.image.iNumImages;
   ASSERT( iRawFrameSize > 4 );

   unsigned char* pColour = NULL;
   int iFrame = 0;

   //PGRAviFile avi;

   // default BPP to use to 32, since by default we save everything as BGRU
   int ibpp = 32;

   // if we are using a non-stippled, mono8 image, save as an 8bit avi instead of a 32-bit.
   if ((image.image.pixelFormat == FLYCAPTURE_MONO8) && !(image.image.bStippled))
   {
      ibpp = 8;
   } 
   
   bool bError = false;
   
   // If avi files are too large, they can not be played by windows media player.
   // For this reason, the avi's can be split up automatically to allow long
   // streams to be saved.
   if ((iRawFrameSize*ibpp/8)*stSize >= AVI_FILE_SPLIT_SIZE)
   { 
      bError = m_avi.openSizeLimitedAVI( 
	 m_dlgRecord.m_csPath, 
	 image.image.iCols*image.image.iNumImages, 
	 image.image.iRows, 
	 ibpp,
	 m_dSaveFrameRate );
   } else {
      bError = m_avi.open( 
	 m_dlgRecord.m_csPath, 
	 image.image.iCols*image.image.iNumImages, 
	 image.image.iRows, 
	 ibpp,
	 m_dSaveFrameRate );
   }
   
   if( !bError )
   {
      CString csMsg;
      csMsg.Format( "Error opening %s.", m_dlgRecord.m_csPath );
      AfxMessageBox( csMsg );

      goto Error;
   }

   pColour = new unsigned char[ iRawFrameSize * 4 ];

   for( iFrame = 0; iFrame < (int)stSize; iFrame++ )
   {
      FlyCaptureImagePlus& imageSrc = m_qImageBuffer.at( iFrame );

	  // if we are using 8-bit image data (non-stippled), we do not need to convert/process the image
	  // so we just append the frame to the avi.
	  if (ibpp == 8) 
	  {
		 if( !m_avi.appendFrame( imageSrc.image.pData ) )
		 {
			CString csMsg;
			   csMsg.Format( "Error writing frame %d to %s.", iFrame, _DEFAULT_AVI_OUTPUT );
			AfxMessageBox( csMsg );            
			goto Error;
		 }
	  }
	  else 
	  {
	     FlyCaptureImage imageDest;
		 imageDest.pData = pColour;
		 imageDest.pixelFormat = FLYCAPTURE_BGRU;

		 FlyCaptureError error;
		 if( ( error = flycaptureConvertImage( 
		    m_flyCaptureContext, &imageSrc.image, &imageDest ) ) == FLYCAPTURE_OK )
		 {
			if( !m_avi.appendFrame( pColour ) )
			{
			   CString csMsg;
			      csMsg.Format( "Error writing frame %d to %s.", iFrame, _DEFAULT_AVI_OUTPUT );
			   AfxMessageBox( csMsg );            
			   goto Error;
			}
		 }
		 else
		 {
			CString csMsg;
			csMsg.Format( "Error converting frame %d.", iFrame );
			AfxMessageBox( csMsg );
			goto Error;
		 }
	  }
   }

   m_avi.close();

Error:

   if( pColour != NULL )
   {
      delete [] pColour;
      pColour = NULL;
   }

   m_qImageBuffer.clear();
   m_loopmode = RECORD;

   ::PostMessage( AfxGetApp()->m_pMainWnd->GetSafeHwnd(), FLYCAPTURE_WINDOWS_MESSAGE_STOPPED_RECORDING, NULL, NULL );

}

void
CFlyCapDoc::saveImages()
{
   FlyCaptureError error = FLYCAPTURE_OK;

   char* pszFileName = new char[ 128 ];

   // Only show the saving message if not streaming since when streaming
   // in image mode (instead of avi), each images is saved in real time.
   // In this case, we would see "Saving ..." for each frame.
   if( m_dlgRecord.m_saveFormat != CDlgRecord::STREAM )
   {
     ::PostMessage( AfxGetApp()->m_pMainWnd->GetSafeHwnd(), FLYCAPTURE_WINDOWS_MESSAGE_SAVING, NULL, NULL );
   }

   sprintf( pszFileName, m_dlgRecord.m_csPath );

   // Remove any extensions.
   char* pcExt = strstr( pszFileName, "." );
   if( pcExt != NULL )
   {
      *( pcExt ) = 0x0;
   }

   size_t stSize = m_qImageBuffer.size();
   FlyCaptureImagePlus& image = m_qImageBuffer.at( 0 );

   const int iRawFrameSize = image.image.iCols * image.image.iRows * image.image.iNumImages;
   ASSERT( iRawFrameSize > 4 );

   unsigned char* pColour = NULL;

   pColour = new unsigned char[ iRawFrameSize * 4 ];

   for( int iFrame = 0; iFrame < (int)stSize; iFrame++ )
   {
      char szTempFileName[ 128 ];

      if( m_loopmode != RECORD_STREAMING )
      {
         m_iStreamedImages = iFrame;
      }

      switch( m_dlgRecord.m_saveOutput )
      {
      case CDlgRecord::BMP:
         sprintf( szTempFileName, "%s%d%s", pszFileName, m_iStreamedImages, ".bmp" );
         break;
      case CDlgRecord::JPG:
	 sprintf( szTempFileName, "%s%d%s", pszFileName, m_iStreamedImages, ".jpg" );
	 break;
      case CDlgRecord::PPM:
         sprintf( szTempFileName, "%s%d%s", pszFileName, m_iStreamedImages, ".ppm" );
         break;
      case CDlgRecord::PGM:
         sprintf( szTempFileName, "%s%d%s", pszFileName, m_iStreamedImages, ".pgm" );
         break;
      case CDlgRecord::RAW:
         sprintf( szTempFileName, "%s%d%s", pszFileName, m_iStreamedImages, ".raw" );
         break;
      default:
         ASSERT( FALSE );
         break;
      }

      FlyCaptureImagePlus& imageSrc = m_qImageBuffer.at( iFrame );
      
      FlyCaptureImageFileFormat fileFormat = FLYCAPTURE_FILEFORMAT_BMP;

      bool bConvert = false;

      switch( m_dlgRecord.m_saveOutput )
      {
      case CDlgRecord::BMP:
         fileFormat = FLYCAPTURE_FILEFORMAT_BMP;
         bConvert = true;
         break;
      case CDlgRecord::JPG:
         fileFormat = FLYCAPTURE_FILEFORMAT_JPG;
         bConvert = true;
         break;
      case CDlgRecord::PPM:
         fileFormat = FLYCAPTURE_FILEFORMAT_PPM;
         bConvert = true;
         break;
      case CDlgRecord::PGM:
         fileFormat = FLYCAPTURE_FILEFORMAT_PGM;
         bConvert = false;
         break;
      case CDlgRecord::RAW:
         fileFormat = FLYCAPTURE_FILEFORMAT_RAW;
         bConvert = false;
         break;
      default:
         ASSERT( FALSE );
         break;
      }

      if( bConvert )
      { 
         FlyCaptureImage imageDest;
         imageDest.pData = pColour;
	 imageDest.pixelFormat = FLYCAPTURE_BGRU;
         
         if( ( error = flycaptureConvertImage( 
            m_flyCaptureContext, &imageSrc.image, &imageDest ) ) == FLYCAPTURE_OK )
         {  
	    error = flycaptureSaveImage( 
	       m_flyCaptureContext, 
	       &imageDest, 
	       szTempFileName, 
	       fileFormat );
         }
      }
      else
      {
         error = flycaptureSaveImage( 
               m_flyCaptureContext, 
               &imageSrc.image, 
               szTempFileName, 
               fileFormat );
      }
   }
   
   // Only clear the message if we are not streaming, since when
   // streaming in image mode (ie. not avi), the pictures are saved
   // real time.  When we click the record button to stop streaming,
   // the windows message is cleared.
   if( m_dlgRecord.m_saveFormat != CDlgRecord::STREAM )
   {
      ::PostMessage( AfxGetApp()->m_pMainWnd->GetSafeHwnd(), FLYCAPTURE_WINDOWS_MESSAGE_STOPPED_RECORDING, NULL, NULL );
   }

   if( error != FLYCAPTURE_OK )
   {
      CString  csMsg;
      csMsg.Format(
         "Could not save image: \"%s\"", flycaptureErrorToString( error ) );
      AfxMessageBox( csMsg );
   }

   delete [] pszFileName;

   if( pColour != NULL )
   {
      delete [] pColour;
      pColour = NULL;
   }

   m_qImageBuffer.clear();
   if( m_loopmode != RECORD_STREAMING )
   {
      m_loopmode = RECORD;
   }
   else
   {
      m_iStreamedImages++;
   }
}


void 
CFlyCapDoc::enqueueForAvi( const FlyCaptureImagePlus* pimage )
{
   ASSERT( m_loopmode == RECORD_STORING || m_loopmode == RECORD_STREAMING );

   m_qImageBuffer.push_back( *pimage );

   if( m_dlgRecord.m_saveFormat == CDlgRecord::TIME )
   {
      if( (signed)m_qImageBuffer.size() == 
         (int)(m_dlgRecord.m_iLengthTime * m_dSaveFrameRate) )
      {
         switch( m_dlgRecord.m_saveOutput )
         {
            case CDlgRecord::AVI:
               saveAVI();
               break;
            case CDlgRecord::BMP:
	    case CDlgRecord::JPG:
            case CDlgRecord::PPM:
            case CDlgRecord::PGM:
            case CDlgRecord::RAW:
               saveImages();
               break;
            default:
               ASSERT( FALSE );
               break;
         }
      }
   }
   else if( m_dlgRecord.m_saveFormat == CDlgRecord::FRAMES )
   {
      if( (signed)m_qImageBuffer.size() == 
         m_dlgRecord.m_iFrames )
      {
         switch( m_dlgRecord.m_saveOutput )
         {
            case CDlgRecord::AVI:
               saveAVI();
               break;
            case CDlgRecord::BMP:
	    case CDlgRecord::JPG:
            case CDlgRecord::PPM:
            case CDlgRecord::PGM:
            case CDlgRecord::RAW:
               saveImages();
               break;
            default:
               ASSERT( FALSE );
               break;
         }
      }
   }
   else if( m_dlgRecord.m_saveFormat == CDlgRecord::STREAM )
   {
      switch( m_dlgRecord.m_saveOutput )
      {
      case CDlgRecord::AVI:
         break;
      case CDlgRecord::BMP:
      case CDlgRecord::JPG:
      case CDlgRecord::PPM:
      case CDlgRecord::PGM:
      case CDlgRecord::RAW:
         saveImages();
         break;
      default:
         ASSERT( FALSE );
         break;
      }
   }
   else
   {
      ASSERT( FALSE );
   }
}


UINT
CFlyCapDoc::threadGrabImage( void* pparam )
{
   CFlyCapDoc* pDoc = ((CFlyCapDoc*)pparam);
   UINT uiRetval = pDoc->doGrabLoop();   
   if( uiRetval != 0 )
   {
      CString csMessage;
      csMessage.Format(
         "The grab thread has encountered a problem and had to terminate." );
      AfxMessageBox( csMessage, MB_ICONSTOP );

      //
      // Signal that the thread has died.
      //
      SetEvent( pDoc->m_heventThreadDone );      

      //
      // Bring up the camera selection dialog again
      // if we can get a pointer to the main window.
      //      
      CWinApp* theApp = AfxGetApp();
      CWnd* mainWnd;
      if( theApp )
      {
         mainWnd = theApp->m_pMainWnd;
         if( mainWnd )
            mainWnd->PostMessage( WM_COMMAND, ID_FILE_NEW, NULL );
      }
   }
   return uiRetval;
}


UINT 
CFlyCapDoc::doGrabLoop()
{
   //
   // Set up the initial states for the grab thread.
   //
   m_loopmode = FREE_RUNNING;
   LoopMode prevLoopmode = NONE;

   FlyCaptureImagePlus  imageP = { 0 };
   FlyCaptureImagePlus  imagePPrev = { 0 };    
   m_framerateGrab.setFrameRate( 10.0 );
   
   while( m_bContinueGrabThread )
   {
      //
      // Read the loop mode once per iteration so the value is
      // consistent.
      //
      LoopMode currLoopmode = m_loopmode;      
      FlyCaptureError error;

      //
      // Check for a change in the loop mode, but ignore a change to RECORD_STORING from RECORD
      //
      if( currLoopmode != prevLoopmode && !( currLoopmode == RECORD_STORING && prevLoopmode == RECORD ) )
      {  
         //
         // NONE indicates the camera is has not been started
         //
         if( prevLoopmode != NONE )
         {
            //
            // Stop the camera.
            //
            FlyCaptureError error;
            if( prevLoopmode == RECORD || prevLoopmode == RECORD_STORING )
            {
               m_qImageBuffer.clear();
               error = flycaptureUnlockAll( m_flyCaptureContext );
               _CHECK( error, "flycaptureUnlockAll()", 1 );
            }
            error = flycaptureStop( m_flyCaptureContext );
            _CHECK( error, "flycaptureStop()", 1 );
         }

         if( currLoopmode == FREE_RUNNING 
            || currLoopmode == RECORD_PRE_STREAMING )
         {
            //
            // Start the camera in lock latest.
            //

            FlyCaptureError error = flycaptureStart( 
               m_flyCaptureContext, 
               FLYCAPTURE_VIDEOMODE_ANY,
               FLYCAPTURE_FRAMERATE_ANY );
            _CHECK( error, "flycaptureStart()", 1 );
         }
         else if( currLoopmode == RECORD || currLoopmode == RECORD_STORING )
         {
            //
            // Start the camera in lock next.
            //
            FlyCaptureError error;
            
            //
            // Re-initialize.  This doesn't have any effect if we are already 
            // initialized, we just need to verify that we've got enough buffers
            // within the library.
            //
            int iSize = 0;

            if( m_dlgRecord.m_saveFormat == CDlgRecord::FRAMES )
            {
               iSize = m_dlgRecord.m_iFrames;
            }
            else if( m_dlgRecord.m_saveFormat == CDlgRecord::TIME )
            {
               iSize = (int)(m_dlgRecord.m_iLengthTime * m_dSaveFrameRate);
            }
            else
            {
               ASSERT( FALSE );
            }

            error = flycaptureInitializePlus(
               m_flyCaptureContext,
               m_iCameraBusIndex,
               iSize + 4,
               NULL );
            _CHECK( error, "flycaptureInitializePlus()", 1 );            
            error = flycaptureStartLockNext( 
               m_flyCaptureContext, 
               FLYCAPTURE_VIDEOMODE_ANY,
               FLYCAPTURE_FRAMERATE_ANY );
            _CHECK( error, "flycaptureStartLockNext()", 1 );
         }
         else if( currLoopmode == RECORD_STREAMING )
         {
            //
            // Start the camera in lock next.
            //
            FlyCaptureError error;
            
            error = flycaptureStartLockNext( 
               m_flyCaptureContext, 
               FLYCAPTURE_VIDEOMODE_ANY,
               FLYCAPTURE_FRAMERATE_ANY );
            _CHECK( error, "flycaptureStartLockNext()", 1 );
         }
      }

      prevLoopmode = currLoopmode;
      imagePPrev = imageP;

      //
      // Grab the raw image.
      //
      switch( currLoopmode )
      {
      case FREE_RUNNING:
      case RECORD_PRE_STREAMING:
		 error = flycaptureUnlockAll(m_flyCaptureContext);
         error = flycaptureLockLatest( m_flyCaptureContext, &imageP );
         break;
      case RECORD:
         error = flycaptureUnlock( 
            m_flyCaptureContext, imagePPrev.uiBufferIndex );
         error = flycaptureLockNext( m_flyCaptureContext, &imageP );
         break;
      case RECORD_STORING:
         error = flycaptureLockNext( m_flyCaptureContext, &imageP );
         if( error == FLYCAPTURE_OK )
         {
            enqueueForAvi( &imageP );
         }
         break;
      case RECORD_STREAMING:
         error = flycaptureUnlock( 
            m_flyCaptureContext, imagePPrev.uiBufferIndex );
         error = flycaptureLockNext( m_flyCaptureContext, &imageP );         
         break;
      default:
         ASSERT( FALSE );
         return 1;
      }

      //
      // Check to see if the thread should die.
      //
      if( !m_bContinueGrabThread )
      {
         break;
      }

      switch( error )
      {
      case FLYCAPTURE_NOT_STARTED:
         //
         // This is an expected error case if we are changing modes or 
         // frame rates in another thread (ie, PGRFlycaptureGUI.dll).  Wait
         // a while and then try again.
         //
         TRACE( 
            "flycaptureGrabImage2() returned an error (\"%s\") "
            "(expected case.)\n",
            flycaptureErrorToString( error ) );         
         Sleep( 50 );         
         continue;
         break;

      case FLYCAPTURE_TOO_MANY_LOCKED_BUFFERS:
         ASSERT( currLoopmode == RECORD_STORING );
	 if ( currLoopmode == RECORD_STORING )
	 {
	    AfxMessageBox( 
	       "Sorry, your computer is too slow to use the Record mode." );
	 }
         break;
      }
            
      //
      // Update current framerate.
      //
      m_framerateGrab.newFrame();
      
      FlyCaptureImage* pimage = &imageP.image;         

      //
      // Check to see if the image size changed.
      //
      if( pimage->iCols != imagePPrev.image.iCols || 
          pimage->iRows != imagePPrev.image.iRows ||
		  pimage->iNumImages != imagePPrev.image.iNumImages)
      {
         initBitmapStruct( pimage->iNumImages*pimage->iCols, pimage->iRows );       
         m_bNewImageSize = true;
      }
      
      //
      // Store the grabbed image so we can draw it later.
      //
      m_imageRaw = *pimage;
      
      //
      // Make sure the destination buffer is big enough for the resultant
      // 32 bit image.
      //
      resizeProcessedImage( pimage->iCols * pimage->iRows * 4 * pimage->iNumImages );

      //
      // We try to detect whether the View is getting behind on servicing
      // the invalidate requests we send to it.  If there is still an 
      // invalid area, don't bother colour processing this frame.
      //
      bool bSkipProcessing = false;

      POSITION pos = GetFirstViewPosition();
      while ( pos != NULL )
      {
         if( GetUpdateRect( GetNextView( pos )->GetSafeHwnd(), NULL, FALSE ) != 0 )
         {
            bSkipProcessing = true;
         }
      }

      if( !bSkipProcessing )
      {
         //
         // Do post processing on the image.
         //
         ASSERT( m_imageProcessed.pData != NULL );
         m_imageProcessed.pixelFormat = FLYCAPTURE_BGRU;
         
         error = flycaptureConvertImage( 
            m_flyCaptureContext, pimage, &m_imageProcessed ); 
	 
	 if( error != FLYCAPTURE_OK )
         {
            TRACE( 
               "flycaptureConvertImage() returned an error (\"%s\")\n",
               flycaptureErrorToString( error ) );
            continue;
         }

         //
         // Append the frame to the avi or image file if we are recording
         // in stream mode.
         //
         _ENTER_MUTEX( m_hMutexAVI, 5000 );

         if( m_loopmode == RECORD_STREAMING )
         {
            switch( m_dlgRecord.m_saveOutput )
            {
            case CDlgRecord::AVI:
			   // if the image is MONO8 and not stippled, save as an 8-bit avi.
			   // If not, we append the processed image.
			   // Note that we still need to convert above since the processed
			   // image still needs to be displayed to the screen.
               if( pimage->pixelFormat == FLYCAPTURE_MONO8 && !(pimage->bStippled) )
			   {
                  if( !m_avi.appendFrame( pimage->pData ) )
				  {
                     CString csMsg;
                     csMsg.Format( "Error writing frame to %s.", 
                        _DEFAULT_AVI_OUTPUT );
                     AfxMessageBox( csMsg );
				  }
			   } 
			   else 
			   {
                  if( !m_avi.appendFrame( m_imageProcessed.pData ) )
				  {
                     CString csMsg;
                     csMsg.Format( "Error writing frame to %s.", 
                        _DEFAULT_AVI_OUTPUT );
                     AfxMessageBox( csMsg );
				  }
			   }
               break;
            case CDlgRecord::BMP:
	    case CDlgRecord::JPG:
            case CDlgRecord::PPM:
            case CDlgRecord::PGM:
            case CDlgRecord::RAW:
               enqueueForAvi( &imageP );               
               break;
            default:
               ASSERT( FALSE );
               break;    
            }
         }

         _EXIT_MUTEX( m_hMutexAVI );

         //
         // Update the image utility if it's displayed
         //
         BOOL bGraphShowing = FALSE;
         pgrcamguiGetGraphWindowState( m_guicontext, &bGraphShowing );         
         if( bGraphShowing == TRUE )
         {
            //
            // If the original image is MONO pass that to the utility, otherwise send
            // the processed (BGRU) image.
            //
            if( pimage->pixelFormat == FLYCAPTURE_MONO8 || pimage->pixelFormat == FLYCAPTURE_MONO16 )
            {
               pgrcamguiUpdateGraphWindowImage( m_guicontext, m_flyCaptureContext, pimage );               
            }
            else
            {
               pgrcamguiUpdateGraphWindowImage( m_guicontext, m_flyCaptureContext, &m_imageProcessed );               
            }
         }
      }

      //
      // Optionally add the crosshair to the image.
      //
      if( m_bViewCrosshair )
      {         
         addCrossHairToImage( &m_imageProcessed );                           
      }
      
      //
      // Finally, notify the view that we want it to redraw our new image.
      //
      if( !bSkipProcessing )
      {
         pos = GetFirstViewPosition();
         while ( pos != NULL )
         {
            InvalidateRect( GetNextView( pos )->GetSafeHwnd(), NULL, FALSE );
         }     
      }
   }
   
   if( m_loopmode != NONE )
   {      
      //
      // Stop the camera.
      //
      FlyCaptureError error;
      if( prevLoopmode == RECORD || prevLoopmode == RECORD_STORING
         || prevLoopmode == RECORD_STREAMING 
         || prevLoopmode == RECORD_PRE_STREAMING )
      {
         m_qImageBuffer.clear();
         error = flycaptureUnlockAll( m_flyCaptureContext );
         _CHECK( error, "flycaptureUnlockAll()", 1 );
      }
      error = flycaptureStop( m_flyCaptureContext );
      _CHECK( error, "flycaptureStop()", 1 );
   }

   SetEvent( m_heventThreadDone );
   return 0;
}


float
CFlyCapDoc::getRequestedFramerate() const
{
   union {
      unsigned long ulValue;
      float fValue;
   } absRegister;

   FlyCaptureError error = flycaptureGetCameraRegister( m_flyCaptureContext, 0x83C, &absRegister.ulValue );
   if( error != FLYCAPTURE_OK || !(absRegister.ulValue & ( 0x1 << 31 ) ) )
   {
      // Unsupported or an error
      return -1.0;
   }
   
   error = flycaptureGetCameraRegister( m_flyCaptureContext, 0x968, &absRegister.ulValue );
   if( error != FLYCAPTURE_OK )
   {
      // Abs Value Unsupported or an error.
      return -1.0;
   }

   return absRegister.fValue;   
}


void
CFlyCapDoc::OnChangeInfoMode( UINT nID )
{
   CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();   
   if( pFrame != NULL )
   {
      switch( nID )
      {
      case ID_VIEW_TB_REQUESTEDFPS:         
         m_CameraInfoMode = REQUESTED_FPS;
         pFrame->GetMenu()->CheckMenuRadioItem( 
            ID_VIEW_TB_REQUESTEDFPS, 
            ID_VIEW_TB_MODELANDSERIAL,
            nID,
            MF_BYCOMMAND );
         break;
      case ID_VIEW_TB_PROCESSEDFPS:
         m_CameraInfoMode = PROCESSED_FPS;
         pFrame->GetMenu()->CheckMenuRadioItem( 
            ID_VIEW_TB_REQUESTEDFPS, 
            ID_VIEW_TB_MODELANDSERIAL,
            nID,
            MF_BYCOMMAND );
         break;
      case ID_VIEW_TB_DISPLAYEDFPS:
         m_CameraInfoMode = DISPLAYED_FPS;
         pFrame->GetMenu()->CheckMenuRadioItem( 
            ID_VIEW_TB_REQUESTEDFPS, 
            ID_VIEW_TB_MODELANDSERIAL,
            nID,
            MF_BYCOMMAND );
         break;
      case ID_VIEW_TB_MODELANDSERIAL:
         m_CameraInfoMode = MODEL_AND_SERIAL;
         pFrame->GetMenu()->CheckMenuRadioItem( 
            ID_VIEW_TB_REQUESTEDFPS, 
            ID_VIEW_TB_MODELANDSERIAL,
            nID,
            MF_BYCOMMAND );
         break;
      case ID_VIEW_SB_REQUESTEDFPS:
         pFrame->m_CameraInfoMode = REQUESTED_FPS;
         pFrame->GetMenu()->CheckMenuRadioItem( 
            ID_VIEW_SB_REQUESTEDFPS, 
            ID_VIEW_SB_DISPLAYEDFPS,
            nID,
            MF_BYCOMMAND );
         break;
      case ID_VIEW_SB_PROCESSEDFPS:
         pFrame->m_CameraInfoMode = PROCESSED_FPS;
         pFrame->GetMenu()->CheckMenuRadioItem( 
            ID_VIEW_SB_REQUESTEDFPS, 
            ID_VIEW_SB_DISPLAYEDFPS,
            nID,
            MF_BYCOMMAND );
         break;
      case ID_VIEW_SB_DISPLAYEDFPS:
         pFrame->m_CameraInfoMode = DISPLAYED_FPS;
         pFrame->GetMenu()->CheckMenuRadioItem( 
            ID_VIEW_SB_REQUESTEDFPS, 
            ID_VIEW_SB_DISPLAYEDFPS,
            nID,
            MF_BYCOMMAND );
         break;
      case ID_VIEW_SB_TIMESTAMP:
         pFrame->m_ImageInfoMode = TIMESTAMP;
         pFrame->GetMenu()->CheckMenuRadioItem( 
            ID_VIEW_SB_TIMESTAMP, 
            ID_VIEW_SB_CURSOR,
            nID,
            MF_BYCOMMAND );
         break;
      case ID_VIEW_SB_CURSOR:
         pFrame->m_ImageInfoMode = CURSOR;
         pFrame->GetMenu()->CheckMenuRadioItem( 
            ID_VIEW_SB_TIMESTAMP, 
            ID_VIEW_SB_CURSOR,
            nID,
            MF_BYCOMMAND );
         break;
      }
   }
}

void 
CFlyCapDoc::GetAviSavePath(char* pszAviSavePath)
{
   HKEY hKey;
   DWORD dwBufLen = MAX_PATH;
   long lRet;
   
   // This will not fail, since if Flycap is installed,
   // the registry key will exist.
   if (RegOpenKeyEx( 
      HKEY_LOCAL_MACHINE,
      REGISTRY_KEY_TEXT,
      0, 
      KEY_QUERY_VALUE | KEY_SET_VALUE | REGISTRY_HIVE, 
      &hKey ) == ERROR_SUCCESS)
   {

	  lRet = (RegQueryValueEx( 
		 hKey, 
		 "AviSavePath", 
		 NULL, 
		 NULL,
		 (LPBYTE)pszAviSavePath,
		 &dwBufLen ));
   
	  if (lRet != ERROR_SUCCESS) {

	     RegSetValueEx(
		 hKey, 
		 _T( "AviSavePath" ), 
		 0,
		 REG_SZ,
		 (unsigned char*)_DEFAULT_AVI_OUTPUT,
		 sizeof(_DEFAULT_AVI_OUTPUT) + 1);

		 sprintf((pszAviSavePath), _DEFAULT_AVI_OUTPUT);
	  } 

	  RegCloseKey( hKey );
   } else {
      // for the cases in-house where we may not have FlyCap installed,
      // just default to the default path.
	  sprintf((pszAviSavePath), _DEFAULT_AVI_OUTPUT);
   }
}

void 
CFlyCapDoc::SaveAviPath(char* pszPath)
{
   HKEY hKey;

   char pszPathToSave[MAX_PATH];
   size_t pathLength = 0;

   sprintf(pszPathToSave, pszPath);
   pathLength = strlen(pszPath);

   // This will not fail since if Flycap is installed, the key is present.
   if (RegOpenKeyEx( 
      HKEY_LOCAL_MACHINE,
      REGISTRY_KEY_TEXT,
      0, 
      KEY_QUERY_VALUE | KEY_SET_VALUE | REGISTRY_HIVE, 
      &hKey ) == ERROR_SUCCESS)
   {
	  // If the value does not exist, it will be created, so this
	  // should not fail.  If it does, there is nothing else we
	  // can do here.
	  RegSetValueEx(
			hKey, 
			_T( "AviSavePath" ), 
			0,
			REG_SZ,
			(unsigned char*)pszPathToSave,
			(DWORD)(pathLength + 1));

	  RegCloseKey( hKey );
   }
}


#ifndef _PGRDIST
void
CFlyCapDoc::OnHelpContents()
{
   HWND result = HtmlHelp(
      NULL,
      m_szHelpPath,
      HH_DISPLAY_TOC,
      NULL );
   if( result == NULL )
   {      
      CString csMsg;
      csMsg.Format(
         "FlyCap was unable to locate the file:\n\n"
         "%s\n\n"
         "Please make sure it exists and is readable.",
         m_szHelpPath );
      AfxMessageBox( csMsg );
   }
}


void
CFlyCapDoc::OnHelpIndex()
{
   HWND result = HtmlHelp(
      NULL,
      m_szHelpPath,
      HH_DISPLAY_INDEX,
      NULL );
   if( result == NULL )
   {      
      CString csMsg;
      csMsg.Format(
         "FlyCap was unable to locate the file:\n\n"
         "%s\n\n"
         "Please make sure it exists and is readable.",
         m_szHelpPath );
      AfxMessageBox( csMsg );
   }
}


void
CFlyCapDoc::OnHelpSearch()
{
   // Setup an empty query otherwise the search tab won't open.
   HH_FTS_QUERY query;
   memset( &query, 0x0, sizeof( HH_FTS_QUERY ) );
   query.cbStruct = sizeof( HH_FTS_QUERY );
   HWND result = HtmlHelp(
      NULL,
      m_szHelpPath,
      HH_DISPLAY_SEARCH,
      (DWORD)&query );
   if( result == NULL )
   {      
      CString csMsg;
      csMsg.Format(
         "FlyCap was unable to locate the file:\n\n"
         "%s\n\n"
         "Please make sure it exists and is readable.",
         m_szHelpPath );
      AfxMessageBox( csMsg );
   }
}
#endif
