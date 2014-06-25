
#include <duct/string.hpp>
#include <duct/GR/Error.hpp>

#include <iostream>

enum class ErrorCode : unsigned {
	scope_none,
	scope_class,
	scope_func,
	scope_fqn
};

using String = duct::u8string;
using Error = duct::GR::Error<ErrorCode, String>;

namespace gr_root {
	using Error = ::Error;
}

char const*
get_error_name(
	ErrorCode const code
) noexcept {
	switch (code) {
		case ErrorCode::scope_none : return "scope_none";
		case ErrorCode::scope_class: return "scope_class";
		case ErrorCode::scope_func : return "scope_func";
		case ErrorCode::scope_fqn  : return "scope_fqn";
		default: return "INVALID";
	}
}

void
print_error(
	Error const& error
) {
	std::cout
		<< '[' << get_error_name(error.get_code()) << ']'
		<< ' ' << error.get_message()
		<< '\n'
	;
}

#define DUCT_GR_ROOT_NAMESPACE ::gr_root
#define DUCT_GR_SCOPE_PREFIX prefix::
#include <duct/GR/throw.hpp>

#define DUCT_GR_SCOPE_CLASS TheAlmightyVoid
#define DUCT_GR_SCOPE_FUNC useless_windchimes
void
do_tests() {
	try {
		DUCT_GR_THROW(
			static_cast<ErrorCode>(0xCA11ACAB),
			"it wasn't me"
		);
	} catch (Error& error) {
		print_error(error);
	}

	try {
		DUCT_GR_THROW(
			ErrorCode::scope_none,
			"boring literal"
		);
	} catch (Error& error) {
		print_error(error);
	}

	try {
		DUCT_GR_THROW_OSTR(
			ErrorCode::scope_none,
			String{"glorious object"}
		);
	} catch (Error& error) {
		print_error(error);
	}

	try {
		DUCT_GR_THROW_CLASS(
			ErrorCode::scope_class,
			"literal"
		);
	} catch (Error& error) {
		print_error(error);
	}

	try {
		DUCT_GR_THROW_FUNC(
			ErrorCode::scope_func,
			"literal"
		);
	} catch (Error& error) {
		print_error(error);
	}

	try {
		DUCT_GR_THROW_FQN(
			ErrorCode::scope_fqn,
			"slimy literal"
		);
	} catch (Error& error) {
		print_error(error);
	}

	#undef DUCT_GR_STR_LIT
	#define DUCT_GR_STR_LIT(s_) \
		"LIT<<" s_ ">>"

	#undef DUCT_GR_MSG
	#define DUCT_GR_MSG(s_, m_) \
		DUCT_GR_STR_LIT("{" s_ "} " m_)

	#undef DUCT_GR_SCOPE_PREFIX
	#define DUCT_GR_SCOPE_PREFIX
	try {
		DUCT_GR_THROW_FQN(
			ErrorCode::scope_fqn,
			"no prefix, modified DUCT_GR_STR_LIT and DUCT_GR_MSG"
		);
	} catch (Error& error) {
		print_error(error);
	}
}
//#undef DUCT_GR_SCOPE_FUNC
//#undef DUCT_GR_SCOPE_CLASS
//#undef DUCT_GR_SCOPE_PREFIX

#include <duct/GR/undefine.hpp>

#if false										\
	|| defined(DUCT_GR_COMMON_HPP_)				\
	|| defined(DUCT_GR_STR_LIT)					\
	|| defined(DUCT_GR_ROOT_NAMESPACE)			\
	|| defined(DUCT_GR_SCOPE_PREFIX)			\
	|| defined(DUCT_GR_SCOPE_CLASS)				\
	|| defined(DUCT_GR_SCOPE_FUNC)				\
	|| defined(DUCT_GR_SCOPE_CLASS_STR)			\
	|| defined(DUCT_GR_SCOPE_CLASS_STR_LIT)		\
	|| defined(DUCT_GR_SCOPE_FUNC_STR)			\
	|| defined(DUCT_GR_SCOPE_FUNC_STR_LIT)		\
	|| defined(DUCT_GR_SCOPE_FQN_STR)			\
	|| defined(DUCT_GR_SCOPE_FQN_STR_LIT)		\
	|| defined(DUCT_GR_MSG_IMPL_)				\
	|| defined(DUCT_GR_MSG)						\
	|| defined(DUCT_GR_MSG_CLASS)				\
	|| defined(DUCT_GR_MSG_FUNC)				\
	|| defined(DUCT_GR_MSG_FQN)
//
	#error "<duct/GR/undefine.hpp> is borked"
#endif

#if false											\
	|| defined(DUCT_GR_CEFORMAT_HPP_)				\
	|| defined(DUCT_GR_DEF_CEFMT_IMPL_)				\
	|| defined(DUCT_GR_THROW_CEFMT_IMPL_)			\
	|| defined(DUCT_GR_STR_LIT_CEFMT_)				\
	|| defined(DUCT_GR_MSG_CEFMT_IMPL_)				\
	|| defined(DUCT_GR_DEF_CEFMT)					\
	|| defined(DUCT_GR_DEF_CEFMT_CLASS)				\
	|| defined(DUCT_GR_DEF_CEFMT_FUNC)				\
	|| defined(DUCT_GR_DEF_CEFMT_FQN)				\
	|| defined(DUCT_GR_THROW_CEFMT)
//
	#error "<duct/GR/undefine.hpp> is borked"
#endif

#if false											\
	|| defined(DUCT_GR_THROW_HPP_)					\
	|| defined(DUCT_GR_THROW_IMPL_)					\
	|| defined(DUCT_GR_THROW_OSTR_IMPL_)			\
	|| defined(DUCT_GR_THROW)						\
	|| defined(DUCT_GR_THROW_OSTR)					\
	|| defined(DUCT_GR_THROW_CLASS)					\
	|| defined(DUCT_GR_THROW_FUNC)					\
	|| defined(DUCT_GR_THROW_FQN)
//
	#error "<duct/GR/undefine.hpp> is borked"
#endif

signed
main() {
	try {
		do_tests();
	} catch (...) {
		std::cerr
			<< "!! unhandled exception\n"
		;
		throw;
	}

	return 0;
}
