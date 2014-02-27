/**
@file GR/common.hpp
@brief GR commons.

@author Tim Howard
@copyright 2010-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_GR_COMMON_HPP_
#define DUCT_GR_COMMON_HPP_

#include "../config.hpp"
#include "../string.hpp"

/**
	@addtogroup gnarly_report
	@{
*/

/**
	@name Configuration

	Before including any of the GR headers, #DUCT_GR_ROOT_NAMESPACE
	should be user-defined to the user's GR implementation namespace.

	#DUCT_GR_STR_LIT() can be user-defined to change the encoding of
	string literals passed to the GR macros, but defaults to
	#DUCT_STR_U8().

	@{
*/

#ifdef DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/**
	The root namespace for definitions required by GR.

	@warning This must be user-defined before including a GR header
	(except for <duct/GR/Error.hpp>).
*/
#define DUCT_GR_ROOT_NAMESPACE

/**
	Macro to pass string literals through.

	This defaults to #DUCT_STR_U8().

	@remarks This is commonly user-defined to encode the string
	literal for a particular codebase's string type.

	@param s_ String literal.
*/
#define DUCT_GR_STR_LIT(s_)

#endif // DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/** @} */ // end of name-group Configuration


/** @cond INTERNAL */
#ifndef DUCT_GR_ROOT_NAMESPACE
	#error DUCT_GR_ROOT_NAMESPACE is required by GR
#endif

#ifndef DUCT_GR_STR_LIT
#define DUCT_GR_STR_LIT(s_) \
	DUCT_STR_U8(s_)
#endif
/** @endcond */ // INTERNAL


/**
	@name Scope

	These macros define and retrieve scope.

	A userspace class implementation should

	@code #define DUCT_GR_SCOPE_CLASS ClassName @endcode

	and

	@code #undef DUCT_GR_SCOPE_CLASS @endcode

	around its implementation space. A function using GR scope macros
	should likewise define and undefine #DUCT_GR_SCOPE_FUNC around its
	body.

	#DUCT_GR_SCOPE_PREFIX can be user-defined to add a scope prefix to
	the result of #DUCT_GR_SCOPE_CLASS_STR, #DUCT_GR_SCOPE_FUNC_STR,
	and #DUCT_GR_SCOPE_FQN_STR. The default definition is empty.

	If userspace will use define the prefix macro differently for
	multiple scopes, it should be undefined after including a GR macro
	header.

	@par
	@note The result literals for #DUCT_GR_SCOPE_CLASS_STR,
	#DUCT_GR_SCOPE_FUNC_STR, and #DUCT_GR_SCOPE_FQN_STR are not passed
	through DUCT_GR_STR_LIT() because the DUCT_GR_MSG() macro (which
	is used by the scope-specific message macros) would concatenate
	two layers of @c %DUCT_GR_STR_LIT(), which causes issues if the
	@c %DUCT_GR_STR_LIT() configuration is prepending an encoding
	prefix.

	@note If userspace needs these string literals passed through
	@c %DUCT_GR_STR_LIT(), the #DUCT_GR_SCOPE_CLASS_STR_LIT,
	#DUCT_GR_SCOPE_FUNC_STR_LIT, and #DUCT_GR_SCOPE_FQN_STR_LIT macros
	are shortcuts.

	@{
*/

#ifdef DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/**
	Scope prefix identifier.

	By default this is empty-defined. Userspace can define this to
	a common scope prefix.
*/
#define DUCT_GR_SCOPE_PREFIX

/**
	Current class identifier.

	Userspace should define this surrounding class implementations.
*/
#define DUCT_GR_SCOPE_CLASS

/**
	Current function identifier.

	Userspace should define this surrounding function implementations.
*/
#define DUCT_GR_SCOPE_FUNC

#endif // DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/** @cond INTERNAL */
#ifndef DUCT_GR_SCOPE_PREFIX
	#define DUCT_GR_SCOPE_PREFIX
#endif
/** @endcond */ // INTERNAL

