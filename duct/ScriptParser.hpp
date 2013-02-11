/**
@file ScriptParser.hpp
@brief ductScript parser.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_SCRIPTPARSER_HPP_
#define DUCT_SCRIPTPARSER_HPP_

#include "./config.hpp"
#include "./debug.hpp"
#include "./aux.hpp"
#include "./char.hpp"
#include "./CharacterSet.hpp"
#include "./StringUtils.hpp"
#include "./Parser.hpp"
#include "./Variable.hpp"
#include "./VariableUtils.hpp"

#include <utility>
#include <exception>

namespace duct {

/**
	@addtogroup ductScript
	@{
*/

// Forward declarations
class ScriptParserException;
class ScriptParser;

/**
	ScriptParser exception.
*/
class ScriptParserException : public std::exception {
private:
	char const* m_scope;
	Token const* m_token;
	ScriptParser const* m_parser;
	char m_message[512];

public:
	/**
		Detailed constructor.
	*/
	ScriptParserException(char const scope[], Token const* token, ScriptParser const* const parser, char const fmt[], ...);
	/**
		Destructor.
	*/
	~ScriptParserException() throw() {}

	/**
		Get error message.
		@returns The error message.
	*/
	inline char const* what() const throw() { return m_message; }
};

/**
	ductScript parser.
*/
class ScriptParser /*final*/ : public Parser {
private:
	duct::aux::deque<Variable*> m_stack;
	u8string m_varname;
	unsigned int m_states;

	DUCT_DISALLOW_COPY_AND_ASSIGN(ScriptParser);

public:
/** @name Constructors */ /// @{
	/**
		Default constructor.
	*/
	ScriptParser()
		: Parser()
		, m_stack(32)
		, m_varname()
		, m_states(0)
	{}
	/**
		Constructor with StreamContext properties.
		@param encoding Encoding to use for StreamContext.
		@param endian Endian to use for StreamContext.
	*/
	ScriptParser(Encoding const encoding, Endian const endian)
		: Parser(encoding, endian)
		, m_stack(32)
		, m_varname()
		, m_states(0)
	{}
	/**
		Constructor with StreamContext.
		@param context StreamContext to copy.
	*/
	ScriptParser(IO::StreamContext const& context)
		: Parser(context)
		, m_stack(32)
		, m_varname()
		, m_states(0)
	{}
/// @}

/** @name State */ /// @{
	void reset();
private:
	void finish();
public:
/// @}

/** @name Operations */ /// @{
	/**
		Skip whitespace.
	*/
	void skip_whitespace();

	/**
		Process a stream.
		@warning The state of @a node is undefined if either @c false is returned or if an exception is thrown whilst processing.
		@returns @c true on success.
		@param[out] node Output node; will be morphed to @c VARTYPE_NODE and existing children will not be removed.
		@param stream Stream to process.
	*/
	bool process(Variable& node, std::istream& stream);

	bool parse();
	void discern_token();
	void read_token();
	void handle_token();
/// @}

private:
	void read_tok_integer();
	void read_tok_floating();
	void read_tok_literal(char32 const match_str[], unsigned int length);
	void read_tok_string();
	void read_tok_string_quoted();
	void read_tok_comment_block();

	void assign_states(unsigned int const states) { m_states|=states; }
	void remove_states(unsigned int const states) { m_states&=~states; }
	void clear_all_states() { m_states=0; }
	bool has_states(unsigned int const states) const { return states==(m_states&states); }
	bool has_states_any(unsigned int const states) const { return 0!=(m_states&states); }

	bool at_root() const;
	bool in_scope(unsigned int const type);
	Variable& get_current_collection();
	void push(Variable& collection);
	void pop();

	void throwex(ScriptParserException&& e);
	void make_name();
	void make_collection(VariableType const type, bool push_collection=true);
	void make_value();
	void make_nameless_value(int override_type=NULL_TOKEN);
};

#include "./impl/ScriptParser.inl"

/** @} */ // end of doc-group ductScript

} // namespace duct

#endif // DUCT_SCRIPTPARSER_HPP_
