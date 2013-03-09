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
// $Id: PGRFlyCaptureTestCSharp.cs,v 1.5 2006/02/17 18:33:26 marko Exp $
//=============================================================================

//==========================================================================
// This code provides an example of how to access PGR's FlyCapture DLLs from
// a C# .NET environment. The examples include the simple data types such as 
// booleans, ints, longs, char arrays, etc, as well as structures, 
// enumerations, and most importantly, images.
//
// To be able to run this code, you will have to enable the unsafe option.
// To do so:
// Project->[Filename] Properties...->Configuration Properties->Build
// Change "Allow unsafe code blocks" to 'True'
//
// In order to run the executable after compiling, you must change the
// .NET Security settings as follows:
//   Start->Settings->Control Panel->Administrative Tools->
//   Microsoft .NET Framework 1.1 Wizards->Adjust .NET Security->
//   Make changes to this computer->My Computer->Full Trust
//==========================================================================


using System;
using System.Runtime.InteropServices;

public struct RGBQUAD
{
	public byte rgbBlue;
	public byte rgbGreen;
	public byte rgbRed;
	public byte rgbReserved;
}

public struct BITMAPINFOHEADER 
{
	public int biSize; 
	public int biWidth; 
	public int biHeight; 
	public short biPlanes;
	public short biBitCount;
	public int biCompression;
	public int biSizeImage; 
	public int biXPelsPerMeter; 
	public int biYPelsPerMeter; 
	public int biClrUsed; 
	public int biClrImportant; 
}

public struct BITMAPINFO 
{
	public BITMAPINFOHEADER bmiHeader;
	public RGBQUAD bmiColors;
}



namespace PGRFlyCaptureTestCSharp
{
	unsafe class PGRFlyCaptureTestCSharp
	{
		//
		// DLL Functions to import
		// 
		// Follow this format to import any DLL with a specific function.
		//

		[DllImport("pgrflycapture.dll")]
		public static extern int flycaptureCreateContext(int* flycapcontext);

		[DllImport("pgrflycapture.dll")]
		public static extern int flycaptureStart(int flycapcontext, 
			FlyCaptureVideoMode videoMode, 
			FlyCaptureFrameRate frameRate);

		[DllImport("pgrflycapture.dll")]
		public static extern string flycaptureErrorToString(int error);

		[DllImport("pgrflycapture.dll")]
		public static extern int flycaptureInitialize(int flycapContext, 
			int cameraIndex);

		[DllImport("pgrflycapture.dll")]
		public static extern int flycaptureGetCameraInformation(int flycapContext, 
			ref FlyCaptureInfo arInfo);

		[DllImport("pgrflycapture.dll")]
		unsafe public static extern int flycaptureGrabImage2(int flycapContext, 
			ref FlyCaptureImage image);

		[DllImport("pgrflycapture.dll")]
		unsafe public static extern int flycaptureSaveImage(int flycapContext, 
			ref FlyCaptureImage image, string filename, 
			FlyCaptureImageFileFormat fileFormat);

		[DllImport("pgrflycapture.dll")]
		public static extern int flycaptureStop(int flycapContext);

		[DllImport("pgrflycapture.dll")]
		public static extern int flycaptureDestroyContext(int flycapContext);

		[DllImport("pgrflycapture.dll")]
		public static extern int flycaptureConvertImage(int flycapContext, 
			ref FlyCaptureImage image, ref FlyCaptureImage imageConvert);

		[DllImport("gdi32.dll")]
		public static extern int CreateDIBSection(int* hDC, 
			ref BITMAPINFO pBitmapInfo, int un, ref byte* lplpVoid, 
			int handle, int dw);


		// Bitmap constant
		public const short DIB_RGB_COLORS = 0;

		// The index of the camera to grab from.
		public const int _CAMERA_INDEX = 0;

		// The maximum number of cameras on the bus.
		public const int _MAX_CAMS = 3;
		
		// The number of images to grab.
		public const int _IMAGES_TO_GRAB = 10;


