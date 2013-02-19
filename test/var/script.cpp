
#include <duct/config.hpp>
#include <duct/char.hpp>
#include <duct/string.hpp>
#include <duct/detail/vartype.hpp>
#include <duct/EncodingUtils.hpp>
#include <duct/IO.hpp>
#include <duct/Variable.hpp>
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

static duct::ScriptParser s_parser{
	{duct::Encoding::UTF8, duct::Endian::SYSTEM}
};

static TestData const s_test_data[]={
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
	TDN("[a,]")
	TDN("[,]")
	TDN("a,")
	TDN(",")

	{nullptr, 0, false}
};

void parse_stream(duct::Variable& root, std::istream& stream, bool const valid) {
	assert(stream.good());
	if (valid) {
		s_parser.process(root, stream);
	} else {
		try {
			s_parser.process(root, stream);
			std::cout<<"Received no exception when one was expected\n"<<std::endl;
			assert(false);
		} catch (duct::ScriptParserException& e) {
			std::cout<<e.what()<<"\n\n";
			return;
		}
	}
	assert(stream.good() || stream.eof());
	print_var(root);
	std::cout<<'\n';
}

void do_test(duct::Variable& root, TestData const& td) {
	duct::IO::imemstream stream{td.data, td.size};
	std::printf("  Testing `%*s`:\n", static_cast<int>(td.size), td.data);
	parse_stream(root, stream, td.valid);
}

int main(int argc, char* argv[]) {
	duct::Variable root{duct::VARTYPE_NODE};
	if (1<argc) {
		TestData td{nullptr, 0, true};
		std::ifstream fs{};
		for (int index=1; argc>index; ++index) {
			fs.open(argv[index]);
			if (fs.is_open()) {
				parse_stream(root, fs, false);
				fs.close();
			} else {
				td.data=argv[index]; td.size=std::strlen(td.data);
				root.reset(); do_test(root, td);
			}
		}
	} else {
		for (TestData const* td=s_test_data; nullptr!=td->data; ++td) {
			root.reset(); do_test(root, *td);
		}
	}
	std::cout.flush();
	return 0;
}
