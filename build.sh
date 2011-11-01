#!/bin/bash

ACTION="$1"
BUILD="$2"

# guess OS
if [ $# -lt 2 ]; then
	unamestr=`uname`
	if [[ "$unamestr" == MINGW32_NT* ]]; then # msysgit
		BUILD="vs2008"
	fi
fi

if [ -z "$BUILD" ]; then
	BUILD="gmake"
fi

function clean_output() {
	if [ "$BUILD" == "gmake" ]; then
		make config=all clean
	fi
	rm -rf "out"
}

if [ "$ACTION" == "clean" ]; then
	clean_output
	premake4 clean
	exit
fi

if [ ! -f "Makefile" ]; then
	premake4 $BUILD
	if [ "$BUILD" == "gmake" ]; then
		# fix `/usr/lib`-firstness for 32-bit and 64-bit
		sed -i -e 's/ -L\/usr\/lib32//g' -e 's/ -L\/usr\/lib64//g' duct.make
	fi
fi

if [ "$BUILD" == "gmake" ]; then
	if [ -z "$ACTION" ]; then
		make config=all
	else
		make config="$ACTION"
	fi
fi

