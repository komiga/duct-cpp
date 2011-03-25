
#include <stdio.h>
#include <iostream>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include "duct/charutils.hpp"

using namespace duct;

int main() {
	UnicodeString str("\t\n\r\\{}=\"");
	UnicodeString out;
	CharUtils::escapeString(out, str, FMT_STRING_ESCAPE_ALL);
	std::cout<<out<<std::endl;
	str.setTo("\"\t\n\r\\{}=\\\"\"");
	CharUtils::escapeString(out, str, FMT_STRING_ESCAPE_ALL);
	std::cout<<out<<std::endl;
	return 0;
}
