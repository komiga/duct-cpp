#!/bin/bash

if [ $1 == 0 ]; then
	echo "No parameters given"
	exit 1
fi

filepath=$1
relative=$(dirname `pwd`)/lib/linux/debug
cd $(dirname "$filepath")
execname=$(basename "$filepath")
LD_LIBRARY_PATH=$LD_LIBRARYPATH:$relative ./$execname $2 $3 $4 $5 $6 $7 $8

