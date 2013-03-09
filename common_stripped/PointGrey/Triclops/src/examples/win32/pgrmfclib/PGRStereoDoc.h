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
// $Id: PGRStereoDoc.h,v 1.4 2007/10/24 01:38:13 mgriffin Exp $
//=============================================================================
#ifndef __PGRSTEREODOC_H__
#define __PGRSTEREODOC_H__

#pragma once

//=============================================================================
// System Includes
//=============================================================================
#include <afxwin.h>

//=============================================================================
// PGR Includes
//=============================================================================
#include <PGRFlycapture.h>
#include <PGRFlyCaptureStereo.h>
#include <pgrcameragui.h>

//=============================================================================
// Project Includes
//=============================================================================
#include "PGRBitmapTriclops.h"
#include "PGRStereoApp.h"
#include "PGRStereoControlDialog.h"
#include "PGRTransformDialog.h"
#include "3dPoint.h"
#include "PointList.h"


//=============================================================================
// Project Defines
//=============================================================================

//
// defines for the disparity Look-up-table
//

#define DISPARITY_LUT_SHIFT_BITS    4
#define DISPARITY_LUT_BITS	    (16 - DISPARITY_LUT_SHIFT_BITS)
#define DISPARITY_VALID_LUT_ENTRIES ( 1 << DISPARITY_LUT_BITS )
// there are a maximum of 256 invalid mappings available
#define DISPARITY_INVALID_LUT_ENTRIES 256


//=======================================================================
// Enums
//
//  We loop through these emums on occasion, so the order within
//  the separate groups is important.
//=======================================================================

typedef enum 
{
   IMAGE_RIGHT_RAW = 0,
   IMAGE_CENTER_RAW,
   IMAGE_LEFT_RAW,
   IMAGE_STEREO_RAW,
   
   // Note: IMAGE_STEREO_RAW is only a placeholder for the stereo images
   // and not to be used elsewhere.  Because of this, we are setting
   // IMAGE_RIGHT_RECTIFIED to the same value to make sure we do not have
   // to many entries in the array.  IMAGE_STEREO_RAW can be ignored for
   // all purposes but the m_tiRawColorImages array.
   IMAGE_RIGHT_RECTIFIED = 3,
   IMAGE_CENTER_RECTIFIED,
   IMAGE_LEFT_RECTIFIED,

   IMAGE_RIGHT_COLOR_RECTIFIED,
   IMAGE_CENTER_COLOR_RECTIFIED,
   IMAGE_LEFT_COLOR_RECTIFIED,
   
   IMAGE_RIGHT_EDGE,
   IMAGE_CENTER_EDGE,
   IMAGE_LEFT_EDGE,
   
   IMAGE_DISPARITY,
   
   IMAGE_TOTAL // number of different types of images.

} enum_IMAGETYPE;


/**
 * The resolutions that this structure supports need to have a separate enum
 * here because it is used to reference into an array of bitmaps, etc.
 * Otherwise we would just use the normal format enum.
 */
typedef enum
{
   RES_160x120 = 0,
   RES_256x192,
   RES_320x240,
   RES_400x300,
   RES_512x384,
   RES_640x480,
   RES_800x600,
   RES_1024x768,
   RES_1280x960,

   /**
    * Number of set, supported resolutions.  This is used to size arrays and 
    * iterate so make sure it's here and last.
    */
   RES_TOTAL

} enum_RESOLUTION;


/**
 * This class defines the Document part of the PGRMFC structure.  It inherits
 * from the MFC CDocument and represents a coupled flycapture stereo camera and
 * triclops lib pair.  It handles grabbing, processing, and keeping track
 * of the images used by PGRImageView and the pointclouds used by 
 * PGROpenGLView.
 */
class CPGRStereoDoc : public CDocument  
{
protected:

   CPGRStereoDoc();
   DECLARE_DYNCREATE(CPGRStereoDoc)

   virtual ~CPGRStereoDoc();


public:

   /**
    * Grab an image and extracts the triclopsInput structure this step must be 
    * done before processFrame(). (in App::OnIdle()).
    */
   virtual BOOL	 flycaptureGrab();

   /**
    * Do the rest of the processing on the frame: extract raw images, do 
    * stereo, point clouds, etc etc..  this is called after flycaptureGrab() 
    * in App::OnIdle().
    */
   virtual BOOL	 processFrame();

   /** Returns the current image. */
   PGRBitmapTriclops* getBitmap( enum_IMAGETYPE imagetype );

   /** Are we using a colour camera? */
   BOOL hasColorData();

   /** Returns the current calculated pointcloud. */
   CPointList* getPointCloud();

   //PGRColourPoint3dRC* getPointArray();
   /** Returns the current calculated point array. */
   C3dColourPointRC* getPointArray();

   //PGRTransform3d* getTransform();
   /** Returns a pointer to the document's PGRTransform3D. */
   CTransformD* getTransform();

   /** Returns the state of the stereo dialog (whether it is visible or not.) */
   virtual  BOOL  isStereoDialogActive();

   /** Returns the state of the transform dialog (whether it is visible or not.) */
   virtual  BOOL  isTransformDialogActive();

