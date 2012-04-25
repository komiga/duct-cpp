
#include <duct/CSV.hpp>

#include <stdio.h>

using namespace duct;

int main() {
	CSVMap map;
	printf("setValue(0, 0, NULL, true): %d\n", map.setValue(0, 0, NULL, true));
	printf("has(0): %d\n", map.has(0));
	printf("hasValue(0, 0): %d\n", map.hasValue(0, 0));
	printf("hasValue(0, 1): %d\n", map.hasValue(0, 1));
	printf("getValue(0, 0): %p\n", (void*)map.getValue(0, 0));
	printf("getValue(0, 1): %p\n", (void*)map.getValue(0, 1));
	printf("setValue(0, 2, newString): %d\n", map.setValue(0, 2, new StringVariable("blahblah")));
	printf("getValue(0, 2): %p\n", (void*)map.getValue(0, 2));
	printf("removeValue(0, 0): %d\n", map.removeValue(0, 0));
	printf("removeValue(0, 1): %d\n", map.removeValue(0, 1));
	return 0;
}

