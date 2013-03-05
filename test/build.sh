#!/bin/bash

ACTION="$1"
BUILD="$2"

# guess OS
if [ $# -lt 2 ]; then
	unamestr=`uname`
	if [[ "$unamestr" == MINGW32_NT* ]]; then # msysgit
		BUILD="vs2010"
	fi
fi

if [ -z "$BUILD" ]; then
	BUILD="gmake"
fi

function clean_output() {
	if [ "$BUILD" == "gmake" ]; then
		make config=all clean
	fi
}

if [ "$ACTION" == "clean" ]; then
	clean_output
	premake4 clean
	exit
fi

if [ ! -f "Makefile" ]; then
	premake4 ${@:3} $BUILD
fi

if [ "$BUILD" == "gmake" ]; then
	if [ -z "$ACTION" ]; then
		make config=all
	else
		make config="$ACTION"
	fi
fi
