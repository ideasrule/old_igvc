#pragma once

#include "assert.h"

// Row-major order array
// so in memory, rows are laid out contiguously
template<typename T>
class Array2D
{
public:
    Array2D(int x, int y) : X_SIZE(x), Y_SIZE(y), xMin(0), yMin(0) {
        data = new T[X_SIZE * Y_SIZE];
        assert(data);
    }

    // specify how you want to index the array
    Array2D(int xMin, int xMax, int yMin, int yMax) :
        X_SIZE(xMax - xMin + 1), Y_SIZE(yMax - yMin + 1), xMin(xMin), yMin(yMin)
    {
        data = new T[X_SIZE * Y_SIZE];
        assert(data);
    }

    ~Array2D(void) {
        delete[] data;
    }

	// Accessor to allow easy updates
    inline T& cell(int x, int y) {
        x -= xMin;
        y -= yMin;

		assertAdjustedValid(x, y);

	    return data[x*Y_SIZE + y];
    }

    inline const T& cell(int x, int y) const {
        x -= xMin;
        y -= yMin;

		assertAdjustedValid(x, y);

	    return data[x*Y_SIZE + y];
    }

    // the minimum allowed X index (inclusive)
    inline int minX() const { return xMin; }

    // the maximum allowed X index (exclusive)
    inline int maxX() const { return xMin + X_SIZE; }

    // the minimum allowed Y index (inclusive)
    inline int minY() const { return yMin; }

    // the maximum allowed Y index (exclusive)
    inline int maxY() const { return yMin + Y_SIZE; }

    void setAll(T value)
    {
        for(int x = minX(); x < maxX(); x++) {
            for(int y = minY(); y < maxY(); y++) {
			    cell(x, y) = value;
		    }
	    }
    }

private:
	const int X_SIZE;
	const int Y_SIZE;

    // the min and max values used as indices, 0 by default
    int xMin, yMin;

	// row-major order
	// if this is changed, cell()
	T *data;

	// assert that adjusted indices (subtract xMin or yMin) are valid
	inline void assertAdjustedValid(int x, int y) const {
		assert(x >= 0);
		assert(x < X_SIZE);
		assert(y >= 0);
		assert(y < Y_SIZE);
	}
};
