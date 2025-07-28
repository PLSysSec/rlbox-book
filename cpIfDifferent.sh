#!/bin/bash

# A copy command that only copies if there is a difference between the files
# This avoids spuriously marking file timestamp as newer
#   and thus can speed up the build process

if [ ! -f $2 ] || [[ $(diff $1 $2) ]]; then
		cp $1 $2
fi
