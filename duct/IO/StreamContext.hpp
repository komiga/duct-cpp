/**
@file IO/StreamContext.hpp
@brief StreamContext class.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include "../config.hpp"
#include "../string.hpp"
#include "../detail/string_traits.hpp"
#include "../EndianUtils.hpp"
#include "./arithmetic.hpp"
#include "./unicode.hpp"

namespace duct {
namespace IO {

// Forward declarations
class StreamContext;

/**
	@addtogroup io
	@{
*/

/**
	Encoding and endian stream context.

	@note Documentation is identical to the static functions. The
	default constructor will use the UTF-8 encoding and the system
	endian.
*/
class StreamContext final {
private:
	Encoding m_encoding{Encoding::UTF8};
	Endian m_endian{Endian::system};

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	StreamContext() = default;

	/**
		Constructor with encoding and default system endian.

		@param encoding An encoding.
	*/
	explicit
	StreamContext(
		Encoding const encoding
	) noexcept
		: m_encoding(encoding)
	{}

	/**
		Constructor with endian and default UTF-8 encoding.

		@param endian An endian.
	*/
	explicit
	StreamContext(
		Endian const endian
	) noexcept
		: m_endian(endian)
	{}

	/**
		Constructor with encoding and endian.

		@param encoding An encoding.
		@param endian An endian.
	*/
	StreamContext(
		Encoding const encoding,
		Endian const endian
	) noexcept
		: m_encoding(encoding)
		, m_endian(endian)
	{}

