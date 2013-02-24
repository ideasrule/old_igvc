/*****************************************************************************
 *	PathFinder.cpp
 *
 *  This is the Anytime D* algorithm for path finding. It works on various 
 *  lattice structures defined in Lattice.h 
 *  Currently works only in two dimensions, but will be expanded to 4D
 *
 *  - Tony (Nov 11, 2009)
 *****************************************************************************/


#include "PathPlanner.h"     //all the necessary includes are in here

//use an unnamed namespace to keep variables local
namespace {

	using namespace Pave_Libraries_Geometry;

	DStarPriorityQueue *open = 0;
	list<Node*> closed;
	list<Node*> incons;

	CostMapInt *costmap = 0;

	int xMax, yMax;    // integer coordinates
	int goalX = -1, goalY = -1;
	int startX = -1, startY = -1;
	Node *startNode, *goalNode;
	Node ***memory = 0;
	vector<point3d> prevPath;
	vector<point2d> waypoints;

	bool receivedCostMap = false;
	bool receivedNew = false;
	bool receivedStateEstimation = false;
	bool receivedWaypoints = false;
	bool isInitialSearch = true;
	vector<int> usedXs, usedYs;

	Lattice lattice;

	CostMapTypeInt *cmpInt = 0;
	
	bool planningFailed = false;
	//bool planningFailedInExtraction = false;

	// original floating point coordinates
	double costmapXMax = 200;  
	double costmapXMin = -200;
	double costmapXUnit = 0.10;
	double costmapYMax = 200;
	double costmapYMin = -200;
	double costmapYUnit = 0.10;

	StateEstimationType stateEstimationUpdate;

	int timesExpanded = 0;
	int spinCounter = 0;
}

// Function prototypes
void ExtractPath();
void SpeedFinder(list_point3d& pathList);
double Node::epsilon = 1.0;

double distanceToStart4(int x, int y);
double distanceToStart8(int x, int y);
double distanceToStart16(int x, int y);
double absDistanceToStart(int x, int y);
double euclideanDistanceToStart(int x, int y);
double zeroDistance(int x, int y);

double (*Node::heuristic)(int, int) = &distanceToStart16;   // function pointer to the heuristics function


#pragma region Heuristics
/*******************************************************************************
 Some different heuristics. euclideanDistanceToStart is
 useful for Field D*. Make sure the heuristic is actually admissible (ie smaller
 than the actually possible path). It needs to be actually smaller, not just the
 same. (AHHHHHH!!!!) For FD*, because paths intersect edges and not just nodes,
 h(s_start, s) must be small enough that when added to the cost of any edge 
 emanating from s it is still not greater than a minimum cost path from s_start to s 

 Update Nov 2009: This is not FD* anymore but anytime D*. 
 *******************************************************************************/


double euclideanDistanceToStart(int x, int y)
{
	int dx = startX - x;
	int dy = startY - y;
	if (dx == 0 && dy == 0) 
		return 0;

	return STRAIGHT * sqrt((double)(dx * dx + dy * dy));
}

// direction-4 search. Manhattan distance
double distanceToStart4(int x, int y)
{
	int dx = startX - x;
	int dy = startY - y;
	return abs(dx) + abs(dy);
}


// direction-8 search
double distanceToStart8(int x, int y) 
{
	int dx = startX - x;
	int dy = startY - y;
	if (dx == 0 && dy == 0) 
		return 0; 
	
	double result;
	int dd = abs(dx) - abs(dy);
	if (dd > 0) {
		result = dd * STRAIGHT + abs(dy) * DIAG;
	} else {
		result = (-dd) * STRAIGHT + abs(dx) * DIAG;
	}

	return result;
}


double distanceToStart16(int x, int y) 
{
	int dx = startX - x;
	int dy = startY - y;
	if (dx == 0 && dy == 0) 
		return 0;

	int adx = abs(dx);
	int ady = abs(dy);
	int dd = adx - ady;

	if (adx > 2 * ady) {
		return ady * 2 * HALF_DIAG + STRAIGHT * (adx - 2 * ady);
	} else if (ady > 2 * adx) {
		return adx * 2 * HALF_DIAG + STRAIGHT * (ady - 2 * adx);
	} else if (dd > 0) {
		return dd * 2 * HALF_DIAG + DIAG * (ady - dd);
	} else {
		return (-dd) * 2 * HALF_DIAG + DIAG * (adx + dd);
	}

}


double absDistanceToStart(int x, int y)
{
	int dx = startX - x;
	int dy = startY - y;
	return (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);
}

double zeroDistance(int x, int y)
{
	return 0;
}
/********************************************************************************/
#pragma endregion Heuristics 



