/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ductScript parser.
*/

#pragma once

#include "./config.hpp"
#include "./debug.hpp"
#include "./aux.hpp"
#include "./char.hpp"
#include "./utility.hpp"
#include "./StateStore.hpp"
#include "./CharacterSet.hpp"
#include "./StringUtils.hpp"
#include "./Parser.hpp"
#include "./Var.hpp"
#include "./VarUtils.hpp"

#include <cstdio>
#include <cstdarg>
#include <utility>
#include <exception>
#include <type_traits>

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
class ScriptParserException final
	: public std::exception
{
private:
	char const* m_scope;
	Token const* m_token;
	ScriptParser const* m_parser;
	char m_message[512];

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor (deleted). */
	ScriptParserException() = delete;

	/**
		Construct with details.

		@param scope Scope of exception.
		@param token Token.
		@param parser Parser.
		@param fmt Message format string.
		@param ... Message parameters.
	*/
	ScriptParserException(
		char const scope[],
		Token const* token,
		ScriptParser const* const parser,
		char const fmt[],
		...
	);

	/** Copy constructor (deleted). */
	ScriptParserException(ScriptParserException const&) = delete;
	/** Move constructor. */
	ScriptParserException(ScriptParserException&&) = default;
	/** Destructor. */
	~ScriptParserException() noexcept override = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	ScriptParserException& operator=(ScriptParserException const&) = delete;
	/** Move assignment operator. */
	ScriptParserException& operator=(ScriptParserException&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get error message.

		@returns The error message.
	*/
	char const*
	what() const noexcept override {
		return m_message;
	}
/// @}
};

/**
	ductScript parser.
*/
class ScriptParser final
	: public Parser
{
private:
	enum class State : unsigned {
		equals = bit(0),
		comma = bit(1),
		open_array = bit(2)
	};

	duct::aux::deque<Var*> m_stack{32u};
	StateStore<State> m_states{};
	Token m_token_ident{NULL_TOKEN, 128u};

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	ScriptParser() = default;

	/**
		Constructor with StreamContext.

		@param context StreamContext to copy.
	*/
	explicit
	ScriptParser(
		IO::StreamContext context
	) noexcept
		: Parser(std::move(context))
	{}

	/** Copy constructor (deleted). */
	ScriptParser(ScriptParser const&) = delete;
	/** Move constructor. */
	ScriptParser(ScriptParser&&) = default;
	/** Destructor. */
	~ScriptParser() override = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	ScriptParser& operator=(ScriptParser const&) = delete;
	/** Move assignment operator. */
	ScriptParser& operator=(ScriptParser&&) = default;
/// @}

/** @name State */ /// @{
	void
	reset() noexcept override;

private:
	void
	finish();

public:
/// @}

/** @name Operations */ /// @{
	/**
		Skip whitespace.
	*/
	void
	skip_whitespace();

	/**
		Process a stream.

		@warning The state of @a node is undefined if either @c false
		is returned or if an exception is thrown whilst processing.

		@returns @c true on success.
		@param[out] node Output node; will be morphed
		to @c VarType::node and existing children will not be removed.
		@param stream Stream to process.
	*/
	bool
	process(
		Var& node,
		std::istream& stream
	);

	bool
	parse() override;

	void
	discern_token() override;

	void
	read_token() override;

	void
	handle_token() override;
/// @}

private:
	void
	read_tok_integer();

	void
	read_tok_floating();

	void
	read_tok_literal(
		char32 const match_str[],
		unsigned length
	);

	void
	read_tok_string();

	void
	read_tok_string_quoted();

	void
	read_tok_comment_block();

	bool
	at_root() const noexcept;

	bool
	in_scope(
		VarMask const mask
	) noexcept {
		return current_collection().is_type_of(mask);
	}

	bool
	in_scope(
		VarType const type
	) noexcept {
		return in_scope(static_cast<VarMask>(type));
	}

	Var&
	current_collection() noexcept;

	void
	push(
		Var& collection
	);

	void
	pop();

	void
	throwex(
		ScriptParserException&& e
	);

	void
	make_ident();

	void
	make_value_from_name();

	void
	make_collection(
		VarType const type,
		bool push_collection = true
	);

	void
	make_value();

	void
	make_nameless_value(
		signed override_type = NULL_TOKEN
	);
};

#include "./impl/ScriptParser.inl"

/** @} */ // end of doc-group ductScript

} // namespace duct
