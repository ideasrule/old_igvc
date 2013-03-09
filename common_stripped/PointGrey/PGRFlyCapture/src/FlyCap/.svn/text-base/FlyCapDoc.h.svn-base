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
// $Id: FlyCapDoc.h,v 1.75 2007/05/01 18:44:06 demos Exp $
//=============================================================================
#if !defined(AFX_FlyCAPDOC_H__01DD9404_3E30_4A92_AE4B_64B995F3FC29__INCLUDED_)
#define AFX_FlyCAPDOC_H__01DD9404_3E30_4A92_AE4B_64B995F3FC29__INCLUDED_

#pragma once

//=============================================================================
// System Includes
//=============================================================================

//
// MS's STL implementation does not compile cleanly under W4.
//
#pragma warning( push, 2 )
#include <deque>
#pragma warning( pop )
#pragma warning( disable:4663 )
#pragma warning( disable:4018 )
#pragma warning( disable:4100 )
#pragma warning( disable:4146 )
#pragma warning( disable:4244 )

#include <afxwin.h>

//=============================================================================
// PGR Includes
//=============================================================================
#include <PGRFlyCaptureStereo.h>
#include <PGRFlyCapture.h>
#include <PGRFlyCapturePlus.h>
#include <PGRFlyCaptureMessaging.h>
#include <pgrflycapturegui.h>
#include "PGRAviFile.h"

//=============================================================================
// Project Includes
//=============================================================================
#include "DlgRecord.h"
#include "FrameRate.h"
#include "MainFrm.h"

//
// Size of the window when it the application first starts.
//
#define _DEFAULT_WINDOW_X  640
#define _DEFAULT_WINDOW_Y  480

//
// Number of frames to capture when saving .avi
//
#define _DEFAULT_AVI_SAVE_FRAMES   100

//
// Number of seconds to capture when saving .avi
//
#define _DEFAULT_AVI_SAVE_TIME     10

//
// Default .AVI file to save to.
//
#define _DEFAULT_AVI_OUTPUT        "c:\\tmp\\flycap.avi"

//
// Default and maximum size of the help information
//
#define _DEFAULT_HELP_PATH          "..\\doc\\FlyCapture SDK Help.chm"

// This is the relative path prefix to the seperate HTML files from the location of
// the "FlyCapture SDK Help.chm" file mentioned above.  They was it is currently built,
// all the files are in the same directory, therefore we leave the prefix blank.
// Prefix is left in place since this can be modified for users who compile their help 
// with a different structure.
#define _DEFAULT_HELP_PREFIX        ""
#define _MAX_HELP_STRING            256


//
// Description: 
//  An enumeration of the different types of camera events.
//  Note that types 2,3, and 4 are reserved for FlyCaptureBusEvent's
//
typedef enum FlyCaptureCameraEvent
{
   // An event enumeration used when the camera is recording.
   FLYCAPTURE_MESSAGE_RECORDING = 0x05,
   // An event enumeration used when the camera is saving.
   FLYCAPTURE_MESSAGE_SAVING,
   // An event enumeration used when the camera has stopped recording/saving.
   FLYCAPTURE_MESSAGE_STOPPED_RECORDING,

} FlyCaptureCameraEvent;

//
// Description: 
//  Defining the custom windows messages which can be sent. 
//  Using the FlyCaptureBusEvent and FlyCaptureCameraEvent
//  enumeration.
//
// Remarks:
//  ulParam contains the serial number of the device.
//
 
// A windows message used to indicate a bus reset.
#define FLYCAPTURE_WINDOWS_MESSAGE_BUS_RESET \
   WM_APP + FLYCAPTURE_MESSAGE_BUS_RESET
// A windows message used to indicate a device has arrived on the bus.
#define FLYCAPTURE_WINDOWS_MESSAGE_DEVICE_ARRIVAL \
   WM_APP + FLYCAPTURE_MESSAGE_DEVICE_ARRIVAL
// A windows message used to indicate a device has been removed from the bus.
#define FLYCAPTURE_WINDOWS_MESSAGE_DEVICE_REMOVAL \
   WM_APP + FLYCAPTURE_MESSAGE_DEVICE_REMOVAL

// A windows message used to indicate the camera has started recording.
#define FLYCAPTURE_WINDOWS_MESSAGE_RECORDING \
   WM_APP + FLYCAPTURE_MESSAGE_RECORDING
// A windows message used to indicate the camera has started saving.
#define FLYCAPTURE_WINDOWS_MESSAGE_SAVING \
   WM_APP + FLYCAPTURE_MESSAGE_SAVING
// A windows message used to indicate the camera has stopped recording/saving.
#define FLYCAPTURE_WINDOWS_MESSAGE_STOPPED_RECORDING \
   WM_APP + FLYCAPTURE_MESSAGE_STOPPED_RECORDING


class CFlyCapDoc : public CDocument
{
protected:
   CFlyCapDoc();
   DECLARE_DYNCREATE( CFlyCapDoc )
      
public:

   //
   // Enum of the different modes the grab thread can be in.
   //
   enum LoopMode
   {
      // None, the grab thread doesn't exist. The camera is stopped.
      NONE,
      // Lock latest.  The normal flycap mode.
      FREE_RUNNING,
      // Record mode.  Lock next.  Every image is displayed.
      RECORD,
      // Storing images for .avi saving.
      RECORD_STORING,
      // Selected to record streaming images, waiting for user to begin.
      RECORD_PRE_STREAMING,
      // Record streaming images.  Lock next.
      RECORD_STREAMING,
   };