   /** Returns the state of the drawing points flag. */
   virtual  BOOL  drawingPoints();

   /** Returns the state of the drawing colour points flag. */
   virtual  BOOL  drawingColorPoints();

   /** Returns the state of the drawing triangles flag. */
   virtual  BOOL  drawingTriangles();

   /** Returns the state of the drawing texture map flag. */
   virtual  BOOL  drawingTexture();

   /** Returns the current size of the points. */
   float getPointSize();

   /** Returns the current resolution. */
   StereoImageResolution getResolution();

   /** Returns the current resolution in cols and rows */
   void getResolution(StereoImageResolution resType, int* iRows, int* iCols);

   /** Returns the maximum resolution */
   StereoImageResolution getMaxResolution();

   /** Is camera a Bumblebee? */
   BOOL isBumblebee();

   /** Returns stereo data for a given row, column point. */
   virtual BOOL	 getPointData( 
      int iRow, int iCol, double* dDisparity, double* dX, double* dY, double* dZ );

   /** Sets the point of rotation in the OpenGL Views. */
   virtual bool updatePointOfRotation( double dX, double dY, double dZ );

   /** Set whether this document is the active one or not in the MDI */
   virtual void setDocumentActive( BOOL bActive );

   /** Return whether this document is the one with the active view in the MDI */
   virtual BOOL isDocumentActive();

   /** Returns whether the indicated serial number is in use by this document. */
   virtual BOOL isCameraAlreadyOpen( FlyCaptureCameraSerialNumber serial );

   /** Decrement number of color rectified views */
   void decrementColorRectifiedViews();
   void incrementColorRectifiedViews();
   
   /** The one and only Triclops context. */
   TriclopsContext  m_triclopsContext;


public:
   virtual BOOL InitDocument();
   virtual BOOL ReInitDocument();
   virtual void OnCloseDocument();
   BOOL	 isOffline();

   
protected:

   // Functions hooked to stubs in CDemoIIDoc class
   void OnFileClose();
   BOOL OnFileNewCameraDocument();
   BOOL OnFileNewOfflineDocument();
   void OnFileLoadCal();
   BOOL OnFileLoadStereoImage();
   void OnFileSaveCurrCal();
   void OnFileSaveDefaultCal();
   void OnFileSaveDisparity();
   void OnFileSaveEdgeLeft();
   void OnFileSaveEdgeRight();
   void OnFileSaveEdgeCenter();
   void OnFileSavePointcloud();
   void OnFileSaveRawLeft();
   void OnFileSaveRawRight();
   void OnFileSaveRawStereo();
   void OnFileSaveRawCenter();
   void OnFileSaveRectifiedColorLeft();
   void OnFileSaveRectifiedColorRight();
   void OnFileSaveRectifiedColorCenter();
   void OnFileSaveRectifiedLeft();
   void OnFileSaveRectifiedRight();
   void OnFileSaveRectifiedCenter();
   void OnFileSaveStereo();
   void OnAppVersionInfo();

   //{{AFX_MSG(CPGRStereoDoc)
   afx_msg void OnButtonStereoParams();
   afx_msg void OnCheckCameraControl();
   afx_msg void OnUpdateCheckCameraControl(CCmdUI* pCmdUI);
   afx_msg void OnButtonTransformation();
   afx_msg void OnComboResolution();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
      
protected:

   /** Closes the camera and destroy all relevant contexts. */
   virtual void	closeGrab();

   /** Initialize the camera.  Returns success or fail. */
   virtual BOOL	initGrab();

   virtual void initTriclopsContext();

   virtual void destroyTriclopsContext();
				      

   /**  */
   virtual void	initializeBitmaps();

   /** Resizes bitmap and triclops buffers when the image resolution changes. */
   virtual BOOL	resetImageResolutions( enum_RESOLUTION resolution );   

   /**
    * Parses through the list of views of this document and resizes the 
    * PGRImageView views.  This happens when the resolution changes, for 
    * example.
    */
   virtual void	resizeAllViews();

   /**
    * Copy the 16 bit disparity image from subpixel stereo into an 8 bit 
    * displayable image.
    */
   virtual void remapDisparityImage();

   /**
    * Copy out the unpacked colour data into a packed 32 bit image for 
    * displaying.
    */
   virtual void	processColorData();

   virtual void processRectifiedEdgeImages();

   /** Compute the pointcloud based on the current stereo image. */
   virtual void	computePoints();

   /** Extract the raw images directly from the library. */
   virtual void	extractRawStereoImages();

   /** Extract the raw images from a TriclopsInput - usually loaded from
    *  a file
    */
   virtual void extractRawImagesFromTriclopsInput( TriclopsInput input );

   /** The main stereo step that calls the rest of these helper functions. */
   virtual void	doStereo();

   /**
    * Update statistics such as frame rate, and update the main window
    * status bar.
    */
   virtual void	doFrameStats();

   /** Overridable method called at the end of every call to processFrame(). */
   virtual void	customProcess();

   /** Overridable method called at the end of OnNewDocument(). */
   virtual void	customInit();

