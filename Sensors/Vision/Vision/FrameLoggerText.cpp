
#include "FrameLoggerText.h"
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

FrameLoggerText::FrameLoggerText(const std::string &baseDirectory)
: FrameLogger(baseDirectory)
{
	mkdir(getDirectory().c_str());
	manifest.open((getDirectory() + "/manifest.txt").c_str(), ios::in|ios::out|ios::app);
	if(printDebug) cout << "Log opened: " << getDirectory() << endl;
}
FrameLoggerText::FrameLoggerText(const std::string &baseDirectory, const std::string &name)
: FrameLogger(baseDirectory, name)
{
	mkdir(getDirectory().c_str());
	manifest.open((getDirectory() + "/manifest.txt").c_str());
	if(printDebug) cout << "Log opened: " << getDirectory() << endl;
	manifest.seekg (0, ios::beg);
}

FrameLoggerText::~FrameLoggerText()
{
	manifest.close();
	if(printDebug) cout << "Log closed: " << getDirectory() << endl;
}

void FrameLoggerText::log(shared_ptr<Pave_Libraries_Camera::Frame> frm, 
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

	string basename = getDirectory();
    basename += "/";
    basename += frm->name;
	cv::imwrite(basename + "-color.png", frm->color);
	cv::Mat_<unsigned char> monoConverted(frm->height, frm->width);
	frm->mono.convertTo(monoConverted, monoConverted.type());
	cv::imwrite(basename + "-mono.png", monoConverted);

	/*
	ofstream valid((basename + "-valid.txt").c_str());
	ofstream cloud((basename + "-cloud.txt").c_str());
	ofstream transformed((basename + "-transformed.txt").c_str());
	int pixels = frm->height * frm->width;
	bool *v = frm->validArr[0];
	cv::Point3f *cpt = frm->cloud[0];
	cv::Point3f *tpt = frm->transformedCloud[0];
	for (int i = 0; i < pixels; i++) {
		valid << *v++;
		cloud << cpt->x << "\t" << cpt->y << "\t" << cpt->z << "\n";
		cpt++;
		transformed << tpt->x << "\t" << tpt->y << "\t" << tpt->z << "\n";
		tpt++;
	}
	*/
	ofstream valid((basename + "-valid").c_str());
	//ofstream cloud((basename + "-cloud").c_str());
	ofstream transformed((basename + "-transformed.txt").c_str());
	valid.write((char *)frm->validArrPtr, frm->height * frm->width * sizeof(bool));
	valid.close();
	MatConstIterator_<Point3f> it = frm->transformedCloud.begin();
	MatConstIterator_<Point3f> itEnd = frm->transformedCloud.end();
	for (; it != itEnd; it++)
	{
		const Point3f &pt = *it;
		transformed << pt.x << "\t" << pt.y << "\t" << pt.z << "\n";
	}
	//cloud.close();
	transformed.flush();
	transformed.close();
}

shared_ptr<Frame> FrameLoggerText::readLog(
	Pave_Libraries_Common::StateEstimationType *state)
{
	shared_ptr<Frame> frm(new Frame());

	// Clunky EOF checking but it works.
	if(printDebug) cout << "Reading at: " << manifest.tellg() << endl;
	manifest >> frm->framenum;
	if (!manifest.good()) return shared_ptr<Frame>();
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
	string colorname = basename + "-color.png";
	cv::Mat_<cv::Vec3b> colorTemp = cv::imread(colorname.c_str(), 1);
	frm->color = colorTemp;
	cv::Mat_<unsigned char> monoConverted = cv::imread(basename + "-mono.png", 0);
	monoConverted.convertTo(frm->mono, frm->mono.type());

	/*
	ifstream valid((basename + "-valid.txt").c_str());
	ifstream cloud((basename + "-cloud.txt").c_str());
	ifstream transformed((basename + "-transformed.txt").c_str());
	for (int r = 0; r < frm->height; r++) {
		for (int c = 0; c < frm->width; c++) {
			cv::Point3f &cpt = frm->cloud[r][c];
			cloud >> cpt.x >> cpt.y >> cpt.z;
			cv::Point3f &tpt = frm->transformedCloud[r][c];
			transformed >> tpt.x >> tpt.y >> tpt.z;
		}
	}
	int pixels = frm->height * frm->width;
	bool *v = frm->validArr[0];
	cv::Point3f *cpt = frm->cloud[0];
	cv::Point3f *tpt = frm->transformedCloud[0];
	for (int i = 0; i < pixels; i++) {
		valid >> *v++;
		cloud >> cpt->x >> cpt->y >> cpt->z;
		cpt++;
		transformed >> tpt->x >> tpt->y >> tpt->z;
		tpt++;
	}
	*/
	ifstream valid((basename + "-valid").c_str());
	//ifstream cloud((basename + "-cloud").c_str());
	ifstream transformed((basename + "-transformed.txt").c_str());
	valid.read((char *)frm->validArrPtr, frm->height * frm->width * sizeof(bool));
	valid.close();
	MatIterator_<Point3f> it = frm->transformedCloud.begin();
	MatIterator_<Point3f> itEnd = frm->transformedCloud.end();
	
	if(printDebug) cout << "Transformed read started at " << transformed.tellg() << endl;
	for (; it != itEnd; it++)
	{
		Point3f &pt = *it;
		transformed >> pt.x >> pt.y >> pt.z;
	}
	if(printDebug) cout << "Transformed read ended at " << transformed.tellg() << endl;
	//cloud.close();
	transformed.close();

	return frm;
}