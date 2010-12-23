#!/bin/bash

if [ $# == 0 ]; then
	echo "No parameters given"
	exit 1
fi

filepath="$1"
relative=$(dirname `pwd`)
filedir=$(dirname "$filepath")
#cd $filedir
execname=$(basename "$filepath")
libpath=$(winepath -w "$relative/lib/win32/debug")
#ln -s $relative/lib/win32/libduct.dll.0.1
wine cmd /c run.bat "$libpath" "$filedir" "$execname"
#rm libduct.dll.0.1
