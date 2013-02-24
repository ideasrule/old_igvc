#include "debug.h"
#include "NJTransit.h"
#include "DStarPriorityQueue.h"
#include "CostMapInt.h"
#include "Common.h"
#include "FloatingPointMath.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
 
using namespace DStar;
using namespace Pave_Libraries_Navigation;
using std::vector;
using std::cout;

void print(Node ***memory, int minX, int minY, int maxX, int maxY )
{
	for (int i = minY; i < maxY+1; ++i) {
		for (int j = minX; j < maxX+1; ++j) {
			if (memory[j][i] != 0) {
				if (memory[j][i]->g == DOUBLE_INF)
					printf("        -");
				else
					printf(" %8.2f", memory[j][i]->g);
			} else {
				printf("        x");
			}
		}
		cout << endl << endl;
	}

}


void exportGValues(Node ***memory, int minX, int minY, int maxX, int maxY )
{
	FILE *myFile;
	if ( fopen_s(&myFile, "C:\\P12\\GValuesDump.txt", "w") != 0 ) {
		cout << "Oh fuck file couldn't be opened";
		return;
	}
	for (int i = minY; i < maxY+1; ++i) {
		for (int j = minX; j < maxX+1; ++j) {
			if (memory[j][i] != 0) {
				if (memory[j][i]->g == DOUBLE_INF)
					fprintf(myFile, "        -");
				else
					fprintf(myFile, " %8.2f", memory[j][i]->g);
			} else {
				fprintf(myFile, "        x");
			}
		}
		fprintf(myFile, "\n\n");
	}
	fclose(myFile);
}


void printCost(CostMapInt *costmap, int minX, int minY, int maxX, int maxY)
{
	for (int i = minY; i < maxY+1; ++i) {
		for (int j = minX; j < maxX+1; ++j) {
#ifdef DEBUGMAP
			printf(" %6.0f", (double)costmap->get(j, i));
#else
			printf(" %8.0f", (double)costmap->get(j, i));
#endif
		}
		cout << endl;
	}

}


void exportPath(vector<Pave_Libraries_Geometry::point3d>& path) 
{
	ofstream myFile;
	myFile.open("C:\\IGVC_2009\\Navigation\\FDStar\\path.txt");
	for(size_t i = 0; i < path.size(); ++i)
	{
		myFile << path[i].x << '\t' << path[i].y << '\n';
	}
	myFile.close();
}


void dump(Node*** memory, CostMapInt *costmap, double x1, double y1, double x2, double y2)
{
	int x = (int)x1;
	int y = (int)y1;

	ofstream myFile;
	myFile.open("C:\\P12\\pathDump.txt");
	myFile << "Problem Node 1: (" << x1 << ", " << y1 << ")\n"
		<< "Problem Node 2: (" << x2 << ", " << y2 << ")\n\n";
	for (int i = -7; i < 8; ++i) {
		for (int j = -7; j < 8; ++j) {
			Node *s = memory[x+i][y+j];
			int X, Y;
			double G, RHS;
			int bptrX, bptrY;

			X = x+i;
			Y = y+j;
			if (s == 0) {
				G = DOUBLE_INF;
				RHS = DOUBLE_INF;
				bptrX = -1;
				bptrY = -1;
			} else {
				G = s->g;
				RHS = s->rhs;
				if (s->bptr != 0) {
					bptrX = s->bptr->x;
					bptrY = s->bptr->y;
				} else {
					bptrX = 0;
					bptrY = 0;
				}
			}

			myFile << "(" << X << ", " << Y << ")\n"
				<< "g = " << G << "\nrhs = " << RHS << "\n"
				<< "bptr = (" << bptrX << ", " << bptrY << ")\n"
				<< "cost = " << (*costmap)(x+i, y+j) << "\n\n";
		
		}
	}
	myFile.close();
}


void dumpAll(Node*** memory, CostMapInt *costmap, const char *fileName, double x1, double y1, double x2, double y2)
{
	ofstream file;
	file.open(fileName);

	for (int i = (int)x1; i < (int)x2; ++i) {
		for (int j = (int)y1; j < (int)y2; ++j) {
			Node *s = memory[i][j];
			double G, RHS;
			double key1, key2;
			int bptrX, bptrY;

			if (s == 0) {
				G = DOUBLE_INF;
				RHS = DOUBLE_INF;
				bptrX = 0;
				bptrY = 0;
				key1 = 0;
				key2 = 0;
			} else {
				G = s->g;
				RHS = s->rhs;
				if (s->bptr != 0) {
					bptrX = s->bptr->x;
					bptrY = s->bptr->y;
				} else {
					bptrX = 0;
					bptrY = 0;
				}
				key1 = s->key.k1;
				key2 = s->key.k2;
			}

			file << "(" << i << ", " << j << ")\n"
				<< "g = " << G << "\nrhs = " << RHS << "\n"
				<< "bptr = (" << bptrX << ", " << bptrY << ")\n" 
				<< "key = (" << key1 << ", " << key2 << ")\n"
				<< "cost = " << (*costmap)(i, j) << "\n\n";
		
		}
	}
	file.close();
}