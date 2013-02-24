#include "Manduchi.h"
#include <iostream>
#include <iomanip>
#include <boost/pending/disjoint_sets.hpp>
#include <windows.h>
#include <omp.h>
#include "PerformanceTimer.h"
#include "highgui.h"

#include <vector>
#include <algorithm>

using std::cout;
using std::endl;
using boost::disjoint_sets;

//#define DO_TIMING  //time and display individual sections of obstacle detection

#ifndef IMAGE_WIDTH
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#endif

#if IMAGE_WIDTH == 640 && IMAGE_HEIGHT == 480
	static const int TILE_WIDTH = 40;
	static const int TILE_HEIGHT = 79;
	static const int CLUSTER_THRESHOLD_SIZE = 20;
	//threshold on min number of compatible points found for a point to be considered an obstacle
	const float MIN_COMPATIBLE_POINTS_COUNT = 16.0f;
#elif IMAGE_WIDTH == 320 && IMAGE_HEIGHT == 240
	static const int TILE_WIDTH = 20;
	static const int TILE_HEIGHT = 39;
	static const int CLUSTER_THRESHOLD_SIZE = 10;
	//threshold on min number of compatible points found for a point to be considered an obstacle
	static const float MIN_COMPATIBLE_POINTS_COUNT = 5.0f;
	//const float MIN_COMPATIBLE_POINTS_COUNT = 4.0f;
#else
#error manduchi not configured for this image size.
#endif

static const int NUMBER_OF_THREADS = 4;

static const float MAX_RANGE = 25.0f; //max detection distance in meters
static const float SIN_THETA_SQRD = 0.71f;  //0.4f
static const float TAN_THETA = 1.0f; //0.84;
static const float TAN_CAM_ANGLE = 0.47; //~22 deg
static const float CAM_HEIGHT = 1.5;

#if IMAGE_WIDTH == 640 && IMAGE_HEIGHT == 480
	static const float SEARCH_MIN = 15.0f;
	static const float SEARCH_MAX = 35.0f;
	static const float SEARCH_BASE = 40.0f;
#elif IMAGE_WIDTH == 320 && IMAGE_HEIGHT == 240
	static const float SEARCH_MIN = 7.0f;
	static const float SEARCH_MAX = 20.0;
	static const float SEARCH_BASE = 20.0f;
#endif

static float focalLength;

Manduchi::Manduchi()
{
	depthTileA.resize(16*6);
	depthTileB.resize(15*6);
	depthTileC.resize(16*5);
	depthTileD.resize(15*5);
	for (int i = 0; i < 16*6; ++i) 
		depthTileA[i] = (float*) _aligned_malloc(4 * TILE_WIDTH * TILE_HEIGHT * sizeof(float), 16);
	for (int i = 0; i < 15*6; ++i) 
		depthTileB[i] = (float*) _aligned_malloc(4 * TILE_WIDTH * TILE_HEIGHT * sizeof(float), 16);
	for (int i = 0; i < 16*5; ++i)
		depthTileC[i] = (float*) _aligned_malloc(4 * TILE_WIDTH * TILE_HEIGHT * sizeof(float), 16);
	for (int i = 0; i < 15*5; ++i) 
		depthTileD[i] = (float*) _aligned_malloc(4 * TILE_WIDTH * TILE_HEIGHT * sizeof(float), 16);

	labelMap = new int[IMAGE_WIDTH * IMAGE_HEIGHT]();   //initialized to 0
	rank.resize(IMAGE_HEIGHT*IMAGE_WIDTH);
	parent.resize(IMAGE_HEIGHT*IMAGE_WIDTH);

	clusteredObstacles.reserve(IMAGE_HEIGHT*IMAGE_WIDTH/16);   //this is probably big enough capacity. if not that's ok
	relabelList.resize(IMAGE_HEIGHT*IMAGE_WIDTH/4); //max number of clusters possible
	std::fill(relabelList.begin(), relabelList.end(), -1);
}

Manduchi::~Manduchi()
{
	for (int i = 0; i < 16*6; ++i) 
		_aligned_free(depthTileA[i]);
	for (int i = 0; i < 15*6; ++i) 
		_aligned_free(depthTileB[i]);
	for (int i = 0; i < 16*5; ++i)
		_aligned_free(depthTileC[i]); 
	for (int i = 0; i < 15*5; ++i) 
		_aligned_free(depthTileD[i]); 

	delete [] labelMap;
}


