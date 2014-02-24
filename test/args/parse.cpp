
#include <duct/aux.hpp>
#include <duct/utility.hpp>
#include <duct/Variable.hpp>
#include <duct/ScriptWriter.hpp>
#include <duct/Args.hpp>

#include <iostream>
#include <iomanip>

signed
main(
	signed argc,
	char* argv[]
) {
	duct::Variable root;
	std::size_t const non_opt_pos = duct::Args::parse_raw(argc, argv, root);

	duct::ScriptWriter writer{
		enum_combine(
			duct::ScriptWriter::Flags::defaults,
			duct::ScriptWriter::Flags::quote
		),
		{duct::Encoding::UTF8, duct::Endian::system}
	};
	duct::aux::stringstream out_stream;
	writer.write(out_stream, root, true);
	std::cout
		<< "non_opt_pos = " << non_opt_pos << '\n'
		<< "raw: `"
		<< out_stream.str()
		<< "`\n"
	;

	duct::Variable cmd;
	bool const has_cmd = duct::Args::parse_cmd(argc, argv, root, cmd);
	out_stream.str({});
	writer.write(out_stream, root, true);
	std::cout
		<< "\n"
		<< "has_cmd: "
		<< std::boolalpha << has_cmd
		<< "\nopt: `"
		<< out_stream.str()
		<< "`\n"
	;

	out_stream.str({});
	writer.write(out_stream, cmd, true);
	std::cout
		<< "cmd: `"
		<< out_stream.str()
		<< "`\n"
	;

	return 0;
}
