//=============================================================================
// Copyright © 2006 Point Grey Research, Inc. All Rights Reserved.
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
// $Id: PGRFlyCaptureTestCSharpModule.cs,v 1.2 2007/06/15 00:00:51 demos Exp $
//=============================================================================

// This file demonstrates how to adapt enumerations and structures from
// PGRFlyCapture.h and use them in a C# .NET environment.

using System.Runtime.InteropServices;

//
// Description:
//   Enum describing different framerates.
//   (adapted from PGRFlyCapture.h)
public enum FlyCaptureFrameRate
{
	// 1.875 fps. (Frames per second)
	FLYCAPTURE_FRAMERATE_1_875,
	// 3.75 fps.
	FLYCAPTURE_FRAMERATE_3_75,
	// 7.5 fps.
	FLYCAPTURE_FRAMERATE_7_5,
	// 15 fps.
	FLYCAPTURE_FRAMERATE_15,
	// 30 fps.
	FLYCAPTURE_FRAMERATE_30,
	// Deprecated.  Please use Custom image.
	FLYCAPTURE_FRAMERATE_50,
	// 60 fps.
	FLYCAPTURE_FRAMERATE_60,
	// 120 fps.
	FLYCAPTURE_FRAMERATE_120,
	// 240 fps.
	FLYCAPTURE_FRAMERATE_240,
	// Number of possible camera frame rates.
	FLYCAPTURE_NUM_FRAMERATES,
	// Custom frame rate.  Used with custom image size functionality.
	FLYCAPTURE_FRAMERATE_CUSTOM,
	// Hook for "any usable frame rate."
	FLYCAPTURE_FRAMERATE_ANY,
}

//
// Description:
//   Enum describing different video modes.
//
// Remarks:
//   The explicit numbering is to provide downward compatibility for this enum.
//   (adapted from PGRFlyCapture.h)
enum FlyCaptureVideoMode
{
	// 160x120 YUV444.
	FLYCAPTURE_VIDEOMODE_160x120YUV444     = 0,
	// 320x240 YUV422.
	FLYCAPTURE_VIDEOMODE_320x240YUV422     = 1,
	// 640x480 YUV411.
	FLYCAPTURE_VIDEOMODE_640x480YUV411     = 2,
	// 640x480 YUV422.
	FLYCAPTURE_VIDEOMODE_640x480YUV422     = 3,
	// 640x480 24-bit RGB.
	FLYCAPTURE_VIDEOMODE_640x480RGB        = 4,
	// 640x480 8-bit greyscale or bayer tiled color image.
	FLYCAPTURE_VIDEOMODE_640x480Y8         = 5,
	// 640x480 16-bit greyscale or bayer tiled color image.
	FLYCAPTURE_VIDEOMODE_640x480Y16        = 6,
	// 800x600 YUV422.
	FLYCAPTURE_VIDEOMODE_800x600YUV422     = 17,
	// 800x600 RGB.
	FLYCAPTURE_VIDEOMODE_800x600RGB        = 18,
	// 800x600 8-bit greyscale or bayer tiled color image.
	FLYCAPTURE_VIDEOMODE_800x600Y8         = 7,
	// 800x600 16-bit greyscale or bayer tiled color image.
	FLYCAPTURE_VIDEOMODE_800x600Y16        = 19,
	// 1024x768 YUV422.
	FLYCAPTURE_VIDEOMODE_1024x768YUV422    = 20,
	// 1024x768 RGB.
	FLYCAPTURE_VIDEOMODE_1024x768RGB       = 21,
	// 1024x768 8-bit greyscale or bayer tiled color image.
	FLYCAPTURE_VIDEOMODE_1024x768Y8        = 8,
	// 1024x768 16-bit greyscale or bayer tiled color image.
	FLYCAPTURE_VIDEOMODE_1024x768Y16       = 9,
	// 1280x960 YUV422.
	FLYCAPTURE_VIDEOMODE_1280x960YUV422    = 22,
	// 1280x960 RGB.
	FLYCAPTURE_VIDEOMODE_1280x960RGB       = 23,
	// 1280x960 8-bit greyscale or bayer titled color image.
	FLYCAPTURE_VIDEOMODE_1280x960Y8        = 10,
	// 1280x960 16-bit greyscale or bayer titled color image.
	FLYCAPTURE_VIDEOMODE_1280x960Y16       = 24,
	// 1600x1200 YUV422.
	FLYCAPTURE_VIDEOMODE_1600x1200YUV422   = 50,
	// 1600x1200 RGB.
	FLYCAPTURE_VIDEOMODE_1600x1200RGB      = 51,
	// 1600x1200 8-bit greyscale or bayer titled color image.
	FLYCAPTURE_VIDEOMODE_1600x1200Y8       = 11,
	// 1600x1200 16-bit greyscale or bayer titled color image.
	FLYCAPTURE_VIDEOMODE_1600x1200Y16      = 52,
   
