
#include <duct/char.hpp>
#include <duct/CharacterSet.hpp>

#include <iostream>
#include <iomanip>

void
test(
	duct::CharacterSet const& cs,
	char const c
) {
	std::cout
		<< '\'' << c << "': " << cs.contains(c)
	<< '\n';
}

signed
main() {
	duct::CharacterSet cs;
	cs.add_alphanumeric();

	std::cout << std::boolalpha;
	test(cs, 'a');
	test(cs, 'z');
	test(cs, 'A');
	test(cs, 'Z');
	std::cout.flush();

	return 0;
}