// This is the inner loop of the Manduchi algorithm
inline void Manduchi::checkCompatibility(float *p1, float *p2, float Hmin2, float Hmax2)
{
	if (p2[3] < 0) return;
	float delta_pz2 = (p2[2] - p1[2]) * (p2[2] - p1[2]);
	if (delta_pz2 < Hmin2 || delta_pz2 > Hmax2) return;

	float delta_p2 = (p1[0] - p2[0])*(p1[0] - p2[0]) + (p1[1] - p2[1])*(p1[1] - p2[1]) + delta_pz2;

	float u = SIN_THETA_SQRD * delta_p2;

	if (delta_pz2 > u) {
		p1[3] += 1.0f;
		p2[3] += 1.0f;
	}
}


// determines size of truncated triangles 
inline void Manduchi::assignWindowSize(float *p1, float& Hmin, float& Hmax, float& searchMin, float& searchMax, float& searchMaxt2, float& searchBase)
{
	float actualMagnification = focalLength / (p1[1] - (p1[2] - CAM_HEIGHT) * TAN_CAM_ANGLE);
	if (actualMagnification > 350.0f) {
		Hmin = 0.05;
		Hmax = 0.15;
	} else if (actualMagnification > 250.0f) {
		Hmin = 0.08;
		Hmax = 0.2;
	} else if (actualMagnification > 180.0f) {
		Hmin = 0.12;
		Hmax = 0.3;
	} else if (actualMagnification > 60.0f) { 
		Hmin = 0.2;
		Hmax = 0.5;
	} else {
		Hmin = 0.3;
		Hmax = 0.5;
	}

	searchMax = min(SEARCH_MAX, focalLength * Hmax / p1[1]);
	searchMin = min(SEARCH_MIN, focalLength * Hmin / p1[1]);
	searchMaxt2 = searchMax * 2.0f;
	searchBase = min(SEARCH_BASE, searchMaxt2 / TAN_THETA);
}


void Manduchi::blockACProcess(float *depthMap)
{
	float *p1, *p2;
	int t;
	float Hmin, Hmax;

	for (int vyi = 0; vyi < (TILE_HEIGHT+1)/2; ++vyi) { 

		for (int vxi = 0; vxi < TILE_WIDTH/2; ++vxi) {
			p1 = &depthMap[(vyi*TILE_WIDTH+vxi)*4];
			if (p1[3] < 0 || p1[1] > MAX_RANGE) continue;

			float searchMax, searchMin, searchMaxt2, searchBase;
			assignWindowSize(p1, Hmin, Hmax, searchMin, searchMax, searchMaxt2, searchBase);
			float Hmin2 = Hmin * Hmin;
			float Hmax2 = Hmax * Hmax;

			int rowCount = int(searchMin);
			int endY = vyi + int(searchMax);
			int startY = vyi + rowCount;
			for (int yi = startY; yi < endY; ++yi, ++rowCount) {
				int shortBase = int(rowCount/searchMaxt2*searchBase);
				int startX = max(0, vxi - shortBase);
				int endX = vxi + shortBase;
				t = (yi * TILE_WIDTH + startX) * 4;
				for (int xi = startX; xi < endX; ++xi, t+=4) {
					p2 = &depthMap[t];   
					checkCompatibility(p1, p2, Hmin2, Hmax2);
				}
			}
		}

		for (int vxi = TILE_WIDTH/2; vxi < TILE_WIDTH; ++vxi) {	
			p1 = &depthMap[(vyi*TILE_WIDTH+vxi)*4];
			if (p1[3] < 0 || p1[1] > MAX_RANGE) continue;

			float searchMax, searchMin, searchMaxt2, searchBase;
			assignWindowSize(p1, Hmin, Hmax, searchMin, searchMax, searchMaxt2, searchBase);
			float Hmin2 = Hmin * Hmin;
			float Hmax2 = Hmax * Hmax;		

			int rowCount = int(searchMin);
			int startY = vyi + rowCount;
			int endY = vyi + int(searchMax);
			for (int yi = startY; yi < endY; ++yi, ++rowCount) {
				int shortBase = int(rowCount/searchMaxt2*searchBase);
				int endX = min(TILE_WIDTH, vxi + shortBase);
				int startX = vxi - shortBase;
				t = (yi * TILE_WIDTH + startX) * 4;
				for (int xi = startX; xi < endX; ++xi, t+=4) {
					p2 = &depthMap[t];	
					checkCompatibility(p1, p2, Hmin2, Hmax2);
				}
			}
		}
	}
}