	/** Copy constructor. */
	StreamContext(StreamContext const&) = default;
	/** Move constructor. */
	StreamContext(StreamContext&&) = default;
	/** Destructor. */
	~StreamContext() = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator. */
	StreamContext& operator=(StreamContext const&) = default;
	/** Move assignment operator. */
	StreamContext& operator=(StreamContext&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Set encoding and endian.

		@param encoding An encoding.
		@param endian An endian.
	*/
	void
	set_properties(
		Encoding const encoding,
		Endian const endian
	) noexcept {
		m_encoding = encoding;
		m_endian = endian;
	}

	/**
		Set encoding and endian from another context.

		@param ctx StreamContext to copy.
	*/
	void
	set_properties(
		StreamContext const& ctx
	) noexcept {
		m_encoding = ctx.m_encoding;
		m_endian = ctx.m_endian;
	}

	/**
		Set encoding.

		@param encoding An encoding.
	*/
	void
	set_encoding(
		Encoding const encoding
	) noexcept {
		m_encoding = encoding;
	}

	/**
		Get encoding.

		@returns The current encoding.
	*/
	Encoding
	get_encoding() const noexcept {
		return m_encoding;
	}

	/**
		Set endian.

		@param endian An endian.
	*/
	void
	set_endian(
		Endian const endian
	) noexcept {
		m_endian = endian;
	}

	/**
		Get endian.

		@returns The current endian.
	*/
	Endian
	get_endian() const noexcept {
		return m_endian;
	}
/// @}

/** @name Raw data */ /// @{
	/** See @c duct::IO::read_arithmetic(). */
	template<
		typename T
	>
	void
	read_arithmetic(
		std::istream& stream,
		T& value
	) const {
		::duct::IO::read_arithmetic<T>(
			stream, value, m_endian
		);
	}

	/** See @c duct::IO::read_arithmetic(). */
	template<
		typename T
	>
	T
	read_arithmetic(
		std::istream& stream
	) const {
		return ::duct::IO::read_arithmetic<T>(
			stream, m_endian
		);
	}

	/** See @c duct::IO::read_arithmetic_array(). */
	template<
		typename T
	>
	void
	read_arithmetic_array(
		std::istream& stream,
		T* dest,
		std::size_t const count
	) const {
		::duct::IO::read_arithmetic_array<T>(
			stream, dest, count, m_endian
		);
	}

	/** See @c duct::IO::write_arithmetic(). */
	template<
		typename T
	>
	void
	write_arithmetic(
		std::ostream& stream,
		T const value
	) const {
		::duct::IO::write_arithmetic<T>(
			stream, value, m_endian
		);
	}

	/** See @c duct::IO::write_arithmetic_array(). */
	template<
		typename T
	>
	void
	write_arithmetic_array(
		std::ostream& stream,
		T const* const src,
		std::size_t const count
	) const {
		::duct::IO::write_arithmetic_array<T>(
			stream, src, count, m_endian
		);
	}
/// @}

/** @name Unicode */ /// @{
	/** See @c duct::IO::read_char(). */
	char32
	read_char(
		std::istream& stream,
		char32 const replacement = CHAR_SENTINEL
	) const {
		switch (m_encoding) {
		case Encoding::UTF8:
			return ::duct::IO::read_char<UTF8Utils>(
				stream, replacement, m_endian
			);
		case Encoding::UTF16:
			return ::duct::IO::read_char<UTF16Utils>(
				stream, replacement, m_endian
			);
		case Encoding::UTF32:
			return ::duct::IO::read_char<UTF32Utils>(
				stream, replacement, m_endian
			);
		default:
			DUCT_DEBUG_ASSERT(false,
				"Somehow the context has an invalid encoding; shame on you!"
			);
			return replacement;
		}
	}

	/** See @c duct::IO::write_char(). */
	std::size_t
	write_char(
		std::ostream& stream,
		char32 const cp,
		unsigned const num = 1u,
		char32 const replacement = CHAR_NULL
	) const {
		switch (m_encoding) {
		case Encoding::UTF8:
			return ::duct::IO::write_char<UTF8Utils>(
				stream, cp, num, replacement, m_endian
			);
		case Encoding::UTF16:
			return ::duct::IO::write_char<UTF16Utils>(
				stream, cp, num, replacement, m_endian
			);
		case Encoding::UTF32:
			return ::duct::IO::write_char<UTF32Utils>(
				stream, cp, num, replacement, m_endian
			);
		default:
			DUCT_DEBUG_ASSERT(false,
				"Somehow the context has an invalid encoding; shame on you!"
			);
			return 0;
		}
	}

	/** See @c duct::IO::read_string(). */
	template<
		class StringT
	>
	void
	read_string(
		std::istream& stream,
		StringT& value,
		std::size_t size,
		char32 const replacement = CHAR_NULL
	) const {
		switch (m_encoding) {
		case Encoding::UTF8:
			::duct::IO::read_string<UTF8Utils>(
				stream, value, size, replacement, m_endian
			);
			return;
		case Encoding::UTF16:
			::duct::IO::read_string<UTF16Utils>(
				stream, value, size, replacement, m_endian
			);
			return;
		case Encoding::UTF32:
			::duct::IO::read_string<UTF32Utils>(
				stream, value, size, replacement, m_endian
			);
			return;
		default:
			DUCT_DEBUG_ASSERT(false,
				"Somehow the context has an invalid encoding; shame on you!"
			);
			return;
		}
	}

	/** See @c duct::IO::write_string(). */
	template<
		class StringT
	>
	std::size_t
	write_string(
		std::ostream& stream,
		StringT const& value,
		char32 const replacement = CHAR_NULL
	) const {
		switch (m_encoding) {
		case Encoding::UTF8:
			return ::duct::IO::write_string<UTF8Utils>(
				stream, value, replacement, m_endian
			);
		case Encoding::UTF16:
			return ::duct::IO::write_string<UTF16Utils>(
				stream, value, replacement, m_endian
			);
		case Encoding::UTF32:
			return ::duct::IO::write_string<UTF32Utils>(
				stream, value, replacement, m_endian
			);
		default:
			DUCT_DEBUG_ASSERT(false,
				"Somehow the context has an invalid encoding; shame on you!"
			);
			return 0;
		}
	}
/// @}
};

/** @} */ // end of doc-group io

} // namespace IO
} // namespace duct

