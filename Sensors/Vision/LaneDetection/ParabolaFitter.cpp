#include "ParabolaFitter.h"
#include <stdlib.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define GREEDY_SEARCH_WIDTH 6

ParabolaFitter::ParabolaFitter(int nrows, int ncols)
{	
	cumsum = new float[nrows * ncols + 1];	
	cumsum[0] = 0.0f;
	this->nrows = nrows;
	this->ncols = ncols;
}

ParabolaFitter::~ParabolaFitter()
{
	delete cumsum;
}

//Samples a row-col pair according to cumsum, within the bounds of [minRow, maxRow)
void ParabolaFitter::sampleFromCumsum(float &row, float &col, int minRow, int maxRow)
{
	float minRand = cumsum[minRow * ncols];
	float maxRand = cumsum[maxRow * ncols];
	//Pick a random value in the range of cumsum
	float r = ((float)rand() / (RAND_MAX + 1.0));
	float sample = minRand + r * (maxRand - minRand);
	
	//Find where sample lives in the distribution
	int minIndex = minRow * ncols;
	int maxIndex = maxRow * ncols;	
	while(maxIndex - minIndex > 1)
	{
		int testIndex = (maxIndex + minIndex) / 2;
		float testVal = cumsum[testIndex];
		if(testVal <= sample)
			minIndex = testIndex;
		else
			maxIndex = testIndex;
	}
	row = minIndex / ncols;
	col = minIndex % ncols;
}

//Find the bounds on the lane, chopping off the lower and upper 5% of pixels, in terms of weight
void ParabolaFitter::findLaneExtent(Parabola& p, Mat_<float> &img)
{
	//first, find points bounds for actually being in the image
	int r = 0;
	while(r < ncols && (p.eval(r) >= ncols || p.eval(r) < 0))
		r++;
	p.minDomain = r;
	r = nrows - 1;
	while(r >= 0 && (p.eval(r) >= ncols || p.eval(r) < 0))
		r--;
	p.maxDomain = r;
	
	//max
	float sumThusFar = 0;
	float termSum = p.totalFitness * .05;
	r = p.minDomain;
	while(r < p.maxDomain && sumThusFar <= termSum) {						
		sumThusFar += img[r][(int)p.eval(r)];
		r++;
	}
	p.minDomain = r;
	
	//min
	sumThusFar = 0;
	termSum = p.totalFitness * .04;
	r = p.maxDomain;
	while(r > p.minDomain && sumThusFar <= termSum) {				
		sumThusFar += img[r][(int)p.eval(r)];
		r--;
	}
	p.maxDomain = r;
}

//Extend lane fit greedily
void ParabolaFitter::extendLaneFit(ImageLane * lane, Mat_<float> &img)
{		
	if(lane->maxRow <= lane->minRow)
		return;
	int lastLaneCol = lane->col(lane->minRow);
	float lane_termination_threshold = .05;
	for(int r = lane->minRow - 1; r >= 0; r--)
	{		
		int bestCol = lastLaneCol;
		//search left & right for best pixel in next row
		int c;
		for(c = max(lastLaneCol - GREEDY_SEARCH_WIDTH, 0); c < min(lastLaneCol + GREEDY_SEARCH_WIDTH, ncols - 1); c++)
		{
			if(img[r][c] > img[r][bestCol]) {
				bestCol = c;				
			}
		}
		//end of the line
		if(img[r][bestCol] < lane_termination_threshold)
			break;

		//one row more
		lastLaneCol = bestCol;
		lane->cols[r] = bestCol;
		lane->minRow = r;		
	}
}

//Finds the best parabola in the range minRow to maxRow, which passes through fixed points
Parabola ParabolaFitter::bestParabola(Mat_<float> &img, int nIterations, int minRow, int maxRow, int nFixedPoints, int * fixedRows, int * fixedCols)
{	
	float bestParabolaFitness = -1;
	Parabola bestP;
	for(int iteration = 0; iteration < nIterations; iteration++)
	{
		//Find random points
		float rows[DEGREE];
		float cols[DEGREE];
		for(int i = 0; i < nFixedPoints; i++) {
			rows[i] = fixedRows[i];
			cols[i] = fixedCols[i];
		}
		for(int i = nFixedPoints; i < DEGREE; i++)		
			sampleFromCumsum(rows[i], cols[i], minRow, maxRow);		
				
		Parabola thisP = parabolaForPoints(rows, cols);		

		//Evaluate fitness by looking at integral along the image
		float parabolaFitness = 0;
		for(int i = minRow; i < maxRow; i++)
		{
			int predCol = thisP.eval(i);
			if(predCol >= 0 && predCol < ncols)
				parabolaFitness += img[i][predCol];
		}

		//normalize for length
		//parabolaFitness = parabolaFitness / (maxRow - minRow);

		if(parabolaFitness > bestParabolaFitness) {
			bestP = thisP;
			bestParabolaFitness = parabolaFitness;
		}
	}	
	bestP.maxDomain = maxRow - 1;
	bestP.minDomain = minRow;
	bestP.totalFitness = bestParabolaFitness;
	findLaneExtent(bestP, img);
	return bestP;
}


void ParabolaFitter::findFit(Mat_<float> &img, int nIterations, 
							 int nParabolas, vector<ImageLane *>& lanes)
{	
	lanes.clear();
	//Calculate cumulative distributition function (for sampling)
	for(int i = 0; i < nrows * ncols; i++)	
	{			
		int r = i / ncols;
		int c = i % ncols;
		float thisImgValue = img[r][c];
		cumsum[i + 1] = cumsum[i] + pow(thisImgValue, 2);
	}

	//Find parabolas	
	for(int i = 0; i < nParabolas; i++) {
		//Find parabola 
		Parabola bestP = bestParabola(img, nIterations, 0, nrows, 0, NULL, NULL);
		ImageLane * thisLane = new ImageLane(nrows, bestP);
		
		//extendLaneFit(thisLane, img); //Don't be greedy
		lanes.push_back(thisLane);

		//White out pixels near the detected lane
		for(int r = 0; r < nrows; r++) {
			int predCol = bestP.eval(r);
			int minCol = max(0, predCol - 15);
			int maxCol = min(predCol + 15, ncols - 1);
			for(int col = minCol; col <= maxCol; col++)
				img[r][col] = 0;
		}
	}
}