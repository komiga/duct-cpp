
#include <duct/config.hpp>
#include <duct/char.hpp>
#include <duct/string.hpp>
#include <duct/debug.hpp>
#include <duct/detail/var.hpp>
#include <duct/EncodingUtils.hpp>
#include <duct/IO/memstream.hpp>
#include <duct/Var.hpp>
#include <duct/ScriptParser.hpp>

#include <cstring>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <fstream>

#include "common.inl"

struct TestData {
	duct::char8 const* data;
	std::size_t size;
	bool valid;
};

#define TDV(data) {data, std::strlen(data), true},
#define TDN(data) {data, std::strlen(data), false},

static duct::ScriptParser
g_parser{
	{duct::Encoding::UTF8, duct::Endian::system}
};

static TestData const
g_test_data[]{
	// Values
	TDV("name=value")
	TDV("name=1234567890")
	TDV("name=0.42")
	TDV("name=true")
	TDV("name=null")
	TDV("x=null y=+10")

	// Identifiers
	TDV("name")
	TDV("name nv=true")
	TDV("name nv=[0, 42] --blah=null")
	TDV("name -42 f=89.0001 b=true false n=null a=[1, 2] last")

	// Nodes
	TDV("name={}")
	TDV("{}")

	// Arrays
	TDV("name=[]")
	TDV("[]")
	TDV("[1, 2.4]")
	TDV("[42]")
	TDV("[\"aba\", caba]")

	// Non-string names
	TDV("1234=4321")
	TDV("12.34=43.21")
	TDV("true=true")
	TDV("false=false")
	TDV("null=null")
	TDV("true 1 2")
	TDV("false={}")
	TDV("null=[]")

	// Children
	TDV("{name=value}")
	TDV("{{}}")
	TDV("{name={}}")
	TDV("[value, value, [value]]")

	// Multi-line
	TDV("\"na\nme\"")
	TDV("\"na\nme\"=\"aba\ncaba\"")

	// Termination
	TDV("name=value identifier")
	TDV("name=value {name}")
	TDV("{name} {}")
	TDV("[value] []")

	// Comments
	TDV("name// asdfasdfadf")
	TDV("name=/*false*/true/*terminal*/")

	// Escape sequences
	TDV("all=\\n\\r\\t\\,\\=\\[\\]\\{\\}\\\"\\\'\\\\")

	// Errors
	TDN("name=")
	TDN("name==")
	TDN("name=//")
	TDN("name=/**/")
	TDN("{//")
	TDN("[//")
	TDN("name {}")
	TDN("name child {}")
	TDN("=")
	TDN("={}")
	TDN("\\")
	TDN("\\X")
	TDN("{]")
	TDN("[}")
	TDN("[{name]")
	TDN("{[name}")
	TDN("[a b]")
	TDN("[=]")
	TDN("[a=]")
	TDN("[a,]")
	TDN("[,]")
	TDN("a,")
	TDN(",")

	{nullptr, 0, false}
};

void
parse_stream(
	duct::Var& root,
	std::istream& stream,
	bool const valid
) {
	DUCT_ASSERTE(stream.good());
	if (valid) {
		g_parser.process(root, stream);
	} else {
		try {
			g_parser.process(root, stream);
			std::cout
				<< "Received no exception when one was expected\n"
			<< std::endl;
			DUCT_ASSERTE(false);
		} catch (duct::ScriptParserException& e) {
			std::cout
				<< e.what()
				<< "\n\n"
			;
			return;
		}
	}
	DUCT_ASSERTE(stream.good() || stream.eof());
	print_var(root);
	std::cout << '\n';
}

void
do_test(
	duct::Var& root,
	TestData const& td
) {
	duct::IO::imemstream stream{td.data, td.size};
	std::printf(
		"  Testing `%*s`:\n",
		static_cast<signed>(td.size),
		td.data
	);
	parse_stream(root, stream, td.valid);
}

signed
main(
	signed argc,
	char* argv[]
) {
	duct::Var root{duct::VarType::node};
	if (1 < argc) {
		TestData td{nullptr, 0u, true};
		std::ifstream fs{};
		for (
			signed index = 1;
			argc > index;
			++index
		) {
			fs.open(argv[index]);
			if (fs.is_open()) {
				parse_stream(root, fs, false);
				fs.close();
			} else {
				td.data = argv[index];
				td.size = std::strlen(td.data);
				root.reset();
				do_test(root, td);
			}
		}
	} else {
		for (
			TestData const* td = g_test_data;
			nullptr != td->data;
			++td
		) {
			root.reset();
			do_test(root, *td);
		}
	}
	std::cout.flush();
	return 0;
}
