
#include <duct/char.hpp>
#include <duct/CharacterSet.hpp>

#include <iostream>
#include <iomanip>

using duct::char32;
using duct::CharacterSet;

void test(CharacterSet const& cs, char const c) {
	std::cout<<'\''<<c<<"': "<<cs.contains(c)<<'\n';
}

int main(int argc, char* argv[]) {
	CharacterSet cs;
	cs.add_alphanumeric();
	std::cout<<std::boolalpha;
	test(cs, 'a');
	test(cs, 'z');
	test(cs, 'A');
	test(cs, 'Z');
	std::cout<<std::endl;
	return 0;
}
