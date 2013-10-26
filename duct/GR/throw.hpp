/**
@file GR/throw.hpp
@brief GR throwing.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_GR_THROW_HPP_
#define DUCT_GR_THROW_HPP_

#include "../config.hpp"
#include "./common.hpp"

#include <utility>

/**
	@addtogroup gnarly_report
	@{
*/

/**
	@name Throwing

	%Error throwing macros.

	The default @c DUCT_GR_THROW_IMPL_(ec_, m_)
	and @c DUCT_GR_THROW_OSTR_IMPL_(ec_, m_) definitions
	require within the root GR namespace (#DUCT_GR_ROOT_NAMESPACE)
	an @c %Error class conforming to the GR::Error construction
	interface (more specifically, with the @c %Error constructor
	taking as its second parameter a string type which conforms to the
	@c std::string construction interface).

	Alternatively, these two @c IMPL_ macros can be user-defined
	before <duct/GR/throw.hpp> to change the throw behavior. @c ec_
	is an error code and @c m_ is either a string literal (already
	passed through #DUCT_GR_STR_LIT()) or an object of userspace
	string type (for @c DUCT_GR_THROW_OSTR_IMPL_()).

	@par
	@note All throw macros except for #DUCT_GR_THROW_OSTR
	encapsulate the final message in #DUCT_GR_STR_LIT (that is,
	@c m_ needn't be #DUCT_GR_STR_LIT-ized).

	@{
*/

/** @cond INTERNAL */
#ifndef DUCT_GR_THROW_IMPL_
#define DUCT_GR_THROW_IMPL_(ec_, m_)		\
	throw std::move(						\
		DUCT_GR_ROOT_NAMESPACE::Error{		\
			ec_,							\
			m_								\
		}									\
	)
#endif

#ifndef DUCT_GR_THROW_OSTR_IMPL_
#define DUCT_GR_THROW_OSTR_IMPL_(ec_, m_) \
	DUCT_GR_THROW_IMPL_(ec_, m_)
#endif
/** @endcond */

/**
	Throw error with message.

	@param ec_ %Error code.
	@param m_ Message (plain string literal).
*/
#define DUCT_GR_THROW(ec_, m_) \
	DUCT_GR_THROW_IMPL_(ec_, DUCT_GR_STR_LIT(m_))

/**
	Throw error with message.

	@param ec_ %Error code.
	@param m_ Message (object of userspace string type).
*/
#define DUCT_GR_THROW_OSTR(ec_, m_) \
	DUCT_GR_THROW_OSTR_IMPL_(ec_, m_)

/**
	Throw error with class scope.

	@param ec_ %Error code.
	@param m_ Message (plain string literal).

	@sa DUCT_GR_THROW_FUNC,
		DUCT_GR_THROW_FQN
*/
#define DUCT_GR_THROW_CLASS(ec_, m_)	\
	DUCT_GR_THROW_IMPL_(				\
		ec_,							\
		DUCT_GR_MSG_CLASS(m_)	\
	)

/**
	Throw error with function scope.

	@param ec_ %Error code.
	@param m_ Message (plain string literal).

	@sa DUCT_GR_THROW_CLASS,
		DUCT_GR_THROW_FQN
*/
#define DUCT_GR_THROW_FUNC(ec_, m_)		\
	DUCT_GR_THROW_IMPL_(				\
		ec_,							\
		DUCT_GR_MSG_FUNC(m_)		\
	)

/**
	Throw error with fully-qualified scope.

	@param ec_ %Error code.
	@param m_ Message (plain string literal).

	@sa DUCT_GR_THROW_CLASS,
		DUCT_GR_THROW_FUNC
*/
#define DUCT_GR_THROW_FQN(ec_, m_)		\
	DUCT_GR_THROW_IMPL_(				\
		ec_,							\
		DUCT_GR_MSG_FQN(m_)		\
	)

/** @} */ // end of name-group Throwing
/** @} */ // end of doc-group gnarly_report

#endif // DUCT_GR_THROW_HPP_
