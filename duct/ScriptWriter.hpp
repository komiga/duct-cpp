/**
@file
@brief ductScript writer.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include "./config.hpp"
#include "./debug.hpp"
#include "./char.hpp"
#include "./utility.hpp"
#include "./detail/string_traits.hpp"
#include "./StateStore.hpp"
#include "./CharacterSet.hpp"
#include "./EncodingUtils.hpp"
#include "./StringUtils.hpp"
#include "./IO/StreamContext.hpp"

#include <utility>
#include <ostream>

namespace duct {

/**
	@addtogroup ductScript
	@{
*/

// Forward declarations
class ScriptWriter;

/**
	ductScript writer.
*/
class ScriptWriter final {
public:
/** @name Types */ /// @{
	/**
		%Flags.
	*/
	enum class Flags : unsigned {
		/** Always surround names in quotation marks. */
		name_quote = bit(0),
		/** Always surround string values in quotation marks. */
		value_string_quote = bit(1),
		/** Escape linefeeds and tabulations (regardless of quotation). */
		escape_whitespace = bit(2),

		/** Always surround names and string values in quotation marks. */
		quote
			= name_quote
			| value_string_quote
		,

		/** Default flags. */
		defaults
			= value_string_quote
			| escape_whitespace
		,
	};
/// @}

private:
	StateStore<Flags> m_flags{Flags::defaults};
	IO::StreamContext m_stream_ctx{};

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	ScriptWriter() = default;

	/**
		Constructor with StreamContext.

		@param context StreamContext to copy.
	*/
	ScriptWriter(
		IO::StreamContext context
	) noexcept
		: m_stream_ctx(std::move(context))
	{}

	/**
		Constructor with flags and default context.

		@param flags Writer flags.
	*/
	ScriptWriter(
		Flags const flags
	) noexcept
		: m_flags(flags)
	{}

	/**
		Constructor with flags and StreamContext.

		@param flags Writer flags.
		@param context StreamContext to copy.
	*/
	ScriptWriter(
		Flags const flags,
		IO::StreamContext context
	) noexcept
		: m_flags(flags)
		, m_stream_ctx(std::move(context))
	{}

	/** Copy constructor (deleted). */
	ScriptWriter(ScriptWriter const&) = delete;
	/** Move constructor. */
	ScriptWriter(ScriptWriter&&) = default;
	/** Destructor. */
	~ScriptWriter() = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	ScriptWriter& operator=(ScriptWriter const&) = delete;
	/** Move assignment operator. */
	ScriptWriter& operator=(ScriptWriter&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Set flags.

		@param flags %Flags.
		@param enable Whether to enable or disable the flags.
	*/
	void
	set_flags(
		Flags const flags,
		bool const enable = true
	) noexcept {
		m_flags.set(flags, enable);
	}

	/**
		Check if a set of flags are enabled.

		@returns @c true if all @a flags are enabled.
		@param flags %Flags to test.
	*/
	bool
	has_flags(
		Flags const flags
	) const noexcept {
		return m_flags.test(flags);
	}

	/**
		Get stream context.

		@returns The current stream context.
	*/
	IO::StreamContext&
	get_stream_context() noexcept {
		return m_stream_ctx;
	}
	/** @copydoc get_stream_context() */
	IO::StreamContext const&
	get_stream_context() const noexcept {
		return m_stream_ctx;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Write a variable to a stream.

		@returns
		- @c true if the variable was written to the stream, or
		- @c false if at some point a stream operation failed.
		@param dest Destination stream.
		@param source Source variable. Can be any type.
		@param treat_as_root Whether to treat @a source as a root node
		if it is a node (if both are true, only the node's contents are
		written).
		@param tab_level Tabulation level; defaults to @c 0.
	*/
	bool
	write(
		std::ostream& dest,
		Var const& source,
		bool const treat_as_root,
		unsigned const tab_level = 0
	) const;
/// @}

private:
	template<
		class StringT,
		class StringU = typename detail::string_traits<StringT>::encoding_utils
	>
	bool
	write_string(
		std::ostream& dest,
		StringT const& str,
		bool const is_name
	) const;

	bool
	write_value(
		std::ostream& dest,
		Var const& var,
		bool const with_name
	) const;

	bool
	write_array(
		std::ostream& dest,
		Var const& var,
		bool const with_name
	) const;

	bool
	write_node(
		std::ostream& dest,
		Var const& var,
		bool const treat_as_root, unsigned tab_level
	) const;

	bool
	write_identifier(
		std::ostream& dest,
		Var const& var
	) const;
};

#include "./impl/ScriptWriter.inl"

/** @} */ // end of doc-group ductScript

} // namespace duct
