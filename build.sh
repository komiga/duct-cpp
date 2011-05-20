#!/bin/bash

ACTION="$1"
BUILD="$2"

# guess OS
if [ $# -lt 2 ]; then
	unamestr=`uname`
	if [ "$unamestr" == "MINGW32_NT-5.1" ]; then # msysgit
		BUILD="vs2008"
	fi
fi

if [ -z "$BUILD" ]; then
	BUILD="gmake"
fi

function clean_output() {
	if [ "$BUILD" == "gmake" ]; then
		make config=debug clean
		make config=release clean
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
fi

if [ "$BUILD" == "gmake" ]; then
	if [ -z "$ACTION" ]; then
		make config=debug
		make config=release
	else
		make config="$ACTION"
	fi
fi

#clean_output
