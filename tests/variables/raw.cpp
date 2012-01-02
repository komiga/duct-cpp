
#include <duct/variables.hpp>

#include <stdio.h>
#include <iostream>
#include <unicode/ustdio.h>
#include <unicode/ustream.h>

using namespace duct;
using namespace std;

void printValue(ValueVariable* var) {
	icu::UnicodeString str;
	cout<<var->getType()<<"; ";
	var->getValueFormatted(str, FMT_ALL_DEFAULT);
	cout<<str<<", ";
	var->getNameFormatted(str, FMT_NAME_DEFAULT);
	cout<<str<<endl;
}

int main() {
	icu::UnicodeString source="1.0";
	ValueVariable* var=Variable::stringToValue(source);
	printValue(var);
	delete var;
	source=".0";
	var=Variable::stringToValue(source);
	printValue(var);
	delete var;
	return 0;
}

