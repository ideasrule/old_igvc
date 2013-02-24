#include <iostream>

#include "ipc.h"
#include "Common.h"

#include "MappingConstants.h"
#include "OccupancyMapGenerator.h"
using namespace Pave_Libraries_Navigation;

static OccupancyMapGenerator *generator;

static CostMapType newCostMapType()
{
    CostMapType cmt;
    cmt.xMin = X_MIN;
    cmt.xMax = X_MAX;
    cmt.xUnitSize = UNIT_SIZE;
    cmt.yMin = Y_MIN;
    cmt.yMax = Y_MAX;
    cmt.yUnitSize = UNIT_SIZE;
    return cmt;
}

static void appendLinePoints(const GroundPoint &start, const GroundPoint &end, vector<GroundPoint> &out)
{
	int x0 = start.x / UNIT_SIZE, y0 = start.y / UNIT_SIZE;
	int x1 = end.x / UNIT_SIZE, y1 = end.y / UNIT_SIZE;

	//Bresenham's Line Algorithm from Wikipedia
	int Dx = x1 - x0; 
	int Dy = y1 - y0;
	bool steep = (abs(Dy) >= abs(Dx));
	if (steep) {
        std::swap(x0, y0);
		std::swap(x1, y1);
		// recompute Dx, Dy after swap
		Dx = x1 - x0;
		Dy = y1 - y0;
	}
	int xstep = 1;
	if (Dx < 0) {
		xstep = -1;
		Dx = -Dx;
	}
	int ystep = 1;
	if (Dy < 0) {
		ystep = -1;		
		Dy = -Dy; 
	}
	int TwoDy = 2*Dy; 
	int TwoDyTwoDx = TwoDy - 2*Dx; // 2*Dy - 2*Dx
	int E = TwoDy - Dx; //2*Dy - Dx
	int y = y0;
	int xDraw, yDraw;	

	for (int x = x0; x != x1; x += xstep) {
		if (steep) {
			xDraw = y;
			yDraw = x;
		} else {			
			xDraw = x;
			yDraw = y;
		}

		// plot
		out.push_back(GroundPoint(xDraw * UNIT_SIZE, yDraw * UNIT_SIZE));

		// next
		if (E > 0) {
		    E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
		    y = y + ystep;
		} else {
		    E += TwoDy; //E += 2*Dy;
		}
	}
}

void StereoUpdateHandler(void *data)
{
	VisionUpdateType *update = (VisionUpdateType*)data;
	
	// Handle state update
	{
		StateEstimationType state = update->state;
		Length x = state.Easting;
		Length y = state.Northing;
		Heading h = state.Heading;
		generator->updateState(GroundPoint(x, y), h);
	}

	// Handle detected obstacles
    vector<GroundPoint> points;
	for(int i = 0; i < update->numPoints; i++) {
        points.push_back(GroundPoint(update->xVals[i], update->yVals[i]));
	}

	// Handle laneso
	for(int i = 0; i < update->nsplines; i++) {
		const SplineIPC &spline = update->all_splines[i];
		for (int j = 0; j < spline.numcontrols - 1; j++) {
			GroundPoint start(spline.xcontrols[j], spline.ycontrols[j]);
			GroundPoint end(spline.xcontrols[j+1], spline.ycontrols[j+1]);
			appendLinePoints(start, end, points);
		}
	}

    typedef vector<OccupancyMapGenerator::CostMapOutput> CostMapOutputVector;
    shared_ptr<CostMapOutputVector> squares = generator->processStereoPoints(points);

    double *xUpdates = new double[squares->size()];
    double *yUpdates = new double[squares->size()];
    double *probUpdates = new double[squares->size()];
    for(unsigned int i = 0; i < squares->size(); i++) {
        xUpdates[i] = (double) (*squares)[i].x;
        yUpdates[i] = (double) (*squares)[i].y;
        probUpdates[i] = (*squares)[i].pOccupied;
    }
    
    CostMapType cmt = newCostMapType();
    cmt.numPoints = squares->size();
    cmt.xUpdates = xUpdates;
    cmt.yUpdates = yUpdates;
    cmt.costUpdates = probUpdates;

    std::cerr << "Received " << points.size()
        << " and publishing " << cmt.numPoints << " points" << endl;

    Messages::CostMap.publish(&cmt);

    delete[] xUpdates;
    delete[] yUpdates;
}

int main(int, char **)
{
    generator = new OccupancyMapGenerator();

    IPCConnect("OccupancyMapGenerator");
	Messages::VisionUpdate.subscribe(&StereoUpdateHandler, true, false);

    // what's this for?  2010-11-21  --Josh
	// 
    //Messages::ClearCostMap.subscribe(&ClearCostMapHandler, true, false);

    IPC_dispatch();
}