		// Function to initialize a bitmap for the converted image.
		static void InitBitmapStructure(int nRows, int nCols, 
			ref FlyCaptureImage flycapRGBImage)
		{

			BITMAPINFO bmi = new BITMAPINFO();	// bitmap header
			byte* pvBits = null; //	pointer	to DIB section
			int	nBytes;

			nBytes = nRows * nCols * 3;	// for R,G,B

			bmi.bmiHeader.biSize = 40; // sizeof(BITMAPINFOHEADER) = 40
			bmi.bmiHeader.biWidth =	nCols;
			bmi.bmiHeader.biHeight = nRows;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 24; // three	8-bit components
			bmi.bmiHeader.biCompression	= 0; //	BI_RGB = 0
			bmi.bmiHeader.biSizeImage =	nBytes;
			CreateDIBSection(null, ref bmi, DIB_RGB_COLORS, 
				ref pvBits, 0,	0);

			// This	is where we	set	the	"to	be converted" image	data pointer 
			// to our newly created bitmap data pointer
			flycapRGBImage.pData = pvBits;
		}

		static void reportError( int ret, string fname )
		{		
			Console.Write(fname + " error: " + flycaptureErrorToString(ret) + "\n");
			Console.Write("\nPress Enter");
			Console.Read();
			return;
		}

		[STAThread]
		static void Main(string[] args)
		{
			int	flycapContext;
			int	ret;
			FlyCaptureInfo flycapInfo = new FlyCaptureInfo();
			FlyCaptureImage image = new FlyCaptureImage();
			FlyCaptureImage flycapRGBImage = new FlyCaptureImage();

			// Create the context.
			ret	= flycaptureCreateContext(&flycapContext);
			if (ret	!= 0 )
			{
				reportError(ret,"flycaptureCreateContext");
				return;
			}
				
			// Initialize the camera.
			ret = flycaptureInitialize( flycapContext, _CAMERA_INDEX );
			if (ret	!= 0 )
			{
				reportError(ret,"flycaptureInitialize");
				return;
			}

			// Get the info for this camera.
			ret = flycaptureGetCameraInformation( flycapContext, ref flycapInfo );
			if (ret	!= 0 )
			{
				reportError(ret,"flycaptureGetCameraInformation");
				return;
			}
			if (flycapInfo.CameraType == 
				FlyCaptureCameraType.FLYCAPTURE_BLACK_AND_WHITE)
			{
				Console.Write( "Model: B&W " + flycapInfo.pszModelString + "\n"
					+ "Serial #: " + flycapInfo.SerialNumber + "\n");
			}
			else if (flycapInfo.CameraType == 
				FlyCaptureCameraType.FLYCAPTURE_COLOR)
			{
				Console.Write( "Model: Colour " + flycapInfo.pszModelString + "\n"
					+ "Serial #: " + flycapInfo.SerialNumber + "\n");
			}
			
			// Start FlyCapture.
			ret	= flycaptureStart( flycapContext, 
				FlyCaptureVideoMode.FLYCAPTURE_VIDEOMODE_ANY,
				FlyCaptureFrameRate.FLYCAPTURE_FRAMERATE_ANY );
			if (ret	!= 0 )
			{
				reportError(ret,"flycaptureStart");
				return;
			}

			// Start grabbing images.
			Console.Write("\nGrabbing Images ");
			for ( int iImage = 0; iImage < _IMAGES_TO_GRAB; iImage++ )
			{
				ret = flycaptureGrabImage2( flycapContext, ref image );
				if (ret	!= 0 )
				{
					reportError(ret,"flycaptureGrabImage2");
					return;
				}
				Console.Write(".");
			}

			// Convert the image.
			InitBitmapStructure(image.iRows, image.iCols, ref flycapRGBImage );
			flycapRGBImage.pixelFormat = FlyCapturePixelFormat.FLYCAPTURE_BGR;
			ret = flycaptureConvertImage( flycapContext, ref image, 
				ref flycapRGBImage);
			if (ret	!= 0 )
			{
				reportError(ret,"flycaptureConvertImage");
				return;
			}
			
			// Save the image.
			Console.Write("\nSaving Last Image ");
			ret = flycaptureSaveImage( flycapContext, ref flycapRGBImage, "raw.bmp", 
				FlyCaptureImageFileFormat.FLYCAPTURE_FILEFORMAT_BMP );
			if (ret	!= 0 )
			{
				reportError(ret,"flycaptureSaveImage");
				return;
			}
			else
				System.Diagnostics.Process.Start("mspaint.exe", "raw.bmp");

			// Stop FlyCapture.
			ret = flycaptureStop(flycapContext);
			if (ret	!= 0 )
			{
				reportError(ret,"flycaptureStop");
				return;
			}

			// Destroy the context.
			ret = flycaptureDestroyContext(flycapContext);
			if (ret	!= 0 )
			{
				reportError(ret,"flycaptureDestroyContext");
				return;
			}

			Console.Write("\nPress Enter");
			Console.Read();
		}			
	}
}