void Manduchi::blockBDProcess(float *depthMap)
{
	float *p1, *p2;
	int t;
	float Hmin, Hmax;

	for (int vyi = 0; vyi < (TILE_HEIGHT+1)/2; ++vyi) {
		
		for (int vxi = 1; vxi < TILE_WIDTH/2; ++vxi) {
			p1 = &depthMap[(vyi*TILE_WIDTH+vxi)*4];
			if (p1[3] < 0 || p1[1] > MAX_RANGE) continue;

			float searchMax, searchMin, searchMaxt2, searchBase;
			assignWindowSize(p1, Hmin, Hmax, searchMin, searchMax, searchMaxt2, searchBase);
			float Hmin2 = Hmin * Hmin;
			float Hmax2 = Hmax * Hmax;
			
			float rowCount = searchMin;
			int endY = vyi + int(searchMax);
			int startY = vyi + int(rowCount);
			for (int yi = startY; yi < endY; ++yi, rowCount+=1.0f) {
				int shortBase = int(rowCount/searchMaxt2*searchBase);
				int endX = vxi + shortBase;
				t = (yi * TILE_WIDTH + TILE_WIDTH/2) * 4;
				for (int xi = TILE_WIDTH/2; xi < endX; ++xi, t+=4) {
					p2 = &depthMap[t];
					checkCompatibility(p1, p2, Hmin2, Hmax2);
				}
			}
		}
	
		for (int vxi = TILE_WIDTH/2; vxi < TILE_WIDTH; ++vxi) {
			p1 = &depthMap[(vyi*TILE_WIDTH+vxi)*4];
			if (p1[3] < 0 || p1[1] > MAX_RANGE) continue;

			float searchMax, searchMin, searchMaxt2, searchBase;
			assignWindowSize(p1, Hmin, Hmax, searchMin, searchMax, searchMaxt2, searchBase);
			float Hmin2 = Hmin * Hmin;
			float Hmax2 = Hmax * Hmax;

			int rowCount = int(searchMin);
			int endY = vyi + int(searchMax);
			int startY = vyi + rowCount;
			for (int yi = startY; yi < endY; ++yi, ++rowCount) {
				int shortBase = int(rowCount/searchMaxt2*searchBase);
				int startX = vxi - shortBase;
				t = (yi * TILE_WIDTH + startX) * 4;
				for (int xi = startX; xi < TILE_WIDTH/2; ++xi, t+=4) {
					p2 = &depthMap[t];
					checkCompatibility(p1, p2, Hmin2, Hmax2);
				}
			}
		}
	}
}


