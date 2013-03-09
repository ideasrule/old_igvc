#!/usr/bin/env python

# Recurses through subdirectories, finds all .sln
# files, and build them.

# Josh

import os
import sys

# make sure visual studio vars are set
if os.system('which devenv') is not 0:
	print 'No devenv found in path'
	print 'Run vsvars32.bat in location given by environment variable VS80COMNTOOLS'
	sys.exit(1)
	
ignore_dirs = ('.svn', 'GeneratedFiles', 'generatedfiles',
               '_UpgradeReport_Files', 'Debug', 'Release')

def prune_list(dirs):
    for dir in ignore_dirs:
        if dir in dirs:
            dirs.remove(dir)
    
def build_cmd(sln_file, configuration):
	return 'devenv ' + sln_file + ' /build ' + configuration

def build_recursively(start_dir, configurations):
	for root, dirs, files in os.walk(start_dir):
		prune_list(dirs)
	
		sln_files = [file for file in files if os.path.splitext(file)[1] == '.sln']
		for sln_file in sln_files:
			for configuration in configurations:
				print "\nBuilding %s in configuration %s" % (sln_file, configuration)
				sys.stdout.flush()
				os.system(build_cmd(os.path.join(root, sln_file), configuration))

build_recursively(sys.argv[1], sys.argv[2:])

