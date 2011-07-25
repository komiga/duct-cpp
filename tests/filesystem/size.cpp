
#include <stdio.h>
#include <unicode/unistr.h>
#include <duct/filesystem.hpp>

int main() {
	uint64_t size=duct::FileSystem::getFileSize("size.cpp");
	printf("size: %lu\n", size);
}