bool Manduchi::process(shared_ptr<Frame> frm)
{
	if (frm->width != IMAGE_WIDTH || frm->height != IMAGE_HEIGHT) {
		cout << "Manduchi not configured for "
			<< frm->width << "x" << frm->height 
			<< " frames! No obstacles detected!" << endl;
		frm->color.copyTo(overlay_); // otherwise it'd be uninitialized
		return false;
	}

#ifdef DO_TIMING
	CPerformanceTimer timer;
	timer.Start();
#endif

	// copy OpenCV matrix to array of floats for faster access
	float *cloud = new float[frm->width*frm->height*3];
	cv::MatIterator_<cv::Point3f> it = frm->transformedCloud.begin();
	cv::MatIterator_<cv::Point3f> itEnd = frm->transformedCloud.end();
	for (int i = 0; it != itEnd; it++, i += 3) {
		float *pt = cloud + i;
		pt[0] = (*it).x;
		pt[1] = (*it).y;
		pt[2] = (*it).z;
	}

    // Setup the overlay image
    frm->color.copyTo(overlay_);

	focalLength = (float)frm->focalLength;

	// create tiles of type (0,0)
	for (int i = 0; i < 16; ++i) {
		for (int j = 0; j < 6; ++j) {
			int r = j * (TILE_HEIGHT + 1);
			float *tile = depthTileA[i*6+j];
			for (int yi = 0; yi < TILE_HEIGHT; ++yi, ++r) {
				int c = i * TILE_WIDTH;
				for (int xi = 0; xi < TILE_WIDTH; ++xi, ++c) {
					int cloudIdx = (r * IMAGE_WIDTH + c)*3;
					int tileIdx = (yi * TILE_WIDTH + xi)*4;
					tile[tileIdx] = cloud[cloudIdx];
					tile[tileIdx+1] = cloud[cloudIdx+1];
					tile[tileIdx+2] = cloud[cloudIdx+2];
					if (frm->validArr[r][c])
						tile[tileIdx+3] = 0.5f;   //arbitrary indicator
					else
						tile[tileIdx+3] = -1.0f;   //invalid
				}
			}
		}
	}

	// create tiles of type (1, 0)
	for (int i = 0; i < 15; ++i) {
		for (int j = 0; j < 6; ++j) {
			float *tile = depthTileB[i*6+j];
			int r = j * (TILE_HEIGHT + 1);
			for (int yi = 0; yi < TILE_HEIGHT; ++yi, ++r) {
				int c = i * TILE_WIDTH + (TILE_WIDTH / 2);
				for (int xi = 0; xi < TILE_WIDTH; ++xi, ++c) {
					int cloudIdx = (r * IMAGE_WIDTH + c) * 3;  
					int tileIdx = (yi * TILE_WIDTH + xi) * 4;    //the last float stores isValid
					tile[tileIdx] = cloud[cloudIdx];
					tile[tileIdx+1] = cloud[cloudIdx+1];
					tile[tileIdx+2] = cloud[cloudIdx+2];
					if (frm->validArr[r][c])
						tile[tileIdx+3] = 0.5f;   //arbitrary indicator
					else
						tile[tileIdx+3] = -1.0f;  //invalid
				}
			}
		}
	}

	// create tiles of type (0,1)
	for (int i = 0; i < 16; ++i) {
		for (int j = 0; j < 5; ++j) {
			float *tile = depthTileC[i*5+j];
			int r = j * (TILE_HEIGHT + 1) + (TILE_HEIGHT + 1) / 2;
			for (int yi = 0; yi < TILE_HEIGHT; ++yi, ++r) {	
				int c = i * TILE_WIDTH;
				for (int xi = 0; xi < TILE_WIDTH; ++xi, ++c) {  
					int cloudIdx = (r * IMAGE_WIDTH + c) * 3;
					int tileIdx = (yi * TILE_WIDTH + xi) * 4;
					tile[tileIdx] = cloud[cloudIdx];
					tile[tileIdx+1] = cloud[cloudIdx+1];
					tile[tileIdx+2] = cloud[cloudIdx+2];
					if (frm->validArr[r][c])
						tile[tileIdx+3] = 0.5f;   //arbitrary indicator
					else
						tile[tileIdx+3] = -1.0f;  //invalid
				}
			}
		}
	}

	// create tiles of type (1, 1)
	for (int i = 0; i < 15; ++i) {
		for (int j = 0; j < 5; ++j) {
			int r = j * (TILE_HEIGHT + 1) + (TILE_HEIGHT + 1) / 2;
			float *tile = depthTileD[i*5+j];
			for (int yi = 0; yi < TILE_HEIGHT; ++yi, ++r) {
				int c = i * TILE_WIDTH + (TILE_WIDTH / 2);
				for (int xi = 0; xi < TILE_WIDTH; ++xi, ++c) {   
					int cloudIdx = (r * IMAGE_WIDTH + c)*3;
					int tileIdx = (yi*TILE_WIDTH+xi)*4;
					tile[tileIdx] = cloud[cloudIdx];
					tile[tileIdx+1] = cloud[cloudIdx+1];
					tile[tileIdx+2] = cloud[cloudIdx+2];
					if (frm->validArr[r][c])
						tile[tileIdx+3] = 0.5f;   //arbitrary indicator
					else
						tile[tileIdx+3] = -1.0f;  //invalid
				}
			}
		}
	}

#ifdef DO_TIMING
	timer.Stop();
	cout << "Time (pre-processing): " << timer.Interval_mS() << "ms" << endl; 
	timer.Reset();
	timer.Start();
#endif

	#pragma omp parallel num_threads(NUMBER_OF_THREADS)
	{
		#pragma omp for nowait schedule(dynamic, 2) 
			for (int i = 0; i < 16*6; i+=1) {
				blockACProcess(depthTileA[i]);
			}

		#pragma omp for nowait schedule(dynamic, 2) 
			for (int i = 0; i < 15*6; ++i) {
				blockBDProcess(depthTileB[i]);
			}

		#pragma omp for nowait schedule(dynamic, 2) 
			for (int i = 0; i < 16*5; ++i) {
				blockACProcess(depthTileC[i]);
			}

		#pragma omp for nowait schedule(dynamic, 2) 
			for (int i = 0; i < 15*5; ++i) {
				blockBDProcess(depthTileD[i]);
			}
	}

#ifdef DO_TIMING
	timer.Stop();
	cout << "Time (actual algorithm): " << timer.Interval_mS() << "ms" << endl;
	timer.Reset();
	timer.Start();
#endif

	// type (0,0)
	for (int i = 0; i < 16; ++i) {
		for (int j = 0; j < 6; ++j) {
			float *tile = depthTileA[i*6+j];
			int r = j * (TILE_HEIGHT + 1);
			for (int yi = 0; yi < TILE_HEIGHT; ++yi, ++r) {
				int c = i * TILE_WIDTH;
				for (int xi = 0; xi < TILE_WIDTH; ++xi, ++c) {	
					if (tile[(yi*TILE_WIDTH+xi)*4+3] > 1.0f + MIN_COMPATIBLE_POINTS_COUNT) {  //1.0 comes from the fact that 0.5 means no compatible points found
						frm->obstacle[r][c] = UCHAR_SAT;
					//	overlay_[r][c] = cv::Vec3b(255,0,0);
					} else if (tile[(yi*TILE_WIDTH+xi)*4+3] < 0) {
						//overlay_[r][c] = cv::Vec3b(128, 128, 128);
					}
				}
			}
		}
	}

	// type (1, 0)
	for (int i = 0; i < 15; ++i) {
		for (int j = 0; j < 6; ++j) {
			float *tile = depthTileB[i*6+j];
			int r = j * (TILE_HEIGHT + 1);
			for (int yi = 0; yi < TILE_HEIGHT; ++yi, ++r) {
				int c = i * TILE_WIDTH + (TILE_WIDTH / 2);
				for (int xi = 0; xi < TILE_WIDTH; ++xi, ++c) {
					if (tile[(yi*TILE_WIDTH+xi)*4+3] > 1.0f + MIN_COMPATIBLE_POINTS_COUNT) {  //1.0 comes from the fact that 0.5 means no compatible points found
						frm->obstacle[r][c] = UCHAR_SAT;
					//	overlay_[r][c] = cv::Vec3b(255,0,0);
					} else if (tile[(yi*TILE_WIDTH+xi)*4+3] < 0) {
						//overlay_[r][c] = cv::Vec3b(128, 128, 128);
					}
				}
			}
		}
	}

	// type (0,1)
	for (int i = 0; i < 16; ++i) {
		for (int j = 0; j < 5; ++j) {
			float *tile = depthTileC[i*5+j];
			int r = j * (TILE_HEIGHT + 1) + (TILE_HEIGHT + 1) / 2;
			for (int yi = 0; yi < TILE_HEIGHT; ++yi, ++r) {	
				int c = i * TILE_WIDTH;
				for (int xi = 0; xi < TILE_WIDTH; ++xi, ++c) {	
					if (tile[(yi*TILE_WIDTH+xi)*4+3] > 1.0f + MIN_COMPATIBLE_POINTS_COUNT) {  //1.0 comes from the fact that 0.5 means no compatible points found
						frm->obstacle[r][c] = UCHAR_SAT;
					//	overlay_[r][c] = cv::Vec3b(255,0,0);
					} else if (tile[(yi*TILE_WIDTH+xi)*4+3] < 0) {
						//overlay_[r][c] = cv::Vec3b(128, 128, 128);
					}
				}
			}
		}
	}

	// type (1, 1)
	for (int i = 0; i < 15; ++i) {
		for (int j = 0; j < 5; ++j) {
			float *tile = depthTileD[i*5+j];
			int r = j * (TILE_HEIGHT + 1) + (TILE_HEIGHT + 1) / 2;
			for (int yi = 0; yi < TILE_HEIGHT; ++yi, ++r) {
				int c = i * TILE_WIDTH + (TILE_WIDTH / 2);
				for (int xi = 0; xi < TILE_WIDTH; ++xi, ++c) {
					if (tile[(yi*TILE_WIDTH+xi)*4+3] > 1.0f + MIN_COMPATIBLE_POINTS_COUNT) {  //1.0 comes from the fact that 0.5 means no compatible points found
						frm->obstacle[r][c] = UCHAR_SAT;
					//	overlay_[r][c] = cv::Vec3b(255,0,0);
					} else if (tile[(yi*TILE_WIDTH+xi)*4+3] < 0) {
						//overlay_[r][c] = cv::Vec3b(128, 128, 128);
					} 
				
				}
			}
		}
	}

	//cv::imshow("wtf", overlay_);
	//cv::waitKey();

	////for testing the simple bar detector
	//bool result[IMAGE_WIDTH*IMAGE_HEIGHT];
	//for (int i = 0; i < IMAGE_WIDTH*IMAGE_HEIGHT; ++i)
	//	result[i] = false;
	//simpleRailingDetector(frm->transformedCloud, frm->validArr[0], result);
	//for (int r = 0; r < IMAGE_HEIGHT; ++r) {
	//	for (int c = 0; c < IMAGE_WIDTH; ++c) {
	//		if (result[r*IMAGE_WIDTH+c]) {
	//			frm->obstacle[r][c] = UCHAR_SAT;
	//			overlay_[r][c] = cv::Vec3b(255,0,0);
	//		}
	//	}
	//}

#ifdef DO_TIMING
	timer.Stop();
	cout << "Time (assign overlay): " << timer.Interval_mS() << "ms" << endl; 
	timer.Reset();
	timer.Start();
#endif

	// Obstacle clustering
	// clustering using connected components
	findConnectedComponent(frm->obstacle[0], labelMap, cloud);

	// Process the clusters
	clusteredObstacles.resize(0);
	int indexCount = 0;

	// draw obstacles with different colors
	// THE LOOP ORDER HERE IS IMPORTANT! It may not look right but it is 
	// necessary to push the points into the cluster vectors in column order, 
	// not row order (to efficiently compute average slope)
	for (int c = 0; c < IMAGE_WIDTH; ++c) {
		for (int r = 0; r < IMAGE_HEIGHT; ++r) {
			int label = labelMap[r*IMAGE_WIDTH+c];
			if (label == 0 && frm->validArr[r][c] == false) {
				//Draw non-valid pixels as black
				//overlay_[r][c] = cv::Vec3b(0, 0, 0);
			} else if (label > 0) {
				//the indices of relabelList are the non-consecutive labels outputted by the connect-components
				//labeling algorithm. it's values are new, consecutive labels. This is so we don't need to use
				//std::map, which is a lot slower
				int newLabel = relabelList[label];    				                                     
				if (newLabel < 0) {
					newLabel = indexCount++;
					relabelList[label] = newLabel;
					clusteredObstacles.resize(newLabel+1);
				}

				clusteredObstacles[newLabel].push_back(cv::Point2i(r, c));
				//switch (label % 6) {
				//	case 0: overlay_[r][c] = cv::Vec3b(255, 0, 0); break;
				//	case 1: overlay_[r][c] = cv::Vec3b(255, 255, 0); break;
				//	case 2: overlay_[r][c] = cv::Vec3b(255, 0, 255); break;
				//	case 3: overlay_[r][c] = cv::Vec3b(0, 255, 0); break;
				//	case 4: overlay_[r][c] = cv::Vec3b(0, 255, 255); break;
				//	case 5: overlay_[r][c] = cv::Vec3b(0, 0, 255); break;
				//	default: break;
				//}
			}
			//else
			//	overlay_[r][c] = cv::Vec3b(0, 0, 0);
		}
	}

	std::fill(relabelList.begin(), relabelList.end(), -1);
	frm->setObstacle(0);

	// Threshold on average slope and size
	vector<vector<cv::Point2i>>::iterator clusterIt;

	for (clusterIt = clusteredObstacles.begin(); clusterIt != clusteredObstacles.end(); ++clusterIt) {

		float totalDistance = 0;
		float netHeightDifference = 0;
		int topPixelRow = -1;
		int bottomPixelRow = IMAGE_HEIGHT;
		vector<cv::Point2i>::iterator it = clusterIt->begin();
		int colCount = 1;
		int currentCol = it->y;
		for (; it != clusterIt->end(); ++it) {
			if (currentCol != it->y) {
				netHeightDifference += 
					(cloud[(topPixelRow*IMAGE_WIDTH+currentCol)*3+2] - cloud[(bottomPixelRow*IMAGE_WIDTH+currentCol)*3+2]);
				currentCol = it->y;
				colCount++;
				topPixelRow = -1;
				bottomPixelRow = IMAGE_HEIGHT;
			}
			if (it->x > topPixelRow)
				topPixelRow = it->x;
			if (it->x < bottomPixelRow)
				bottomPixelRow = it->x;
			totalDistance += cloud[((it->x)*IMAGE_WIDTH+currentCol)*3+1];
		}

		float avgSlope = netHeightDifference/float(colCount);
		float avgDistance = totalDistance / float(clusterIt->size());

		//cout << "Cluster Size: " << setw(5) << clusterIt->size() 
		//	<< " Avg height: " << setw(10) << avgSlope << " Avg dist: " << avgDistance << endl;

		// now do the actual thresholding on slope
		if ((avgDistance > 18.0 && fabs(avgSlope) < 0.4) ||
			(avgDistance > 12.0 && fabs(avgSlope) < 0.3) ||
			(avgDistance > 8.0 && fabs(avgSlope) < 0.2) ||
			fabs(avgSlope) < 0.1) {
			continue;
		}

		// do thresholding on cluster size
		if (clusterIt->size() < CLUSTER_THRESHOLD_SIZE)
			continue;

		
		for (it = clusterIt->begin(); it != clusterIt->end(); ++it) {
			overlay_[(*it).x][(*it).y] = cv::Vec3b(255, 0, 0);
			frm->obstacle[(*it).x][(*it).y] = UCHAR_SAT;
		}
	}

	

	//cv::imshow("wtf", overlay_);
	//cv::waitKey();

#ifdef DO_TIMING
	timer.Stop();
	cout << "Time (clustering): " << timer.Interval_mS() << "ms" << endl; 
#endif

	delete[] cloud;

	return true;
}


