#ifndef INCLUDE_COMMANDARGS_H
#define INCLUDE_COMMANDARGS_H

#include <stdio.h>
#include "Data.h"

void printUsageAndExit()
{
	fprintf(stderr, "USAGE: Vision.exe [options]\n");
	fprintf(stderr, "Runs obstacle and (optionally) lane detection.\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "\t-help                   \tdisplays this help message\n");
	fprintf(stderr, "\t-logInput dir           \tlog input to a new folder in dir\n");
	fprintf(stderr, "\t-logOnly dir            \tlog input to a new folder in dir, and do nothing else\n");
	fprintf(stderr, "\t-readLog dir basename   \tread logged input from a folder in dir\n");
	fprintf(stderr, "\t-detectLanes            \tdetect lanes\n");
	fprintf(stderr, "\t-laneOverlayDir dir     \tsaves the overlay image in dir\n");
	fprintf(stderr, "\t-showOriginal           \tshows original image from camera\n");
	fprintf(stderr, "\t-showOverlayImage       \tshows images with overlayed lanes\n");
	fprintf(stderr, "\t-ipc                    \tsends and recieves IPC messages\n");	
	fprintf(stderr, "\t-saveInput [dir]        \tsaves input files in [dir]\n");
	fprintf(stderr, "\t-transformDemo          \tlets user play with transform\n");
	fprintf(stderr, "\t-time                   \treport timing information\n");
	fprintf(stderr, "\t-topDown                \tShows local frame\n");
	fprintf(stderr, "\t-useThreads             \tuses threads. If -useThreads and \n\t-readLog are both arguments, readLogs takes precedence.\n");
	getchar();
	exit(1);
}

/* This structure describes the command line flags were used when the program was started. */

enum cameraType {LIVE, FILES};
enum fileListType {ONE_FILE, LIST_OF_FILES, LEFT_RIGHT_LIST};

class commandArgs
{
public:
	char * logToDir;
	bool logOnly;
	char * readLogDir;
	char * readLogBasename;
	bool detectLanes;
    bool showOriginal;
	bool showOverlayImage;
	bool time;
	bool ipc;
	bool transformDemo;
	bool topDown;

	bool useThreads;
};

//Parse command line arguments
commandArgs * parseArgs(int argc, char** argv)
{	
	commandArgs * args = new commandArgs();		
	
	// Set to defaults:
	args->logToDir = NULL;
	args->logOnly = false;
	args->readLogDir = NULL;
	args->readLogBasename = NULL;
	args->detectLanes = false;
	args->useThreads = false;
	
	// first argument is always program name --- advance to next one
	argc--; argv++;
	while(argc > 0)
	{
		if (!strcmp(*argv, "-logInput"))
		{
			argc--; argv++;
			if (argc == 0)
			{
				fprintf(stderr, "Must specify log directory\n");
				printUsageAndExit();
			}
			args->logToDir = *argv;
		}

		else if (!strcmp(*argv, "-logOnly"))
		{
			argc--; argv++;
			if (argc == 0)
			{
				fprintf(stderr, "Must specify log directory\n");
				printUsageAndExit();
			}
			args->logToDir = *argv;
			args->logOnly = true;
		}

		else if (!strcmp(*argv, "-readLog"))
		{
			argc--; argv++;
			if (argc == 0)
			{
				fprintf(stderr, "Must specify log directory\n");
				printUsageAndExit();
			}
			args->readLogDir = *argv;
			argc--; argv++;
			if (argc == 0)
			{
				fprintf(stderr, "Must specify log basename to read\n");
				printUsageAndExit();
			}
			args->readLogBasename = *argv;
		}

        else if(!strcmp(*argv, "-detectLanes"))
		{
            args->detectLanes = true;
		}
		
        else if(!strcmp(*argv, "-showOriginal")) 
			args->showOriginal = true;

		else if(!strcmp(*argv, "-showOverlayImage"))
			args->showOverlayImage = true;

		else if(!strcmp(*argv, "-topDown"))
			args->topDown = true;

		else if(!strcmp(*argv, "-ipc"))
			args->ipc = true;	

		else if(!strcmp(*argv, "-help"))			
			printUsageAndExit();

		else if(!strcmp(*argv, "-time"))			
			args->time = true;
	
		else if(!strcmp(*argv, "-useThreads"))	
			args->useThreads = true;
	
		else
		{
			fprintf(stderr, "Invalid program argument %s\n", *argv);
			printUsageAndExit();			
		}
		
		argv++; argc--;
	}

	if(args->useThreads && args->logToDir) {
		args->useThreads = false;
		fprintf(stderr, "cannot useThreads and logInput, ignoring useThreads...\n");
	}

	return args;
}

#endif