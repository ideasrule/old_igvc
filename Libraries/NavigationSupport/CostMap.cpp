#include "CostMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Define for automatic bounds checking
#define BOUNDSCHECK

namespace Pave_Libraries_Navigation
{
	CostMap::CostMap(double xMin, double xMax, double yMin, double yMax, double xUnitSize, double yUnitSize)
	{
		this->xMin = xMin;
		this->xMax = xMax;
		this->xUnitSize = xUnitSize;
		this->yMin = yMin;
		this->yMax = yMax;
		this->yUnitSize = yUnitSize;

		this->xArrayMax = getNumUnits(xMin, xMax, xUnitSize) - 1;
		this->yArrayMax = getNumUnits(yMin, yMax, yUnitSize) - 1;

		this->costs = (double*)malloc((this->xArrayMax + 1) * (this->yArrayMax + 1) * sizeof(double));
		this->storage = (void**)calloc((this->xArrayMax + 1) * (this->yArrayMax + 1), sizeof(double));

		setAll(STARTING_COST);
	}
	
	// Find the number of units required to express a range, given its units
	int CostMap::getNumUnits(double min, double max, double unitSize)
	{
		return round((max - min) / unitSize) + 1;
		/*
		double val = (max - min) / unitSize;
		int numUnits = (int)val;
		double integer;
		// Check to see if a a spillover cell is needed
		if(abs(modf(val, &integer)) > 0.00000001)
			numUnits++;
		return numUnits;*/
	}

	CostMap::~CostMap()
	{
		free(costs);
		free(storage);
	}

	bool CostMap::equals(CostMapType *update)
	{
		if(update == NULL)
			return false;

		if(update->xMin != xMin || update->xMax != xMax || update->xUnitSize != xUnitSize)
			return false;

		if(update->yMin != yMin || update->yMax != yMax || update->yUnitSize != yUnitSize)
			return false;

		return true;
	}

	void CostMap::update(CostMapType *update)
	{
		if(update == NULL) return;
		for(int i = 0; i < update->numPoints; i++)
			set(update->xUpdates[i], update->yUpdates[i], update->costUpdates[i]);
	}

	int CostMap::arrayX(double x)
	{
		return round((x - xMin) / xUnitSize);
	}

	int CostMap::arrayY(double y)
	{
		return round((y - yMin) / yUnitSize);
	}

	int CostMap::round(double d)
	{
		if(d - floor(d) >= .5)
			return (int)d + 1;
		return (int)d;
	}

	double CostMap::get(double x, double y)
	{
		int arrayXVal = arrayX(x);
		int arrayYVal = arrayY(y);
#ifdef BOUNDSCHECK
		if(!boundsCheckByIndex(arrayXVal, arrayYVal))
			return 0;
#endif
		int index = arrayXVal * (yArrayMax + 1) + arrayYVal;
		return costs[index];
	}

	int CostMap::getArrayIndexByIndex(int arrayXVal, int arrayYVal)
	{
#ifdef BOUNDSCHECK
		if(!boundsCheckByIndex(arrayXVal, arrayYVal))
			return 0;
#endif
		return arrayXVal * (yArrayMax + 1) + arrayYVal;
	}

	int CostMap::getArrayIndex(double x, double y)
	{
		int arrayXVal = arrayX(x);
		int arrayYVal = arrayY(y);
		return getArrayIndexByIndex(arrayXVal, arrayYVal);
	}

	double CostMap::getXFromArrayIndex(int index)
	{
		return xMin + xUnitSize * (index / (yArrayMax + 1));
	}

	double CostMap::getYFromArrayIndex(int index)
	{
		return yMin + yUnitSize * (index % (yArrayMax + 1));
	}

	double CostMap::get(double x, double y, void **storage)
	{
		int arrayXVal = arrayX(x);
		int arrayYVal = arrayY(y);
		#ifdef BOUNDSCHECK
		if(!boundsCheckByIndex(arrayXVal, arrayYVal))
			return 0;
		#endif
		int index = arrayXVal * (yArrayMax + 1) + arrayYVal;
		*storage = this->storage[index];
		return costs[index];
	}

	void CostMap::setAll(double value)
	{
		int size = getArraySize();
		for(int i = 0; i < size; i++)
			costs[i] = value;
	}

	void CostMap::set(double x, double y, double cost)
	{
#ifdef BOUNDSCHECK
		if(!boundsCheck(x, y))
			return;
#endif
		costs[arrayX(x) * (yArrayMax + 1) + arrayY(y)] = cost;
	}

	void CostMap::set(double x, double y, void *storage)
	{
#ifdef BOUNDSCHECK
		if(!boundsCheck(x, y))
			return;
#endif
		this->storage[arrayX(x) * (yArrayMax + 1) + arrayY(y)] = storage;
	}
	
	double CostMap::getCell(int x, int y)
	{
#ifdef BOUNDSCHECK
		if(x < 0 || x > xArrayMax || y < 0 || y > yArrayMax)
			return 0;
#endif
		return costs[x * (yArrayMax + 1) + y];
	}

	double CostMap::getCell(int x, int y, void **stored)
	{
#ifdef BOUNDSCHECK
		if(x < 0 || x > xArrayMax || y < 0 || y > yArrayMax)
			return 0;
#endif
		int index = x * (yArrayMax + 1) + y;
		*stored = storage[index];
		return costs[index];
	}

	void CostMap::setCell(int x, int y, double cost)
	{
#ifdef BOUNDSCHECK
		if(x < 0 || x > xArrayMax || y < 0 || y > yArrayMax)
			return;
#endif
		costs[x * (yArrayMax + 1) + y] = cost;
	}

	void CostMap::setCell(int x, int y, void *storage)
	{
#ifdef BOUNDSCHECK
		if(x < 0 || x > xArrayMax || y < 0 || y > yArrayMax)
			return;
#endif
		this->storage[x * (yArrayMax + 1) + y] = storage;
	}

	double& CostMap::operator ()(double x, double y)
	{
#ifdef BOUNDSCHECK
		if(!boundsCheck(x, y))
			return throwAway;
#endif 
		return costs[arrayX(x) * (yArrayMax + 1) + arrayY(y)];
	}

	bool CostMap::boundsCheck(double x, double y)
	{
		int arrayXVal = arrayX(x);
		int arrayYVal = arrayY(y);
		bool result = boundsCheckByIndex(arrayXVal, arrayYVal);
		//if(!result)
		//	printf("Error: Can't get(%f, %f) in CostMap(%f, %f, %f, %f)\n", x, y, xMin, xMax, yMin, yMax);
		return result;
	}

	bool CostMap::boundsCheckByIndex(int arrayXVal, int arrayYVal)
	{
		if(arrayXVal < 0 || arrayXVal > xArrayMax || arrayYVal < 0 || arrayYVal > yArrayMax)
			return false;
		return true;
	}

	int CostMap::getArraySize()
	{
		return (this->xArrayMax + 1) * (this->yArrayMax + 1);
	}

	double CostMap::getCellX(double x)
	{
		return xMin + arrayX(x) * xUnitSize;
	}

	double CostMap::getCellY(double y)
	{
		return yMin + arrayY(y) * yUnitSize;
	}

	// Accessors
	double CostMap::getXMin()
	{
		return xMin;
	}
	double CostMap::getXMax()
	{
		return xMax;
	}
	double CostMap::getYMin()
	{
		return yMin;
	}
	double CostMap::getYMax()
	{
		return yMax;
	}
	double CostMap::getXUnitSize()
	{
		return xUnitSize;
	}
	double CostMap::getYUnitSize()
	{
		return yUnitSize;
	}
}