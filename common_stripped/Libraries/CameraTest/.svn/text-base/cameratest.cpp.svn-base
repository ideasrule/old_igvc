#include "Camera.h"
#include "Frame.h"
#include "highgui.h"
#include "highgui.hpp"
#include <conio.h>
#include <iostream>

using namespace Pave_Libraries_Camera;
using namespace std;
using namespace cv;

int main() {
	shared_ptr<Frame> frm;
	CameraVidere cam(0);
	cam.setPointCloudNeeded(true);
	string winname("Color");
	cv::namedWindow(winname, CV_WINDOW_AUTOSIZE);
	_timeb start, end;
	while ((!_kbhit())) {
		_ftime(&start);
		if (!cam.getNextImage(frm)) break;
		_ftime(&end);
		cout << end.time * 1000 + end.millitm - start.time * 1000 - start.millitm << endl;
		cv::imshow(winname, frm->color);
		cv::waitKey(10);
	}
}