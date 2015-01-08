/**
@file
@brief Memory stream.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include "../config.hpp"
#include "../debug.hpp"

#include <limits>
#include <iostream>

namespace duct {
namespace IO {

// Forward declarations
template<
	typename CharT,
	class TraitsT = std::char_traits<CharT>
>
class basic_memstreambuf;

template<
	typename CharT,
	class TraitsT = std::char_traits<CharT>
>
class basic_imemstream;

template<
	typename CharT,
	class TraitsT = std::char_traits<CharT>
>
class basic_omemstream;

template<
	typename CharT,
	class TraitsT = std::char_traits<CharT>
>
class basic_memstream;

/**
	@addtogroup io
	@{
*/

/**
	@name Memory stream type aliases

	@warning All memory stream buffer classes are incapable of
	automatically growing.
	@{
*/

/** Memory streambuf. */
using memstreambuf = basic_memstreambuf<char>;
/** Input memory stream. */
using imemstream = basic_imemstream<char>;
/** Output memory stream. */
using omemstream = basic_omemstream<char>;
/** Input/output memory stream. */
using memstream = basic_memstream<char>;

/** @} */ // end of name-group Memory stream type aliases

/**
	Generic memory streambuf.

	@warning Reassigning the buffer will not clear stream state.
*/
template<
	typename CharT,
	class TraitsT
