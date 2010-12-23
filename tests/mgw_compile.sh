#!/bin/bash

relative=$(dirname `pwd`)
CPP=i586-mingw32msvc-g++
CCFLAGS="-I$relative/include -Wall -pedantic -g -DDEBUG"
LIBPATH=$relative/lib/win32/debug/libduct.dll

compile() {
	filepath=$1
	
	filename=$filepath #$(basename "$filepath")
	extension=${filename##*.}
	filename=${filename%.*}
	
	echo "Compiling $filepath"
	$CPP $CCFLAGS -o "$filename.o" -c "$filepath"
	if [ ! $? == 0 ]; then
		echo "compile failed with exit code: $?"
		return 1
	fi
	
	$CPP -o "$filename.exe" "$filename.o" $LIBPATH
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

