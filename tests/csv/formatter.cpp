
#include <stdio.h>
#include <iostream>
#include <unicode/ustream.h>
#include <duct/csv.hpp>

using namespace duct;

const char* inpath="in.csv";
const char* outpath="out.csv";
const UChar32 sepchar=',';
const unsigned int headercount=1;

void printRow(const CSVRow& row) {
	printf("[%d:%lu] ", row.getIndex(), row.getCount(true));
	UnicodeString temp;
	CSVFormatter::formatRow(row, temp, sepchar);
	std::cout<<temp<<std::endl;
}

int main() {
	CSVMap* map=NULL;
	try {
		map=CSVFormatter::loadFromFile(inpath, sepchar, headercount);
	} catch (CSVParserException e) {
		printf("caught exception:\n%s\n", e.what());
		return 1;
	}
	if (map) {
		CSVRowMap::const_iterator iter;
		for (iter=map->begin(); iter!=map->end(); ++iter) {
			printRow(*iter->second);
		}
		if (CSVFormatter::writeToFile(map, outpath, sepchar, "utf8", FMT_STRING_SAFE)) {
			printf("Wrote %s\n", outpath);
		} else {
			printf("Failed to write %s\n", outpath);
			return 1;
		}
	} else {
		printf("Failed to read %s\n", inpath);
		return 1;
	}
	return 0;
}