inline bool isInMap(int x, int y)
{
	return (x >= 0 && y >= 0 && x <= xMax && y <= yMax);
}



//cost of u from Node(x, y)
double ComputeCost(Node *u, int x, int y)
{		
	int dx = u->x - x;
	int dy = u->y - y;

	//equivalent to pre-making the masks and convolving
	double c = (*costmap)(x, y) + (*costmap)(u->x, u->y);
	int index = (dx + 2) + 5 * (dy + 2);

	switch (index)
	{
	case 1:
		c += (*costmap)(x, y-1) + (*costmap)(x-1, y-1); break;
	case 3:
		c += (*costmap)(x, y-1) + (*costmap)(x+1, y-1); break;
	case 5:
		c += (*costmap)(x-1, y) + (*costmap)(x-1, y-1); break;
	case 9:
		c += (*costmap)(x+1, y) + (*costmap)(x+1, y-1); break;
	case 15:
		c += (*costmap)(x-1, y) + (*costmap)(x-1, y+1); break;
	case 19:
		c += (*costmap)(x+1, y) + (*costmap)(x+1, y+1); break;
	case 21:
		c += (*costmap)(x, y+1) + (*costmap)(x-1, y+1); break;
	case 23:
		c += (*costmap)(x, y+1) + (*costmap)(x+1, y+1); break;
	default:  
		//inner ring; no additional cost
		break;
	}

	c += memory[x][y]->g;

	if ((abs(u->x - x) == 1 && abs(u->y - y) == 2) || (abs(u->x - x) == 2 && abs(u->y - y) == 1))
		c += 2 * HALF_DIAG;
	else if ((u->x - x == 0 && abs(u->y - y) == 1) || (u->y - y == 0 && abs(u->x - x) == 1))
		c += STRAIGHT;
	else if (abs(u->x - x) == 1 && abs(u->y - y) == 1)
		c += DIAG;
	else
		;
		
	return c;
}



void UpdateRHSandBptr(Node *u)
{
	double temp;
	int x = u->x;
	int y = u->y;

	u->bptr = 0;
	u->rhs = DOUBLE_INF;
	for (int i = 0; i < lattice.n; ++i) {

		int x1 = x + lattice.dx[i];
		int y1 = y + lattice.dy[i];
		
		if (!isInMap(x1, y1)) continue;
		Node *t = memory[x1][y1];
		if (t == 0 || t->bptr == u) continue;

		temp = ComputeCost(u, x1, y1);

		if (temp < u->rhs) {
			u->rhs = temp;
			u->bptr = t;
		}	
	}
}



void UpdateState(Node *s)  
{

	if (not_equal(s->g, s->rhs)) {
		if (!s->inClosed) {
			s->recalcKey();
			if (s->inOpen)
				open->update(s);
			else {
				open->add(s);
				s->inOpen = true;
			}
		} else if (!s->inIncons) {
			s->inIncons = true;
			incons.push_back(s);
		}
	} else {
		if (s->inOpen) {
			s->inOpen = false;
			open->del(s);
		}
	}

}


Node *GetOrCreateNode(int x, int y)
{
	Node *v = memory[x][y];
	if (!v) {
		v = new Node(x, y, DOUBLE_INF, DOUBLE_INF);
		usedXs.push_back(x);
		usedYs.push_back(y);
		memory[x][y] = v;
	} else if (v->used == false) {
		usedXs.push_back(x);
		usedYs.push_back(y);
		v->used = true;
	}
	return v;
}



void ComputeOrImprovePath()
{
	while (1) {

		if (timesExpanded > MAX_TIMES_EXPANDED) {
			planningFailed = true;
			return;
		}

		Node *s1 = open->peekTop();
		if (s1 == 0) {
			planningFailed = true;
			return;
		}

		if (less_than(s1->key.k1, startNode->key.k1) || (equals(s1->key.k1, startNode->key.k1) && less_than(s1->key.k2, startNode->key.k2))
			|| (greater_than(startNode->rhs, startNode->g)) || (startNode->rhs == DOUBLE_INF && startNode->g == DOUBLE_INF))
			;
		else
			break; 

		Node *s = open->removeTop();

		if (greater_than(s->g, s->rhs)) {
			s->g = s->rhs;
			if (!s->inClosed) {
				s->inClosed = true;
				closed.push_back(s);
			}
			for (int i = 0; i < lattice.n; ++i) {
				int x = s->x + lattice.dx[i];  
				int y = s->y + lattice.dy[i];
				if (!isInMap(x, y)) 
					continue;
				Node *v = GetOrCreateNode(x, y);
				if (s->bptr == v) continue;

				double newRhs = ComputeCost(v, s->x, s->y);
				if (greater_than(v->rhs, newRhs)) {
					v->bptr = s;
					v->rhs = newRhs;
					UpdateState(v);
				}
			}
			++timesExpanded;

		} else {
			s->g = DOUBLE_INF;
			UpdateState(s);

			for (int i = 0; i < lattice.n; ++i) {
				int x = s->x + lattice.dx[i];
				int y = s->y + lattice.dy[i];
				Node *v = GetOrCreateNode(x, y);
				if (v != goalNode && v->bptr == s) {
					UpdateRHSandBptr(v);
					UpdateState(v);
				}
				
			}
			++timesExpanded;
			
		}
	}

}



