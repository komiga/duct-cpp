
#include "duct/debug.hpp"
#include "duct/filesystem.hpp"
#include "duct/filestream.hpp"
#include "duct/endianstream.hpp"
#include "duct/streamwrapper.hpp"
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <iostream>

using namespace std;
using namespace duct;

int main(int argc, char* argv[]) {
	const char* data="あa";
	if (argc>1) {
		data=argv[1];
	}
	UnicodeString str(data);
	const char* encoding="utf-8";
	if (argc>2) {
		encoding=argv[2];
	}
	const char* path="data/utf8";
	if (argc>3) {
		path=argv[3];
	}
	char operation='r';
	if (argc>4) {
		operation=argv[4][0];
	}
	unsigned int mode;
	if (operation=='r') {
		mode=STREAM_READABLE;
	} else if (operation=='w') {
		mode=STREAM_WRITEABLE;
	} else if (operation=='b') {
		mode=STREAM_READABLE|STREAM_WRITEABLE;
		//FileSystem::deleteFile(path);
		FileSystem::createFile(path);
	} else {
		printf("Unknown operation\n");
		return 1;
	}
	FileStream* fs=FileStream::openFile(path, mode, encoding);
	if (fs) {
		printf("encoding: %s\n", fs->getEncoding());
		if (operation=='r') {
			while (!fs->eof()) {
				fs->readLine(str);
				cout<<str<<endl;
			}
		} else if (operation=='b') {
			printf("writeLine count: %lu\n", fs->writeLine(str));
			fs->seek(0);
			fs->readLine(str);
			cout<<str<<endl;
		} else {
			printf("writeLine count: %lu\n", fs->writeLine(str));
		}
		delete fs;
	} else {
		printf("Unable to open %s\n", path);
		return 1;
	}
	return 0;
}