int isNeighbor(int* labelMap, int neighborIdx, int selfIdx, float *cloud)
{
	int m = labelMap[neighborIdx];
	if (m == 0)
		return 0;
	int x1 = cloud[selfIdx*3];
	int y1 = cloud[selfIdx*3+1];
	int x2 = cloud[neighborIdx*3];
	int y2 = cloud[neighborIdx*3+1];

	if ((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2) > 0.2)   //max distance in meters^2 for two points to be consided as neighbors
		return 0;
	else
		return m;

}


void Manduchi::findConnectedComponent(unsigned char *img, int *labelMap, float *cloud) 
{
	// 4-connected using the classic two-pass algorithm with union-find

	int Nrows = IMAGE_HEIGHT;
	int Ncols = IMAGE_WIDTH;
	
	disjoint_sets<int*, int*> d_set(&parent[0], &rank[0]);

	int currMaxLabel = 0;

	// process first row
	{
		//first column
		if (img[0]) {
			labelMap[0] = ++currMaxLabel;
		} else {
			labelMap[0] = 0;
		}

		for (int c = 1; c < Ncols; ++c) {
			if (img[c] == 0) { //background
				labelMap[c] = 0;
				continue;
			}
			if (img[c-1] == 0) {   //neighbor is not connected
				d_set.make_set(++currMaxLabel);
				labelMap[c] = currMaxLabel;
			} else if (img[c-1] != 0) {
				labelMap[c] = labelMap[c-1];
			}
		}
	}

	int neighborLabelLeft, neighborLabelTop;
	int neighborLeftIdx, neighborTopIdx;
	// process the rest of the rows
	for (int r = 1; r < Nrows; ++r) {
		//first column
		int currIndex = r*Ncols;
		if (img[currIndex]) {
			neighborLabelTop = labelMap[(r-1)*Ncols+0];
			if (neighborLabelTop != 0) {
				labelMap[currIndex] = neighborLabelTop;
			} else {
				d_set.make_set(++currMaxLabel);
				labelMap[currIndex] = currMaxLabel;
			}
		} else {
			labelMap[r*Ncols] = 0;
		}

		//rest of the columns
		for (int c = 1; c < Ncols; ++c) {
			int currIndex = r * Ncols + c;
			if (img[currIndex] == 0) {    // is background
				labelMap[currIndex] = 0;
				continue;
			}
			neighborLabelLeft = labelMap[currIndex-1];  //left
			neighborLabelTop = labelMap[(r-1)*Ncols+c];    //top
			neighborLeftIdx = currIndex-1;
			neighborTopIdx = (r-1)*Ncols+c;
			//if (neighborLabelLeft == 0 && neighborLabelTop == 0) {
			//	d_set.make_set(++currMaxLabel);
			//	labelMap[currIndex] = currMaxLabel;
			//} else if (neighborLabelLeft == 0 && neighborLabelTop > 0) {
			//	labelMap[currIndex] = neighborLabelTop;
			//} else if (neighborLabelLeft > 0 && neighborLabelTop == 0) {
			//	labelMap[currIndex] = neighborLabelLeft;
			//} else {    //both neighbors are connected
			//	if (neighborLabelLeft == neighborLabelTop) {
			//		labelMap[currIndex] = neighborLabelLeft;
			//	} else {
			//		labelMap[currIndex] = min(neighborLabelLeft, neighborLabelTop);
			//		d_set.union_set(neighborLabelLeft, neighborLabelTop);
			//	}
			//}
			if (isNeighbor(labelMap, neighborLeftIdx, currIndex, cloud) == 0 && 
				isNeighbor(labelMap, neighborTopIdx, currIndex, cloud) == 0) {
				d_set.make_set(++currMaxLabel);
				labelMap[currIndex] = currMaxLabel;
			} else if (isNeighbor(labelMap, neighborLeftIdx, currIndex, cloud) == 0 && 
				isNeighbor(labelMap, neighborTopIdx, currIndex, cloud) > 0) {
				labelMap[currIndex] = neighborLabelTop;
			} else if (isNeighbor(labelMap, neighborLeftIdx, currIndex, cloud) > 0 && 
				isNeighbor(labelMap, neighborTopIdx, currIndex, cloud) == 0) {
				labelMap[currIndex] = neighborLabelLeft;
			} else {    //both neighbors are connected
				if (neighborLabelLeft == neighborLabelTop) {
					labelMap[currIndex] = neighborLabelLeft;
				} else {
					labelMap[currIndex] = min(neighborLabelLeft, neighborLabelTop);
					d_set.union_set(neighborLabelLeft, neighborLabelTop);
				}
			}
		}
	}	

	// second pass to replace labels with their equivalent set representatives
	for (int r = 0; r < Nrows; ++r) {
		for (int c = 0; c < Ncols; ++c) {
			labelMap[r * Ncols + c] = d_set.find_set(labelMap[r * Ncols + c]);
		}
	}
}