void InitialSearch()
{

	Node::epsilon = 1.0;

#ifdef DEBUGMAP
	goalX = 1;
	goalY = 1;
	startX = 39;
	startY = 29;

	xMax = 40;
	yMax = 40;
	costmapXUnit = 1.0;
	costmapYUnit = 1.0;
	costmapXMin = 0;
	costmapYMin = 0;

	costmap = new CostMapInt(xMax+1, yMax+1);

	/* for testing only */
	static bool searched = false;
	if (searched == true) return;
	searched = true;

#else
	if (!receivedNew || !receivedCostMap || !receivedStateEstimation || !receivedWaypoints)
		return;
	receivedNew = false;
	receivedWaypoints = false;

	cout << "Starting search in InitialSearch()" << endl;

	if (!isInMap(goalX, goalY) || !isInMap(startX, startY))
		return;
	// planning failure if we start in an obstacle
	if ((*costmap)(startX, startY) > 280 || (*costmap)(goalX, goalY) > 280) {
		cout << "Planning failed before it began in InitialSearch()" << endl;
		return;
	}


#endif

	LARGE_INTEGER frequency, start, finish;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	// Initialize memory (first time only), which holds pointers to all nodes created
	if (!memory) {
		memory = new Node**[xMax + 1];
		Node **curPtr = new Node* [(xMax+1) * (yMax+1)]();   //parenthesis initializes pointers to 0 
		for( int i = 0; i < xMax+1; i++) { 
			*(memory + i) = curPtr; 
			curPtr += yMax + 1; 
		} 

	} else {
		vector<int>::iterator itX, itY;
		for (itX = usedXs.begin(), itY = usedYs.begin(); itX != usedXs.end(); ++itX, ++itY) {
			memory[*itX][*itY]->reset(); 
		}
		usedXs.resize(0);
		usedYs.resize(0);
	}

	// Initialize OPEN priority queue if first time
	if (open != 0) 
		open->clear();
	else
		open = new DStarPriorityQueue(costmap);

	if (closed.size() != 0)
		closed.clear();
	if (incons.size() != 0)
		incons.clear();

	startNode = new Node(startX, startY, DOUBLE_INF, DOUBLE_INF);
	Node::currStart = startNode;
	startNode->oldStart = startNode;
	goalNode = new Node(goalX, goalY, DOUBLE_INF, 0);
	usedXs.push_back(startX);
	usedYs.push_back(startY);
	usedXs.push_back(goalX);
	usedYs.push_back(goalY);

	memory[startX][startY] = startNode;
	memory[goalX][goalY] = goalNode;

	open->add(goalNode);

	QueryPerformanceCounter(&finish);
	double timeelapsed = ((double)(finish.QuadPart - start.QuadPart))/frequency.QuadPart;
	cout << "Time (initialization): " << timeelapsed << "s" << endl;

	QueryPerformanceCounter(&start);


	ComputeOrImprovePath();

#ifdef DEBUGMAP
	print(memory, 0, 0, xMax, yMax);
#endif

	if (planningFailed) {
		planningFailed = false;
		cout << "Planning Failed with " << timesExpanded << " times expanded." << endl;
		timeelapsed = ((double)(finish.QuadPart - start.QuadPart))/frequency.QuadPart;
		cout << "Time: " << timeelapsed << "s" << endl;
		timesExpanded = 0;
		return;
	}

	ExtractPath();

	isInitialSearch = false;
	PauseDisableType pd;
	pd.Pause = false;
	pd.Disable = false;
	Messages::PauseDisable.publish(&pd);


	QueryPerformanceCounter(&finish);
	timeelapsed = ((double)(finish.QuadPart - start.QuadPart))/frequency.QuadPart;
	cout << "Time (initial search): " << timeelapsed << "s\n";
	cout << "Times expanded: " << timesExpanded << " with epsilon = " << Node::epsilon << endl;
	timesExpanded = 0;

}



