
#include <duct/FileSystem.hpp>

#include <stdio.h>
#include <iostream>
#include <unicode/ustream.h>

using namespace std;
using namespace duct;

void testFile(icu::UnicodeString const& path) {
	cout<<"(file) \""<<path<<'\"';
	if (FileSystem::fileExists(path)) {
		cout<<" exists"<<endl;
	} else {
		cout<<" does not exist"<<endl;
	}
}

void testDir(icu::UnicodeString const& path) {
	cout<<"(dir) \""<<path<<'\"';
	if (FileSystem::dirExists(path)) {
		cout<<" exists"<<endl;
	} else {
		cout<<" does not exist"<<endl;
	}
}

int main() {
	icu::UnicodeString dirname("asdf");
	icu::UnicodeString filename=dirname+"/asdf.foobar";
	testDir(dirname);
	testFile(filename);
	printf("createDir:%d\n", FileSystem::createDir(dirname));
	printf("createFile:%d\n", FileSystem::createFile(filename));
	testDir(dirname);
	testFile(filename);
	
	DirStream ds(dirname);
	if (ds.isOpen()) {
		icu::UnicodeString name;
		while (ds.nextEntry(name)) {
			printf("entry type:%d parent/relative:%d entry name:\"", ds.entryType(), ds.isEntryParentOrRelative());
			cout<<name<<'\"'<<endl;
		}
		printf("close: %d\n", ds.close());
	} else {
		cout<<"Could not open \""<<dirname<<"\""<<endl;
	}
	
	printf("deleteFile:%d\n", FileSystem::deleteFile(filename));
	printf("deleteDir:%d\n", FileSystem::deleteDir(dirname));
	return 0;
}