// detects thin horizontal bars missed by the slope/Manduchi method
// completely untested, and probably doesn't really work
void Manduchi::simpleRailingDetector(cv::Mat_<cv::Point3f> depthMap, bool *validArr, bool *result)
{
	const float DELTA_Y1 = 2.5;  //difference in distance between a bar and its background
	const int MAX_BAR_WIDTH = 10; //assumed width of the bar in pixels at MAX_BAR_WIDTH_REF_DIST from the robot. 
	                              //everything else will be scaled according to this value
	const float MAX_BAR_WIDTH_REF_DIST = 2.0;
	const float MAX_RANGE = 8.0; //meters

	for (int c = 0; c < IMAGE_WIDTH; ++c) {
		int prevValidRow = -1;
		float prevValidRowDist;

		//find the first valid row
		for (int r = IMAGE_HEIGHT-1; r >= 0; --r) {   //search upwards in the image
			if (validArr[r*IMAGE_WIDTH+c]) {
				prevValidRow = r;
				prevValidRowDist = depthMap[r][c].y;
				break;
			}
		}

		if (prevValidRow == -1) continue;    //no valid row found

		//now find overhangs
		for (int r = prevValidRow - 1; r >= 0; --r) {
			if (!validArr[r*IMAGE_WIDTH+c])
				continue;
			float currDist = depthMap[r][c].y;
 			if (currDist < MAX_RANGE && currDist - prevValidRowDist < -DELTA_Y1 && r - prevValidRow > -15
				&& validArr[(r-1)*IMAGE_WIDTH+c] && fabs(depthMap[r-1][c].y - currDist) < 0.1) {
				//overhang detected!
				int endloop = max(0, r - MAX_BAR_WIDTH);   //*f/distance
				for (int j = c - 2; j < c+2; ++j) {
					for (int i = r; i > endloop; --i) {
						result[i*IMAGE_WIDTH+c] = true;
					}
				}

			}
			prevValidRow = r;
			prevValidRowDist = currDist;
		}
	}
}