/*******************************************************************************
 *  CameraFiles.cpp
 *
 *  Blah nothing here yet.
 *
 *  Author: Brendan Collins
 *******************************************************************************/

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <highgui.h>

#include "Camera.h"
#include "Frame.h"

// Opens the camera and prepares for input. Returns a pointer to the camera, or NULL if the camera can not be opened
CameraFiles::CameraFiles(const char *directory, ifstream *listOfFilenames)
{
    // Read first line to make sure the file contains data
    string firstline;
    getline(*listOfFilenames, firstline);
    if (!listOfFilenames) {
        nFilesRead = 0;
        nFiles     = 0;
        return;
    }

    // If the first line is a file, reverse course so we can read it
    if (firstline.rfind(".ppm") != string::npos ||
        firstline.rfind(".bmp") != string::npos)
        listOfFilenames->seekg(0, ios::beg);

    // Read in all filenames and add to vector of filenames.
    fileNames = vector<const char *>();
    bool keepReading = true;
    while (keepReading && !listOfFilenames->eof()) {
        string *file = new string();
        getline(*listOfFilenames, *file);
        if (!listOfFilenames) keepReading = false;
        fileNames.push_back(file->c_str());
    }

    nFiles        = static_cast<int>(fileNames.size());	
    nFilesRead    = 0;
    fileDirectory = directory;

    cerr << "Read " << nFiles << " files" << endl;	
}

// Overloaded constructor used when there is only one file
CameraFiles::CameraFiles(const char *directory, const char *imageFilename)
{
    nFiles    = 1;
    fileNames = vector<const char *>();
    fileNames.push_back(imageFilename);
    nFilesRead    = 0;
    fileDirectory = directory;
}

bool CameraFiles::getImage(int index, Frame *frm, char *dir, int minrow, int maxrow)
{
    string *fileName = new string(fileNames[index]);

    // Generate absolute path to image
    string fileComplete;
    stringstream sstream;
    if (fileDirectory != NULL)
        sstream << fileDirectory << "\\" << *fileName;
    else
        sstream << *fileName;
    fileComplete = sstream.str();

    // Open image
    IplImage *readImage;
    readImage = cvLoadImage(fileComplete.c_str(), -1);
    if (readImage == NULL) {
        cout << "Could not open image " << fileComplete << endl;
        exit(1);
    }

	// Figure out height
	if(maxrow == 0) maxrow = readImage->height;
    int nrows = maxrow - minrow;
    int ncols = readImage->width;

    if (readImage->nChannels == 3) {
		// Create the color image, if necessary
        if (frm->colorImage == NULL || frm->colorImage->height != nrows ||
            frm->colorImage->width != ncols) {
            if (frm->colorImage != NULL) cvReleaseImage(&frm->colorImage);
            frm->colorImage = cvCreateImage( cvSize(ncols,nrows), IPL_DEPTH_8U, 3);
            frm->cData = RgbImage(frm->colorImage);
        }

		// Create the hsv image, if necessary
		if (!frm->hsvImage) {
            if (frm->hsvImage == NULL || frm->hsvImage->height != nrows ||
                frm->hsvImage->width != ncols) {
                if (frm->hsvImage != NULL) cvReleaseImage(&frm->hsvImage);
                frm->hsvImage = cvCreateImage(cvSize(ncols,nrows), IPL_DEPTH_8U, 3);
            }
        }

        // Copy color data
        RgbImage readData = RgbImage(readImage);
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
                frm->cData[i][j].r = readData[i + minrow][j].r;
                frm->cData[i][j].g = readData[i + minrow][j].g;
                frm->cData[i][j].b = readData[i + minrow][j].b;
            }
        }

		// Create the hsv image, if necessary
        if (frm->hsvImage == NULL)
            frm->hsvImage = cvCreateImage( cvSize(ncols,nrows), IPL_DEPTH_8U, 3);
        //cvCvtColor(frm->colorImage, frm->hsvImage, CV_BGR2HSV);
        // Create the mono image, if necessary
        if (frm->monoImage == NULL || frm->monoImage->height != nrows ||
            frm->monoImage->width != ncols) {
            if (frm->monoImage != NULL) cvReleaseImage(&frm->monoImage);
			frm->monoImage = cvCreateImage( cvSize(ncols,nrows), IPL_DEPTH_32F, 1);
            frm->mData = BwImageFloat(frm->monoImage);
        }

		// Convert to mono floats
        for (int i = 0; i < nrows; i++) {
            for (int j = 0; j < ncols; j++) {
                //Weighted average, according to perceptual response (from CS426 lecture notes)
                //Actually, the bayer matrix seems to be out of alignment. just use green.
                frm->mData[i][j] = frm->cData[i][j].g; //* .587 + frm->cData[i][j].r * .299 + frm->cData[i][j].b * .144;
                frm->mData[i][j] /= 256.0;
            }
        }
        cvReleaseImage(&readImage);
    }
    // Mono
    else {
        // Take care of memory as needed
        if (frm->colorImage != NULL) cvReleaseImage(&(frm->colorImage));
        frm->colorImage = NULL;
        frm->cData = NULL;
        if (frm->monoImage == NULL || frm->monoImage->height != nrows ||
            frm->monoImage->width != ncols) {
            if (frm->monoImage != NULL) cvReleaseImage(&frm->monoImage);
            frm->monoImage = cvCreateImage( cvSize(ncols,nrows), IPL_DEPTH_32F, 1);
            frm->mData = BwImageFloat(frm->monoImage);
        }

		/* Convert to float */
        BwImage bwByte(readImage);
        for (int i = 0; i < nrows; i++) {
            for (int j = 0; j < ncols; j++) {
                frm->mData[i][j] = (float) bwByte[i + minrow][j];
                frm->mData[i][j] /= 256.0;
            }
        }
        cvReleaseImage(&readImage);
    }
	
	/* Create the integral image, if necessary */

	// DGH: have to temporarily cut this out so that sift stuff works
	/**********

	if(frm->integralImage == NULL || frm->integralImage->height != nrows || frm->integralImage->width != ncols)
	{
		if(frm->integralImage !=% NULL) cvReleaseImage(&frm->integralImage);
		frm->integralImage = cvCreateImage( cvSize(ncols + 1,nrows + 1), IPL_DEPTH_32F, 1);
	}

	cvIntegral(frm->monoImage, frm->integralImage);
	frm->iData = BwImageDouble(frm->integralImage);

	***********/

    // Remove extension from fileName and set as name of the frame
    size_t dot = fileName->find_last_of('.');
    string noExtension;
    if (dot != string::npos)
        noExtension = fileName->substr(0, dot);
    else
        noExtension = *fileName;
    frm->name = noExtension;

    return true;
}

bool CameraFiles::getNextFrame(Frame *frm, char *dir, int minrow, int maxrow)
{
    int index = nFilesRead;
    nFilesRead++;
    return getImage(index, frm, dir, minrow, maxrow);
}

bool CameraFiles::isNextImage()
{
    return nFilesRead < nFiles;
}