/**
	Returns the plain string literal of #DUCT_GR_SCOPE_CLASS.

	@sa DUCT_GR_SCOPE_CLASS_STR_LIT
*/
#define DUCT_GR_SCOPE_CLASS_STR				\
	DUCT_STRINGIFY(DUCT_GR_SCOPE_PREFIX)	\
	DUCT_STRINGIFY(DUCT_GR_SCOPE_CLASS)

/**
	#DUCT_GR_SCOPE_CLASS_STR passed through DUCT_GR_STR_LIT().
*/
#define DUCT_GR_SCOPE_CLASS_STR_LIT \
	DUCT_GR_STR_LIT(DUCT_GR_SCOPE_CLASS_STR)

/**
	Returns the plain string literal of #DUCT_GR_SCOPE_FUNC.

	@sa DUCT_GR_SCOPE_FUNC_STR_LIT
*/
#define DUCT_GR_SCOPE_FUNC_STR				\
	DUCT_STRINGIFY(DUCT_GR_SCOPE_PREFIX)	\
	DUCT_STRINGIFY(DUCT_GR_SCOPE_FUNC)

/**
	#DUCT_GR_SCOPE_FUNC_STR passed through DUCT_GR_STR_LIT().
*/
#define DUCT_GR_SCOPE_FUNC_STR_LIT \
	DUCT_GR_STR_LIT(DUCT_GR_SCOPE_FUNC_STR)

/**
	Returns the plain string literal of the fully-qualified name of
	the current function.

	@sa DUCT_GR_SCOPE_FQN_STR_LIT
*/
#define DUCT_GR_SCOPE_FQN_STR					\
	DUCT_STRINGIFY(DUCT_GR_SCOPE_PREFIX)		\
	DUCT_STRINGIFY(DUCT_GR_SCOPE_CLASS) "::"	\
	DUCT_STRINGIFY(DUCT_GR_SCOPE_FUNC)

/**
	#DUCT_GR_SCOPE_FQN_STR passed through DUCT_GR_STR_LIT().
*/
#define DUCT_GR_SCOPE_FQN_STR_LIT \
	DUCT_GR_STR_LIT(DUCT_GR_SCOPE_FQN_STR)

/** @} */ // end of name-group Scope


/**
	@name Message

	These macros use the scope macros (or an arbitrary scope string
	literal) to construct a #DUCT_GR_STR_LIT-ized string literal in
	the form <code>"S: M"</code> (by default), where @c S is the scope
	and @c M is the message.

	To change the structure, define @c %DUCT_GR_MSG_IMPL_(s_, m_),
	where @c s_ and @c m_ are plain string literals for the scope and
	message (respectively), before including any GR macro headers.

	@{
*/

/** @cond INTERNAL */
#ifndef DUCT_GR_MSG_IMPL_
#define DUCT_GR_MSG_IMPL_(s_, m_) \
	s_ ": " m_
#endif
/** @endcond */ // INTERNAL

/**
	Build message string literal with a scope.

	@param s_ Scope (plain string literal).
	@param m_ Message (plain string literal).
*/
#define DUCT_GR_MSG(s_, m_) \
	DUCT_GR_STR_LIT(DUCT_GR_MSG_IMPL_(s_, m_))

/**
	Build message string literal with class scope.

	@param m_ Message (plain string literal).
*/
#define DUCT_GR_MSG_CLASS(m_) \
	DUCT_GR_MSG(DUCT_GR_SCOPE_CLASS_STR, m_)

/**
	Build message string literal with function scope.

	@param m_ Message (plain string literal).
*/
#define DUCT_GR_MSG_FUNC(m_) \
	DUCT_GR_MSG(DUCT_GR_SCOPE_FUNC_STR, m_)

/**
	Build message string literal with fully-qualified scope.

	@param m_ Message (plain string literal).
*/
#define DUCT_GR_MSG_FQN(m_) \
	DUCT_GR_MSG(DUCT_GR_SCOPE_FQN_STR, m_)

/** @} */ // end of name-group Message
/** @} */ // end of doc-group gnarly_report

#endif // DUCT_GR_COMMON_HPP_
