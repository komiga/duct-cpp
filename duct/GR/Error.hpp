/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief GR exception classes.
*/

#pragma once

#include "../config.hpp"

#include <utility>

namespace duct {
namespace GR {

// Forward declarations
template<class ErrorCodeT, class MsgStringT>
class Error;

/**
	@addtogroup gnarly_report
	@{
*/

/**
	Generic GR exception class.

	@pre For standard GR configuration, @a MsgStringT conforms to the
	@c std::string construction interface.

	@remarks @a ErrorCodeT should generally be an enum-class, but
	there is no restriction against non-enum types.

	@tparam ErrorCodeT %Error code type.
	@tparam MsgStringT Message string type.
*/
template<class ErrorCodeT, class MsgStringT>
class Error {
public:
	/**
		%Error code type.
	*/
	using error_code_type = ErrorCodeT;

	/**
		Message string type.
	*/
	using message_string_type = MsgStringT;

private:
	error_code_type const m_code;
	message_string_type const m_message;

	Error() = delete;
	Error(Error const&) = delete;
	Error& operator=(Error const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Destructor. */
	~Error() noexcept = default;

	/**
		Constructor with error code and message.

		@param code %Error code.
		@param message %Error message.
	*/
	Error(
		error_code_type const code,
		message_string_type message
	) noexcept
		: m_code(code)
		, m_message(std::move(message))
	{}

	/**
		Constructor with bound string literal.

		@tparam N Length of string literal (including ending NUL);
		inferred from @a message.

		@param code %Error code.
		@param message %Error message.
	*/
	template<std::size_t N>
	Error(
		error_code_type const code,
		typename message_string_type::value_type const message[N]
	) noexcept
		: m_code(code)
		, m_message(message, N - 1u)
	{}

	/** Move constructor. */
	Error(Error&&) = default;
/// @}

/** @name Operators */ /// @{
	/** Move-assignment operator. */
	Error& operator=(Error&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get error code.
	*/
	error_code_type
	get_code() const noexcept {
		return m_code;
	}

	/**
		Get error message.
	*/
	message_string_type const&
	get_message() const noexcept {
		return m_message;
	}
/// @}
};

/** @} */ // end of doc-group gnarly_report

} // namespace GR
} // namespace duct
