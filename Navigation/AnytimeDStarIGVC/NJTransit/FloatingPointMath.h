#include <cmath>
#include <limits>

#define DOUBLE_INF std::numeric_limits<double>::infinity()
#define FLOAT_INF std::numeric_limits<float>::infinity()

#pragma region floating_point_comparison

inline bool not_equal(double x, double y)
{
	return ((x-y) > 1.0E-8 || (x-y) < -1.0E-8);
}

inline bool equals(double x, double y)
{
	return (!not_equal(x, y));
}

//x < y
inline bool less_than(double x, double y)
{
	return (y - x > 1.0E-8);
}

//x > y
inline bool greater_than(double x, double y)
{
	return (x - y > 1.0E-8);
}

// x <= y
inline bool less_equal_to(double x, double y)
{
	return (x - y < 1.0E-8);
}

// x >= y
inline bool greater_equal_to(double x, double y)
{
	return (y - x < 1.0E-8);
}
#pragma endregion floating_point_comparison
