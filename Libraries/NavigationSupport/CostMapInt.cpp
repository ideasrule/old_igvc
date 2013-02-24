#include "CostMapInt.h"
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <cassert>
#include <algorithm>

// Define for automatic bounds checking
//#define BOUNDSCHECK

namespace Pave_Libraries_Navigation
{

	//radius is radius of the hard buffer
	//TODO: finish implementing this
	void CreateCircularMask(int radius, vector<vector<int>> &mappingMask)
	{
		int dim = 2 * radius + 1;
		mappingMask.resize(dim);
		for (int i = 0; i < dim; ++i) {
			mappingMask[i].resize(dim);
		}

		//iterate from -radius to +radius, skipping 0
		for (int i = -radius; i <= radius; ++i) {
			for (int j = -radius; j <= radius; ++j) {
				//assign value to mask
				if (i*i+j*j <= radius * radius) {
					mappingMask[i + radius][j + radius] = 1; //1+int(sqrtf(float(i*i+j*j))/2);
				} else {
					mappingMask[i + radius][j + radius] = 0;
				}
			}
		}
	}



	CostMapInt::CostMapInt(int xMax, int yMax)
	{
		this->xMax = xMax;
		this->yMax = yMax;

		hardObstacles = cvCreateMat(yMax+1, xMax+1, CV_8UC1);
		cvZero( hardObstacles );

		costs = new int*[xMax+1];
		int *costPtr1 = new int[(xMax+1) * (yMax+1)];
		costPtr = costPtr1;
		for( int i = 0; i < xMax+1; ++i) { 
			*(costs + i) = costPtr1; 
			costPtr1 += yMax + 1; 
		} 
		std::fill(costPtr, costPtr+(xMax+1)*(yMax+1), STARTING_COST);


		outputCosts = new int*[xMax+1];
		int *outputCostPtr1 = new int[(xMax+1) * (yMax+1)]();
		outputCostPtr = outputCostPtr1;
		for( int i = 0; i < xMax+1; i++) { 
			*(outputCosts + i) = outputCostPtr1; 
			outputCostPtr1 += yMax + 1; 
		}

		cellsUpdatedXs.reserve(1000);
		cellsUpdatedYs.reserve(1000);

		CreateCircularMask(15, this->mappingMask);

	}
	
	CostMapInt::~CostMapInt()
	{
		delete [] costs;
		delete [] costPtr;
		delete [] outputCosts;
		delete [] outputCostPtr;
		 
		cvReleaseMat(&hardObstacles);
	}

/* TODO: Should be called compatible() */
	bool CostMapInt::equals(CostMapTypeInt *update)
	{
		if(update == NULL)
			return false;

		if(update->xMax+1 != xMax)
			return false;

		if(update->yMax+1 != yMax)
			return false;

		return true;
	}



