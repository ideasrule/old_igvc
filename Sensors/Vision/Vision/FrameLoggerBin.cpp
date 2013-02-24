
#include "FrameLoggerBin.h"
#include "Frame.h"
#include "Common.h"
#include <direct.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <fstream>
using namespace std;
using namespace cv;

using Pave_Libraries_Camera::Frame;

FrameLoggerBin::FrameLoggerBin(const std::string &baseDirectory)
: FrameLogger(baseDirectory)
{
	mkdir(getDirectory().c_str());
	manifest.open((getDirectory() + "/manifest.txt").c_str(), ios::in|ios::out|ios::app);
	if(printDebug) cout << "Log opened: " << getDirectory() << endl;
}
FrameLoggerBin::FrameLoggerBin(const std::string &baseDirectory, const std::string &name)
: FrameLogger(baseDirectory, name)
{
	mkdir(getDirectory().c_str());
	manifest.open((getDirectory() + "/manifest.txt").c_str());
	if(printDebug) cout << "Log opened: " << getDirectory() << endl;
	manifest.seekg (0, ios::beg);
}

FrameLoggerBin::~FrameLoggerBin()
{
	manifest.close();
	if(printDebug) cout << "Log closed: " << getDirectory() << endl;
}

void FrameLoggerBin::log(shared_ptr<Pave_Libraries_Camera::Frame> frm, 
	Pave_Libraries_Common::StateEstimationType *state)
{
	manifest << frm->framenum << "\t";
	manifest << frm->name << "\t";
	manifest << frm->focalLength << "\t";
	manifest << frm->height << "\t";
	manifest << frm->width << "\t";
	if (state) {
		manifest << state->Northing << "\t";
		manifest << state->Easting << "\t";
		manifest << state->Heading << "\t";
		manifest << state->Speed << endl;
	} else {
		manifest << "0\t0\t0\t0" << endl;
	}

	string basename = getDirectory() + "/" + frm->name;
	cv::imwrite(basename + "-color.png", frm->color);

	ofstream valid((basename + "-valid").c_str());
    ofstream transformed((basename + "-transformed.bin").c_str(), ios::binary);

	valid.write((char *)frm->validArrPtr, frm->height * frm->width * sizeof(bool));
	valid.close();

	MatConstIterator_<Point3f> it = frm->transformedCloud.begin();
	MatConstIterator_<Point3f> itEnd = frm->transformedCloud.end();
	for (; it != itEnd; ++it)
	{
		const Point3f &pt = *it;
        transformed.write((char *) &pt.x, sizeof(pt.x));
        transformed.write((char *) &pt.y, sizeof(pt.y));
        transformed.write((char *) &pt.z, sizeof(pt.z));
	}
	transformed.close();
}

shared_ptr<Frame> FrameLoggerBin::readLog(
	Pave_Libraries_Common::StateEstimationType *state)
{
	shared_ptr<Frame> frm(new Frame());

	// Clunky EOF checking but it works.
	if(printDebug) cout << "Reading at: " << manifest.tellg() << endl;
	manifest >> frm->framenum;
	if (!manifest.good())
		return shared_ptr<Frame>();
	else
		if(printDebug)
			cout << "Manifest Is Good." << endl;
	manifest >> frm->name;
	manifest >> frm->focalLength;
	int h, w;
	manifest >> h >> w;
	frm->setSize(w, h);
	Pave_Libraries_Common::StateEstimationType temp;
	if (!state) state = &temp;
	manifest >> state->Northing;
	manifest >> state->Easting;
	manifest >> state->Heading;
	manifest >> state->Speed;

	string basename = getDirectory() + "/" + frm->name;
	frm->color = cv::imread(basename + "-color.png", 1);
	cv::Mat_<unsigned char> monoConverted = cv::imread(basename + "-mono.png", 0);
	monoConverted.convertTo(frm->mono, frm->mono.type());

	ifstream valid((basename + "-valid").c_str());
	ifstream transformed((basename + "-transformed.bin").c_str(), ios::binary);

	valid.read((char *)frm->validArrPtr, frm->height * frm->width * sizeof(bool));
	valid.close();
	
	if(printDebug) cout << "Transformed read started at " << transformed.tellg() << endl;
    int numpts = 0;

	float *rawCloud = new float[frm->width*frm->height*3];
	transformed.read((char *) rawCloud, frm->width*frm->height*3*sizeof(float));
	MatIterator_<Point3f> it = frm->transformedCloud.begin();
	MatIterator_<Point3f> itEnd = frm->transformedCloud.end();
	for (int i = 0; it != itEnd; it++, i += 3) {
		numpts++;
		float *pt = rawCloud + i;
		*it = Point3f(pt[0], pt[1], pt[2]);
	}
	delete[] rawCloud;

    if(printDebug) cerr << "Number of points is " << numpts << endl;
	if(printDebug) cout << "Transformed read ended at " << transformed.tellg() << endl;
	transformed.close();

	return frm;
}
