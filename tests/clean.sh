#!/bin/bash

files=$(find . -maxdepth 2 -type f -name \*.cpp)

for file in $files
do
	bf=${file%.*}
	if [ -f $bf ]; then
		echo "rm $bf"
		rm -f "$bf"
	fi
done

