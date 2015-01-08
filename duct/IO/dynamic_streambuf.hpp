/**
@file
@brief Automatically-growing streambuf.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include "../config.hpp"
#include "../debug.hpp"

#include <cstring>
#include <utility>
#include <stdexcept>
#include <algorithm>
#include <streambuf>
#include <vector>

namespace duct {
namespace IO {

// Forward declarations
template<
	typename CharT,
	class TraitsT = std::char_traits<CharT>,
	class AllocT = DUCT_CONFIG_ALLOCATOR<CharT>
>
class basic_dynamic_streambuf;

/**
	@addtogroup io
	@{
*/

/**
	@name Dynamic memory stream type aliases.

	@{
*/

/** Dynamic @c char streambuf. */
using dynamic_streambuf = basic_dynamic_streambuf<char>;

/** @} */ // end of name-group Dynamic memory stream type aliases

/**
	Automatically-growing stream buffer.

	@warning If a stream seeks past the current capacity of the
	output sequence, it will be resized to fit the new put position.

	@tparam CharT Character type. This should most often be @c char;
	see dynamic_streambuf.
	@tparam TraitsT Character traits type.
	@tparam AllocT Allocator type.
*/
template<
	typename CharT,
	class TraitsT,
	class AllocT
>
class basic_dynamic_streambuf final
	: public std::basic_streambuf<CharT, TraitsT>
{
private:
	using base_type = std::basic_streambuf<CharT, TraitsT>;

public:
/** @name Types */ /// @{
	/** Character type. */
	using typename base_type::char_type;

	// FIXME [TRAITS_TYPE]: For some reason g++ 4.7.3 simply does
	// not understand traits_type where it is used if it is a
	// using-declaration instead of a type alias.
	/** Traits type. */
	using traits_type = typename base_type::traits_type;
	/** @c traits_type::int_type. */
	using typename base_type::int_type;
	/** @c traits_type::pos_type. */
	using typename base_type::pos_type;
	/** @c traits_type::off_type. */
	using typename base_type::off_type;

	/** Allocator type */
	using allocator_type = AllocT;

	// std::ios_base::openmode has evil^W annoying
	// modes we don't want to deal with.
	/** Sequence mode. */
	enum class Sequence : unsigned {
		/** Input sequence (s >> x). */
		input = 1u,
		/** Output sequence (s << x). */
		output
	};

	/** Buffer type. */
	using buffer_type = std::vector<char_type, allocator_type>;
/// @}

private:
	Sequence m_seq;
	std::size_t m_seq_size;
	buffer_type m_buffer;
	std::size_t m_max_size;
	std::size_t m_growth_rate;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with capacity, growth rate, and allocator.

		Stream buffer will be initialized in output mode.

		@note See set_growth_rate() and get_max_size() for notes
		on the @a growth_rate and @a max_size parameters.

		@note Unlike reset(), a @a capacity of @c 0u does not cause
		the capacity to be @a growth_rate. Due to this, an empty
		buffer can be constructed.

		@post @code
			get_sequence() == Sequence::output &&
			get_sequence_size() == 0u &&
			get_buffer().get_size() == capacity
		@endcode

		@param capacity Initial capacity.
		@param growth_rate Amount to grow during an overflow.
		@param max_size Maximum buffer size.
		@param allocator Allocator.
	*/
	basic_dynamic_streambuf(
		std::size_t const capacity,
		std::size_t const growth_rate = 256u,
		std::size_t const max_size = 0u,
		allocator_type const& allocator = allocator_type()
	)
		: base_type()
		, m_seq(Sequence::output)
		, m_seq_size(0u)
		, m_buffer(allocator)
		, m_max_size(max_size)
		, m_growth_rate(growth_rate)
	{
		// Permit empty buffer on construction
		this->setg(nullptr, nullptr, nullptr);
		DUCT_ASSERTE(resize(capacity));
	}

	/** Default constructor (deleted). */
	basic_dynamic_streambuf() = delete;
	/** Copy constructor (deleted). */
	basic_dynamic_streambuf(basic_dynamic_streambuf const&) = delete;
	/** Move constructor. */
	basic_dynamic_streambuf(basic_dynamic_streambuf&&) = default;
	/** Destructor. */
	~basic_dynamic_streambuf() override = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	basic_dynamic_streambuf& operator=(basic_dynamic_streambuf const&) = delete;
	/** Move assignment operator. */
	basic_dynamic_streambuf& operator=(basic_dynamic_streambuf&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get sequence.
	*/
	Sequence
	get_sequence() const noexcept {
		return m_seq;
	}

	/**
		Get sequence size.

		@note If the stream buffer is in output mode, this will
		return the total number of chars written -- i.e., the
		farthest observed put position.
	*/
	std::size_t
	get_sequence_size() const noexcept {
		const_cast<basic_dynamic_streambuf*>(
			this
		)->refresh_seq_size();
		return m_seq_size;
	}

	/**
		Get buffer.
	*/
	buffer_type const&
	get_buffer() const noexcept {
		return m_buffer;
	}

	/**
		Get buffer.
	*/
	buffer_type&
	get_buffer() noexcept {
		return m_buffer;
	}

	/**
		Get maximum buffer size.

		@note If this is equal to @c 0u, the amount the buffer can
		grow is only (theoretically) bound by buffer_type::max_size(),
		which is typically @c std::numeric_limits<std::size_t>::max().
	*/
	std::size_t
	get_max_size() const noexcept {
		return m_max_size;
	}

	/**
		Get growth rate.
	*/
	std::size_t
	get_growth_rate() const noexcept {
		return m_growth_rate;
	}

	/**
		Set growth rate.

		@warning If @c growth_rate==0u, any operations that
		trigger growth, such as @c overflow() and @c pubseekpos(),
		may fail.

		@param growth_rate Growth rate.
	*/
	void
	set_growth_rate(
		std::size_t const growth_rate
	) const noexcept {
		m_growth_rate = growth_rate;
	}

	/**
		Get the remaining space between the get position and the end
		of the input sequence.

		@returns The number of chars remaining between the get
		position and the end of the input sequence, or @c 0 if the
		current mode is output.
	*/
	std::size_t
	get_remaining() const noexcept {
		if (Sequence::input == m_seq) {
			return m_seq_size - (this->gptr() - this->eback());
		}
		return 0u;
	}

	/**
		Get current position in sequence.

		@note This is more efficient than using @c tellg()
		and @c tellp() from an stdlib stream.

		@par
		@note If the stream buffer is in output mode, this will not
		update the sequence size.
	*/
	std::size_t
	get_position() const noexcept {
		switch (m_seq) {
		case Sequence::input:
			return this->gptr() - this->eback();

		case Sequence::output:
			return this->pptr() - this->pbase();
		}
		// FIXME: Hack to prevent shoddy heuristics in GCC 4.7.3's
		// -Wreturn-type from triggering
		DUCT_ASSERTE(false);
	}
/// @}

/** @name Operations */ /// @{
	/**
		Reset to empty output sequence.

		@warning If the buffer is seeked past not-overwritten
		data, the input sequence after a commit() may contain it.

		@note If @c capacity==0u, the growth rate is used in
		its place. If the growth rate is also @c 0, subsequent put
		operations will fail. If the resultant capacity is larger
		than the (nonzero) maximum size, this operation will fail.

		@post @code
			get_sequence() == Sequence::output &&
			get_sequence_size() == 0u &&
			get_buffer().size()
			== (capacity == 0u)
				? capacity
				: get_growth_rate()
		@endcode

		@returns @c true on success.
		@param capacity Optional number of chars to ensure are
		available in the buffer.
	*/
	bool
	reset(
		std::size_t const capacity = 0u
	) {
		m_seq = Sequence::output;
		m_seq_size = 0u;
		this->setg(nullptr, nullptr, nullptr);
		return resize(
			(0u != capacity)
			? capacity
			: m_growth_rate
		);
	}

	/**
		Commit output sequence to input sequence.

		@note This has no effect if the stream buffer is already in
		input mode.

		@post @code
			get_sequence_size() == size
		@endcode

		@throws std::invalid_argument
		If <code>size > get_sequence_size()</code>.

		@param size Size of the input sequence.
	*/
	void
	commit(
		std::size_t const size
	) {
		if (Sequence::output == m_seq) {
			if (get_sequence_size() < size) {
				throw std::invalid_argument(
					"commit size is larger than output sequence"
				);
			}
			commit_priv(size, false);
		}
	}

	/**
		Commit entire output sequence to input sequence.

		@see commit(std::size_t const)
	*/
	void
	commit() {
		refresh_seq_size();
		commit(m_seq_size);
	}

	/**
		Commit input sequence size from external modification.

		@note Unlike commit(std::size_t const), this permits @a size
		up to the size of the underlying buffer. This is handy for
		operating with interfaces that do not deal with the I/O
		library directly, but should not otherwise be used.

		@post @code
			get_sequence_size() == size
		@endcode

		@throws std::invalid_argument
		If <code>size > get_buffer().size()</code>.

		@param size Size of the input sequence.
		@param retain_input_position Whether to retain the current
		input position if the buffer is already in input mode.
	*/
	void
	commit_direct(
		std::size_t const size,
		bool const retain_input_position
	) {
		if (m_buffer.size() < size) {
			throw std::invalid_argument(
				"commit size is larger than buffer"
			);
		}
		commit_priv(size, retain_input_position);
	}

	/**
		Discard data from front of buffer.

		@note The result sequence size is @code
			get_sequence_size() - size
		@endcode

		@par
		@note If either @a size is @c 0 or the sequence is empty, this
		has no effect. If @c size==get_sequence_size(), the sequence
		will be empty.

		@post @c get_position()==0u (subsequently, put/get position
		according to mode is also @c 0u).

		@throws std::invalid_argument
		If <code>size > get_sequence_size()</code>.

		@returns The new sequence size.
		@param size Number of characters to discard.
	*/
	std::size_t
	discard(
		std::size_t const size
	) {
		refresh_seq_size();
		if (m_seq_size < size) {
			throw std::invalid_argument(
				"size is larger than sequence"
			);
		} else if (0u == size || 0u == m_seq_size) {
			return m_seq_size;
		}

		m_seq_size -= size;
		if (0u < m_seq_size) {
			std::memmove(
				m_buffer.data(),
				m_buffer.data() + size,
				m_seq_size
			);
		}

		switch (m_seq) {
		case Sequence::input:
			this->setg(
				m_buffer.data(),
				m_buffer.data(),
				m_buffer.data() + m_seq_size
			);
			break;

		case Sequence::output:
			this->setp(
				m_buffer.data(),
				m_buffer.data() + m_buffer.size()
			);
			break;
		}
		return m_seq_size;
	}
/// @}

private:
	void
	refresh_seq_size() noexcept {
		if (Sequence::output == m_seq) {
			m_seq_size = std::max<std::size_t>(
				m_seq_size,
				this->pptr() - this->pbase()
			);
		}
	}

	bool
	resize(
		std::size_t const size
	) {
		if (0u == m_max_size || size <= m_max_size) {
			m_buffer.resize(size);
			this->setp(m_buffer.data(), m_buffer.data() + size);
			return true;
		}
		this->setp(nullptr, nullptr);
		return false;
	}

	bool
	grow(
		std::size_t append_size = 0u
	) {
		append_size += m_buffer.size();
		if (
			0u < m_growth_rate &&
			(0u == m_max_size || append_size <= m_max_size)
		) {
			append_size += m_growth_rate;
			// check overflow
			DUCT_ASSERTE(m_buffer.size() < append_size);
			return resize(
				(0u == m_max_size)
				? append_size
				: std::min<std::size_t>(
					m_max_size,
					append_size
				)
			);
		}
		return false;
	}

	void
	commit_priv(
		std::size_t const size,
		bool const retain_input_position
	) {
		std::size_t pos = 0;
		if (Sequence::input == m_seq && retain_input_position) {
			pos = get_position();
			if (size < pos) {
				pos = size;
			}
		}
		this->setg(
			m_buffer.data(),
			m_buffer.data() + pos,
			m_buffer.data() + size
		);
		this->setp(nullptr, nullptr);
		m_seq = Sequence::input;
		m_seq_size = size;
	}

	enum : signed {
		max_signed = std::numeric_limits<signed>::max()
	};

	// because pos_type is not necessarily equivalent
	// to signed, which pbump() takes
	void
	pbump_to(
		pos_type pos
	) {
		while (max_signed < pos) {
			this->pbump(max_signed);
			pos -= max_signed;
		}
		this->pbump(static_cast<signed>(pos));
	}

	static constexpr std::ios_base::openmode const
	ios_openmode_io
		= std::ios_base::in
		| std::ios_base::out
	;

	pos_type
	seek_to(
		pos_type const pos,
		std::ios_base::openmode const mode
	) {
		// cache the frontmost put position
		refresh_seq_size();

		if (0u > pos) {
			goto l_bad_pos_;
		} else if (ios_openmode_io == (ios_openmode_io & mode)) {
			// we only provide one sequence at a time
			goto l_bad_pos_;
		} else if (Sequence::input == m_seq && (mode & std::ios_base::in)) {
			if (pos > static_cast<off_type>(m_seq_size)) {
				goto l_bad_pos_;
			}
			char_type* const data = m_buffer.data();
			this->setg(data, data + pos, data + m_seq_size);
			return pos;
		} else if (Sequence::output == m_seq && (mode & std::ios_base::out)) {
			if (pos > static_cast<off_type>(m_buffer.size())) {
				// must use the difference to prevent
				// double-counting the existing size (pos)
				if (!grow(static_cast<std::size_t>(pos) - m_buffer.size())) {
					goto l_bad_pos_;
				}
			} else {
				// reset pptr()
				this->setp(m_buffer.data(), m_buffer.data() + m_buffer.size());
			}
			pbump_to(pos);
			return pos;
		}
		// else: mode is neither in nor out, so we
		// don't know which sequence to reposition

	l_bad_pos_:
		return pos_type{off_type{-1}};
	}

// std::basic_streambuf implementation
	pos_type
	seekoff(
		off_type off,
		std::ios_base::seekdir way,
		std::ios_base::openmode which
			= std::ios_base::in
			| std::ios_base::out
	) override {
		switch (way) {
		case std::ios_base::beg:
			if (0u <= off) {
				return seek_to(pos_type{off}, which);
			} else {
				return pos_type{off_type{-1}};
			}
			break;

		case std::ios_base::cur:
			return seek_to(
				pos_type{
					off
					+ (Sequence::input == m_seq
						? (this->gptr() - m_buffer.data())
						: (this->pptr() - m_buffer.data())
					)
				},
				which
			);
			break;

		case std::ios_base::end:
			return seek_to(
				pos_type{
					off + static_cast<off_type>(m_buffer.size())
				},
				which
			);
			break;

		default:
			DUCT_ASSERTE(false);
		}
	}

	pos_type
	seekpos(
		pos_type pos,
		std::ios_base::openmode which
			= std::ios_base::in
			| std::ios_base::out
	) override {
		return seek_to(pos, which);
	}

	int_type
	overflow(
		int_type ch = traits_type::eof()
	) override {
		refresh_seq_size();

		if (!traits_type::eq_int_type(ch, traits_type::eof())) {
			if (this->pptr() == this->epptr()) {
				if (grow()) {
					pbump_to(pos_type{static_cast<off_type>(m_seq_size)});
				} else {
					return traits_type::eof();
				}
			}

			*this->pptr() = traits_type::to_char_type(ch);
			this->pbump(1);
			return ch;
		}

		// else: ch == eof
		return traits_type::eof();
	}
};

/** @} */ // end of doc-group io

} // namespace IO
} // namespace duct
