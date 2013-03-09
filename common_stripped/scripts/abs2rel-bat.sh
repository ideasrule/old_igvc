#!/bin/bash

# Edits batch files in .\IGVC\Launch so they use relative paths.

# Josh Newman
# 2010-11-24

for bat in $(ls IGVC/Launch/*.bat 2> /dev/null); do
	echo "$bat"
	mv "$bat" "$bat.old"
	cat "$bat.old" | sed 's|C:|..\\..|g' | unix2dos > "$bat"
done