void FindPath(void *data, unsigned long a, unsigned long b)
{

	if (isInitialSearch) {
		startX = (int)((stateEstimationUpdate.Easting - costmapXMin) / costmapXUnit);
		startY = (int)((stateEstimationUpdate.Northing - costmapYMin) / costmapYUnit);

		//startX = 1940;
		//startY = 1093;

		//goalX = 1850;
		//goalY = 2150;
		InitialSearch();
	} else {

#ifdef DEBUGMAP
		static bool searched = false;
		if (searched == true) return;
		searched = true;

		vector<int> cellsUpdatedXs, cellsUpdatedYs;
		for (int i = 11; i < 20; ++i) 
			for (int j = 7; j < 17; ++j) {
				cellsUpdatedXs.push_back(i);
				cellsUpdatedYs.push_back(j);
				costmap->set(i, j, 3000, 0);
			}

		//for (int i = 2; i < 9; ++i) 
		//	for (int j = 5; j < 9; ++j) {
		//		cellsUpdatedXs.push_back(i);
		//		cellsUpdatedYs.push_back(j);
		//		costmap->set(i, j, 3000, 2);
		//	}


		for (size_t i = 0; i < cellsUpdatedXs.size(); ++i) {
			int x = cellsUpdatedXs[i];
			int y = cellsUpdatedYs[i];

			for (int j = 0; j < lattice.update_n; ++j) {
				//for each corner of updated cell
				int m = x + lattice.update_dx[j];
				int n = y + lattice.update_dy[j];
				if (!isInMap(m, n)) return;
				Node *s = GetOrCreateNode(m, n);
				if (s != startNode)
					UpdateRHSandBptr(s);
				UpdateState(s); 

			}
			//update itself		
			if (!isInMap(x, y)) return;
			Node *s = GetOrCreateNode(x, y);
			if (s != startNode)
				UpdateRHSandBptr(s);
			UpdateState(s);
		}

#else
		if (goalX == -1 || goalY == -1) return;    //no new waypoints

		if (receivedWaypoints) {
			InitialSearch();
			receivedWaypoints = false;
			return;
		}

		if(!receivedNew || !receivedCostMap || !receivedStateEstimation )
			return;
		receivedNew = false;

		LARGE_INTEGER frequency, start, finish;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);

		int oldStartX = startX;
		int oldStartY = startY;

		startX = (int)((stateEstimationUpdate.Easting - costmapXMin) / costmapXUnit);
		startY = (int)((stateEstimationUpdate.Northing - costmapYMin) / costmapYUnit);

		//startX = 1940;
		//startY = 1093;

		//spinCounter++;
		//if (spinCounter >= 20) spinCounter = 0;
		//if (spinCounter == 0) {
		//	startX = 2000;
		//	startY = 2000;

		//	goalX = 1700;
		//	goalY = 2300;
		//	receivedNew = true;
		//	receivedCostMap = true;
		//	InitialSearch();
		//	return;
		//} else if (spinCounter == 10) {
		//	startX = 2000;
		//	startY = 2000;

		//	goalX = 2500;
		//	goalY = 2500;
		//	receivedNew = true;
		//	receivedCostMap = true;
		//	InitialSearch();
		//	return;
		//}

		//Avoid Oscillations
		int closestX , closestY, closestIndex;
		int closestDistSquare = LARGE_INT;
		for (int i = 0; i < 50; ++i) {
			if (i == prevPath.size()) break;
			int prevPathX = int((prevPath[i].x - costmapXMin) / costmapXUnit + 0.5f);
			int prevPathY = int((prevPath[i].y - costmapYMin) / costmapYUnit + 0.5f);
			int prevDistSquare = (prevPathX - startX) * (prevPathX - startX) + (prevPathY - startY) * (prevPathY - startY);
			if (prevDistSquare < closestDistSquare) {
				closestDistSquare = prevDistSquare;
				closestX = prevPathX;
				closestY = prevPathY;
				closestIndex = i;
			}
		}
		
		if (closestDistSquare < 40) {
			startX = int((prevPath[max(closestIndex-20, 0)].x - costmapXMin) / costmapXUnit + 0.5f);
			startY = int((prevPath[max(closestIndex-20, 0)].y - costmapYMin) / costmapYUnit + 0.5f);
		}
	

		// planning failure if we start in an obstacle
		if ((*costmap)(startX, startY) > 280){
			startX = (int)((stateEstimationUpdate.Easting - costmapXMin) / costmapXUnit);
			startY = (int)((stateEstimationUpdate.Northing - costmapYMin) / costmapYUnit);
		}
			
		if ((*costmap)(startX, startY) > 280 || (*costmap)(goalX, goalY) > 280) {
			cout << "Planning failed before it began." << endl;
			return;
		}
			
		if (memory[startX][startY] != 0) {
			startNode = memory[startX][startY];
		} else {
			startNode = new Node(startX, startY, DOUBLE_INF, DOUBLE_INF);
			memory[startX][startY] = startNode;
			usedXs.push_back(startX);
			usedYs.push_back(startY);
		}
		if (memory[oldStartX][oldStartY] != 0 && memory[oldStartX][oldStartY]->used == false) {
			usedXs.push_back(oldStartX);
			usedYs.push_back(oldStartY);
			memory[oldStartX][oldStartY]->used = true;
		}
		if (startNode->used == false) {
			usedXs.push_back(startX);
			usedYs.push_back(startY);
			memory[startX][startY]->used = true;
		}

		Node::currStart = startNode;

#endif
		//if (Node::epsilon == 1.9)
		//	Node::epsilon = 1.5;
		//else if (Node::epsilon == 1.5)
		//	Node::epsilon = 1.1;
		//else if (Node::epsilon == 1.1)
		//	Node::epsilon = 1.05;
		//else if (Node::epsilon == 1.05)
		//	Node::epsilon = 1.03;
		//else if (Node::epsilon == 1.03)
		//	Node::epsilon = 1.02;
		//else if (Node::epsilon == 1.02)
		//	Node::epsilon = 1.01;
		//else if (Node::epsilon == 1.01)
		//	Node::epsilon = 1.0;
		//else if (Node::epsilon == 1.0) {
		//	isInitialSearch = true;
		//	//Node::epsilon = 2.5;
		//	return;
		//}

		Node::epsilon = 1.0;
			

		vector<Node*> oldOpen = open->clear();
		for (size_t i = 0; i < oldOpen.size(); ++i ) {
			oldOpen[i]->recalcKey();
			open->add(oldOpen[i]);
		}

		for (list<Node*>::iterator it = incons.begin(); it != incons.end(); ++it) {
			(*it)->inIncons = false;	
			if (!(*it)->inOpen) {
				(*it)->recalcKey();
				open->add(*it);
			}
		}
		incons.clear();
		
		for (list<Node*>::iterator it = closed.begin(); it != closed.end(); ++it) {
			(*it)->inClosed = false;
		}
		closed.clear();

		//cout << "Replanning..." << endl;
		ComputeOrImprovePath();
#ifdef DEBUGMAP
		print(memory, 0, 0, xMax, yMax);
#else

		QueryPerformanceCounter(&finish);

		if (planningFailed) {
			planningFailed = false;
			cout << "Planning Failed with " << timesExpanded << " times expanded." << endl;
			double timeelapsed = ((double)(finish.QuadPart - start.QuadPart))/frequency.QuadPart;
			cout << "Time: " << timeelapsed << "s" << endl;
			timesExpanded = 0;
			return;
		}

		if (timesExpanded > 0) {
			cout << timesExpanded << " times expanded, epsilon = " << Node::epsilon << endl;
			timesExpanded = 0;
			double timeelapsed = ((double)(finish.QuadPart - start.QuadPart))/frequency.QuadPart;
			cout << "Time: " << timeelapsed << "s" << endl;
		}
		//cout << "Making path..." << endl;
		ExtractPath();
#endif
	}
}



