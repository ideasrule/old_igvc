#pragma once
#include "Common.h"
#include "cv.h"
#include <cassert>
using namespace Pave_Libraries_Common;

namespace Pave_Libraries_Navigation
{

	// Cost Map Update using integer representation, used internally for NJTransitcmcmcm
	struct CostMapTypeInt
	{
		int xMax;
		int yMax;

		// Updated points
		int numPoints;
		int *xUpdates;
		int *yUpdates;
		int *costUpdates;
	};

	void CreateCircularMask(int radius, vector<vector<int>> &mappingMask);

	// Copied from Common
	//typedef struct Pave_Libraries_Common::CostMapType CostMapType;

	class CostMapInt
	{
	public:
		CostMapInt(int xMax, int yMax);
		~CostMapInt();

		int get(int x, int y) const { assert( x >= 0 && x <= xMax && y >= 0 && y <= yMax); return outputCosts[x][y]; }
		// not including squareCosts
		int getRawCost(int x, int y) const { assert( x >= 0 && x <= xMax && y >= 0 && y <= yMax); return costs[x][y]; }

		//padding is the number of padding cells to be filled
		//void set(int x, int y, int cost, int padding);   
		//void setAll(int value);

		// Helpful for streaming a costmap over IPC
		bool equals(CostMapTypeInt *update);
		void update(CostMapTypeInt *update);

		int getArraySize();
		int getXMax();
		int getYMax();
		
		vector<int> cellsUpdatedXs;
		vector<int> cellsUpdatedYs;
		vector<vector<int>> mappingMask;

		// Parentheses operator to make life easier
		int operator()(const int x, const int y) const { assert( x >= 0 && x <= xMax && y >= 0 && y <= yMax); return outputCosts[x][y]; }

	private:

		int xMax, yMax;

		static const int BUFFER_SIZE = 8;
		static const int COST_THRESHOLD = 280;
		static const int STARTING_COST = 0;
		static const int BUFFER_MULTIPLIER = 1;   //cost of a buffered cell is BUFFER_MULTIPLIER * (BUFFER_SIZE - distance)

		// raw costs (i.e. the actual obstacle points only)
		int **costs;    //2D array for easier access
		int *costPtr;   //1D array that actually stores the costs

		CvMat *hardObstacles;

		//the final flattened costmap with buffering and shit
		int **outputCosts;
		int *outputCostPtr;

	};
}