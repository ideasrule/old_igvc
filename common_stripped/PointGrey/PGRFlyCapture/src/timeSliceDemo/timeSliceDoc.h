//=============================================================================
// Copyright © 2001 Point Grey Research, Inc. All Rights Reserved.
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
//
// Digiclops® is a registered Trademark of Point Grey Research Inc.
//=============================================================================
//=============================================================================
// $Id: timeSliceDoc.h,v 1.25 2006/05/05 20:11:38 marko Exp $
//=============================================================================
#if !defined(AFX_TIMESLICEDOC_H__BC7641D8_84FA_4BA8_94B4_E4914B03C495__INCLUDED_)
#define AFX_TIMESLICEDOC_H__BC7641D8_84FA_4BA8_94B4_E4914B03C495__INCLUDED_

//=============================================================================
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================

//
// MS's STL implementation does not compile cleanly under W4.
//
#pragma warning( push, 2 )
#include <deque>
#pragma warning( pop )

//=============================================================================
// PGR Includes
//=============================================================================
#include <pgrflycapture.h>
#include <pgrflycaptureplus.h>
#include <pgrcameragui.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "DialogScrollMode.h"

//=============================================================================
// Defines
//=============================================================================

//
// Maximum number of cameras on the bus.
//
#define MAX_CAMERAS	32

//
// Number of low-level grab buffers (per camera)
//
#define NUM_BUFFERS     4

//
// Use this colour processing method by default.
//
#define COLOUR_PROCESSING_METHOD FLYCAPTURE_NEAREST_NEIGHBOR_FAST

//
// Tolerance to use when comparing cycle times to see if the images are 
// actually synchronized.
//
#define SYNC_TOLERANCE  5

//
// Count and report out of sync images.
//
#define COUNT_OUT_OF_SYNC

//
// Number of buffers to use per camera for scroll mode.  Half of these buffers
// will be before the timeslice even, and half after.
//
#define SCROLL_MODE_BUFFERS   40

//
// The maximum size of the static RGB buffer.  We won't attempt to dynamically
// resize above this.
//
#define MAX_BGR_BUFFER_SIZE  5000 * 5000 * 4

//
// The minimum size of the BGR image buffer.
//
#define MIN_BGR_BUFFER_SIZE  640 * 480 * 4

//
// The maximum amount of bytes available on a bus (less 10 percent).
//
#define MAX_BANDWIDTH_BYTES  4915 * 0.9

//
// The maximum amount of 1394 cards currently installed.
//
#define MAX_NUM_1394_CARDS   10



class CTimeSliceDoc : public CDocument
{
protected:
   CTimeSliceDoc();
   DECLARE_DYNCREATE(CTimeSliceDoc)      
      
public:

   //
   // Structure for holding information for one camera.
   //
   class CameraInfo
   {
   public:
      //
      // Context for this camera
      //
      FlyCaptureContext	m_context;

      //
      // Member for storing the error for the grab call for this
      // camera.  Checked on the post-processing step.
      //
      FlyCaptureError	m_error;

      //
      // GUI Context for this camera's parameters
      //
      CameraGUIContext	m_guicontext;

      //
      // Info structure for this camera
      //
      FlyCaptureInfoEx	m_info;

      //
      // Current image grabbed by this camera.
      //
      FlyCaptureImagePlus  m_image;

      //
      // Current processed image grabbed by this camera.
      //
      FlyCaptureImage  m_imageProcessed;

      //
      // Sequence number from the previous grab - to check if we are missing 
      // images.
      //
      unsigned int      m_uiPrevSeqNum;

      //
      // 32-bit bitmap info for drawing the 32 bit BGR colour image, when
      // approprate.
      //
      BITMAPINFO*	m_pbitmapinfo32;

      //
      // Vertical offset tweak for this camera's display
      //
      int		m_iVerticalTweak;

      //
      // Buffer that will hold the "timeslice" image - this is the image that is
      // captured and then panned though by one of the views.
      //
      unsigned char*	m_pTimesliceImageBuffer;
   };

   //
   // Array of CameraInfo structures, one for each camera on the bus.
   //
   CameraInfo     m_arCameraInfo[ MAX_CAMERAS ];
   unsigned int   m_uiNumCameras;

   //
   // Camera info struct for time slice view to draw .
   // (including a valid timeslice image buffer)
   //
   CameraInfo  m_cameraInfoTimeSlice;

