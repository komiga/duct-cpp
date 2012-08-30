
#include <duct/config.hpp>
#include <duct/char.hpp>
#include <duct/string.hpp>
#include <duct/StringUtils.hpp>

#include <cassert>
#include <iostream>
#include <iomanip>

using duct::u8string;

static duct::StringUtils::EscapeablePair const s_esc_pair{"\n\r\t,=[]{}\"\'\\", "nrt,=[]{}\"\'\\"};

#define test_equivalent(str, ignore_invalids) test_string(str, str, ignore_invalids)

void test_string(u8string const& str, u8string const& expected, bool const ignore_invalids) {
	u8string result;
	unsigned int escaped_count=duct::StringUtils::escape_string(result, str, s_esc_pair, ignore_invalids);
	std::cout<<std::setw(3)<<std::left<<escaped_count<<std::right<<": `"<<result<<'`'<<std::endl;
	if (!expected.empty() && 0!=expected.compare(result)) {
		std::cout<<"     `"<<expected<<"`  doesn't match!"<<std::endl;
		assert(false);
	}
}

int main(int argc, char* argv[]) {
	u8string const empty_str{};
	if (1<argc) {
		bool const ignore_invalids=(0!=atoi(argv[1]));
		for (auto str=argv+2; (argv+argc)>str; ++str) {
			test_string({*str}, empty_str, ignore_invalids);
		}
	} else {
		test_equivalent({"abacaba"}, false);
		test_equivalent({"あ—"}, false);
		test_equivalent({"\\a"}, true);
		test_string({"\\a"}, {"\\\\a"}, false);
		test_string({"\\"}, {"\\\\"}, false);
		test_string({"\r\n\t,=[]{}\"\'\\z"}, {"\\r\\n\\t\\,\\=\\[\\]\\{\\}\\\"\\\'\\z"}, true);
	}
	return 0;
}
