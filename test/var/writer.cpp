
#include <duct/config.hpp>
#include <duct/aux.hpp>
#include <duct/char.hpp>
#include <duct/string.hpp>
#include <duct/detail/vartype.hpp>
#include <duct/EncodingUtils.hpp>
#include <duct/IO.hpp>
#include <duct/Variable.hpp>
#include <duct/ScriptWriter.hpp>
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

static duct::ScriptParser
g_parser{
	{duct::Encoding::UTF8, duct::Endian::SYSTEM}
};

static duct::ScriptWriter
g_writer{
	duct::DSWF_ESCAPE_WHITESPACE,
	{duct::Encoding::UTF8, duct::Endian::SYSTEM}
};

static TestData const
g_test_data[]{
	// Values
	TDV("name=value")
	TDV("name=1234567890")
	TDV("name=0.42")
	TDV("name=true")
	TDV("name=null")

	// Identifiers
	TDV("name")
	TDV("name 42 89.0001 true false null [1,2] last")

	// Nodes
	TDV("name={}")
	TDV("{}")

	// Arrays
	TDV("name=[]")
	TDV("[]")
	TDV("[1, 2.4]")
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

	{nullptr, 0, false}
};

void
parse_stream(
	duct::Variable& root,
	std::istream& stream,
	bool const /*valid*/
) {
	assert(stream.good());
	try {
		g_parser.process(root, stream);
	} catch (duct::ScriptParserException& e) {
		std::cout
			<< "Unexpected exception when parsing:\n"
			<< e.what() << '\n'
		<< std::endl;
		assert(false);
	}
	assert(stream.good() || stream.eof());
	//print_var(root);
}

void
write_var(
	duct::Variable const& var,
	std::ostream& stream
) {
	if (!g_writer.write(stream, var, true)) {
		std::cout
			<< "Failed to write variable"
		<< std::endl;
		assert(false);
	}
}

void
do_test(
	duct::Variable& root,
	TestData const& td
) {
	duct::IO::imemstream in_stream{td.data, td.size};
	std::printf(
		"  Testing `%*s`:\n",
		static_cast<signed>(td.size),
		td.data
	);
	parse_stream(root, in_stream, td.valid);
	duct::aux::stringstream out_stream;
	write_var(root, out_stream);
	std::cout
		<< "          `"
		<< out_stream.str()
		<< "`\n\n"
	;
}

signed
main(
	signed argc,
	char* argv[]
) {
	duct::Variable root{duct::VARTYPE_NODE};
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
