#include "imageviewer.h"
#include <QtGui/QApplication>

#include <cv.h>
#include <highgui.h>

#include <vector>
using std::vector;
#include <iostream>

#include "commandArgs.h"
#include "FrameLoggerBin.h"
#include "Memory.h"

int runApp(vector<shared_ptr<Frame>> frames)
{
	char *argv[1] = { 0 };
	int argc = 0;
	QApplication app(argc, argv);
    ImageViewer w(frames);
    w.show();
    return app.exec();
}

int main(int argc, char *argv[])
{
    commandArgs args(argc, argv);
	if(!args.readLogDir || !args.readLogBasename) {
		std::cerr << "Specify log directory and basename." << std::endl;
		return 1;
	}
    FrameLoggerBin logger(args.readLogDir, args.readLogBasename);

    shared_ptr<Frame> frm;

    vector<shared_ptr<Frame>> frames;
	
	int n = 0;
    while(frm = logger.readLog(0))
    {
		n++;
		if (n>10) break;
        frames.push_back(frm);
        cv::Point3f p = frames[0]->transformedCloud(3, 3);
    }

    return runApp(frames);
}
