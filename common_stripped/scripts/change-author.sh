#!/bin/bash

# you need to have server-side SVN permissions to change properties
# to run this.

repo="https://svn.princetonpave.org/svn/PAVE"

author="$1"
shift

while (( "$#" )); do
    echo "Getting log message for revision $1..."
    svn log -r "$1" https://svn.princetonpave.org/svn/PAVE
    echo
    read -p "Confirm change author to $author? (y/n) "
    if [ "$REPLY" == "y" ]; then
        svn ps svn:author --revprop -r "$1" "$author" "$repo"
    fi
    shift
    echo
done
