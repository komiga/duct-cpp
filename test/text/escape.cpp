
#include <duct/config.hpp>
#include <duct/char.hpp>
#include <duct/string.hpp>
#include <duct/StringUtils.hpp>

#include <cassert>
#include <iostream>
#include <iomanip>

static duct::StringUtils::EscapeablePair const
g_esc_pair{
	"\n\r\t" ",=[]{}\"\'\\",
	"n""r""t"",=[]{}\"\'\\"
};

void
test_string(
	bool const ignore_invalids,
	duct::u8string const& str,
	duct::u8string const& expected
) {
	duct::u8string result;
	unsigned const
	escaped_count = duct::StringUtils::escape_string(
		result, str, g_esc_pair, ignore_invalids
	);
	std::cout
		<< std::setw(3) << std::left << escaped_count
		<< std::right << ": `" << result << "`'\n"
	;
	if (!expected.empty() && 0 != expected.compare(result)) {
		std::cout
			<< "     `" << expected << "`  doesn't match!\n"
		;
		assert(false);
	}
}

void
test_equivalent(
	bool const ignore_invalids,
	duct::u8string const& str
) {
	test_string(ignore_invalids, str, str);
}

signed
main(
	signed argc,
	char* argv[]
) {
	duct::u8string const empty_str{};
	if (1 < argc) {
		bool const ignore_invalids = (0 != atoi(argv[1u]));
		for (
			auto str = argv + 2u;
			(argv + argc) > str;
			++str
		) {
			test_string(ignore_invalids, {*str}, empty_str);
		}
	} else {
		test_equivalent(false, {"abacaba"});
		test_equivalent(false, {"あ—"});
		test_equivalent(true, {"\\a"});
		test_string(false, {"\\a"}, {"\\\\a"});
		test_string(false, {"\\"}, {"\\\\"});
		test_string(
			true,
			{"\r\n\t,=[]{}\"\'\\z"},
			{"\\r\\n\\t\\,\\=\\[\\]\\{\\}\\\"\\\'\\z"}
		);
	}
	std::cout.flush();
	return 0;
}