void ExtractPath()
{
	vector<point3d> currPath;
	Node *s = startNode;
	int currX = startNode->x;
	int currY = startNode->y;

	Pave_Libraries_Geometry::point3d p;
	p.x = currX * costmapXUnit + costmapXMin;
	p.y = currY * costmapYUnit + costmapYMin;
	currPath.push_back(p);

	//exportGValues(memory, 2000, 2000, 2300, 2200);

	do {
		s = s->bptr;
		currX = s->x;
		currY = s->y;

		p.x = currX * costmapXUnit + costmapXMin;
		p.y = currY * costmapYUnit + costmapYMin;
		currPath.push_back(p);

		//if (abs(s->g - s->rhs) > 0.01) {
		//	dump(memory, costmap, currX, currY, 0, 0);
		//}

	} while (s != goalNode);

	prevPath = currPath;
#ifndef DEBUGMAP
	list_point3d container;
	container.numPoints = (int)currPath.size();
	container.points = &currPath[0];

	SpeedFinder(container);

	//cout << "publishing path update... \n";
	Messages::PathUpdate.publish(&container);

	//exportPath(currPath);

#else
	exportPath(currPath);

#endif
		

}



/**************************************************************************
 StateEstimation handler... updates starting location
 **************************************************************************/
void StateEstimationHandler(void *data)
{
	stateEstimationUpdate = *((StateEstimationType*)data);
	receivedStateEstimation = true;
	receivedNew = true;
}



