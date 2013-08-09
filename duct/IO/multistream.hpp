/**
@file IO/multistream.hpp
@brief Multicast stream.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_IO_MULTISTREAM_HPP_
#define DUCT_IO_MULTISTREAM_HPP_

#include "../config.hpp"
#include "../aux.hpp"
#include "./memstream.hpp"

#include <type_traits>
#include <utility>
#include <ostream>

namespace duct {
namespace IO {

// Forward declarations
template<
	typename CharT,
	class TraitsT = std::char_traits<CharT>
>
class basic_multistreambuf;

template<
	typename CharT,
	class TraitsT = std::char_traits<CharT>
>
class basic_omultistream;

/**
	@addtogroup io
	@{
*/

/**
	Vector of output streams.

	@remarks The element type is a pointer so that stream disabling
	is more efficient. @c basic_memstreambuf::multicast() will
	ignore @c nullptr elements.
*/
using multicast_vector_type = aux::vector<std::ostream*>;

/**
	@name Multicast stream type aliases
	@{
*/

/** Multicast streambuf. */
using multistreambuf = basic_multistreambuf<char>;
/** Output multicast stream. */
using omultistream = basic_omultistream<char>;

/** @} */ // end of name-group Multicast stream type aliases

/**
	Multicast output streambuf.

	@remarks The data in this streambuf will be flushed to the
	multicast streams when @c overflow() or @c pubsync() are called.
	By extension, flushing a @c basic_omultistream will
	call @c pubsync().

	@sa
		basic_omultistream
*/
template<
	typename CharT,
	class TraitsT
>
class basic_multistreambuf
	: public basic_memstreambuf<CharT, TraitsT>
{
private:
	using base_type = basic_memstreambuf<CharT, TraitsT>;

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
	multicast_vector_type& m_streams;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor (deleted). */
	basic_multistreambuf() = delete;

	/**
		Constructor with output buffer.

		@param streams Streams to multicast to.
		@param buffer Data buffer.
		@param size Size of @a buffer.
		@param mode @c openmode for the streambuf; defaults to and
		forces @c std::ios_base::out.
	*/
	basic_multistreambuf(
		multicast_vector_type& streams,
		void* const buffer,
		std::size_t const size,
		std::ios_base::openmode const mode = std::ios_base::out
	)
		: base_type(buffer, size, mode)
		, m_streams(streams)
	{}

	/** Destructor. */
	virtual ~basic_multistreambuf() override = default;
/// @}

/** @name Properties */ /// @{
	/**
		Set multicast stream vector.

		@param streams New multicast stream vector (reference).
	*/
	void
	set_streams(
		multicast_vector_type& streams
	) noexcept {
		m_streams = streams;
	}

	/**
		Get multicast stream vector.

		@returns The multicast stream vector.
	*/
	multicast_vector_type&
	get_streams() noexcept {
		return m_streams;
	}

	/** @copydoc basic_multistreambuf::get_streams() noexcept */
	multicast_vector_type const&
	get_streams() const noexcept {
		return m_streams;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Write data in put area to all streams and reset put area.

		@remarks This does not explicitly @c flush() the multicast
		streams.
	*/
	void
	multicast() {
		if (this->pbase() != this->pptr()) {
			for (std::ostream* stream : m_streams) {
				if (nullptr != stream) {
					stream->write(
						this->pbase(),
						this->pptr() - this->pbase()
					);
				}
			}
			// Reset put area to beginning of data buffer
			this->setp_all(this->pbase(), this->pbase(), this->epptr());
		}
	}
/// @}

protected:
/** @cond INTERNAL */
	virtual int_type
	overflow(
		int_type c = traits_type::eof()
	) override {
		multicast();
		if (traits_type::not_eof(c) == c) {
			(*this->pptr()) = traits_type::to_char_type(c);
			this->pbump(1);
		}
		return traits_type::not_eof(c);
	}

	virtual int_type
	sync() override {
		multicast();
		return 0;
	}
/** @endcond */ // INTERNAL
};

/**
	Output multicast stream.

	@remarks If the streambuf does not overflow or sync before the
	stream is destroyed, no data will be multicast. Thus, multicast
	streams must be flushed manually to ensure data propagates.

	@sa
		basic_multistreambuf
*/
template<
	typename CharT,
	class TraitsT
>
class basic_omultistream final
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

	/** Multistream buffer type. */
	using multistreambuf_type = basic_multistreambuf<char_type, traits_type>;
/// @}

private:
	multicast_vector_type m_streams;
	multistreambuf_type m_streambuf;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor (deleted). */
	basic_omultistream() = delete;
	/**
		Constructor with buffer.

		@param streams Streams to multicast to.
		@param buffer Data buffer.
		@param size Size of data buffer.
		@param mode @c openmode for the stream; defaults to and
		forces @c std::ios_base::out; removes @c std::ios_base::in.
	*/
	basic_omultistream(
		multicast_vector_type&& streams,
		void* const buffer,
		std::size_t const size,
		std::ios_base::openmode const mode = std::ios_base::out
	)
		: base_type()
		, m_streams(std::move(streams))
		, m_streambuf(
			m_streams,
			buffer,
			size,
			(mode & ~std::ios_base::in) | std::ios_base::out
		)
	{
		this->init(&m_streambuf);
	}
	/** Copy constructor (deleted). */
	basic_omultistream(basic_omultistream const&) = delete;
	/** Move constructor. */
	basic_omultistream(basic_omultistream&&) = default;
	/** Destructor. */
	~basic_omultistream() override = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	basic_omultistream& operator=(basic_omultistream const&) = delete;
	/** Move assignment operator. */
	basic_omultistream& operator=(basic_omultistream&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Set multicast stream vector.

		@note This does not update the streambuf's multicast stream
		vector reference. If the streambuf's vector reference was not
		changed externally, it will still point to the stream's
		vector. This function moves @a streams into the stream's
		vector.

		@param streams New multicast stream vector.
	*/
	void
	set_streams(
		multicast_vector_type streams
	) noexcept {
		m_streams.assign(std::move(streams));
	}

	/**
		Get multicast stream vector.

		@returns Multicast stream vector.
	*/
	multicast_vector_type&
	get_streams() noexcept {
		return m_streams;
	}

	/** @copydoc basic_omultistream::get_streams() noexcept */
	multicast_vector_type const&
	get_streams() const noexcept {
		return m_streams;
	}

	/**
		Get streambuf.

		@returns Pointer to the stream's streambuf (never @c nullptr).
	*/
	multistreambuf_type*
	rdbuf() const noexcept {
		return const_cast<multistreambuf_type*>(&m_streambuf);
	}
/// @}
};

/** @} */ // end of doc-group io

} // namespace IO
} // namespace duct

#endif // DUCT_IO_MULTISTREAM_HPP_
