
#include <duct/filesystem.hpp>

#include <stdio.h>
#include <unicode/unistr.h>

int main() {
	uint64_t size=duct::FileSystem::getFileSize("size.cpp");
	printf("size: %lu\n", size);
}

