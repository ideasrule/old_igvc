/*******************************************************************************
 *  Frame.h
 *
 *  Structure that represents a captured frame
 *  
 *  blank
 *
 *  Usage:
 *  blank
 *  
 *  Dependancies: string, sys/timeb.h, Image.h
 *
 *  Author: Brendan Collins '?? and Brenton Partridge '12
 *******************************************************************************/

#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <sys/timeb.h>
#include <sys/types.h>
#include "cv.h"

namespace Pave_Libraries_Camera
{
	class Camera;
	
	enum {RGB_R = 0, RGB_G = 1, RGB_B = 2, BGR_R = 2, BGR_G = 1, BGR_B = 0};

	const int IMAGE_FULL_HEIGHT = 480;
	const int IMAGE_FULL_WIDTH = 640;

	//only supports 320x240 and 640x480
	const int FRAME_IMAGE_HEIGHT = 240;   
	const int FRAME_IMAGE_WIDTH = 320;

	class Frame 
	{
	public:
        Frame();
        Frame(int width, int height);
        Frame(const Frame &other);
        const Frame & operator = (const Frame &other);
        virtual ~Frame();

		int width;
		int height;
        
        // Resize buffers to the given width and height if necessary.
        void setSize(int width, int height);

        Camera *camera;
        double focalLength;
        std::string name;
		_timeb time;
        int framenum;
        
        // Color data.
        // Can read/write as unsigned char r = color[row][col][BGR::R], etc.
		cv::Mat_<cv::Vec3b> color;
		cv::Mat_<cv::Vec3b> colorFull;   //always 640 by 480 images
        
        // Mono data. May be computed directly from color data.
        // Can read/write as float m = mono[row][col].
        cv::Mat_<float> mono;

        // Point cloud of 3D points; uninitialized if camera is not set to generate point cloud.
		// Must be transformed relative to center of vehicle, with z=0 as ground, in meters.
		// Can read/write as float x = transformedCloud[row][col].x, etc.
		cv::Mat_<cv::Point3f> transformedCloud;
        
        // On if the point at the location is valid; off if it is not.
        // All set to off if camera is not set to generate point cloud.
        // Can read/write as bool isValid = (bool)valid[row][col].
        //cv::Mat_<unsigned char> valid; 
		bool **validArr;
		bool *validArrPtr;
        
        // Convenience matrix to store if the point at the location is an obstacle.
        // Can read/write as bool isObstacle = (bool)obstacle[row][col].
        //cv::Mat_<unsigned char> obstacle;
		unsigned char **obstacle;
		unsigned char *obstaclePtr;
        
        // Minimum and maximum 3D values in point cloud.
		cv::Point3f min, max;

		void setObstacle(unsigned char value);

	private: 
		void createValidArr(int h, int w);
		void createObstacle(int h, int w);
	};
}


// Associated state estimation data is not stored here to keep this API simple.
// A client wishing to associate frames with state estimations taken IMMEDIATELY BEFORE
// the camera capture call is made, which makes global transforms more accurate when
// moving quickly and allows us to reconstruct cost maps from logged data to help the
// cognition people, should do the following:
// 
// When reading from a live camera, call the IPC blocking API before getNextImage(),
// and store state est in a separate file based on the timestamp.
// When reading from a file, getNextImage(), get the timestamp, and read the corresponding file.
// Check the IGVC 2009 code for how this is done

#endif  // FRAME_H