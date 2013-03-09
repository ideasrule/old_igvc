#!/usr/bin/env python

# Updates a sln file and the necessary projects, and opens it.

import sys
import os
import os.path

def print_usage_and_exit():
	print 'Please specify a single solution directory (with one .pro file).'
	print '  Ex. ' + os.path.basename(sys.argv[0]) + ' Sensors/Vision'
	sys.exit(1)

if len(sys.argv) < 2:
	print_usage_and_exit()

initial_path = os.getcwd();
for path in sys.argv[1:]:
	project_path = path;
	os.chdir(os.path.join(initial_path, project_path))

	files = [obj for obj in os.listdir('.') if os.path.isfile(obj)]
	pro_files = [file for file in files if os.path.splitext(file)[1] == '.pro']

	if len(pro_files) != 1:
		print_usage_and_exit()
		
	pro_file = pro_files[0]

	sln_file = os.path.splitext(pro_file)[0] + '.sln'
	os.system('devenv ' + sln_file)