>
class basic_memstreambuf
	: public std::basic_streambuf<CharT, TraitsT>
{
private:
	using base_type = std::basic_streambuf<CharT, TraitsT>;

public:
/** @name Types */ /// @{
	/** Character type. */
	using typename base_type::char_type;
	/** Traits type. */
	using typename base_type::traits_type;
	/** @c traits_type::int_type. */
	using typename base_type::int_type;
	/** @c traits_type::pos_type. */
	using typename base_type::pos_type;
	/** @c traits_type::off_type. */
	using typename base_type::off_type;
/// @}

private:
	std::ios_base::openmode m_mode;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor (deleted). */
	basic_memstreambuf() = delete;
	/**
		Constructor with input buffer.

		@param buffer Data buffer.
		@param size Size of @a buffer.
		@param mode @c openmode for the streambuf; defaults to and
		forces @c std::ios_base::in; removes @c std::ios_base::out.
	*/
	basic_memstreambuf(
		void const* const buffer,
		std::size_t const size,
		std::ios_base::openmode const mode = std::ios_base::in
	)
		: base_type()
		, m_mode((mode & ~std::ios_base::out) | std::ios_base::in)
	{
		assign(const_cast<void*>(buffer), size);
	}
	/**
		Constructor with input/output buffer.

		@param buffer Data buffer.
		@param size Size of @a buffer.
		@param mode @c openmode for the streambuf; defaults to and
		forces @c std::ios_base::out.
	*/
	basic_memstreambuf(
		void* const buffer,
		std::size_t const size,
		std::ios_base::openmode const mode = std::ios_base::out
	)
		: base_type()
		, m_mode(mode | std::ios_base::out)
	{
		assign(buffer, size);
	}
	/** Copy constructor (deleted). */
	basic_memstreambuf(basic_memstreambuf const&) = delete;
	/** Move constructor. */
	basic_memstreambuf(basic_memstreambuf&&) = default;
	/** Destructor. */
	virtual ~basic_memstreambuf() override = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	basic_memstreambuf& operator=(basic_memstreambuf const&) = delete;
	/** Move assignment operator. */
	basic_memstreambuf& operator=(basic_memstreambuf&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Assign the streambuf's get and put areas.

		@note The current get and put positions will be reset.

		@param buffer Data buffer.
		@param size Size of @a buffer.
	*/
	void
	assign(
		void* const buffer,
		std::size_t const size
	) {
		if (nullptr == buffer) {
			this->setg(nullptr, nullptr, nullptr);
			this->setp(nullptr, nullptr);
		} else {
			char_type* const cbuf = static_cast<char_type*>(buffer);
			if (m_mode & std::ios_base::in) {
				this->setg(cbuf, cbuf, cbuf + size);
			}
			if (m_mode & std::ios_base::out) {
				this->setp(cbuf, cbuf + size);
			}
		}
	}
/// @}

protected:
/** @cond INTERNAL */
	pos_type
	seekoff(
		off_type off,
		std::ios_base::seekdir way,
		std::ios_base::openmode
			which = std::ios_base::in | std::ios_base::out
	) override {
		bool const
			do_in  = m_mode & which & std::ios_base::in,
			do_out = m_mode & which & std::ios_base::out;
		char_type *beg = nullptr, *cur, *end;

		// NB: memstreambuf will only ever point to a single buffer.
		// m_mode reflects the active modes: if either do_in or do_out
		// are true, their respective pointers are non-null, so it
		// doesn't matter which one is picked.
		if (do_in) {
			// eback? stdlib is schizo
			beg = this->eback(); cur = this->gptr(); end = this->egptr();
		} else if (do_out) {
			beg = this->pbase(); cur = this->pptr(); end = this->epptr();
		}
		if (nullptr != beg) {
			// std::ios_base::beg is just beg+off
			if 		(std::ios_base::cur == way) { off += cur - beg; }
			else if (std::ios_base::end == way) { off += end - beg; }
			if (0 <= off && (end - beg) >= off) {
				if (do_in) { this->setg(beg, beg + off, end); }
				if (do_out) { setp_all(beg, beg + off, end); }
				return pos_type{off};
			}
		}
		return pos_type{off_type{-1}};
	}

	pos_type
	seekpos(
		pos_type pos,
		std::ios_base::openmode
			which = std::ios_base::in | std::ios_base::out
	) override {
		// pos_type should be std::streampos, which should be
		// std::fpos<>, which stores an off_type, which should be
		// std::streamoff.
		// Pass to seekoff() instead of duplicating code.
		return seekoff(static_cast<off_type>(pos), std::ios_base::beg, which);
	}

	// The setp that should have been (for some arcane reason, setp
	// doesn't take a new current pointer like setg does).
	// Also, the stdlib hates us. pbump just _has to_ take an int
	// instead of off_type (it's even signed for you! come on!).
	void
	setp_all(
		char_type* const beg,
		char_type* cur,
		char_type* const end
	) {
		// May the hammermaestro have mercy on your femur(s)
		DUCT_ASSERTE(beg <= cur && end >= cur);
		this->setp(beg, end);
		std::ptrdiff_t off = cur - beg;
		// std::ptrdiff_t can be 64 bits of signed goodness
		// while int could be only 32, which means we have
		// to chip away at it.
		while (std::numeric_limits<signed>::max() < off) {
			this->pbump(std::numeric_limits<signed>::max());
			off -= std::numeric_limits<signed>::max();
		}
		this->pbump(static_cast<signed>(off));
	}
/** @endcond */ // INTERNAL
};

/**
	Input memory stream.

	@sa
		basic_omemstream,
		basic_memstream,
		basic_memstreambuf
*/
template<
	typename CharT,
	class TraitsT
>
class basic_imemstream final
	: public std::basic_istream<CharT, TraitsT>
{
private:
	using base_type = std::basic_istream<CharT, TraitsT>;

public:
/** @name Types */ /// @{
	/** Character type. */
	using typename base_type::char_type;
	/** Traits type. */
	using typename base_type::traits_type;
	/** @c traits_type::int_type. */
	using typename base_type::int_type;
	/** @c traits_type::pos_type. */
	using typename base_type::pos_type;
	/** @c traits_type::off_type. */
	using typename base_type::off_type;

	/** Memory buffer type. */
	using membuf_type = basic_memstreambuf<char_type, traits_type>;
/// @}

private:
	membuf_type m_membuf;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor (deleted). */
	basic_imemstream() = delete;
	/**
		Constructor with buffer.

		@param buffer Data buffer.
		@param size Size of @c buffer.
		@param mode @c openmode for the stream; defaults to and
		forces @c std::ios_base::in; removes @c std::ios_base::out.
	*/
	basic_imemstream(
		void const* const buffer,
		std::size_t const size,
		std::ios_base::openmode const mode = std::ios_base::in
	)
		: base_type(&m_membuf)
		, m_membuf(
			buffer,
			size,
			(mode & ~std::ios_base::out) | std::ios_base::in
		)
	{}
	/** Copy constructor (deleted). */
	basic_imemstream(basic_imemstream const&) = delete;
	/** Move constructor. */
	basic_imemstream(basic_imemstream&&) = default;
	/** Destructor. */
	~basic_imemstream() override = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	basic_imemstream& operator=(basic_imemstream const&) = delete;
	/** Move assignment operator. */
	basic_imemstream& operator=(basic_imemstream&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get streambuf.

		@returns Pointer to the stream's streambuf (never @c nullptr).
	*/
	membuf_type*
	rdbuf() const noexcept {
		// STROUSTRUP MADE ME DO IT
		return const_cast<membuf_type*>(&m_membuf);
	}
/// @}
};

/**
	Output memory stream.

	@sa
		basic_imemstream,
		basic_memstream,
		basic_memstreambuf
*/
template<
	typename CharT,
	class TraitsT
>
class basic_omemstream final
	: public std::basic_ostream<CharT, TraitsT>
{
private:
	using base_type = std::basic_ostream<CharT, TraitsT>;

public:
/** @name Types */ /// @{
	/** Character type. */
	using typename base_type::char_type;
	/** Traits type. */
	using typename base_type::traits_type;
	/** @c traits_type::int_type. */
	using typename base_type::int_type;
	/** @c traits_type::pos_type. */
	using typename base_type::pos_type;
	/** @c traits_type::off_type. */
	using typename base_type::off_type;

	/** Memory buffer type. */
	using membuf_type = basic_memstreambuf<char_type, traits_type>;
/// @}

private:
	membuf_type m_membuf;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor (deleted). */
	basic_omemstream() = delete;
	/**
		Constructor with buffer.

		@param buffer Data buffer.
		@param size Size of @c buffer.
		@param mode @c openmode for the stream; defaults to and
		forces @c std::ios_base::out; removes @c std::ios_base::in.
	*/
	basic_omemstream(
		void* const buffer,
		std::size_t const size,
		std::ios_base::openmode const mode = std::ios_base::out
	)
		: base_type(&m_membuf)
		, m_membuf(
			buffer,
			size,
			(mode & ~std::ios_base::in) | std::ios_base::out
		)
	{}
	/** Copy constructor (deleted). */
	basic_omemstream(basic_omemstream const&) = delete;
	/** Move constructor. */
	basic_omemstream(basic_omemstream&&) = default;
	/** Destructor. */
	~basic_omemstream() override = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	basic_omemstream& operator=(basic_omemstream const&) = delete;
	/** Move assignment operator. */
	basic_omemstream& operator=(basic_omemstream&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get streambuf.

		@returns Pointer to the stream's streambuf (never @c nullptr).
	*/
	membuf_type*
	rdbuf() const noexcept {
		return const_cast<membuf_type*>(&m_membuf);
	}
/// @}
};

/**
	Input/output memory stream.
	@sa
		basic_imemstream,
		basic_omemstream,
		basic_memstreambuf
*/
template<
	typename CharT,
	class TraitsT
>
class basic_memstream final
	: public std::basic_iostream<CharT, TraitsT>
{
private:
	using base_type = std::basic_iostream<CharT, TraitsT>;

public:
/** @name Types */ /// @{
	/** Character type. */
	using typename base_type::char_type;
	/** Traits type. */
	using typename base_type::traits_type;
	/** @c traits_type::int_type. */
	using typename base_type::int_type;
	/** @c traits_type::pos_type. */
	using typename base_type::pos_type;
	/** @c traits_type::off_type. */
	using typename base_type::off_type;

	/** Memory buffer type. */
	using membuf_type = basic_memstreambuf<char_type, traits_type>;
/// @}

private:
	membuf_type m_membuf;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor (deleted). */
	basic_memstream() = delete;
	/**
		Constructor with buffer.

		@param buffer Data buffer.
		@param size Size of @c buffer.
		@param mode @c openmode for the stream; defaults to and
		forces @c std::ios_base::in and @c std::ios_base::out.
	*/
	basic_memstream(
		void* const buffer,
		std::size_t const size,
		std::ios_base::openmode const
			mode = std::ios_base::in | std::ios_base::out
	)
		: base_type(&m_membuf)
		, m_membuf(
			buffer,
			size,
			mode | std::ios_base::in | std::ios_base::out
		)
	{}
	/** Copy constructor (deleted). */
	basic_memstream(basic_memstream const&) = delete;
	/** Move constructor. */
	basic_memstream(basic_memstream&&) = default;
	/** Destructor. */
	~basic_memstream() override = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	basic_memstream& operator=(basic_memstream const&) = delete;
	/** Move assignment operator. */
	basic_memstream& operator=(basic_memstream&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get streambuf.

		@returns Pointer to the stream's streambuf (never @c nullptr).
	*/
	membuf_type*
	rdbuf() const noexcept {
		return const_cast<membuf_type*>(&m_membuf);
	}
/// @}
};

/** @} */ // end of doc-group io

} // namespace IO
} // namespace duct
