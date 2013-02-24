#include "FrameLoggerTextBz2.h"
#include "Frame.h"
#include "Common.h"
#include <direct.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <sstream>

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp> 
#include <boost/iostreams/filter/bzip2.hpp>

using namespace std;
using namespace cv;

using Pave_Libraries_Camera::Frame;

#include <PerformanceTimer.h>

FrameLoggerTextBz2::FrameLoggerTextBz2(const std::string &baseDirectory)
: FrameLogger(baseDirectory)
{
	mkdir(getDirectory().c_str());
	manifest.open((getDirectory() + "/manifest.txt").c_str(), ios::in|ios::out|ios::app);
	if(printDebug) cout << "Log opened: " << getDirectory() << endl;
}

FrameLoggerTextBz2::FrameLoggerTextBz2(const std::string &baseDirectory, const std::string &name)
: FrameLogger(baseDirectory, name)
{
	mkdir(getDirectory().c_str());
	manifest.open((getDirectory() + "/manifest.txt").c_str());
	if(printDebug) cout << "Log opened: " << getDirectory() << endl;
	manifest.seekg (0, ios::beg);
}

FrameLoggerTextBz2::~FrameLoggerTextBz2()
{
	manifest.close();
	if(printDebug) cout << "Log closed: " << getDirectory() << endl;
}

void FrameLoggerTextBz2::log(shared_ptr<Pave_Libraries_Camera::Frame> frm, 
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

	ofstream valid((basename + "-valid").c_str());
	valid.write((char *)frm->validArrPtr, frm->height * frm->width * sizeof(bool));
	valid.close();

	MatConstIterator_<Point3f> it = frm->transformedCloud.begin();
	MatConstIterator_<Point3f> itEnd = frm->transformedCloud.end();
	std::stringstream trText;
	for (; it != itEnd; it++)
	{
		const Point3f &pt = *it;
		trText << pt.x << "\t" << pt.y << "\t" << pt.z << "\n";
	}

	boost::iostreams::filtering_ostream tcStream;
	tcStream.push(boost::iostreams::bzip2_compressor());
	tcStream.push(boost::iostreams::file_sink(basename + "-transformed-txt.bz2", std::ios::binary));

	CPerformanceTimer timer;
	timer.Start();

	tcStream << trText.rdbuf();
	
	timer.Stop();
	if(printDebug) std::cerr << "compressed write time (ms): " << timer.Interval_mS() << std::endl;
}

shared_ptr<Frame> FrameLoggerTextBz2::readLog(
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

	ifstream valid((basename + "-valid").c_str());

	boost::iostreams::filtering_istream tcStream;
	tcStream.push(boost::iostreams::bzip2_decompressor());
	tcStream.push(boost::iostreams::file_source(basename + "-transformed-txt.bz2", std::ios::binary));

	valid.read((char *)frm->validArrPtr, frm->height * frm->width * sizeof(bool));
	valid.close();
	MatIterator_<Point3f> it = frm->transformedCloud.begin();
	MatIterator_<Point3f> itEnd = frm->transformedCloud.end();
	
	if(printDebug) cout << "Transformed read started." << endl;

	CPerformanceTimer timer;
	timer.Start();

	for (; it != itEnd; it++)
	{
		Point3f &pt = *it;
		tcStream >> pt.x >> pt.y >> pt.z;
	}

	timer.Stop();
	if(printDebug) std::cerr << "decompressing read time (ms): " << timer.Interval_mS() << std::endl;

	if(printDebug) cout << "Transformed read ended." << endl;

	return frm;
}