   //
   // Buffer space for extra images
   //
   unsigned char**   m_arpImageBuffers;

   //
   // Dynamic configuration data - these values are dependent on the frame
   // rate (15 Hz or 30Hz.)
   //
   int	 m_iFramesPerCameraPanning;
   int	 m_iFramesForEndCamera;
   int	 m_iFramesAtEnd;
   int	 m_iNumberOfPanCycles;
   int	 m_iExtraBuffers;

   //
   // Circular buffering of initial raw data before the grab event so that
   // we can save it later, if desired.  We use two circular buffers so that 
   // we don't overwrite the one we're about to save to an .avi.
   //
   int		     m_iInitialBufferedImages;

   unsigned char**   m_arpInitialBuffers[ 2 ];
   int		     m_iInitialBufferIndex;

   std::deque< CameraInfo > m_listSavingCameraInfo;

   //
   // Whether or not we're drawing the alignment cross or not
   //
   bool	 m_bDrawAlignCross;

   //
   // The location of the align cross to draw.
   //
   CPoint   m_pointAlignCross;

   //
   // Scroll mode variables.
   //
   unsigned char***  m_arpScrollModeBuffers;
   int               m_iScrollModeCamera;
   int               m_iScrollModeTimeSliceBuffer;
   int               m_iScrollModeLoc;
   bool              m_bScrollMode;
   DialogScrollMode  m_dialogScrollMode;

   //
   // View types
   //
   enum ViewType
   {
      SINGLE_CAMERA,
      PANNING
   };

   double   m_dFrameRate;
   
   //
   // Debug counters for missed and out of sync images.
   //
   unsigned int   m_uiMissedImages;
   unsigned int   m_uiOutOfSyncImages;

   
   void initAllViews();

   //
   // Called by the view when the space bar is hit.
   //
   void spaceBarEvent();
   
   void scrollModifyCamera( int iMod );
   void scrollModifyFrame( int iMod );
   
   //{{AFX_VIRTUAL(CTimeSliceDoc)
   public:
      virtual BOOL OnNewDocument();
      virtual void Serialize(CArchive& ar);
      virtual void OnCloseDocument();
   //}}AFX_VIRTUAL
      
   public:
      virtual ~CTimeSliceDoc();
      
      
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif
   
   
protected:
   //{{AFX_MSG(CTimeSliceDoc)
   afx_msg void OnConfig();
   afx_msg void OnEditSaveAnimation();
   afx_msg void OnButtonScrollMode();
   afx_msg void OnUpdateButtonScrollMode(CCmdUI* pCmdUI);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      

protected:

   static void threadGrab( void* pparam );

   void grabLoop();

   void updateViews() const;   
   BOOL startSync();
   void grabAndCheckSync();
   void updateBitmaps();
   void mostNumberOfCamerasOnASingleBus();

   //
   // State of panning
   //
   enum TimeSliceState
   {
      // Normal running state
      RUNNING,

      // Grabbing a single timeslice - a single image from all cameras.
      GRABBING_TIMESLICE,  

      // Grabbing images that will appear after the timeslice.
      GRABBING_DELAYED,

      // Displaying those images.
      DISPLAYING_DELAYED,

      // Displaying the entire saved sequence.
      DISPLAYING_TIMESLICE,

      // Grabbing the other half of the scrollmode buffers.
      SCROLLMODE_GRABBING,

      // In scroll mode and scrolling around with the dialog.
      SCROLLMODE_SCROLLING,
   };


   bool	    m_bContinueGrab;

   HANDLE   m_eventGrabFinished;

   int	    m_iTotalExtraImageBuffers;

   int      m_iMostNumberOfCameras;

   int      m_iBufferSize;

   void initTempImages();
   void initProcessedImages();
   void initBitmapInfo();
   void resizeExtraImageBuffers( int iSizeBytes );
   void dealocateExtraImageBuffers();

   void	 readTweakValues();
   void	 writeTweakValues();

   void allocateScrollModeBuffers( int iSizeBytes );
   void deallocateScrollModeBuffers();
   bool checkScrollModeCompat();

   //
   // Event to trigger the timeslice grab
   //
   bool           m_bSliceEvent;
   TimeSliceState m_stateSlice;
};


//{{AFX_INSERT_LOCATION}}



#endif // !defined(AFX_TIMESLICEDOC_H__BC7641D8_84FA_4BA8_94B4_E4914B03C495__INCLUDED_)
