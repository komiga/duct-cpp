
#include <duct/csv.hpp>
#include <duct/csvtemplate.hpp>

#include <stdio.h>
#include <iostream>
#include <unicode/ustream.h>

using namespace duct;

char const* inpath="tpl.csv";
UChar32 const sepchar=',';
unsigned int const headercount=0;

CSVTRecord const __tpl_layout[]={
	{VARTYPE_BOOL, false},
	{VARTYPE_BOOL, false},
	{VARTYPE_INTEGER, true},
	{VARTYPE_FLOAT, true},
	{VARTYPE_ANY, true}
};

CSVTemplate tpl(5, __tpl_layout);
CSVTemplate tpl2(5, NULL); // only requires a count of 5

void printRow(CSVRow const& row) {
	printf("{%s, %s} [%d:%lu] ", tpl.validate(row) ? "true " : "false", tpl2.validate(row) ? "true " : "false", row.getIndex(), row.getCount(true));
	icu::UnicodeString temp;
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
	} else {
		printf("Failed to read %s\n", inpath);
		return 1;
	}
	return 0;
}

