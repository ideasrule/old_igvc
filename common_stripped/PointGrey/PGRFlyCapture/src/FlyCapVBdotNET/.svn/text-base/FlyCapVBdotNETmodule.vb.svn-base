'=============================================================================
' Copyright © 2006 Point Grey Research, Inc. All Rights Reserved.
' 
' This software is the confidential and proprietary information of Point
' Grey Research, Inc. ("Confidential Information").  You shall not
' disclose such Confidential Information and shall use it only in
' accordance with the terms of the license agreement you entered into
' with Point Grey Research, Inc. (PGR).
' 
' PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
' SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
' IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
' PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
' SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
' THIS SOFTWARE OR ITS DERIVATIVES.
'=============================================================================
'=============================================================================
' $Id: FlyCapVBdotNETmodule.vb,v 1.4 2007/06/14 23:48:15 demos Exp $
'=============================================================================

Imports System.Runtime.InteropServices
Module FlyCapVBdotNETmodule

    ' =========================================================================
    ' Public constants.

    ' Used with CreateDIBSection
    Public Const DIB_RGB_COLORS As Short = 0
    ' Any usable frame rate
    Public Const FLYCAPTURE_FRAMERATE_ANY As Integer = 11
    ' Any usable video mode
    Public Const FLYCAPTURE_VIDEOMODE_ANY As Integer = 16

    ' Difference between Form Height and PictureBox Height
    Public Const HEIGHT_DIFFERENCE As Integer = 78
    ' Difference between Form Width and PictureBox Width
    Public Const WIDTH_DIFFERENCE As Integer = 8

    ' Description:
    '   Used to indicate the type of the returned pixels.  These are 
    '   used as a member of FlyCaptureImage and as a parameter
    '   to FlyCaptureStartCustomImage.
    '   (adapted from PGRFlyCapture.h)
    '
    ' 8 bits of mono information.
    Public Const FLYCAPTURE_MONO8 = &H1
    ' YUV 4:1:1.
    Public Const FLYCAPTURE_411YUV8 = &H2
    ' YUV 4:2:2.
    Public Const FLYCAPTURE_422YUV8 = &H4
    ' YUV 4:4:4.
    Public Const FLYCAPTURE_444YUV8 = &H8
    ' R = G = B = 8 bits.
    Public Const FLYCAPTURE_RGB8 = &H10
    ' 16 bits of mono information.
    Public Const FLYCAPTURE_MONO16 = &H20
    ' R = G = B = 16 bits.
    Public Const FLYCAPTURE_RGB16 = &H40
    ' 16 bits of signed mono information.
    Public Const FLYCAPTURE_S_MONO16 = &H80
    ' R = G = B = 16 bits signed.
    Public Const FLYCAPTURE_S_RGB16 = &H100
    ' 8 bit raw data output of sensor.
    Public Const FLYCAPTURE_RAW8 = &H200
    ' 16 bit raw data output of sensor.
    Public Const FLYCAPTURE_RAW16 = &H400
    ' 24 bit BGR
    Public Const FLYCAPTURE_BGR = &H10000001
    ' 32 bit BGRU
    Public Const FLYCAPTURE_BGRU = &H10000002
    ' Unused member to force this enum to compile to 32 bits.
    Public Const FCPF_FORCE_QUADLET = &H7FFFFFFF
    ' =========================================================================


    ' =========================================================================
    ' Global FlyCapture structures

    ' The PGRFlyCapture context
    Public flycapContext As Integer
    ' The PGRFlyCaptureGUI context
    Public camguiContext As Integer
    ' The raw image that is grabbed using flycaptureGrabImage2
    Public flycapImage As FlyCaptureImage
    ' The image that we convert flycapImage to in order that it is in the
    ' proper BGR format to be displayed in our PictureBox. The allocated data
    ' in this structure resides in the bitmap we have here on the VB side.
    Public flycapRGBImage As FlyCaptureImage
    ' Have we successfully started flycapture yet?
    Public bFlyCapStarted As Boolean
    ' Stop the thread.
    Public bStopThread As Boolean
    ' Do we have contexts created?
    Public bValidContexts As Boolean
    '==========================================================================


    ' =========================================================================
    ' Examples of converted enumerations 

    ' Error codes returned from all PGRCameraGUI functions.
    ' (adapted from pgrcameragui.h)
    Public Enum CameraGUIError
        PGRCAMGUI_OK
        PGRCAMGUI_FAILED
        PGRCAMGUI_COULD_NOT_CREATE_DIALOG
        PGRCAMGUI_INVALID_ARGUMENT
        PGRCAMGUI_INVALID_CONTEXT
        PGRCAMGUI_MEMORY_ALLOCATION_ERROR
        PGRCAMGUI_INTERNAL_CAMERA_ERROR
    End Enum
    ' Description:
    '   Enumerates the image file formats that flycaptureSaveImage() 
    '   can write to.
    '   (adapted from PGRFlyCapture.h)
    Public Enum FlyCaptureImageFileFormat
        ' Single channel (8 or 16 bit) greyscale portable grey map.
        FLYCAPTURE_FILEFORMAT_PGM
        ' 3 channel RGB portable pixel map.
        FLYCAPTURE_FILEFORMAT_PPM
        ' 3 or 4 channel RGB windows bitmap.
        FLYCAPTURE_FILEFORMAT_BMP
        ' JPEG format.  Not implemented.
        FLYCAPTURE_FILEFORMAT_JPG
        ' Portable Network Graphics format.  Not implemented.
        FLYCAPTURE_FILEFORMAT_PNG
        ' Raw data output.
        FLYCAPTURE_FILEFORMAT_RAW
    End Enum
    ' Description: 
    '  An enumeration used to describe the different camera color 
    '  configurations.
    '  (adapted from PGRFlyCapture.h)
    Public Enum FlycaptureCameraType
        FLYCAPTURE_BLACK_AND_WHITE
        FLYCAPTURE_COLOR
    End Enum
    '==========================================================================

    ' =========================================================================
    ' Examples of converted C structures 

    ' Description:
    '  This structure defines the format by which time is represented in the 
    '  PGRFlycapture SDK.  The ulSeconds and ulMicroSeconds values represent the
    '  absolute system time when the image was captured.  The ulCycleSeconds
    '  and ulCycleCount are higher-precision values that have either been 
    '  propagated up from the 1394 bus or extracted from the image itself.  The 
    '  data will be extracted from the image if image timestamping is enabled and
    '  directly (and less accurately) from the 1394 bus otherwise.
    '
    '  The ulCycleSeconds value will wrap around after 128 seconds.  The ulCycleCount 
    '  represents the 1/8000 second component. Use these two values when synchronizing 
    '  grabs between two computers sharing a common 1394 bus that may not have 
    '  precisely synchronized system timers.
    '  (adapted from PGRFlyCapture.h)
    Public Structure FlyCaptureTimestamp
        ' The number of seconds since the epoch.
        Public ulSeconds As Integer
        ' The microseconds component.
        Public ulMicroSeconds As Integer
        ' The cycle time seconds.  0-127.
        Public ulCycleSeconds As Integer
        ' The cycle time count.  0-7999. (1/8000ths of a second.)
        Public ulCycleCount As Integer
        ' The cycle offset.  0-3071 (1/3072ths of a cycle count.)
        Public ulCycleOffset As Integer
    End Structure
    ' Description:
    '  This structure is used to pass image information into and out of the
    '  API.
    '
    ' Remarks:
    '  The size of the image buffer is iRowInc * iRows, and depends on the
    '  pixel format.
    '  (adapted from PGRFlyCapture.h)
    Public Structure FlyCaptureImage
        ' Rows, in pixels, of the image.
        Public iRows As Integer
        ' Columns, in pixels, of the image.
        Public iCols As Integer
        ' Row increment.  The number of bytes per row.
        Public iRowInc As Integer
        ' Video mode that this image was captured with.  Only populated when 
        ' the image is returned from a grab call.
        Public videoMode As Integer
        ' Timestamp of this image.
        Public TimeStamp As FlyCaptureTimestamp
        ' Pointer to the actual image data.
        Public pData As IntPtr
        '
        ' If the returned image is Y8 or Y16, this flag indicates whether it is
        ' a greyscale or stippled (bayer tiled) image.  In modes other than Y8
        ' or Y16, this flag has no meaning.
        '
        Public bStippled As Boolean
        ' The pixel format of this image.
        Public pixelFormat As Integer
        ' The number of images which make up the data.  
        ' Used for stereo cameras where images are interleaved.
        Public uiNumImages As Integer
        ' Reserved for future use.
        <VBFixedArray(5)> Public ulReserved() As Integer
    End Structure
    ' Description: 
    '  Camera information structure.  This structure will eventually be replaced
    '  by FlyCaptureInfoEx.
    '  (adapted from PGRFlyCapture.h)
    Public Structure FlyCaptureInfo
        Public SerialNumber As Integer
        Public CameraType As FlycaptureCameraType
        Public CameraModel As Integer
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=512)> _
            Public pszModelString As String
    End Structure
    '==========================================================================

    ' =========================================================================
    ' BITMAP data structures required to display an image
    Public Structure RGBQUAD
        Public rgbBlue As Byte
        Public rgbGreen As Byte
        Public rgbRed As Byte
        Public rgbReserved As Byte
    End Structure

    Public Structure BITMAPINFOHEADER
        Public biSize As Integer
        Public biWidth As Integer
        Public biHeight As Integer
        Public biPlanes As Short
        Public biBitCount As Short
        Public biCompression As Integer
        Public biSizeImage As Integer
        Public biXPelsPerMeter As Integer
        Public biYPelsPerMeter As Integer
        Public biClrUsed As Integer
        Public biClrImportant As Integer
    End Structure

    Public Structure BITMAPINFO
        Public bmiHeader As BITMAPINFOHEADER
        Public bmiColors As RGBQUAD
    End Structure
    '==========================================================================

    ' =========================================================================
    ' Various windows API calls that enable image display
    Public Declare Function GetActiveWindow Lib "user32" () As Integer
    Public Declare Function CreateDIBSection Lib "gdi32" _
        (ByVal hDC As IntPtr, ByRef pBitmapInfo As BITMAPINFO, _
        ByVal un As Integer, ByRef lplpVoid As IntPtr, _
        ByVal handle As Integer, ByVal dw As Integer) As Integer
    '==========================================================================

    ' =========================================================================
    ' Flycapture context functions
    Public Declare Function flycaptureCreateContext Lib "PGRFlyCapture" _
        (ByRef flycapContext As Integer) As Integer
    Public Declare Function flycaptureInitializeFromSerialNumber Lib _
        "PGRFlyCapture" (ByVal flycapContext As Integer, _
        ByVal SerialNumber As Integer) As Integer
    Public Declare Function flycaptureDestroyContext Lib "PGRFlyCapture" _
        (ByVal flycapContext As Integer) As Integer
    Public Declare Function flycaptureErrorToString Lib "PGRFlyCapture.dll" _
        (ByVal err As Integer) As String
    '==========================================================================

    ' =========================================================================
    ' Flycapture Control functions
    Public Declare Function flycaptureStart Lib "PGRFlyCapture" _
        (ByVal flycapContext As Integer, ByVal videoMode As Integer, _
        ByVal framerate As Integer) As Integer
    Public Declare Function flycaptureStop Lib "PGRFlyCapture" _
        (ByVal flycapContext As Integer) As Integer
    Public Declare Function flycaptureGrabImage2 Lib "PGRFlyCapture" _
        (ByVal flycapContext As Integer, ByRef pimage As FlyCaptureImage) _
        As Integer
    Public Declare Function flycaptureConvertImage Lib "PGRFlyCapture" _
        (ByVal flycapContext As Integer, ByRef pimageSrc As FlyCaptureImage, _
        ByRef pimageDest As FlyCaptureImage) As Integer
    Public Declare Function flycaptureSaveImage Lib "PGRFlyCapture" _
        (ByVal flycapContext As Integer, ByRef pimage As FlyCaptureImage, _
        ByVal pszPath As String, ByVal format_Renamed As Integer) As Integer
    Public Declare Function flycaptureGetCameraInformation Lib _
        "PGRFlyCapture" (ByVal flycapContext As Integer, _
        ByRef info As FlyCaptureInfo) As Integer
    '==========================================================================

    ' =========================================================================
    ' Flycapgui context and init functions
    Public Declare Function pgrcamguiCreateContext Lib "pgrflycapturegui" _
        (ByRef context As Integer) As Integer
    Public Declare Function pgrcamguiDestroyContext Lib "pgrflycapturegui" _
        (ByVal context As Integer) As Integer
    Public Declare Function pgrcamguiShowCameraSelectionModal Lib _
        "pgrflycapturegui" (ByVal camguiContext As Integer, _
        ByVal flycapContext As Integer, ByRef pulSerialNumber As Integer, _
        ByRef piDialogStatus As Integer) As Integer
    '==========================================================================

    ' =========================================================================
    ' Settings dialog
    Public Declare Function pgrcamguiCreateSettingsDialog Lib _
        "pgrflycapturegui" (ByVal camguiContext As Integer, _
        ByVal guiType As Integer, ByVal flycapContext As Integer) As Integer
    Public Declare Function pgrcamguiToggleSettingsWindowState Lib _
        "pgrflycapturegui" (ByVal camguiContext As Integer, _
        ByVal hwndParent As Integer) As Integer
    Public Declare Function pgrcamguiGetSettingsWindowState Lib _
        "pgrflycapturegui" (ByVal camguiContext As Integer, _
        ByRef pbShowing As Short) As Integer
    ' =========================================================================

    ' This is the first function called.  It opens the FlyCapForm.
    Public Sub Main()
        Dim fFlyCapForm As New FlyCapForm
        fFlyCapForm.ShowDialog()
        fFlyCapForm.Dispose()
    End Sub

End Module


