#include <iostream>
#include <iomanip>
#include <cv.h>
#include <cstdlib>

using namespace std;

int main()
{
	double cameraY[] = {371, 388, 369, 277, 272, 267, 292, 216, 216, 214, 206, 162, 154};
	double cameraX[] = {100, 238, 499, 171, 313, 485, 619, 174, 252, 372, 485, 336, 431};
	double garageY[] = {166.5, 152.4, 160.0, 259.4, 259.4, 259.4, 223.8, 369.1, 365.1, 358.9, 369.6, 525.5, 551.2};
	double garageX[] = {-112.4, -39.4, 87.3, -100.6, 0, 119.5, 187.0, -125.3, -51.8, 56.7, 161.1, 36.8, 161.4};

	cv::Mat_<double> garage(13, 2);
	cv::Mat_<double> camera(13, 2);

	for (int i = 0; i < 13; ++i) {
		camera(i, 0) = cameraX[i];
		camera(i, 1) = cameraY[i];
		garage(i, 0) = garageX[i];
		garage(i, 1) = garageY[i];
	}

	cv::Mat_<double> H = cv::findHomography(camera, garage, 0);

	ostream& os = cout << setprecision(8);

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			os << H(i, j) << '\t';
		}
		os << endl;
	}

	//system("pause");

	return 0;
}