/**************************************************************************
 Determines the maximum velocity of each point on the path, as a function 
 of the curvature and the costs near it. 
 **************************************************************************/
//helper function
double distance2To(const point3d& a, const point3d& b)
{
	return sqrt((a.x - b.x)* (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

//void SpeedFinder2(list_point3d& pathList)
//{
//	double desiredSpeed = 3.0;
//	const double maxV = TranslationalSpeed;
//	const double minV = 0.40;   // in m/s
//	const double maxA = 0.4;    // in m/s^2
//
//	pathList.points[0].z = max(0.0, min(maxV, stateEstimationUpdate.Speed));
//
//	for (int i = 1; i < pathList.numPoints; ++i) {
//		pathList.points[i].z = desiredSpeed;
//	}
//
//
//	for (int i = 1; i < pathList.numPoints; ++i) {
//		// make sure the maximum acceleration is not exceeded
//		for (int j = i; j >= 1; --j) {
//			double dv = pathList.points[j].z - pathList.points[j-1].z;
//			double dx = pathList.points[j].x - pathList.points[j-1].x;
//			double dy = pathList.points[j].y - pathList.points[j-1].y;
//			double dr = sqrt(dx * dx + dy * dy);
//			
//			// dV/dR = (1/V)*(dV/dt)
//			double avgV = (pathList.points[j-1].z + pathList.points[j].z)/2.0;
//			if (dv > maxA * dr / avgV) {
//				pathList.points[j].z = pathList.points[j-1].z + maxA * dr / avgV;
//			} else if (dv < -maxA * dr / pathList.points[j-1].z) {
//				pathList.points[j-1].z = pathList.points[j].z + maxA * dr / avgV;
//			} else {
//				break;
//			}
//		}
//	}
//}

void SpeedFinder(list_point3d& pathList)
{
	//const double maxV = 2.24;   // in m/s, = 5mph
	const double maxV = TranslationalSpeed;
	const double minV = 0.40;   // in m/s
	const double maxA = 0.4;    // in m/s^2

	pathList.points[0].z = max(0.0, min(maxV, stateEstimationUpdate.Speed));
	int maxNumPoints = min(40, pathList.numPoints-1);
	point2d pathHeadingVector(pathList.points[maxNumPoints].x - pathList.points[0].x, 
							  pathList.points[0].x - pathList.points[0].x);
	// normalize this vector
	double temp = sqrt(pathHeadingVector.x * pathHeadingVector.x + pathHeadingVector.y * pathHeadingVector.y);
	if (temp > 1E-8) {
		pathHeadingVector.x /= temp;
		pathHeadingVector.y /= temp;
	}

	for (int i = 1; i < pathList.numPoints; ++i) {

		point3d pointB = pathList.points[i];
		point3d pointA = pathList.points[max(0, i-10)];
		point3d pointC = pathList.points[min(pathList.numPoints - 1, i+10)];

		//calculate menger curvature
		double a = distance2To(pointA, pointB);
		double b = distance2To(pointC, pointB);
		double c = distance2To(pointA, pointC);

		// menger curvature
		double mengerC = sqrt(max(0.0, (a+b+c)*(b+c-a)*(a+c-b)*(a+b-c))) / (a * b * c);
		// a number between 0 and 1
		double mengerFactor = max(0.0, 1.0 - mengerC);

		const int thresholdCost = 280;     //each cell is either occupied or free
		//old: computed max possible clutter using formula maximum += max(0, 1 - (1 - costSlopeFactor) * d / 25) for each cell with costSlopeFactor = 0.5
		// maximum += max(0, 1-(x/25)*(x/25))
		const double maxClutter = 15000; //1600;
		const double clutterNormalization = maxClutter / 5;  
		const double costSlopeFactor = 0.2;    //used in formula to determine how much each cell contribute to the clutter
		double clutter = 0;    // clutter 
		double clutterFactor;  // between 0 and 1 (0 being cluttered and 1 being uncluttered)

		int x = int((pathList.points[i].x - costmapXMin) / costmapXUnit + 0.5);
		int y = int((pathList.points[i].y - costmapYMin) / costmapYUnit + 0.5);

		//TODO: careful... does not check bounds to see if it is in map
		for (int j = -80; j < 80; ++j) {
			for (int k = -80; k < 80; ++k) {
				double d = sqrt(double(j * j + k * k));

				// normalized vector from origin to point on costmap
				point2d mapPoint;
				if (j != 0 && k != 0) {
					mapPoint.x = double(j) / d;
					mapPoint.y = double(k) / d;
				} else {
					mapPoint.x = 0;
					mapPoint.y = 0;
				}

				double currCost;
				currCost = ((*costmap)(x+j, y+k) < thresholdCost) ? 0.0 : max(0.0, 1.0 - (1.0 - costSlopeFactor) * d / 80.0);
				// now take care of the angles (basically cosine of the angle + 1.2 to make everything positive)	
				double multiplier = (pathHeadingVector.x * mapPoint.x + pathHeadingVector.y * mapPoint.y + 1.2);
				currCost *= multiplier;
				clutter += currCost;
			}
		}	
		clutterFactor = max(0.0, 1 - clutter / clutterNormalization);

		// stop at waypoint
		double distToGoal = distance2To(pathList.points[pathList.numPoints - 1], pointB);
		// start decelerating at 2.5 metres to goal
		double accelerationFactor = (distToGoal > 2.5) ? 1 : distToGoal / 2.5;

		pathList.points[i].z = (minV + (maxV - minV) * min(mengerFactor, clutterFactor)) * accelerationFactor;
		//pathList.points[i].z = (minV + (maxV - minV) * min(mengerFactor, clutterFactor));

		// make sure the maximum acceleration is not exceeded
		for (int j = i; j >= 1; --j) {
			double dv = pathList.points[j].z - pathList.points[j-1].z;
			double dx = pathList.points[j].x - pathList.points[j-1].x;
			double dy = pathList.points[j].y - pathList.points[j-1].y;
			double dr = sqrt(dx * dx + dy * dy);
			
			// dV/dR = (1/V)*(dV/dt)
			double avgV = (pathList.points[j-1].z + pathList.points[j].z)/2.0;
			if (dv > maxA * dr / avgV) {
				pathList.points[j].z = pathList.points[j-1].z + maxA * dr / avgV;
			} else if (dv < -maxA * dr / pathList.points[j-1].z) {
				pathList.points[j-1].z = pathList.points[j].z + maxA * dr / avgV;
			} else {
				break;
			}

		}
	}

	//for (int i = 1; i < pathList.numPoints; ++i) {
	//	pathList.points[i].z = 0;
	//}
}


/**************************************************************************
 Converts doubles and stuff to int (i.e. from CostMap to CostMapInt)
 TODO: change costmap generator to generate integer costmap directly
 **************************************************************************/
void CostMapConvert(CostMapType costmapUpdate)
{
	if (cmpInt != 0) {
		delete [] cmpInt->xUpdates;
		delete [] cmpInt->yUpdates;
		delete [] cmpInt->costUpdates;
	} else {
		cmpInt = new CostMapTypeInt;
	}

	cmpInt->xMax = (int)((costmapUpdate.xMax - costmapUpdate.xMin) / costmapUpdate.xUnitSize);
	cmpInt->yMax = (int)((costmapUpdate.yMax - costmapUpdate.yMin) / costmapUpdate.yUnitSize);
	cmpInt->numPoints = costmapUpdate.numPoints;
	cmpInt->xUpdates = new int[cmpInt->numPoints];
	cmpInt->yUpdates = new int[cmpInt->numPoints];
	cmpInt->costUpdates = new int[cmpInt->numPoints];
	for (int i = 0; i < cmpInt->numPoints; i++) {
		double currX = (int)((stateEstimationUpdate.Easting - costmapXMin) / costmapXUnit);
		double currY = (int)((stateEstimationUpdate.Northing - costmapXMin) / costmapXUnit);

		cmpInt->xUpdates[i] = (int)((costmapUpdate.xUpdates[i] - costmapUpdate.xMin) / costmapUpdate.xUnitSize);
		cmpInt->yUpdates[i] = (int)((costmapUpdate.yUpdates[i] - costmapUpdate.yMin) / costmapUpdate.yUnitSize);
		double wtf = costmapUpdate.costUpdates[i];
		if (wtf > 0.01 && (abs(cmpInt->xUpdates[i] - currX) > 5 || abs(cmpInt->yUpdates[i] - currY) > 5))
			cmpInt->costUpdates[i] = 50000;
		else
			cmpInt->costUpdates[i] = 0;
	}

}   



/**************************************************************************
 CostMap handler
 updates costmap for D* path planning
 **************************************************************************/
void CostMapHandler(void *data)
{
	LARGE_INTEGER frequency, start, finish;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	CostMapConvert(*(reinterpret_cast<CostMapType*>(data)));
	receivedCostMap = true;
	receivedNew = true;

	static bool **cellsUpdated;
	if(costmap == NULL || !costmap->equals(cmpInt))
	{
		//Todo: actually delete memory here, not just resetting them
		cout << "Creating new costmap...\n";
		if (costmap != NULL) {
			delete costmap;
			delete [] cellsUpdated[0];
			delete [] cellsUpdated;
		}

		/* save the original number for back conversion in the end */
		CostMapType *costmapUpdate = reinterpret_cast<CostMapType*>(data);
		
		//TODO: Hack this for IGVC 2010
		//costmapXMax = costmapUpdate->xMax; 
		//costmapXMin = costmapUpdate->xMin;
		//costmapXUnit = costmapUpdate->xUnitSize;
		//costmapYMax = costmapUpdate->yMax;
		//costmapYMin = costmapUpdate->yMin;
		//costmapYUnit = costmapUpdate->yUnitSize;

		xMax = cmpInt->xMax;
		yMax = cmpInt->yMax;
		costmap = new CostMapInt(xMax+1, yMax+1);

		// Variables and arrays to keep track of which cells have been updated and what their old values are
		cellsUpdated = new bool*[xMax+1];    //cells changed in both cmpInt and squarecosts
		bool *curPtr1 = new bool[(xMax+1) * (yMax+1)]();
		for( int i = 0; i < xMax+1; i++) { 
			*(cellsUpdated + i) = curPtr1; 
			curPtr1 += yMax + 1; 
		} 
	}

	static vector<int> cellsUpdatedXs;
	static vector<int> cellsUpdatedYs;

	int cellUpdateCount = 0;   //take out in release!!!
	static const double cellsD = 6;
	static int cells = 5;

	costmap->update(cmpInt); 

	//QueryPerformanceCounter(&finish);
	//double timeelapsed = ((double)(finish.QuadPart - start.QuadPart))/frequency.QuadPart;
	//cout << "Map Update Time: " << timeelapsed << " s\n";

	// Clears cellsUpdated, cellsUpdatedXs, and cellsUpdatedYs.
	//cout << "Cells Updated: " << cellUpdateCount << endl;

	if (!isInitialSearch) {
		for (size_t i = 0; i < costmap->cellsUpdatedXs.size(); ++i) {
			int x = costmap->cellsUpdatedXs[i];
			int y = costmap->cellsUpdatedYs[i];

			for (int j = 0; j < lattice.update_n; ++j) {
				//for each corner of updated cell
				int m = x + lattice.update_dx[j];
				int n = y + lattice.update_dy[j];
				if (!isInMap(m, n)) continue;
				if (memory[m][n] == 0) continue;
				Node *s = GetOrCreateNode(m, n);
				if (s->bptr == 0) continue;
				if (s != startNode) {
					UpdateRHSandBptr(s);
					UpdateState(s); 
				}
			}
			//update itself		
			if (!isInMap(x, y)) continue;
			if (memory[x][y] == 0) continue;
			Node *s = GetOrCreateNode(x, y);
			if (s->bptr == 0) continue;
			if (s != startNode) {
				UpdateRHSandBptr(s);
				UpdateState(s);
			}
		}
	}

	//use resize() instead of clear() to keep capacity unchanged
	//this is also done in the costmap internal function, but I'm keeping this here in case no points are updated
	costmap->cellsUpdatedXs.resize(0);
	costmap->cellsUpdatedYs.resize(0);

	QueryPerformanceCounter(&finish);
	double timeelapsed = ((double)(finish.QuadPart - start.QuadPart))/frequency.QuadPart;
	cout << "Map Update Time: " << timeelapsed << " s\n";

}



/**************************************************************************
 Updates waypoints
 **************************************************************************/
void SetWaypointsHandler(void *data)
{ 
	if (!receivedCostMap)
		return;
	SetWaypointsType *wp = (SetWaypointsType*)data;
	if (wp->numPoints > 0) {
		int goalXNew = (int)((wp->points[0].x - costmapXMin) / costmapXUnit);
		int goalYNew = (int)((wp->points[0].y - costmapYMin) / costmapYUnit);
		if (goalXNew != goalX || goalYNew != goalY) {
			goalX = goalXNew;
			goalY = goalYNew;
			receivedWaypoints = true;
			receivedNew = true;
		}
	} else {
		goalX = -1;
		goalY = -1;
	}
	waypoints.clear();
	for(int i = 0; i < wp->numPoints; i++) {
		waypoints.push_back(wp->points[i]);
	}
}


void MaxSpeedHandler(void *data)
{
	MaxSpeedUpdate *sp = (MaxSpeedUpdate*)data;
	TranslationalSpeed = sp->speed;
}