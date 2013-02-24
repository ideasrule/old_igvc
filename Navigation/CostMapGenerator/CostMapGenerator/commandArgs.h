#ifndef INCLUDE_COMMANDARGS_H
#define INCLUDE_COMMANDARGS_H

#include <stdio.h>

void printUsageAndExit()
{
	fprintf(stderr, "USAGE: CostMapGenerator.exe [options]\n");
	fprintf(stderr, "Generate a cost map using vision updates\n\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "-lanes                \t consider lanes");
	fprintf(stderr, "-interpolateLanes     \t infer costs to ensure contiguous lanes");
	fprintf(stderr, "-generateWaypoints    \t generate waypoints from new lanes");
	fprintf(stderr, "-boundaries           \t load and send boundary points from file specified in config");
	exit(1);
}

/* This structure describes the command line flags were used when the program was started. */

class commandArgs
{
public:
	bool lanes;
	bool interpolateLanes;
	bool generateWaypoints;
	bool boundaries;
};

//Parse command line arguments
commandArgs * parseArgs(int argc, char** argv)
{
	commandArgs * args = new commandArgs();
	
	// Set to defaults:
	args->lanes = false;
	args->interpolateLanes = false;
	args->generateWaypoints = false;
	args->boundaries = false;
	//args->boundaryFile = NULL;

	// first argument is always program name --- advance to next one
	argc--; argv++;
	while(argc > 0)
	{
		if(!strcmp(*argv, "-lanes")) 
			args->lanes = true;
		
		else if(!strcmp(*argv, "-interpolateLanes")) 
			args->interpolateLanes = true;
		
		else if(!strcmp(*argv, "-generateWaypoints")) 
			args->generateWaypoints = true;
		
		else if(!strcmp(*argv, "-boundaries"))
			args->boundaries = true;
		
		else if(!strcmp(*argv, "-help"))			
			printUsageAndExit();
		
		else
		{
			fprintf(stderr, "Invalid program argument %s\n", *argv);
			printUsageAndExit();			
		}
		
		argv++; argc--;
	}

	return args;
}

#endif