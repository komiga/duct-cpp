/**
@file ScriptWriter.hpp
@brief ductScript writer.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
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

#include <utility>
#include <ostream>

namespace duct {

/**
	@addtogroup ductScript
	@{
*/

// Forward declarations
enum ScriptWriterFlags : unsigned;
class ScriptWriter;

/**
	ScriptWriter flags.
*/
enum ScriptWriterFlags : unsigned {
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
class ScriptWriter /*final*/ {
private:
	ScriptWriterFlags m_flags{DSWF_DEFAULT};
	IO::StreamContext m_stream_ctx{};

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	ScriptWriter()=default;
	/**
		Constructor with flags and StreamContext.
		@param flags Writer flags.
		@param context StreamContext to copy.
	*/
	ScriptWriter(ScriptWriterFlags const flags, IO::StreamContext context)
		: m_flags(flags)
		, m_stream_ctx(std::move(context))
	{}
	/** Copy constructor (deleted). */
	ScriptWriter(ScriptWriter const&)=delete;
	/** Move constructor. */
	ScriptWriter(ScriptWriter&&)=default;
	/** Destructor. */
	~ScriptWriter()=default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	ScriptWriter& operator=(ScriptWriter const&)=delete;
	/** Move assignment operator. */
	ScriptWriter& operator=(ScriptWriter&&)=default;
/// @}

/** @name Properties */ /// @{
	/**
		Set formatting flags.
		@param flags New formatting flags.
	*/
	void set_flags(ScriptWriterFlags const flags) { m_flags=flags; }
	/**
		Get formatting flags.
		@returns The current formatting flags.
	*/
	ScriptWriterFlags get_flags() const { return m_flags; }
	/**
		Get stream context.
		@returns The current stream context.
	*/
	IO::StreamContext& get_stream_context() { return m_stream_ctx; }
	/** @copydoc get_stream_context() */
	IO::StreamContext const& get_stream_context() const { return m_stream_ctx; }
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
	bool write(std::ostream& dest, Variable const& source, bool const treat_as_root, unsigned const tab_level=0) const;
/// @}

private:
	template<
		class StringT,
		class StringU=typename detail::string_traits<StringT>::encoding_utils
	>
	bool write_string(std::ostream& dest, StringT const& str, bool const is_name) const;
	bool write_value(std::ostream& dest, Variable const& var, bool const with_name) const;
	bool write_array(std::ostream& dest, Variable const& var, bool const with_name) const;
	bool write_node(std::ostream& dest, Variable const& var, bool const treat_as_root, unsigned tab_level) const;
	bool write_identifier(std::ostream& dest, Variable const& var) const;
};

#include "./impl/ScriptWriter.inl"

/** @} */ // end of doc-group ductScript

} // namespace duct

#endif // DUCT_SCRIPTWRITER_HPP_
