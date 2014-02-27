/**
@file IO/util.hpp
@brief IO utilities.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_IO_UTIL_HPP_
#define DUCT_IO_UTIL_HPP_

#include "../config.hpp"
#include "../debug.hpp"

#include <cstring>
#include <istream>

namespace duct {
namespace IO {

// Forward declarations
class StreamContext;

/**
	@addtogroup io
	@{
*/

/**
	@name Utilities
	@{
*/

/**
	Get the size of a stream.

	@note This requires bidirectional seeking
	(e.g., @c memstream, @c std::ifstream, @c std::istringstream).

	@warning @c stream may be in an @c std::ios_base::failbit state
	upon return (in which case the return value will be @c 0)
	or <strong>existing error states may even be removed</strong>.

	@returns The size of the stream (0 may mean failure).
	@param stream Stream to measure.
*/
template<
	typename CharT,
	class TraitsT
>
std::size_t
size(
	std::basic_istream<CharT, TraitsT>& stream
) {
	if (stream.eof()) {
		// Already at eof: don't have to seek anywhere
		DUCT_DEBUG("duct::IO::size: eof() initial");
		// Get rid of all other states
		stream.setstate(std::ios_base::eofbit);
		auto const end = stream.tellg();
		if (decltype(end)(-1) != end) {
			return static_cast<std::size_t>(end);
		} else {
			DUCT_DEBUG("duct::IO::size: -1 == end");
		}
	} else {
		// Toss all states
		stream.clear();
		auto const original = stream.tellg();
		// If the stream will give position
		if (decltype(original)(-1) != original) {
			stream.seekg(0, std::ios_base::end);
			auto const end = stream.tellg();
			if (decltype(end)(-1) != end) {
				stream.seekg(original);
				return static_cast<std::size_t>(end);
			} else {
				DUCT_DEBUG("duct::IO::size: -1 == end");
			}
		} else {
			DUCT_DEBUG("duct::IO::size: -1 == original");
		}
	}
	return 0u;
}

/** @} */ // end of name-group Utilities
/** @} */ // end of doc-group io

} // namespace IO
} // namespace duct

#endif // DUCT_IO_UTIL_HPP_
