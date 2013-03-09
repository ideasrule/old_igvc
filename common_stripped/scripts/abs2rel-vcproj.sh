#!/bin/bash

# Recurses through subdirectories, finds all .vcproj
# files, and replaces C:/ with ../../etc. (the equivalent
# relative path.

# Josh Newman
# 2010-11-23

process_dir() {
	local path_to_root="$2/.."
	for dir in $(ls -d "$1"/*/ 2> /dev/null); do
		echo "enter $dir"
		echo "$path_to_root"
		for vcproj in $(ls "$dir"/*.vcproj 2> /dev/null); do
			mv "$vcproj" "$vcproj.old"
			cat "$vcproj.old" | sed "s|C:|$path_to_root|g" > "$vcproj"
		done
		process_dir "$dir" "$path_to_root"
	done
}

process_dir "$(pwd)/$1" ..
