#!/usr/bin/env python

import os
import sys

start_dir = sys.argv[1]

extensions = set()

for root, dirs, files in os.walk(start_dir):
    for file in files:
        extensions.add(os.path.splitext(file)[1])

print "\n".join(sorted(extensions))
