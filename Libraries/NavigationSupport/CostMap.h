#pragma once
#include "Common.h"
using namespace Pave_Libraries_Common;

#define STARTING_COST 0

namespace Pave_Libraries_Navigation
{
	// Copied from Common
	//typedef struct Pave_Libraries_Common::CostMapType CostMapType;

	class CostMap
	{
	public:
		CostMap(double xMin, double xMax, double yMin, double yMax, double xUnitSize, double yUnitSize);
		~CostMap();
		double get(double x, double y);
		double get(double x, double y, void **stored);
		void set(double x, double y, double cost);
		void set(double x, double y, void *storage);
		double getCell(int x, int y);
		double getCell(int x, int y, void **stored);
		void setCell(int x, int y, double cost);
		void setCell(int x, int y, void *storage);
		void setAll(double value);
		// Parentheses operator to make life easier
		double& operator()(double x, double y);

		// Helpful for streaming a costmap over IPC
		bool equals(CostMapType *update);
		void update(CostMapType *update);

		int getArrayIndex(double x, double y);
		int getArraySize();
		int arrayX(double x);
		int arrayY(double y);
		int round(double d);
		int getNumUnits(double min, double max, double unitSize);
		bool boundsCheck(double x, double y);
		bool boundsCheckByIndex(int arrayXVal, int arrayYVal);
		double getCellX(double x);
		double getCellY(double y);
		double getXFromArrayIndex(int index);
		double getYFromArrayIndex(int index);
		int getArrayIndexByIndex(int arrayXVal, int arrayYVal);

		double getXMin();
		double getXMax();
		double getYMin();
		double getYMax();
		double getXUnitSize();
		double getYUnitSize();

	private:
		double xMin, xMax, xUnitSize;
		double yMin, yMax, yUnitSize;
		unsigned int xArrayMax, yArrayMax;
		double *costs;
		double throwAway;
		void **storage;
	};
}