	// Custom video mode.  Used with custom image size functionality.
	FLYCAPTURE_VIDEOMODE_CUSTOM            = 15,
	// Hook for "any usable video mode."
	FLYCAPTURE_VIDEOMODE_ANY               = 16,

	// Number of possible video modes.
	FLYCAPTURE_NUM_VIDEOMODES              = 23,
}

//
// Description: 
//  An enumeration used to describe the different camera models that can be 
//  accessed through this SDK.
//  (adapted from PGRFlyCapture.h)
public enum FlyCaptureCameraModel
{
	FLYCAPTURE_FIREFLY,
	FLYCAPTURE_DRAGONFLY,
	FLYCAPTURE_AIM,
	FLYCAPTURE_SCORPION,
	FLYCAPTURE_TYPHOON,
	FLYCAPTURE_FLEA,
	FLYCAPTURE_DRAGONFLY_EXPRESS,
	FLYCAPTURE_UNKNOWN = -1,

}

//
// Description: 
//  An enumeration used to describe the different camera color configurations.
//  (adapted from PGRFlyCapture.h)
public enum FlyCaptureCameraType
{
	// black and white system.
	FLYCAPTURE_BLACK_AND_WHITE,
	// color system.
	FLYCAPTURE_COLOR
}

//
// Description:
//   Enumerates the image file formats that flycaptureSaveImage() can write to.
//   (adapted from PGRFlyCapture.h)
public enum FlyCaptureImageFileFormat
{
	// Single channel (8 or 16 bit) greyscale portable grey map.
	FLYCAPTURE_FILEFORMAT_PGM,
	// 3 channel RGB portable pixel map.
	FLYCAPTURE_FILEFORMAT_PPM,
	// 3 or 4 channel RGB windows bitmap.
	FLYCAPTURE_FILEFORMAT_BMP,
	// JPEG format.  Not implemented.
	FLYCAPTURE_FILEFORMAT_JPG,
	// Portable Network Graphics format.  Not implemented.
	FLYCAPTURE_FILEFORMAT_PNG,
	// Raw data output.
	FLYCAPTURE_FILEFORMAT_RAW
}

//
// Description: 
//  Camera information structure.  This structure will eventually be replaced
//  by FlyCaptureInfoEx.
//  (adapted from PGRFlyCapture.h)
public struct FlyCaptureInfo
{
	// camera serial number.
	public int SerialNumber;
	// type of CCD (color or b&w).
	public FlyCaptureCameraType CameraType;
	// Camera model.
	public FlyCaptureCameraModel CameraModel;
	// Null-terminated camera model string for attached camera.
	//public string pszModelString;
	[MarshalAs(UnmanagedType.ByValTStr, SizeConst=512)]
	public string pszModelString;
}

