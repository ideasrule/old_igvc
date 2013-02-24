#include "tests.h"

#include <cmath>

// add declarations here, and put functions in the array
void construction();
void getAndSet();

TestFunction OCCUPANCY_MAP_TESTS[] = {
	construction,
    getAndSet,
	0
};

bool equals(double d1, double d2)
{
    return fabs(d1 - d2) < 1e-6;
}

void construction()
{
    cout << "construction 0 x 0 map..." << endl;
	OccupancyMap m1(0, 0, -10.0, 10.0);
    cout << "construction 100 x 100 map..." << endl;
	OccupancyMap m2(100, 100, -10.0, 10.0);
    cout << "construction 10000 x 10000 map..." << endl;
	OccupancyMap m3(10000, 10000, -10.0, 10.0);
}

void getAndSet_applyUpdate(OccupancyMap& m, Probability (*updateFunc)(int x, int y))
{
    for(int x = m.minX(); x < m.maxX(); x++) {
        for(int y = m.minY(); y < m.maxY(); y++) {
            m.update(x, y, updateFunc(x, y));
        }
    }
}

Probability getAndSet_update1(int, int)
{
    return 0.95;
}

Probability getAndSet_update2(int, int)
{
    return 0.01;
}

void getAndSet_testMap(OccupancyMap& m)
{
    getAndSet_applyUpdate(m, getAndSet_update1);

    for(int x = m.minX(); x < m.maxX(); x++) {
        for(int y = m.minY(); y < m.maxY(); y++) {
            assert(equals(m.lgoOccupied(x, y),
                probabilityToLogOdds(getAndSet_update1(x, y))));
        }
    }

    getAndSet_applyUpdate(m, getAndSet_update2);

    for(int x = m.minX(); x < m.maxX(); x++) {
        for(int y = m.minY(); y < m.maxY(); y++) {
            assert(equals(m.lgoOccupied(x, y),
                probabilityToLogOdds(getAndSet_update1(x, y)) +
                probabilityToLogOdds(getAndSet_update2(x, y))));
        }
    }
}

void getAndSet()
{
    cout << "getAndSet 10 x 10 map..." << endl;
    OccupancyMap m1(10, 10, -10.0, 10.0);
    getAndSet_testMap(m1);
    cout << "getAndSet 10000 x 10000 map..." << endl;
    OccupancyMap m2(10000, 10000, -10.0, 10.0);
    getAndSet_testMap(m2);
}