   /** Save a specified image. */
   void saveRawImage( const char* szDefaultFilename );

   /** 
    * Save the calibration file.  bCurrent determines whether it is the
    * "current" calibration settings as modified through the stereo 
    * stereo parameter dialog (true) or the default calibration file as
    * saved on the camera (false)
    */
   void saveCalAs( bool bCurrent );


protected:

   /**
    * A flag that determines whether the application is grabbing from a
    * camera or if it is simply working from loaded image and calibration
    * files.  
    */
   BOOL	 m_bOffline;

   bool	 m_bImageAvailable;

   BOOL	 m_bDocumentActive;

   /**
    * The flycapture context for this document.  Potentially we could have
    * multiple documents and multiple cameras used within a single 
    * application.
    */
   FlyCaptureContext m_flycaptureContext;

   /** A FlyCaptureInfoEx structure describing the camera that we're using. */
   FlyCaptureInfoEx m_flycaptureInfo;

   /** FlycaptureImage structure */
   FlyCaptureImage m_flycaptureImage;

   /** Current rows */
   int m_nCurrRows;

   /** Current cols */
   int m_nCurrCols;

   /** Max rows for the current camera. */
   int m_nMaxRows;

   /** Max cols for the current camera. */
   int m_nMaxCols;

   /** Current stereo resolution */
   StereoImageResolution m_currentStereoResolution;

   /** Maximum stereo resolution */
   StereoImageResolution m_maxStereoResolution;

   /** The serial number of the camera system */
   FlyCaptureCameraSerialNumber      m_serialNumber;

   /** Frame rate we are currently _processing_ at. */
   double m_dFrameRate;

   /**
    * Maximum number of points in the pointcloud - defined as rows * cols since
    * that is the upper bound.
    */
   int m_nMaximumPoints;  

   /** The current pointcloud. */
   CPointList* m_pPointCloud;

   /**
    * List index in application for this pointcloud - for removing it when this
    * document closes.
    */
   int m_iPointCloudListIndex;

   /**
    * A static array of points that can be redefined each frame and inserted
    * as pointers into the pointcloud.  So we don't have to reallocate them
    * for each frame.
    */
   C3dColourPointRC* m_pStaticPointArray;

   /** An array of all the bitmap objects we use. */
   PGRBitmapTriclops*  m_arpBitmap[ IMAGE_TOTAL ];

   /** Has the camera been initialized? */
   BOOL	 m_bCameraInitialized;

   /** Is this camera a Bumblebee? */
   BOOL	 m_bIsBumblebee;
      
   /** Colour triclops input for the current frame. */
   TriclopsInput  m_tiRawColorImages[4];
   
   /** Stereo triclops input for the current frame. */
   TriclopsInput  m_tiStereo;

   /** General Triclops input */
   TriclopsInput m_tiGeneral;
   
   /** Do we have colour data? */
   BOOL	m_bColorData;

   /** Are any color rectification views up?
    *  Rectifying color views does not come for free, so we want to
    *  only rectify them if they are required.
    *  This is a counter to tell us how many color rectify views are
    *  open for this document.
    */
   int m_nRectifyColorViews;

   /** The stereo control dialog object. */
   CPGRStereoControlDialog m_dialogStereo;

   /** Whether or not the dialog is currently visible. */
   BOOL m_bStereoDialogActive;

   /** The transform dialog. */
   CPGRTransformDialog m_dialogTransform;

   /** Whether or not the dialog is currently visible. */
   BOOL m_bTransformDialogActive;

   /** Context for the FlycaptureGUI library. */
   CameraGUIContext m_guicontext;

   /** The raw image buffers */
   unsigned char* m_pRedRaw;
   unsigned char* m_pGreenRaw;
   unsigned char* m_pBlueRaw;

   /** The 3 image buffers for building Triclops input from an image */
   unsigned char* m_pRedProc;
   unsigned char* m_pGreenProc;
   unsigned char* m_pBlueProc;

private:

   /**
    * The look up table for valid disparity values.
    * 
    * @see remapDisparityImage()
    * @see generateLookupTable()
    */
   unsigned char m_ucValidDisparityMapLUT[ DISPARITY_VALID_LUT_ENTRIES ][3];

   /**
    * The look up table for invalid disparity values 
    * 
    * @see remapDisparityImage()
    * @see generateLookupTable()
    */
   unsigned char m_ucInvalidDisparityMapLUT[ DISPARITY_INVALID_LUT_ENTRIES ][3];

   /** Look up table min disparity. */
   int m_nLUTMinDisp;

   /** Look up table max disparity. */
   int m_nLUTMaxDisp;

   /** Checks and generates the disparity look up table. */ 
   void generateLookupTables();

   /** Get the product and serial name for this camera
    *  Buffer needs to have enough room (25+)
    */
   void getCameraTitle( char* buffer, const bool bTerse = false );

   /** Load a TriclopsContext from a .cal file selected by the user
    *  This pops up the file selection dialog.
    */
   bool loadTriclopsContextFromFile( TriclopsContext* pTriclops );
};


//{{AFX_INSERT_LOCATION}}

#endif // #ifndef __PGRSTEREODOC_H__
