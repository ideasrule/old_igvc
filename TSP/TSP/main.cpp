#include "Common.h"
#include "Geometry.h"
#include "GPSTransforms.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace Pave_Libraries_Common;
using namespace Pave_Libraries_Geometry;
using namespace Pave_Libraries_GPSTransforms;

vector<point2d> waypoints;
vector<point2d> temppoints;
int N;
double shortestPath = HUGE_VAL;

double length( vector<point2d> *v )
{
	double length = 0;
	point2d prev = v->at(0);
	for( int i = 1; i < N; v++ )
	{
		length += prev.distanceTo( v->at(i) );
		prev = v->at(i);
	}
	length += prev.distanceTo( v->at(0) );
	return length;
}

void exch( int i, int j )
{
	point2d temp = temppoints[i];
	temppoints[i] = temppoints[j];
	temppoints[j] = temp;
}

void enumerate(int n)
{
	// found a solution
	if( n == N )
	{
		double path = length( &temppoints );
		if( path < shortestPath )
		{
			shortestPath = path;
			for( int i = 0; i < N; i++ )
			{
				waypoints[i] = temppoints[i];
			}
		}
	}

	// continue searching
	for (int i = n; i < N; i++)
	{
		exch(n, i);
		enumerate(n+1);
		exch(i,n);
	}

}

void main(int argc, char* argv[])
{
	// Read the waypoints
	std::string line;
	std::ifstream waypointFile("broadmead.txt");
	if(!waypointFile.is_open())
	{
		cout << "waypoints file not found";
		return;
	}
	vector<double> latitudes, longitudes;
	while(!waypointFile.eof())
	{
		getline(waypointFile, line);
		latitudes.push_back(atof(line.c_str()));
		if(waypointFile.eof())
		{
			latitudes.pop_back();
			break;
		}
		getline(waypointFile, line);
		longitudes.push_back(atof(line.c_str()));
	}

	if(latitudes.size() == 0)
		return;

	for(int i = 0; i < latitudes.size(); i++)
	{
		CartesianLocalPoint p = GPSTransforms::ConvertToLocal(GeodeticPoint(latitudes[i], longitudes[i]));
		point2d point = point2d::spawn(p.X, p.Y);
		waypoints.push_back(point);
		temppoints.push_back(point);
	}
	N = waypoints.size();
	
	enumerate(1);
    
	cout << "print";
	ofstream output ("path.txt");
	if (output.is_open())
	{
		for( int i = 0; i < N; i++)
		{
			char buffer[50];
			double x = waypoints[i].x, y = waypoints[y].y;
			sprintf(buffer, "%f, %f\n", x, y);
		}
		output.close();
	}
	else cout << "Unable to open file";

	return;
}