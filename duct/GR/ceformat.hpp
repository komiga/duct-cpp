/**
@file GR/ceformat.hpp
@brief GR definitions for ceformat.

@author Tim Howard
@copyright 2010-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_GR_CEFORMAT_HPP_
#define DUCT_GR_CEFORMAT_HPP_

#include "../config.hpp"
#include "./common.hpp"

#include <ceformat/String.hpp>
#include <ceformat/Format.hpp>
#include <ceformat/print.hpp>

#include <utility>

/**
	@addtogroup gnarly_report
	@{
*/

/**
	@name ceformat support

	ceformat (https://github.com/komiga/ceformat) macros for GR.

	The @c DUCT_GR_DEF_CEFMT_IMPL_(ident_, fmt_)
	and @c DUCT_GR_THROW_CEFMT_IMPL_(ec_, cefmt_, ...) macros
	can be user-defined.

	For the former, @c ident_ is the identifier for the
	@c ceformat::Format variable to be defined, and @c fmt_ is the
	format string literal (which is passed through @c %CEFORMAT_STR_LIT()
	instead of #DUCT_GR_STR_LIT(); see @c %CEFORMAT_CONFIG_STRING_TYPE).

	For the latter, @c ec_ is an error code, @c cefmt_ is
	a @c ceformat::Format (as defined with @c *_DEF_CEFMT_*), and
	@c ... is @c __VA_ARGS__.

	@par
	@note Due to the limitations of template parameter pointers, the
	definitions from the @c *_DEF_CEFMT_* macros cannot be used if
	they are placed in function scope. To avoid linkage, cram the
	definitions into an anonymous namespace.

	@{
*/

/** @cond INTERNAL */
#ifndef DUCT_GR_DEF_CEFMT_IMPL_
#define DUCT_GR_DEF_CEFMT_IMPL_(ident_, fmt_)	\
	static constexpr ceformat::Format const		\
	ident_{fmt_}
#endif

#ifndef DUCT_GR_THROW_CEFMT_IMPL_
#define DUCT_GR_THROW_CEFMT_IMPL_(ec_, cefmt_, ...)			\
	throw std::move(DUCT_GR_ROOT_NAMESPACE::Error{			\
		ec_,												\
		::ceformat::print<cefmt_>(__VA_ARGS__)				\
	})
#endif

#ifndef DUCT_GR_STR_LIT_CEFMT_
#define DUCT_GR_STR_LIT_CEFMT_(s_) \
	CEFORMAT_STR_LIT(s_)
#endif

#ifndef DUCT_GR_MSG_CEFMT_IMPL_
#define DUCT_GR_MSG_CEFMT_IMPL_(s_, fmt_) \
	DUCT_GR_STR_LIT_CEFMT_(DUCT_GR_MSG_IMPL_(s_, fmt_))
#endif
/** @endcond */ // INTERNAL

/**
	Define format message with no scope.

	@param ident_ Identifier for format message.
	@param fmt_ Format message (plain string literal).
*/
#define DUCT_GR_DEF_CEFMT(ident_, fmt_) \
	DUCT_GR_DEF_CEFMT_IMPL_(ident_, CEFORMAT_STR_LIT(fmt_))

/**
	Define format message with class scope.

	@param ident_ Identifier for format message.
	@param fmt_ Format message (plain string literal).
*/
#define DUCT_GR_DEF_CEFMT_CLASS(ident_, fmt_)					\
	DUCT_GR_DEF_CEFMT_IMPL_(									\
		ident_,													\
		DUCT_GR_MSG_CEFMT_IMPL_(DUCT_GR_SCOPE_CLASS_STR, fmt_)	\
	)

/**
	Define format message with function scope.

	@param ident_ Identifier for format message.
	@param fmt_ Format message (plain string literal).
*/
#define DUCT_GR_DEF_CEFMT_FUNC(ident_, fmt_)					\
	DUCT_GR_DEF_CEFMT_IMPL_(									\
		ident_,													\
		DUCT_GR_MSG_CEFMT_IMPL_(DUCT_GR_SCOPE_FUNC_STR, fmt_)	\
	)

/**
	Define format message with fully-qualified scope.

	@param ident_ Identifier for format message.
	@param fmt_ Format message (plain string literal).
*/
#define DUCT_GR_DEF_CEFMT_FQN(ident_, fmt_)						\
	DUCT_GR_DEF_CEFMT_IMPL_(									\
		ident_,													\
		DUCT_GR_MSG_CEFMT_IMPL_(DUCT_GR_SCOPE_FQN_STR, fmt_)	\
	)

/**
	Throw error with ceformat.

	@param ec_ %Error code.
	@param cefmt_ @c ceformat::Format.
	@param ... Format arguments.
*/
#define DUCT_GR_THROW_CEFMT(ec_, cefmt_, ...) \
	DUCT_GR_THROW_CEFMT_IMPL_(ec_, cefmt_, __VA_ARGS__)

/** @} */ // end of name-group ceformat support
/** @} */ // end of doc-group gnarly_report

#endif // DUCT_GR_CEFORMAT_HPP_
