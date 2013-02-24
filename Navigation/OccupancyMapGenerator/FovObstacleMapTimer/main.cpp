#include "FovObstacleMap.h"
#include "PerformanceTimer.h"
#include <wykobi.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace Pave_Libraries_Navigation;

int main(void)
{
	FovObstacleMap fom(-1000, 1000, wykobi::PI/3);

	fom.initialize(0);

	CPerformanceTimer timer;
	timer.Start();

	for(int i = 0; i < 5000; i++) {
		int x = rand() % 1000;  // good random numbers are unnecessary here
		int y = rand() % 1000;
		fom.putObstacle(GridSquare(x, y));
	}

	timer.Stop();

	// prevent loop above from being optimized away
	int obstacleCount = 0;
	for(int x = fom.minX(); x < fom.maxX(); x++) {
		for(int y = fom.minY(); y < fom.maxY(); y++) {
			if(fom.getState(GridSquare(x, y)) == FovObstacleMap::OBSTACLE) {
				obstacleCount++;
			}
		}
	}
	std::cout << "Obstacle count: " << obstacleCount << std::endl;

	std::cout << "Elapsed time: " << timer.Interval_mS() << " ms" << std::endl;

	system("pause");

	return 0;
}
