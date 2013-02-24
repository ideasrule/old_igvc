#include "Validator.h"
#include "Data.h"

#include "highgui.h"

using namespace Pave_Libraries_Common;

#define CLOSE_DIST 5
#define MIN_NUM_CLOSE 20 //10

bool _success;
double MIN_CONF = Data::getDouble("MinimumLaneConfidence", _success);

//Estimated from ForrestalLoop1
#define N_WHITE_POINTS 21
double whiteHistTicks[N_WHITE_POINTS] =   {0,   DBL_EPSILON, 10,  20,  30,  40,  50,  60,  70,  80,  90,  100, 110, 120, 130, 140, 150, 160, 170, 180, DBL_MAX};
double whitePValues[N_WHITE_POINTS - 1] = {0, .02,        .50, .70, .85, .85, .88, .90, .93, .96, .98, .98, .98, .98, .98, .98, .98, .98, .98, .98};

#define N_YELLOW_POINTS 13
double yellowHistTicks[N_YELLOW_POINTS] =   {0,   DBL_EPSILON,  10,  20,  30,  40,  50,  60,  70,  80,  90,  100,  DBL_MAX};
double yellowPValues[N_YELLOW_POINTS - 1] = {0, .02,         .10, .20, .90, .95, .98, .98, .98, .98, .98, .98};


void Validator::validateLanes(vector<ImageLane *>& lanes, vector<ImageLane *>& validatedLanes)
{
	/*
	//convert confidence to a probability	
	for(int i = 0; i < lanes.size(); i++)
	{
		if(lanes[i]->color == COLOR_WHITE) {
			for(int j = 0; j < N_WHITE_POINTS - 1; j++) {
				if(lanes[i]->confidence >= whiteHistTicks[j] && lanes[i]->confidence < whiteHistTicks[j + 1]) {
					//printf("White %f to %f\n", lanes[i]->confidence , whitePValues[j]);
					lanes[i]->confidence = whitePValues[j];
					break;
				}
			}
		}

		if(lanes[i]->color == COLOR_YELLOW) {
			for(int j = 0; j < N_YELLOW_POINTS - 1; j++) {
				if(lanes[i]->confidence >= yellowHistTicks[j] && lanes[i]->confidence < yellowHistTicks[j + 1]) {
					//printf("Yellow %f to %f\n", lanes[i]->confidence , yellowPValues[j]);
					lanes[i]->confidence = yellowPValues[j];
					break;
				}
			}
		}
	}
	*/

	bool * validFlag = new bool[lanes.size()];
	//innocent until proven guilty
	for(int i = 0; i < (int)lanes.size(); i++)
	{
		if (lanes[i]->confidence > MIN_CONF)
		{
			//printf("Accepting conf %f\n", lanes[i]->confidence);
			validFlag[i] = true;
		}
		else
		{
			//printf("Rejecting conf %f\n", lanes[i]->confidence);
			validFlag[i] = false;
		}
	}

	
	for(int i = 0; i < lanes.size(); i++)
	{	
		
		for(int j = i + 1; j < lanes.size(); j++)
		{
			int nClose = 0;
			int minSharedRow = max(lanes[i]->minRow, lanes[j]->minRow);
			int maxSharedRow = min(lanes[i]->maxRow, lanes[j]->maxRow);
			for(int k = minSharedRow; k < maxSharedRow; k++)
			{
				if(abs(lanes[i]->col(k) - lanes[j]->col(k)) < CLOSE_DIST)
					nClose++;
			}

			//if they're too close, cancel the crappier one 
			if(nClose >= MIN_NUM_CLOSE && lanes[i]->color == lanes[j]->color)
			{
				
				int crappyLane = (lanes[i]->confidence > lanes[j]->confidence) ? j : i;			
				//printf("Lanes %d and %d are too close for comfort. Deleting %d\n", i, j, crappyLane);
				validFlag[crappyLane] = false;

			}
		}
	}
	
	for(int i = 0; i < lanes.size(); i++)
		if(validFlag[i])
			validatedLanes.push_back(lanes[i]);
	delete validFlag;
}

/*
void Validator::handValidateLanes(IplImage *mono, IplImage *color, vector<ImageLane * > &lanes, FILE * f, char *name)
{
	cv::namedWindow("Original Image", 1);
	cv::namedWindow("Hand label overlay", 1);
	cv::imshow("Original Image", color);
	IplImage * overlay = overlayImage(lanes, mono);		
	cvShowImage("Hand label overlay", overlay);
	printf("\nImage %s. Press space to continue.\n", name);
	printf("===============================================\n");
	cvWaitKey(0);
	cvReleaseImage(&overlay);


	for(int i = 0; i < lanes.size(); i++)
	{
		if(lanes[i]->confidence < DBL_EPSILON)
			continue;
		IplImage * overlay = overlayImage(lanes, mono);		
		RgbImage overlayData = RgbImage(overlay);
		lanes[i]->overlayImage(overlay, cvScalar(255, 0, 0), 3) ;
		cvShowImage("Hand label overlay", overlay);
		printf("Lane %d: has color: ", i);
		if(lanes[i]->color == COLOR_WHITE)
			printf("WHITE ");
		else 
			printf("YELLOW ");
		printf("and confidence %f.\n", lanes[i]->confidence);
		
		bool laneIsValid;
		while(true) 
		{
			printf("Valid (V) or invalid (I)?\n");
			int validChar = cvWaitKey(0);						
			if(validChar == 'i' || validChar == 'I') {
				laneIsValid = false;
				break;
			}
			else if(validChar == 'v' || validChar == 'V') {
				laneIsValid = true;
				break;
			}
			else {				
				printf("Character not recognized. Please use 'I' or 'V'\n");
			}
		} 

		fprintf(f, "%d %d %f\n", lanes[i]->color, laneIsValid, lanes[i]->confidence);

		if(laneIsValid)
			printf("Lane is valid.\n");
		else
			printf("Lane is not valid.\n");

		cvReleaseImage(&overlay);
	}	
}
*/