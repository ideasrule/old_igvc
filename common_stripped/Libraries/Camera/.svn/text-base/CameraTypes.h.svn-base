/*****************************************************************************
 * Represents the camera. Supports functions to initialize the camera and get
 * the next image.
 *
 * The Camera class itself is virtual, so should never be instantiated. It
 * implements some general functions, like setting triclops parameters and
 * transformation.
 *
 * The mechanics of getting images is implemented by each of its two
 * subclasses: 
 *     CameraLive, which takes data in realtime from a real camera, and 
 *     CameraFiles, which simulates a camera based on saved stereo images
 *
 * Dependancies: vector, pgrflycapture.h
 *
 * Author: Brendan Collins 
 *****************************************************************************/

#pragma once

#include "Camera.h"

#include <vector>

#include <pgrflycapture.h>

class CameraLive : public Camera
{
    public:
        CameraLive();
        bool getNextFrame(Frame *frm, char *dir = 0, int minrow = 0, int maxrow = 0);
        bool isNextImage();

    private:
        FlyCaptureContext context;
        void adjustGain(Frame *frm);
        void nameAfterTime(Frame *frm);
        void printError(FlyCaptureError error, char *function);
};

class CameraFiles : public Camera
{
    public:
        // For when the user provides a file which contains a list of image  filenames
        CameraFiles(const char *fileDirectory, ifstream *listOfFilenames);

        // For when the user provides a single filename
        CameraFiles(const char *fileDirectory, const char *imageFilename);

		// Populates frm with the next image in the file. 		   
		// If any member of frm is not null, it is reused where possible. Images should all be the same size, etc
		bool getNextFrame(Frame *frm, char *dir = 0, int minrow = 0, int maxrow = 0);
		bool getImage(int index, Frame *frm, char *dir = 0, int minrow = 0, int maxrow = 0);
		bool isNextImage();
		int getNumImages() { return nFiles; }

    private:
        const char *fileDirectory;
        vector<const char *> fileNames;
        int nFiles;
        int nFilesRead;
};