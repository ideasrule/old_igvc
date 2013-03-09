/*******************************************************************************
 *  CameraLive.cpp
 *
 *  Blah nothing here yet.
 *
 *  Author: Brendan Collins
 *******************************************************************************/

// TODO: auto gain control?
//       image timestamping through 1934 bus

#include <iostream>
#include <sstream>

#include <sys/timeb.h>

#include <highgui.h>

#include "Camera.h"
#include "Frame.h"

#define DESIRED_INTENSITY .45
#define P_TERM 0	
#define I_TERM .01

CameraLive::CameraLive()
{
   FlyCaptureError error;
   
   error = flycaptureCreateContext(&context);
   printError(error, "flycaptureCreateContext()");
   
   error = flycaptureInitialize(context, 0);
   printError(error, "flycaptureInitialize()");

   // TODO: work on auto-gain function?
   //allow code control
//   error = flycaptureSetCameraRegister( context, 0x000, 0x80000000 );
//   _HANDLE_ERROR( error, "flycaptureSetCameraRegister()" );
   
   // Start grabbing images in the current videomode and framerate.   
   error = flycaptureStart(context, 
                           FLYCAPTURE_VIDEOMODE_640x480RGB,
                           FLYCAPTURE_FRAMERATE_7_5);
   printError(error, "flycaptureStart()" );
}

bool CameraLive::getNextFrame(Frame *frm, char *dir, int minrow, int maxrow)
{
    FlyCaptureError error;
    FlyCaptureImage image;
    memset(&image, 0x0, sizeof( FlyCaptureImage));

    nameAfterTime(frm);	

    error = flycaptureGrabImage2(context, &image);
    printError(error, "flycaptureGrabImage2()");

    // Save image to a file if directory name was passed during initalization
    if (dir) {
        FlyCaptureImage imageConverted;
        imageConverted.pData = new unsigned char[image.iCols * image.iRows * 4];
        imageConverted.pixelFormat = FLYCAPTURE_BGRU;		
        error = flycaptureConvertImage(context, &image, &imageConverted);
        printError(error, "flycaptureConvertImage()");	

        stringstream sstream;
        sstream << dir << "\\" << frm->name << ".bmp";
        string *filename = &(sstream.str());
        error = flycaptureSaveImage(context, &imageConverted, filename->c_str(), FLYCAPTURE_FILEFORMAT_BMP);
        printError(error, "flycaptureSaveImage");

        delete[] imageConverted.pData;
    }

    IplImage readImage;
    readImage.width     = image.iCols;
    readImage.height    = image.iRows;
    readImage.widthStep = image.iRowInc;
    readImage.imageData = (char *)image.pData;
    RgbImage readData   = RgbImage(&readImage);

    // figure out height
    if (maxrow == 0)
        maxrow = readImage.height;
    int nrows = maxrow - minrow;
    int ncols = readImage.width;

    // Create the color image, if necessary
    if (frm->colorImage == NULL) {
        frm->colorImage = cvCreateImage( cvSize(ncols,nrows), IPL_DEPTH_8U, 3);
        frm->cData = RgbImage(frm->colorImage);
    }

	// Copy color data
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            // note reversal
            frm->cData[i][j].r = readData[i + minrow][j].b;
            frm->cData[i][j].g = readData[i + minrow][j].g;
            frm->cData[i][j].b = readData[i + minrow][j].r;
        }
    }

    // Create the mono image, if necessary
    if (frm->monoImage == NULL) {
        frm->monoImage = cvCreateImage( cvSize(ncols,nrows), IPL_DEPTH_32F, 1);
        frm->mData = BwImageFloat(frm->monoImage);
    }

    // Convert to mono floats
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            // Weighted average, according to perceptual response (from CS426
            // lecture notes)
            // Actually, the bayer matrix seems to be out of alignment. just
            // use green.
            frm->mData[i][j] = frm->cData[i][j].g; //* .587 + frm->cData[i][j].r * .299 + frm->cData[i][j].b * .144;
            frm->mData[i][j] /= 256.0;
        }
    }

    adjustGain(frm);
    return true;
}

bool CameraLive::isNextImage()
{
    return true;
}

void CameraLive::adjustGain(Frame * frm)
{		
	//flycaptureSetCameraProperty(context, FLYCAPTURE_SHUTTER, newGain, 0, false);
}

void CameraLive::printError(FlyCaptureError error, char *function)
{
   if (error != FLYCAPTURE_OK) {
      cerr << function << ": " << flycaptureErrorToString(error) << endl;
      exit(1);
   }
}

void CameraLive::nameAfterTime(Frame *frm)
{
    _ftime_s(&frm->time);
    stringstream sstream;
    sstream << static_cast<long int>(frm->time.time);
    sstream.fill('0');
    sstream.width(3);
    sstream << static_cast<int>(frm->time.millitm);
    frm->name = sstream.str();
    //sprintf(frm->name, "%ld%03d", (long int)frm->time.time, (int)frm->time.millitm);
    cerr << frm->name.c_str() << endl;
}

// yields a number in [0, 1)
inline double randUnit()
{
    return (double)rand() / ((double)RAND_MAX - 1.0);
}
