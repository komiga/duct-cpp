/**
@file ScriptWriter.hpp
@brief ductScript formatter.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_SCRIPTWRITER_HPP_
#define DUCT_SCRIPTWRITER_HPP_

#include "./config.hpp"
#include "./debug.hpp"
#include "./char.hpp"
#include "./detail/string_traits.hpp"
#include "./CharacterSet.hpp"
#include "./EncodingUtils.hpp"
#include "./StringUtils.hpp"
#include "./IO.hpp"

#include <ostream>

namespace duct {

/**
	@addtogroup ductScript
	@{
*/

// Forward declarations
enum ScriptWriterFlags : unsigned int;
class ScriptWriter;

/**
	ScriptWriter flags.
*/
enum ScriptWriterFlags : unsigned int {
	/** Always surround names in quotation marks. */
	DSWF_NAME_QUOTE=1<<0,
	/** Always surround string values in quotation marks. */
	DSWF_VALUE_STRING_QUOTE=1<<2,
	/** Escape linefeeds and tabulations (regardless of quotation). */
	DSWF_ESCAPE_WHITESPACE=1<<3,
	/** Always surround names and string values in quotation marks. */
	DSWF_QUOTE=0
		|DSWF_NAME_QUOTE
		|DSWF_VALUE_STRING_QUOTE,
	/** Default flags. */
	DSWF_DEFAULT=0
		|DSWF_VALUE_STRING_QUOTE
		|DSWF_ESCAPE_WHITESPACE
};

/**
	ductScript writer.
*/
class ScriptWriter {
private:
	ScriptWriterFlags m_flags;
	IO::StreamContext m_stream_ctx;

	DUCT_DISALLOW_COPY_AND_ASSIGN(ScriptWriter);

public:
/** @name Constructors */ /// @{
	/**
		Default constructor.
		@note Uses @c DSWF_DEFAULT and the default StreamContext constructor.
	*/
	ScriptWriter()
		: m_flags(DSWF_DEFAULT)
		, m_stream_ctx()
	{}
	/**
		Constructor with flags and StreamContext properties.
		@param flags Writer flags.
		@param encoding Encoding to use for StreamContext.
		@param endian Endian to use for StreamContext.
	*/
	ScriptWriter(ScriptWriterFlags const flags, Encoding const encoding, Endian const endian)
		: m_flags(flags)
		, m_stream_ctx(encoding, endian)
	{}
	/**
		Constructor with flags and StreamContext.
		@param flags Writer flags.
		@param context StreamContext to copy.
	*/
	ScriptWriter(ScriptWriterFlags const flags, IO::StreamContext const& context)
		: m_flags(flags)
		, m_stream_ctx(context)
	{}
/// @}

/** @name Properties */ /// @{
	/**
		Set formatting flags.
		@param flags New formatting flags.
	*/
	inline void set_flags(ScriptWriterFlags const flags) { m_flags=flags; }
	/**
		Get formatting flags.
		@returns The current formatting flags.
	*/
	inline ScriptWriterFlags get_flags() const { return m_flags; }
	/**
		Get stream context.
		@returns The current stream context.
	*/
	inline IO::StreamContext& get_stream_context() { return m_stream_ctx; }
	/** @copydoc get_stream_context() */
	inline IO::StreamContext const& get_stream_context() const { return m_stream_ctx; }
/// @}

/** @name Operations */ /// @{
	/**
		Write a variable to a stream.
		@returns @c true if the variable was written to the stream, or @c false if at some point a stream operation failed.
		@param dest Destination stream.
		@param source Source variable. Can be any type.
		@param treat_as_root Whether to treat @a source as a root node if it is a node (if both are true, only the node's contents are written).
		@param tab_level Tabulation level; defaults to @c 0.
	*/
	bool write(std::ostream& dest, Variable const& source, bool const treat_as_root, unsigned int const tab_level=0) const;
/// @}

private:
	template<class stringT, class stringU=typename detail::string_traits<stringT>::encoding_utils>
	bool write_string(std::ostream& dest, stringT const& str, bool const is_name) const;
	bool write_value(std::ostream& dest, Variable const& var, bool const with_name) const;
	bool write_array(std::ostream& dest, Variable const& var, bool const with_name) const;
	bool write_node(std::ostream& dest, Variable const& var, bool const treat_as_root, unsigned int tab_level) const;
	bool write_identifier(std::ostream& dest, Variable const& var) const;
};

#include "./impl/ScriptWriter.inl"

/** @} */ // end of doc-group ductScript

} // namespace duct

#endif // DUCT_SCRIPTWRITER_HPP_
