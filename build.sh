#!/bin/bash

ACTION="$1"

function clean_output() {
	make config=debug clean
	make config=release clean
	rm -rf "out"
}

if [ "$ACTION" == "clean" ]; then
	clean_output
	premake4 clean
	exit
fi

if [ ! -f "Makefile" ]; then
	premake4 gmake
fi

if [ -z "$ACTION" ]; then
	make config=debug
	make config=release
else
	make config="$ACTION"
fi

#clean_output