//
// Description:
//   An enumeration used to indicate the type of the returned pixels.  This
//   enumeration is used as a member of FlyCaptureImage and as a parameter
//   to FlyCaptureStartCustomImage.
//   (adapted from PGRFlyCapture.h)
public enum FlyCapturePixelFormat
{
	// 8 bits of mono information.
	FLYCAPTURE_MONO8     = 0x00000001,
	// YUV 4:1:1.
	FLYCAPTURE_411YUV8   = 0x00000002,
	// YUV 4:2:2.
	FLYCAPTURE_422YUV8   = 0x00000004,
	// YUV 4:4:4.
	FLYCAPTURE_444YUV8   = 0x00000008,
	// R = G = B = 8 bits.
	FLYCAPTURE_RGB8      = 0x00000010,
	// 16 bits of mono information.
	FLYCAPTURE_MONO16    = 0x00000020,
	// R = G = B = 16 bits.
	FLYCAPTURE_RGB16     = 0x00000040,
	// 16 bits of signed mono information.
	FLYCAPTURE_S_MONO16  = 0x00000080,
	// R = G = B = 16 bits signed.
	FLYCAPTURE_S_RGB16   = 0x00000100,
	// 8 bit raw data output of sensor.
	FLYCAPTURE_RAW8      = 0x00000200,
	// 16 bit raw data output of sensor.
	FLYCAPTURE_RAW16     = 0x00000400,
	// 24 bit BGR
	FLYCAPTURE_BGR       = 0x10000001,
	// 32 bit BGRU
	FLYCAPTURE_BGRU      = 0x10000002,
	// Unused member to force this enum to compile to 32 bits.
	FCPF_FORCE_QUADLET   = 0x7FFFFFFF,
}

//
// Description:
//  This structure defines the format by which time is represented in the 
//  PGRFlycapture SDK.  The ulSeconds and ulMicroSeconds values represent the
//  absolute system time when the image was captured.  The ulCycleSeconds
//  and ulCycleCount are higher-precision values that have either been 
//  propagated up from the 1394 bus or extracted from the image itself.  The 
//  data will be extracted from the image if image timestamping is enabled and
//  directly (and less accurately) from the 1394 bus otherwise.
//
//  The ulCycleSeconds value will wrap around after 128 seconds.  The ulCycleCount 
//  represents the 1/8000 second component. Use these two values when synchronizing 
//  grabs between two computers sharing a common 1394 bus that may not have 
//  precisely synchronized system timers.
//  (adapted from PGRFlyCapture.h)
unsafe public struct FlyCaptureTimestamp
{
	// The number of seconds since the epoch. 
	public uint ulSeconds;
	// The microseconds component.
	public uint ulMicroSeconds;
	// The cycle time seconds.  0-127.
	public uint ulCycleSeconds;
	// The cycle time count.  0-7999. (1/8000ths of a second.)
	public uint ulCycleCount;
	// The cycle offset.  0-3071 (1/3072ths of a cycle count.)
	public uint ulCycleOffset;
} 

//
// Description:
//  This structure is used to pass image information into and out of the
//  API.
//
// Remarks:
//  The size of the image buffer is iRowInc * iRows, and depends on the
//  pixel format.
//  (adapted from PGRFlyCapture.h)
unsafe public struct FlyCaptureImage
{
	// Rows, in pixels, of the image.
	public int iRows;
	// Columns, in pixels, of the image.
	public int iCols;
	// Row increment.  The number of bytes per row.
	public int iRowInc;
	// Video mode that this image was captured with.  Only populated when the
	// image is returned from a grab call.
	public int videoMode;
	// Timestamp of this image.
	public FlyCaptureTimestamp timeStamp;
	// Pointer to the actual image data.
	public byte* pData;
	//
	// If the returned image is Y8 or Y16, this flag indicates whether it is
	// a greyscale or stippled (bayer tiled) image.  In modes other than Y8
	// or Y16, this flag has no meaning.
	//
	public bool bStippled;
	// The pixel format of this image.
	public FlyCapturePixelFormat pixelFormat;
    // The number of images which make up the data.  
    // Used for stereo cameras where images are interleaved.
    public uint uiNumImages;   
	// Reserved for future use.
	//[MarshalAs(System.Runtime.InteropServices.UnmanagedType.ByValArray,
	//		 SizeConst=6)]
	public long ulReserved;
} 