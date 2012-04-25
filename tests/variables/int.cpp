
#include <duct/Variables.hpp>

#include <stdio.h>
#include <iostream>
#include <unicode/ustdio.h>
#include <unicode/ustream.h>

using namespace duct;
using namespace std;

int main() {
	IntVariable a;
	IntVariable b(100);
	IntVariable c(200, "foobar");
	
	printf("%d, %d, %d\n", a.get(), b.get(), c.get());
	cout<<a.getName()<<", "<<b.getName()<<", "<<c.getName()<<endl;
	
	return 0;
}

