/**
@file Args.hpp
@brief GR exception classes.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_ARGS_HPP_
#define DUCT_ARGS_HPP_

#include "./config.hpp"
#include "./utility.hpp"
#include "./detail/var.hpp"
#include "./VarType.hpp"
#include "./Var.hpp"
#include "./VarUtils.hpp"

#include <utility>

namespace duct {
namespace Args {

/**
	Parse arguments into raw-form identifier.

	@returns
	- Position of the first non-option argument; or
	- the number of parsed arguments if there are no non-option
	  arguments.
	@param argc Number of arguments.
	@param argv Arguments.
	@param[out] root Root output identifier.
*/
std::size_t
parse_raw(
	signed const argc,
	char* argv[],
	Var& root
) {
	root.morph(VarType::identifier, false);
	root.reset();
	if (0 < argc) {
		root.set_name(argv[0]);
	} else {
		return 0;
	}

	signed cmd_pos = argc;
	signed aidx;
	char const* str;
	signed pos;
	signed pos_eq;
	bool dashed;
	for (aidx = 1; argc > aidx; ++aidx) {
		str = argv[aidx];
		pos = 0;
		pos_eq = 0;
		dashed = false;
		for (; '\0' != str[pos]; ++pos) {
			if (0 == pos && '-' == str[pos]) {
				dashed = true;
			} else if ('=' == str[pos]) {
				pos_eq = pos;
				break;
			}
		}
		for (; '\0' != str[pos]; ++pos) {
			// Continue to the end of the string
			// (pos becomes size of string)
		}
		if (dashed) {
			if (0 == pos_eq) {
				pos_eq = pos;
			}
			root.emplace_back(
				detail::var_config::name_type{str, unsigned_cast(pos_eq)},
				VarType::null
			);
			++pos_eq;
			if (pos_eq < pos) {
				VarUtils::convert_typed(
					root.back(),
					detail::var_config::string_type{
						str + pos_eq,
						unsigned_cast(pos - pos_eq)
					}
				);
			}
		} else {
			if (argc == cmd_pos) {
				cmd_pos = max_ce(0, aidx - 1);
			}
			root.emplace_back(
				detail::var_config::string_type{str, unsigned_cast(pos)}
			);
		}
	}
	return
		argc == cmd_pos
		? root.size()
		: unsigned_cast(cmd_pos)
	;
}

/**
	Parse arguments into an option-command identifier pair.

	@returns Whether there is a command.
	@param argc Number of arguments.
	@param argv Arguments.
	@param[out] opt Global options.
	@param[out] cmd Command and command arguments.
*/
bool
parse_cmd(
	signed const argc,
	char* argv[],
	Var& opt,
	Var& cmd
) {
	std::size_t const cmd_pos = parse_raw(argc, argv, opt);
	bool const has_cmd = opt.size() != cmd_pos;
	cmd.morph(VarType::identifier, true);
	cmd.reset();
	if (has_cmd) {
		cmd.get_children().reserve(opt.size() - cmd_pos);
		cmd.set_name(opt.at(cmd_pos).get_as_str());
		for (
			auto it = opt.begin() + cmd_pos + 1u;
			opt.end() != it;
			++it
		) {
			cmd.emplace_back(std::move(*it));
		}
		opt.get_children().resize(cmd_pos);
	} else {
		cmd.set_name({});
	}
	return has_cmd;
}

/** @} */ // end of doc-group args

} // namespace Args
} // namespace duct

#endif // DUCT_ARGS_HPP_
