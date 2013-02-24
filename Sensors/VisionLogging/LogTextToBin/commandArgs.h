#pragma once

#include <stdio.h>

void printUsageAndExit()
{
	fprintf(stderr, "USAGE: ImageViewer [options]\n");
	fprintf(stderr, "Displays obstacle detection results offline.\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "\t-readLog dir basename   \tread logged input from a folder in dir\n");
	getchar();
	exit(1);
}

/* This structure describes the command line flags were used when the program was started. */

class commandArgs
{
public:
	const char *readLogDir;
	const char *readLogBasename;
  
    //Parse command line arguments
    commandArgs(int argc, char** argv)
    {
	    // Set to defaults:
	    this->readLogDir = NULL;
	    this->readLogBasename = NULL;
    	
	    // first argument is always program name --- advance to next one
	    argc--; argv++;
	    while(argc > 0)
	    {
		    if (!strcmp(*argv, "-readLog"))
		    {
			    argc--; argv++;
			    if (argc == 0)
			    {
				    fprintf(stderr, "Must specify log directory\n");
				    printUsageAndExit();
			    }
			    this->readLogDir = *argv;
			    argc--; argv++;
			    if (argc == 0)
			    {
				    fprintf(stderr, "Must specify log basename to read\n");
				    printUsageAndExit();
			    }
			    this->readLogBasename = *argv;
		    }
		    else
		    {
			    fprintf(stderr, "Invalid program argument %s\n", *argv);
			    printUsageAndExit();			
		    }
    		
		    argv++; argc--;
	    }
    }
};
