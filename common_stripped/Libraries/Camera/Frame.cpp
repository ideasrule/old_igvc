#include "Frame.h"
#include <cstdlib>
#include <algorithm>

using namespace Pave_Libraries_Camera;

Frame::Frame() : 
camera(NULL), focalLength(0), name(""), time(), framenum(0), 
width(0), height(0), 
color(), mono(), //cData(&color), mData(&mono),
transformedCloud(),/* valid(),*/ obstacle(),
min(), max() {
	validArr = NULL;
	validArrPtr = NULL;
	obstacle = NULL;
	obstaclePtr = NULL;
}

Frame::Frame(int w, int h) :
	camera(NULL), focalLength(0), name(""), time(), framenum(0),
	width(w), height(h), 
	color(h, w), mono(h, w), colorFull(480, 640),//cData(&color), mData(&mono),
	transformedCloud(h, w), /*valid(h, w), obstacle(h, w),*/
	min(), max()
{
	createValidArr(h, w);
	createObstacle(h, w);
}

Frame::Frame(const Frame &other) :
	camera(other.camera), focalLength(other.focalLength), 
	name(other.name), time(other.time), framenum(other.framenum), 
	color(other.color.clone()), mono(other.mono.clone()), colorFull(other.colorFull.clone()),
	transformedCloud(other.transformedCloud.clone()),
	min(other.min), max(other.max) 
{

	if (other.width != width || other.height != height) {
		if (validArr) {
			delete validArr;
			validArr = NULL;
		}
		if (validArrPtr) {
			delete validArrPtr;
			validArrPtr = NULL;
		}
		if (obstacle) {
			delete obstacle;
			obstacle = NULL;
		}
		if (obstaclePtr) {
			delete obstaclePtr;
			obstaclePtr = NULL;
		}
	}
	createValidArr(other.height, other.width);
	memcpy(validArrPtr, other.validArrPtr, other.width*other.height*sizeof(bool));
	createObstacle(other.height, other.width);
	memcpy(obstacle, other.obstacle, other.width*other.height*sizeof(unsigned char));
    /*
	if (other.stateEst && other.stateEstSize) {
        stateEstSize = other.stateEstSize;
        stateEst = malloc(stateEstSize);
        memcpy(stateEst, other.stateEst, stateEstSize);
    }
	*/
}

const Frame & Frame::operator = (const Frame &other) {
    if (&other != this) {
        camera = other.camera;
        focalLength = other.focalLength;
        name = std::string(other.name);
        time = other.time;
        framenum = other.framenum;
        color = other.color.clone();
		colorFull = other.colorFull.clone();
        mono = other.mono.clone();
        min = other.min;
        max = other.max;

		transformedCloud = other.transformedCloud;

		if (other.width != width || other.height != height) {
			if (validArr) {
				delete validArr;
				validArr = NULL;
			}
			if (validArrPtr) {
				delete validArrPtr;
				validArrPtr = NULL;
			}
			if (obstacle) {
				delete obstacle;
				obstacle = NULL;
			}
			if (obstaclePtr) {
				delete obstaclePtr;
				obstaclePtr = NULL;
			}
		}
		createValidArr(other.height, other.width);
		memcpy(validArrPtr, other.validArrPtr, other.width*other.height*sizeof(bool));

		createObstacle(other.height, other.width);
		//for (int iRow = 0; iRow < other.height; ++iRow) 
		//	for (int iCol = 0; iCol < other.width; ++iCol) 
		//		obstacle[i][j] = other.obstacle[i][j];
		memcpy(obstacle, other.obstacle, other.width*other.height*sizeof(unsigned char));   //Does this work?

		/*
        if (other.stateEst && other.stateEstSize) {
            stateEstSize = other.stateEstSize;
            stateEst = malloc(stateEstSize);
            memcpy(stateEst, other.stateEst, stateEstSize);
        }
		*/
    }
    return *this;
}

void Frame::setSize(int w, int h) {
    if (w > 0 && h > 0 && width != w || height != h) {
        width = w;
        height = h;
        color.create(h, w);
        mono.create(h, w);
		colorFull.create(480, 640);
		transformedCloud.create(h, w);
        /*
        if (ptInfo) {
            for (int i = 0; i < h; i++) {
                if (ptInfo[i]) delete ptInfo[i];
            }
            delete ptInfo;
        }
        ptInfo = new PointInfo *[h];
        for (int i = 0; i < h; i++) {
            ptInfo[i] = new PointInfo[w];
        }
        */
	
		if (validArr) delete validArr;
		if (validArrPtr) delete validArrPtr;
		createValidArr(h, w);

		if (obstacle) delete obstacle;
		if (obstaclePtr) delete obstaclePtr;
		createObstacle(h, w);
    }
}

Frame::~Frame() {
    /*
    if (ptInfo) {
        for (int i = 0; i < height; i++) {
            if (ptInfo[i]) delete ptInfo[i];
        }
        delete ptInfo;
    }
    */
    //if (stateEst) free(stateEst);

	if (validArr) delete [] validArr;
	if (validArrPtr) delete [] validArrPtr;

	if (obstacle) delete [] obstacle;
	if (obstaclePtr) delete [] obstaclePtr;
}


void Frame::createValidArr(int h, int w)
{
	validArr = new bool*[h];
	bool *validArrPtr1 = new bool[h*w]();
	validArrPtr = validArrPtr1;
	for( int i = 0; i < h; i++) { 
		*(validArr + i) = validArrPtr1; 
		validArrPtr1 += w; 
	}
}


void Frame::createObstacle(int h, int w)
{
	obstacle = new unsigned char*[h];
	unsigned char *obstaclePtr1 = new unsigned char[h*w]();
	for( int i = 0; i < h; i++) { 
		*(obstacle + i) = obstaclePtr1; 
		obstaclePtr1 += w; 
	}
	obstaclePtr = obstacle[0];
}

void Frame::setObstacle(unsigned char value)
{	
	//obstacle is an 2D array, but obstacle[0] is a continuous 1D array
	std::fill(obstacle[0], obstacle[0] + width*height, value);
}