
#include "Visualizations.h"
#include "ImageLane.h"
#include <iostream> // for debugging
#include <limits>
#include <iomanip>

#define X_MAX 10
#define Y_MAX 20
#define Y_MIN 0
#define Z_MAX 5
#define Z_MIN -2

#define TDROWS 480
#define TDCOLS 640
#define UC_SAT 255

using namespace cv;
using namespace std;

static Mat_<Vec3b> topDown = Mat_<Vec3b>(TDROWS,TDCOLS);
static const Vec3b vecBlack(0,0,0), vecYellow(0,255,255), vecWhite(255,255,255);
static const Vec3b vecRed(0,0,255), vecGreen(0,255,0), vecBlue(255,0,0);
static const Scalar scBlack(0,0,0), scYellow(0,255,255), scWhite(255,255,255);
static const Scalar scRed(0,0,255), scGreen(0,255,0), scBlue(255,0,0);


// colormap for disparities
// colormap
#define DMAP_SIZE 768
static unsigned char dmap[DMAP_SIZE] = 
  { 108, 0, 0,
    107, 0, 12,
    106, 0, 18,
    105, 0, 24,
    103, 0, 30,
    102, 0, 36,
    101, 0, 42,
    99, 0, 48,
    98, 0, 54,
    97, 0, 60,
    96, 0, 66,
    94, 0, 72,
    93, 0, 78,
    92, 0, 84,
    91, 0, 90,
    89, 0, 96,
    88, 0, 102,
    87, 0, 108,
    85, 0, 114,
    84, 0, 120,
    83, 0, 126,
    82, 0, 131,
    80, 0, 137,
    79, 0, 143,
    78, 0, 149,
    77, 0, 155,
    75, 0, 161,
    74, 0, 167,
    73, 0, 173,
    71, 0, 179,
    70, 0, 185,
    69, 0, 191,
    68, 0, 197,
    66, 0, 203,
    65, 0, 209,
    64, 0, 215,
    62, 0, 221,
    61, 0, 227,
    60, 0, 233,
    59, 0, 239,
    57, 0, 245,
    56, 0, 251,
    55, 0, 255,
    54, 0, 255,
    52, 0, 255,
    51, 0, 255,
    50, 0, 255,
    48, 0, 255,
    47, 0, 255,
    46, 0, 255,
    45, 0, 255,
    43, 0, 255,
    42, 0, 255,
    41, 0, 255,
    40, 0, 255,
    38, 0, 255,
    37, 0, 255,
    36, 0, 255,
    34, 0, 255,
    33, 0, 255,
    32, 0, 255,
    31, 0, 255,
    29, 0, 255,
    28, 0, 255,
    27, 0, 255,
    26, 0, 255,
    24, 0, 255,
    23, 0, 255,
    22, 0, 255,
    20, 0, 255,
    19, 0, 255,
    18, 0, 255,
    17, 0, 255,
    15, 0, 255,
    14, 0, 255,
    13, 0, 255,
    11, 0, 255,
    10, 0, 255,
    9, 0, 255,
    8, 0, 255,
    6, 0, 255,
    5, 0, 255,
    4, 0, 255,
    3, 0, 255,
    1, 0, 255,
    0, 4, 255,
    0, 10, 255,
    0, 16, 255,
    0, 22, 255,
    0, 28, 255,
    0, 34, 255,
    0, 40, 255,
    0, 46, 255,
    0, 52, 255,
    0, 58, 255,
    0, 64, 255,
    0, 70, 255,
    0, 76, 255,
    0, 82, 255,
    0, 88, 255,
    0, 94, 255,
    0, 100, 255,
    0, 106, 255,
    0, 112, 255,
    0, 118, 255,
    0, 124, 255,
    0, 129, 255,
    0, 135, 255,
    0, 141, 255,
    0, 147, 255,
    0, 153, 255,
    0, 159, 255,
    0, 165, 255,
    0, 171, 255,
    0, 177, 255,
    0, 183, 255,
    0, 189, 255,
    0, 195, 255,
    0, 201, 255,
    0, 207, 255,
    0, 213, 255,
    0, 219, 255,
    0, 225, 255,
    0, 231, 255,
    0, 237, 255,
    0, 243, 255,
    0, 249, 255,
    0, 255, 255,
    0, 255, 249,
    0, 255, 243,
    0, 255, 237,
    0, 255, 231,
    0, 255, 225,
    0, 255, 219,
    0, 255, 213,
    0, 255, 207,
    0, 255, 201,
    0, 255, 195,
    0, 255, 189,
    0, 255, 183,
    0, 255, 177,
    0, 255, 171,
    0, 255, 165,
    0, 255, 159,
    0, 255, 153,
    0, 255, 147,
    0, 255, 141,
    0, 255, 135,
    0, 255, 129,
    0, 255, 124,
    0, 255, 118,
    0, 255, 112,
    0, 255, 106,
    0, 255, 100,
    0, 255, 94,
    0, 255, 88,
    0, 255, 82,
    0, 255, 76,
    0, 255, 70,
    0, 255, 64,
    0, 255, 58,
    0, 255, 52,
    0, 255, 46,
    0, 255, 40,
    0, 255, 34,
    0, 255, 28,
    0, 255, 22,
    0, 255, 16,
    0, 255, 10,
    0, 255, 4,
    2, 255, 0,
    8, 255, 0,
    14, 255, 0,
    20, 255, 0,
    26, 255, 0,
    32, 255, 0,
    38, 255, 0,
    44, 255, 0,
    50, 255, 0,
    56, 255, 0,
    62, 255, 0,
    68, 255, 0,
    74, 255, 0,
    80, 255, 0,
    86, 255, 0,
    92, 255, 0,
    98, 255, 0,
    104, 255, 0,
    110, 255, 0,
    116, 255, 0,
    122, 255, 0,
    128, 255, 0,
    133, 255, 0,
    139, 255, 0,
    145, 255, 0,
    151, 255, 0,
    157, 255, 0,
    163, 255, 0,
    169, 255, 0,
    175, 255, 0,
    181, 255, 0,
    187, 255, 0,
    193, 255, 0,
    199, 255, 0,
    205, 255, 0,
    211, 255, 0,
    217, 255, 0,
    223, 255, 0,
    229, 255, 0,
    235, 255, 0,
    241, 255, 0,
    247, 255, 0,
    253, 255, 0,
    255, 251, 0,
    255, 245, 0,
    255, 239, 0,
    255, 233, 0,
    255, 227, 0,
    255, 221, 0,
    255, 215, 0,
    255, 209, 0,
    255, 203, 0,
    255, 197, 0,
    255, 191, 0,
    255, 185, 0,
    255, 179, 0,
    255, 173, 0,
    255, 167, 0,
    255, 161, 0,
    255, 155, 0,
    255, 149, 0,
    255, 143, 0,
    255, 137, 0,
    255, 131, 0,
    255, 126, 0,
    255, 120, 0,
    255, 114, 0,
    255, 108, 0,
    255, 102, 0,
    255, 96, 0,
    255, 90, 0,
    255, 84, 0,
    255, 78, 0,
    255, 72, 0,
    255, 66, 0,
    255, 60, 0,
    255, 54, 0,
    255, 48, 0,
    255, 42, 0,
    255, 36, 0,
    255, 30, 0,
    255, 24, 0,
    255, 18, 0,
    255, 12, 0,
    0, 0, 0,
    0, 0, 0
  };

