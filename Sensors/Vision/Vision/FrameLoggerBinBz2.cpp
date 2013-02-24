#include "FrameLoggerBinBz2.h"
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

FrameLoggerBinBz2::FrameLoggerBinBz2(const std::string &baseDirectory)
: FrameLogger(baseDirectory)
{
	mkdir(getDirectory().c_str());
	manifest.open((getDirectory() + "/manifest.txt").c_str(), ios::in|ios::out|ios::app);
	if(printDebug) cout << "Log opened: " << getDirectory() << endl;
}

FrameLoggerBinBz2::FrameLoggerBinBz2(const std::string &baseDirectory, const std::string &name)
: FrameLogger(baseDirectory, name)
{
	mkdir(getDirectory().c_str());
	manifest.open((getDirectory() + "/manifest.txt").c_str());
	if(printDebug) cout << "Log opened: " << getDirectory() << endl;
	manifest.seekg (0, ios::beg);
}

FrameLoggerBinBz2::~FrameLoggerBinBz2()
{
	manifest.close();
	if(printDebug) cout << "Log closed: " << getDirectory() << endl;
}

int FrameLoggerBinBz2::cloudToArray(scoped_array<char> &dest, const cv::Mat_<cv::Point3f>& src)
{
	size_t bytes = src.rows * src.step;
	dest.reset(new char[bytes]);
	memcpy(dest.get(), src.data, bytes);
	return bytes;
}

void FrameLoggerBinBz2::arrayToCloud(cv::Mat_<cv::Point3f>& dest, const scoped_array<char> &src)
{
	size_t bytes = dest.rows * dest.step;
	memcpy(dest.data, src.get(), bytes);
}

void FrameLoggerBinBz2::log(shared_ptr<Pave_Libraries_Camera::Frame> frm, 
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

	CPerformanceTimer timer;
	timer.Start();

	boost::iostreams::filtering_ostream tcStream;
	tcStream.push(boost::iostreams::bzip2_compressor());
	tcStream.push(boost::iostreams::file_sink(basename + "-transformed-bin.bz2", std::ios::binary));

	scoped_array<char> binCloud;
	int N = cloudToArray(binCloud, frm->transformedCloud);
	boost::iostreams::write(tcStream, binCloud.get(), N);

	timer.Stop();
	if(printDebug) std::cerr << "compressed write time (ms): " << timer.Interval_mS() << std::endl;
}

shared_ptr<Frame> FrameLoggerBinBz2::readLog(
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
	valid.read((char *)frm->validArrPtr, frm->height * frm->width * sizeof(bool));
	valid.close();

	if(printDebug) cout << "Transformed read started." << endl;

	boost::iostreams::filtering_istream tcStream;
	tcStream.push(boost::iostreams::bzip2_decompressor());
	tcStream.push(boost::iostreams::file_source(basename + "-transformed-bin.bz2", std::ios::binary));

	CPerformanceTimer timer;
	timer.Start();

	cv::Mat_<cv::Point3f>& dest = frm->transformedCloud;
	size_t bytes = dest.rows * dest.step;
	scoped_array<char> binCloud(new char[bytes]);
	boost::iostreams::read(tcStream, binCloud.get(), bytes);
	arrayToCloud(dest, binCloud);

	timer.Stop();
	if(printDebug) std::cerr << "decompressing read time (ms): " << timer.Interval_mS() << std::endl;

	if(printDebug) cout << "Transformed read ended." << endl;

	return frm;
}
