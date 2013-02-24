#pragma once

namespace Pave_Libraries_GPSTransforms
{
	class CartesianLocalPoint
	{
	public:
		CartesianLocalPoint();
		CartesianLocalPoint(double x, double y, double z);
		double X;
		double Y;
		double Z;
	};
}