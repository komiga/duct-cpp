/**
@file IO/cacophony_support.hpp
@brief Cacophony support.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include "../config.hpp"
#include "./dynamic_streambuf.hpp"

#include <Cacophony/types.hpp>
#include <Cacophony/traits.hpp>
#include <Cacophony/utility.hpp>
#include <Cacophony/support/binary_blob.hpp>

namespace duct {

/**
	@addtogroup io
	@{
*/

/**
	@name Cacophony support

	@{
*/

namespace IO {

/**
	Read dynamic stream buffer.

	@note Uses 32-bit unsigned integer for data size.
*/
template<
	class Ser,
	class TraitsT,
	class AllocT
>
Cacophony::ser_result_type
read(
	Cacophony::tag_read,
	Ser& ser,
	duct::IO::basic_dynamic_streambuf<char, TraitsT, AllocT>& value
) {
	// NB: duct::dynamic_streambuf buffer grows beyond the actual
	// stream data, so we have to handle serialization on *that* size
	// not the buffer's size
	auto& data = value.get_buffer();
	std::uint32_t size = 0u;
	ser(size);
	data.resize(size);
	ser(Cacophony::make_binary_blob(data.data(), data.size()));
	value.commit_direct(size, false);
}

/**
	Write dynamic stream buffer.

	@note Uses 32-bit unsigned integer for data size.
*/
template<
	class Ser,
	class TraitsT,
	class AllocT
>
Cacophony::ser_result_type
write(
	Cacophony::tag_write,
	Ser& ser,
	duct::IO::basic_dynamic_streambuf<char, TraitsT, AllocT> const& value
) {
	auto const& data = value.get_buffer();
	ser(static_cast<std::uint32_t>(data.size()));
	ser(Cacophony::make_binary_blob(data.data(), data.size()));
}

} // namespace IO

/** @} */ // end of name-group Cacophony support

/** @} */ // end of doc-group io

} // namespace duct

