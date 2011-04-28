#!/bin/bash

relative=$(dirname `pwd`)
CPP=g++
CCFLAGS="-I$relative/include -Wall -pedantic -g -DDEBUG"
LIBS="-licui18n -licudata -licuio -licuuc"
LIBPATH=$relative/lib/linux/libduct_debug.so

compile() {
	filepath=$1
	
	filename=$filepath #$(basename "$filepath")
	extension=${filename##*.}
	filename=${filename%.*}
	
	echo "Compiling $filepath"
	$CPP $CCFLAGS $LIBS -o "$filename.o" -c "$filepath"
	if [ ! $? == 0 ]; then
		echo "compile failed with exit code: $?"
		return 1
	fi
	
	$CPP $LIBS -o "$filename" "$filename.o" $LIBPATH 
	if [ ! $? == 0 ]; then
		echo "link failed with exit code: $?"
		return 1
	fi
	
	if [ -f "$filename.o" ]; then
		rm "$filename.o"
	fi
}

files="$@"
if [ $# == 0 ]; then
	files=$(find ./ -type f -name \*.cpp)
fi

for file in $files
do
	compile "$file"
done