	void CostMapInt::update(CostMapTypeInt *updateList) 
	{
		//even though the cost may be any number, it is thresholded here so it is effectively a binary operation

		int minUpdateX = xMax+1, maxUpdateX = -1, minUpdateY = yMax+1, maxUpdateY = -1;

		for (int k = 0; k < updateList->numPoints; k++) {
			int x = updateList->xUpdates[k];
			int y = updateList->yUpdates[k];
			int cost = updateList->costUpdates[k];

			int oldCost = costs[x][y];
			costs[x][y] = cost;

			if ((oldCost > COST_THRESHOLD && cost > COST_THRESHOLD) ||
				(oldCost <= COST_THRESHOLD && cost <= COST_THRESHOLD)) {
				// do nothing, since nothing has changed
				continue;

			} else if (oldCost <= COST_THRESHOLD && cost > COST_THRESHOLD) {
				// add hard buffer by convolving with mask
				for (int i = 0; i < (int)mappingMask.size(); ++i) {
					int currX = x - (mappingMask.size() / 2) + i;
					if (currX < 0 || currX > xMax) continue;
					for (int j = 0; j < (int)mappingMask[i].size(); ++j) {
						int currY = y - (mappingMask[i].size() / 2) + j;
						if (currY < 0 || currY > yMax) continue;
						int temp = int(cvGet2D(hardObstacles, currX, currY).val[0] + 1E-8) + mappingMask[i][j];
						cvSet2D(hardObstacles, currX, currY, cvScalar(temp));
					}
				}

			} else if (oldCost > COST_THRESHOLD && cost <= COST_THRESHOLD) {
				// remove hard buffer by convolving with negative of mask
				for (int i = 0; i < (int)mappingMask.size(); ++i) {
					int currX = x - (mappingMask.size() / 2) + i;
					if (currX < 0 || currX > xMax) continue;
					for (int j = 0; j < (int)mappingMask[i].size(); ++j) {
						int currY = y - (mappingMask[i].size() / 2) + j;
						if (currY < 0 || currY > yMax) continue;
 						int temp = int(cvGet2D(hardObstacles, currX, currY).val[0] + 1E-8);
						int temp2 = temp - mappingMask[i][j];
						cvSet2D(hardObstacles, currX, currY, cvScalar(temp2));
					}
				}
			}

			if (x - (int)(mappingMask.size()/2) < minUpdateX)	minUpdateX = x - (mappingMask.size() / 2);
			if (x + (int)(mappingMask.size()/2) > maxUpdateX)	maxUpdateX = x + (mappingMask.size() / 2);
			if (y - (int)(mappingMask.size()/2) < minUpdateY)	minUpdateY = y - (mappingMask.size() / 2);
			if (y + (int)(mappingMask.size()/2) > maxUpdateY)	maxUpdateY = y + (mappingMask.size() / 2);

			if (minUpdateX < 0)		 minUpdateX = 0;
			if (minUpdateY < 0)		 minUpdateY = 0;
			if (maxUpdateX > xMax)   maxUpdateX = xMax;
			if (maxUpdateY > yMax)   maxUpdateY = yMax;
		}

		if (minUpdateX > xMax || minUpdateY > yMax || maxUpdateX < 0 || maxUpdateY < 0) //no points updated 
			return;

		// we use 2 * BUFFER_SIZE to unambiguously determine what the new values in the ring outside of the updated rectangle should be
		int minUpdateXEnlarged2 = (minUpdateX - 2*BUFFER_SIZE < 0) ? 0 : minUpdateX - 2*BUFFER_SIZE;
		int minUpdateYEnlarged2 = (minUpdateY - 2*BUFFER_SIZE < 0) ? 0 : minUpdateY - 2*BUFFER_SIZE;
		int maxUpdateXEnlarged2 = (maxUpdateX + 2*BUFFER_SIZE > xMax) ? xMax : maxUpdateX + 2*BUFFER_SIZE;
		int maxUpdateYEnlarged2 = (maxUpdateY + 2*BUFFER_SIZE > yMax) ? yMax : maxUpdateY + 2*BUFFER_SIZE;


		CvMat *enlarged = cvCreateMat(maxUpdateXEnlarged2 - minUpdateXEnlarged2 + 1, maxUpdateYEnlarged2 - minUpdateYEnlarged2 + 1, CV_8UC1);
		cvGetSubRect(hardObstacles, enlarged, cvRect(minUpdateYEnlarged2, minUpdateXEnlarged2, 
													   maxUpdateYEnlarged2 - minUpdateYEnlarged2 + 1, maxUpdateXEnlarged2 - minUpdateXEnlarged2 + 1));
		
		CvMat *binaryEnlarged = cvCreateMat(maxUpdateXEnlarged2 - minUpdateXEnlarged2 + 1, maxUpdateYEnlarged2 - minUpdateYEnlarged2 + 1, CV_8UC1);	
		cvThreshold(enlarged, binaryEnlarged, 0, 1, CV_THRESH_BINARY_INV);

		CvMat *distTransform = cvCreateMat(maxUpdateXEnlarged2 - minUpdateXEnlarged2 + 1, maxUpdateYEnlarged2 - minUpdateYEnlarged2 + 1, CV_32FC1);
		cvDistTransform(binaryEnlarged, distTransform);

		// update the ring around the updated rectange (i.e. use BUFFER_SIZE here, not 2*BUFFER_SIZE)
		int minUpdateXEnlarged1 = (minUpdateX - BUFFER_SIZE < 0) ? 0 : minUpdateX - BUFFER_SIZE;
		int minUpdateYEnlarged1 = (minUpdateY - BUFFER_SIZE < 0) ? 0 : minUpdateY - BUFFER_SIZE;
		int maxUpdateXEnlarged1 = (maxUpdateX + BUFFER_SIZE > xMax) ? xMax : maxUpdateX + BUFFER_SIZE;
		int maxUpdateYEnlarged1 = (maxUpdateY + BUFFER_SIZE > yMax) ? yMax : maxUpdateY + BUFFER_SIZE;

		cellsUpdatedXs.resize(0);
		cellsUpdatedYs.resize(0);

		//cout << "\n\n\n";

		for (int i = minUpdateXEnlarged1; i <= maxUpdateXEnlarged1; ++i) {
			for (int j = minUpdateYEnlarged1; j <= maxUpdateYEnlarged1; ++j) {
				
				double dist = cvmGet(distTransform, i - minUpdateXEnlarged2, j - minUpdateYEnlarged2);
					
				//cout << std::setw(4) << (int)dist;
				//cout << std::setw(4) << int(cvGet2D(binaryEnlarged, i - minUpdateXEnlarged2, j - minUpdateYEnlarged2).val[0]); 

				int newOutputCost;
				if (dist == 0)
					newOutputCost = 1000000;  //TODO: change this to something not so magical
				else if (dist > BUFFER_SIZE)
					newOutputCost = 0;
				else
					newOutputCost = int(BUFFER_MULTIPLIER * (BUFFER_SIZE - dist + 1));

				int temp = outputCosts[i][j];
				if (outputCosts[i][j] != newOutputCost) {
					outputCosts[i][j] = newOutputCost;
					cellsUpdatedXs.push_back(i);
					cellsUpdatedYs.push_back(j);
				}

				//if (outputCosts[i][j] >= 1000)
				//	cout << "   x";
				//else
				//	cout << std::setw(4) << outputCosts[i][j];

			}
			//cout << '\n';
		}



		cvReleaseMat(&enlarged);
		cvReleaseMat(&distTransform);

	} 


	int CostMapInt::getArraySize()
	{
		return (xMax + 1) * (yMax + 1);
	}


	// Accessors
	int CostMapInt::getXMax()
	{
		return xMax;
	}

	int CostMapInt::getYMax()
	{
		return yMax;
	}

}