   //
   // Enum the different pause conditions
   //
   typedef enum PauseCondition
   {
      // Force a pause
      PAUSE_YES,
      // Force an unpause
      PAUSE_NO,
      // Toggle the current state;
      PAUSE_TOGGLE,
      // Error when pausing
      PAUSE_ERROR,
   } PauseCondition;

   // Structure used to draw to the screen.
   BITMAPINFO        m_bitmapInfo;  

   // Copy of the image structure grabbed from the SDK.
   FlyCaptureImage   m_imageRaw;

   // Processed image for displaying and saving
   FlyCaptureImage   m_imageProcessed;

   // Camera information.
   FlyCaptureInfoEx    m_cameraInfo;
   
   // Is there a new image size?
   bool m_bNewImageSize;

   // Current grab frame rate
   FrameRate   m_framerateGrab;
   
   // Current display frame rate
   FrameRate   m_framerateDisplay;

   // The type of information to display in the title bar.
   CameraInfoMode m_CameraInfoMode;

   // Returns the current image size.
   void getImageSize( int* piX, int* piY ) const;

   // Updates and returns the currently requested frame rate.
   float getRequestedFramerate() const;
   
   //
   //  Callback function that is registered to receive bus information 
   //  messages.
   //
   static void busCallback( void* pparam, int iMessage, unsigned long ulParam );

   // Add some data to the image as a post-processing step.
   static void addCrossHairToImage( FlyCaptureImage* pimage );

   //{{AFX_VIRTUAL(CFlyCapDoc)
public:
   virtual BOOL OnNewDocument();
   virtual void OnCloseDocument();
   //}}AFX_VIRTUAL
   
public:
   virtual ~CFlyCapDoc();
      
protected:

   //{{AFX_MSG(CFlyCapDoc)
   afx_msg void OnFileSaveAs();
   afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
   afx_msg void OnViewCameracontrol();
   afx_msg void OnUpdateViewCameracontrol(CCmdUI* pCmdUI);
   afx_msg void OnButtonCameraStart();
   afx_msg void OnUpdateButtonCameraStart(CCmdUI* pCmdUI);
   afx_msg void OnFileDisplaycrosshair();
   afx_msg void OnUpdateFileDisplaycrosshair(CCmdUI* pCmdUI);
   afx_msg void OnFileGrabAvi();
   afx_msg void OnUpdateFileGrabAvi(CCmdUI* pCmdUI);
   afx_msg void OnButtonStartRecord();
   afx_msg void OnUpdateButtonStartRecord(CCmdUI* pCmdUI);
   afx_msg void OnButtonHistogram();
   afx_msg void OnUpdateButtonHistogram(CCmdUI* pCmdUI);
   afx_msg void OnAppVersionInfo();
   afx_msg void OnChangeInfoMode( UINT nID );

#ifndef _PGRDIST
   afx_msg void OnHelpContents();
   afx_msg void OnHelpIndex();
   afx_msg void OnHelpSearch();
#endif

   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      
protected:

   // FlyCapture library context
   FlyCaptureContext    m_flyCaptureContext;  

   // PGRFlyCaptureGUI context
   CameraGUIContext	m_guicontext;

   // Current loopmode the program is in.
   LoopMode m_loopmode;

   PauseCondition m_pause;

   PGRAviFile m_avi;

   HANDLE m_hMutexAVI;

   // Thread flag to continue grabbing.
   bool m_bContinueGrabThread;

   // Current size of the processed image buffer
   int m_iProcessedBufferSize;

   // Whether or not crosshair should be displayed.
   bool m_bViewCrosshair;

   // JPG compression quality
   int m_iJPGCompressionQuality;

   // Event indicating that the thread is about to exit.
   HANDLE   m_heventThreadDone;

   // Queue of images that we use for saving .AVIs. 
   std::deque< FlyCaptureImagePlus > m_qImageBuffer;

   // Bus index of the current camera.  For reinitialization.
   int m_iCameraBusIndex;

   // Warned flag for reminding user they must press F9 to start .avi
   // recording.
   bool m_bAviKeyWarned;

   // Our recording dialog.  Also stores recording parameters.
   CDlgRecord m_dlgRecord;

   // Either the current grab frame rate or the frame rate entered
   // in the Record Dialog.
   double m_dSaveFrameRate;

   // The prefix and path to the context sensitive help.
   char m_szHelpPath[ _MAX_HELP_STRING ];
   char m_szHelpPrefix[ _MAX_HELP_STRING ];

   // Pause the camera
   PauseCondition pauseCamera( PauseCondition pauseCondition );

   // Initialize the bitmap struct used for drawing.
   void initBitmapStruct( int iCols, int iRows );

   // Check the registry for a path to the installed help file
   void initHelpStrings();

   // Resize the processed image buffer.
   void resizeProcessedImage( int iSizeBytes );

   // Helper function to do various shut down tasks.
   void shutdown();

   // Save the buffered images to an avi file.
   void saveAVI();

   // Save the buffered images to file.
   void saveImages();

   // Number of streamed images that have been saved.
   int  m_iStreamedImages;

   // If currently buffering images, save this image, and save the entire
   // .avi if appropriate.
   void enqueueForAvi( const FlyCaptureImagePlus* pimage );

   // The image grab thread.
   static UINT threadGrabImage( void* pparam );

   // The object grab image loop.  Only executed from within the grab thread.
   UINT doGrabLoop();

   // Get the last .avi Save Path from the registry. Returned via parameter.
   void CFlyCapDoc::GetAviSavePath(char* pszAviSavePath);

   // Save the parameter into the registry as the last .avi save path.
   void CFlyCapDoc::SaveAviPath(char* pszPath);
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_FlyCAPDOC_H__01DD9404_3E30_4A92_AE4B_64B995F3FC29__INCLUDED_)