void showDisparity(const shared_ptr<Frame> frm)
{
	
	static Mat_<Vec3b> disp;
	disp.create(frm->height, frm->width);
	Mat_<Point3f>::const_iterator cloudIt = frm->transformedCloud.begin();
	Mat_<Point3f>::const_iterator cloudEnd = frm->transformedCloud.end();
	
	float maxy = 0, miny = numeric_limits<float>::max();
	for (int i = 0; cloudIt != cloudEnd; cloudIt++, i++) {
		if (frm->validArrPtr[i])
		{
			float curry = log((*cloudIt).y);
			if (curry > maxy) maxy = curry;
			else if (curry < miny) miny = curry;
		}
	}

	cloudIt = frm->transformedCloud.begin();
	Mat_<Vec3b>::iterator dispIt = disp.begin(), dispEnd = disp.end();
	Mat_<Vec3b>::const_iterator colorIt = frm->color.begin();

	for (int i = 0; cloudIt != cloudEnd; cloudIt++, dispIt++, colorIt++, i++) {
		if (frm->validArrPtr[i]) {
			float yUnit = (log((*cloudIt).y) - miny) / (maxy - miny);
			if (yUnit < 1 && yUnit > 0) {
				int v = int(yUnit * 255) * 3;
				(*dispIt)[2] = dmap[v];
				(*dispIt)[1] = dmap[v+1];
				(*dispIt)[0] = dmap[v+2];
			} else {
				*dispIt = vecRed;
			}
		} else {
			*dispIt = *colorIt;
		}
	}

	cv::imshow("Point Cloud", disp);
}

void showOverlay(const shared_ptr<Frame> frm)
{
	Mat_<Vec3b> overlay(480, 640);

	//frm->color.copyTo(overlay);

	/*
	bool *v = frm->validArrPtr;
	unsigned char *obs = frm->obstaclePtr;

	Mat_<Vec3b>::iterator over = overlay.begin(), overEnd = overlay.end();
	Mat_<Vec3b>::const_iterator c = frm->color.begin(), cEnd = frm->color.end();
	for (; over != overEnd; over++, c++) {
		*over = (*v++ && *obs++) ? obsColor : *c;
	}
	*/

	for (int r = 0; r < frm->height; r++) {
		for (int c = 0; c < frm->width; c++) {
			if (frm->validArr[r][c] && frm->obstacle[r][c]) {
				//	overlay[r][c] = vecBlue;
				//}
				overlay[r][c] = frm->obstacle[r][c];
			} else {
				overlay[r][c] = 0;
			}
		}
	}

	cv::imshow("Overlay Image", overlay);
}

