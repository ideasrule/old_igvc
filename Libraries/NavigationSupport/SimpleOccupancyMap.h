#pragma once

#include "Array2D.h"
#include "OccupancyMap.h"

namespace Pave_Libraries_Navigation {

class SimpleOccupancyMap : public OccupancyMap
{
public:
	SimpleOccupancyMap(int x, int y, LogOdds minValue, LogOdds maxValue);
	SimpleOccupancyMap(int xMin, int xMax, int yMin, int yMax, LogOdds minValue, LogOdds maxValue);
	virtual ~SimpleOccupancyMap();

    virtual void update(int x, int y, Probability inverseSensorModelValue);
    virtual Probability pOccupied(int x, int y) const;
    virtual LogOdds lgoOccupied(int x, int y) const;

    virtual int minX() const { return lgoObstacle.minX(); }
    virtual int maxX() const { return lgoObstacle.maxX(); }
    virtual int minY() const { return lgoObstacle.minY(); }
    virtual int maxY() const { return lgoObstacle.maxY(); }

private:
	Array2D<LogOdds> lgoObstacle;
    const LogOdds MIN_VALUE;
    const LogOdds MAX_VALUE;

	// Initial value of all cells is LogOdds(0) = Probability(0.5) (no info known)
	static const LogOdds INITIAL_VALUE;
	void clear();
};

} // namespace Pave_Libraries_Navigation