static int visNumLaneOverlay = 0;

void showLaneOverlay(const shared_ptr<Frame> frm, const vector<Spline> &splines, bool save)
{
	Mat_<Vec3b> overlay(480, 640);
	frm->color.copyTo(overlay);
	for (int r = 0; r < frm->height; r++) {
		for (int c = 0; c < frm->width; c++) {
			if (!frm->validArr[r][c]) {
				for (int chan = 0; chan < 3; ++chan) overlay[r][c][chan] /= 3;
			} else if (frm->obstacle[r][c]) {
				overlay[r][c] = vecBlue;
				//overlay[r][c] = frm->obstacle[r][c];
			}
		}
	}

	for (int iSpline = 0; iSpline < splines.size(); ++iSpline)
	{
		const Spline &spline = splines[iSpline];
		spline.drawTo(overlay, vecRed);
		for (int iCtrl = 0; iCtrl < spline.controlPoints.size(); ++iCtrl)
		{
			cv::circle(overlay, spline.controlPoints[iCtrl], 5, scRed);
		}
	}

	cv::imshow("Lane Overlay Image", overlay);

	if (save)
	{
		std::stringstream filename;
		filename << "vis_lane_overlay_" << std::setfill('0') << std::setw(5) << (visNumLaneOverlay++) << ".png";
		cv::imwrite(filename.str(), overlay);
	}
}

void showTopDown(const shared_ptr<Frame> frm)
{
    topDown.setTo(scBlack);

	for (int i = Y_MIN; i < Y_MAX; i += 2) {
		cv::line(topDown, Point(0, 479-(i-Y_MIN)*TDROWS/(Y_MAX-Y_MIN)), 
			Point(TDCOLS, 479-(i-Y_MIN)*TDROWS/(Y_MAX-Y_MIN)), scRed);
		ostringstream text;
		text << i;
		cv::putText(topDown, text.str(), Point(TDCOLS/2, 475-(i-Y_MIN)*TDROWS/(Y_MAX-Y_MIN)),
			FONT_HERSHEY_SIMPLEX, 0.5, scRed);
	}
	for (int i = -X_MAX; i < X_MAX; i += 5) {
		cv::line(topDown, Point((i+X_MAX)*TDCOLS/(2*X_MAX), 0), 
			Point((i+X_MAX)*TDCOLS/(2*X_MAX), TDROWS), scRed);
		if (i != 0) {
			ostringstream text;
			text << i;
			cv::putText(topDown, text.str() , Point((i+X_MAX)*TDCOLS/(2*X_MAX), 475-(-Y_MIN)*TDROWS/(Y_MAX-Y_MIN)), 
				FONT_HERSHEY_SIMPLEX, 0.5, scRed);
		}
	}

	int called = 0;
	for(int r = 0; r < frm->height; r++) {
		for(int c = 0; c < frm->width; c++)
		{
			if (frm->validArr[r][c] && frm->obstacle[r][c])
			{
                //Point3f pt = frm->newCloud[r*frm->width+c];
				Point3f pt = frm->transformedCloud(r, c);

				int row = TDROWS - (pt.y - Y_MIN)*TDROWS/(Y_MAX - Y_MIN);
				int col = (pt.x + X_MAX)*TDCOLS/(2*X_MAX);
				//color = (pt.y - Y_MIN)*UC_SAT/(Y_MAX - Y_MIN);
				if (row > 0 && row < TDROWS && col > 0 && col < TDCOLS)
				{
					topDown[row][col][BGR_G] = 255;
					called++;
				}
			}
		}
	}

	/*
	for(vector<ImageLane*>::const_iterator it = lanes.begin(); it < lanes.end(); it++)
	{
		ImageLane *lane = *it;
		const Vec3b &color = lane->color == YELLOW ? vecYellow : vecWhite;
		for (int r = lane->minRow; r < lane->maxRow; r++) {
			int c = lane->col(r);
			cv::Point3f pt = frm->transformedCloud[r][c];
			int row = TDROWS - ((int)pt.y - Y_MIN)*TDROWS/(Y_MAX - Y_MIN);
			int col = ((int)pt.x + X_MAX)*TDCOLS/(2*X_MAX);
			//color = (pt.y - Y_MIN)*UC_SAT/(Y_MAX - Y_MIN);
			if (row > 0 && row < TDROWS && col > 0 && col < TDCOLS)
			{
				topDown[row][col][BGR_G] = 255;
			}
		}
	}
	*/

    cv::imshow("Top Down View", topDown);
}