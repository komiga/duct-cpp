/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
*/

namespace {
enum {
	TOK_STRING = 1,
	TOK_STRING_QUOTED,
	TOK_INTEGER,
	TOK_FLOATING,
	TOK_LITERAL_TRUE,
	TOK_LITERAL_FALSE,
	TOK_LITERAL_NULL,

	TOK_EQUALS,
	TOK_COMMA,

	TOK_OPEN_BRACE,
	TOK_CLOSE_BRACE,
	TOK_OPEN_BRACKET,
	TOK_CLOSE_BRACKET,

	TOK_COMMENT,
	TOK_COMMENT_BLOCK,
	TOK_EOL,
	TOK_EOF
};

char const*
get_token_name(
	Token const& token
) noexcept {
	static char const* const
	s_names[]{
		"TOK_STRING",
		"TOK_STRING_QUOTED",
		"TOK_INTEGER",
		"TOK_FLOATING",
		"TOK_LITERAL_TRUE",
		"TOK_LITERAL_FALSE",
		"TOK_LITERAL_NULL",

		"TOK_EQUALS",
		"TOK_COMMA",

		"TOK_OPEN_BRACE",
		"TOK_CLOSE_BRACE",
		"TOK_OPEN_BRACKET",
		"TOK_CLOSE_BRACKET",

		"TOK_COMMENT",
		"TOK_COMMENT_BLOCK",
		"TOK_EOL",
		"TOK_EOF"
	};

	if (token.is_null()) {
		return "NULL_TOKEN";
	} else if (
		1 <= token.type() &&
		signed_cast(std::extent<decltype(s_names)>::value)
		>= token.type()
	) {
		return s_names[token.type() - 1];
	} else {
		return "TOK_UNKNOWN";
	}
}

static CharacterSet const
	s_set_whitespace{"\t "},
	s_set_sign{"\\-+"},
	s_set_numeral{"0-9"},
	// Linefeed, s_set_whitespace, and functors
	s_set_terminator{"\n\t ,=[]{}"}
;
static StringUtils::EscapeablePair const
s_esc_pair{
	"\n\r\t,=[]{}\"\'\\",
	"nrt,=[]{}\"\'\\"
};

static char32 const
	s_lit_true[]{'t','r','u','e'},
	s_lit_false[]{'f','a','l','s','e'},
	s_lit_null[]{'n','u','l','l'}
;
} // anonymous namespace

#define DUCT_SP_THROW_(fmt_) \
	throwex({DUCT_FUNC_NAME, &m_token, this, fmt_})
#define DUCT_SP_THROWF_(fmt_, ...) \
	throwex({DUCT_FUNC_NAME, &m_token, this, fmt_, __VA_ARGS__})
#define DUCT_SP_THROW_NO_INFO_(fmt_) \
	throwex({DUCT_FUNC_NAME, nullptr, nullptr, fmt_})
#define DUCT_SP_THROWF_NO_INFO_(fmt_, ...) \
	throwex({DUCT_FUNC_NAME, nullptr, nullptr, fmt_, __VA_ARGS__})

// class ScriptParserException implementation

ScriptParserException::ScriptParserException(
	char const scope[],
	Token const* token,
	ScriptParser const* const parser,
	char const fmt[],
	...
)
	: std::exception()
	, m_scope(scope)
	, m_token(token)
	, m_parser(parser)
{
	char formatted_message[256];
	signed start_line = -1, start_column = -1;
	signed end_line = -2, end_column = -2;
	va_list args;
	va_start(args, fmt);
	std::vsnprintf(formatted_message, 256, fmt, args);
	va_end(args);
	if (m_parser && !m_token) {
		m_token = &(m_parser->token());
	}
	if (m_token) {
		start_line = m_token->line();
		start_column = m_token->column();
	}
	if (m_parser) {
		end_line = m_parser->line();
		end_column = m_parser->column();
	}
	if (start_line == end_line && start_column == end_column) {
		std::snprintf(m_message, 512,
			"(%s) at [%d:%d]: %s",
			m_scope,
			start_line, start_column,
			formatted_message
		);
	} else if (-1 != start_line) {
		std::snprintf(m_message, 512,
			"(%s) from [%d:%d] to [%d:%d]: %s",
			m_scope,
			start_line, start_column,
			end_line, end_column,
			formatted_message
		);
	} else {
		std::snprintf(m_message, 512,
			"(%s): %s",
			m_scope,
			formatted_message
		);
	}
}

// class ScriptParser implementation

void
ScriptParser::reset() noexcept {
	Parser::reset();
	m_stack.clear();
	m_states.clear();
	m_token_ident.reset(NULL_TOKEN, true);
}

void
ScriptParser::finish() {
	if (m_states.test(State::equals)) {
		DUCT_SP_THROW_("Expected value, got EOL/EOF");
	} else if (m_token.is_type(TOK_EOF) && m_states.test(State::comma)) {
		DUCT_SP_THROW_("Expected value, got EOF");
	} else if (!m_token_ident.is_null()) { // No-child identifier
		if (in_scope(VarType::identifier)) {
			make_nameless_value();
		} else {
			make_collection(VarType::identifier, false);
		}
	} else if (in_scope(VarType::identifier)) { // Terminates identifier
		pop();
	}
}

void
ScriptParser::skip_whitespace() {
	while (CHAR_EOF != m_curchar && s_set_whitespace.contains(m_curchar)) {
		next_char();
	}
}

bool
ScriptParser::process(
	Var& node,
	std::istream& stream
) {
	if (initialize(stream)) {
		node.morph(VarType::node, false); // Make sure variable is a node
		push(node);
		do {} while (parse());
		finish();
		if (!at_root()) {
			DUCT_SP_THROWF_NO_INFO_(
				"Unclosed collection at EOF: %lu deep in %s scope",
				static_cast<unsigned long>(m_stack.size()),
				var_type_name(m_stack.back()->type())
			);
		}
		reset();
		return true;
	} else {
		return false;
	}
}

bool
ScriptParser::parse() {
	//next_char();
	skip_whitespace();
	discern_token();
	read_token();
	if (m_token.is_type(TOK_EOF)) {
		return false;
	} else {
		return true;
	}
}

void
ScriptParser::discern_token() {
	m_token.reset(NULL_TOKEN, false);
	m_token.set_position(m_line, m_column);
	switch (m_curchar) {
	case CHAR_QUOTE:
		m_token.set_type(TOK_STRING_QUOTED);
		break;

	case CHAR_DECIMALPOINT:
		m_token.set_type(TOK_FLOATING);
		// Add the decimal
		m_token.buffer().push_back(m_curchar);
		break;

	case CHAR_PLUS:
	case CHAR_DASH:
		m_token.set_type(TOK_INTEGER);
		// Add sign
		m_token.buffer().push_back(m_curchar);
		break;

	// Literals
	case CHAR_T: m_token.set_type(TOK_LITERAL_TRUE); break;
	case CHAR_F: m_token.set_type(TOK_LITERAL_FALSE); break;
	case CHAR_N: m_token.set_type(TOK_LITERAL_NULL); break;

	// Scope/functors
	case CHAR_EQUALSIGN: m_token.set_type(TOK_EQUALS); break;
	case CHAR_COMMA: m_token.set_type(TOK_COMMA); break;
	case CHAR_OPENBRACE: m_token.set_type(TOK_OPEN_BRACE); break;
	case CHAR_CLOSEBRACE: m_token.set_type(TOK_CLOSE_BRACE); break;
	case CHAR_OPENBRACKET: m_token.set_type(TOK_OPEN_BRACKET); break;
	case CHAR_CLOSEBRACKET: m_token.set_type(TOK_CLOSE_BRACKET); break;

	// Terminators
	case CHAR_ASTERISK:
		if (CHAR_SLASH == peek_char()) {
			DUCT_SP_THROW_("Encountered unexpected end of block comment");
		}
		m_token.set_type(TOK_STRING);
		break;

	case CHAR_SLASH:
		if (CHAR_SLASH == peek_char()) {
			m_token.set_type(TOK_COMMENT);
		} else if (CHAR_ASTERISK == m_peekchar) {
			m_token.set_type(TOK_COMMENT_BLOCK);
		} else {
			m_token.set_type(TOK_STRING);
		}
		break;

	case CHAR_NEWLINE: m_token.set_type(TOK_EOL); break;
	case CHAR_EOF: m_token.set_type(TOK_EOF); break;

	// None of the above; guess whether it's a string or an integer
	// depending on first character
	default:
		m_token.set_type(
			(s_set_numeral.contains(m_curchar))
			? TOK_INTEGER
			: TOK_STRING
		);
		break;
	}
}

void
ScriptParser::read_token() {
	//DUCT_DEBUGCF("token-type:%s line:%d, col:%d\n",
	//	get_token_name(m_token), m_token.line(), m_token.column());
	switch (m_token.type()) {
	case TOK_STRING_QUOTED:
		read_tok_string_quoted();
		next_char();
		break;

	case TOK_STRING:
		read_tok_string();
		break;

	case TOK_INTEGER:
		if (CHAR_PLUS == m_curchar || CHAR_DASH == m_curchar) {
			// Move out of sign
			next_char();
		}
		read_tok_integer();
		break;

	case TOK_FLOATING:
		// Move out of decimal
		next_char();
		read_tok_floating();
		break;

	// Literals
	case TOK_LITERAL_TRUE:
		read_tok_literal(s_lit_true, 4u);
		break;

	case TOK_LITERAL_FALSE:
		read_tok_literal(s_lit_false, 5u);
		break;

	case TOK_LITERAL_NULL:
		read_tok_literal(s_lit_null, 4u);
		break;

	// Scope/functors
	case TOK_EQUALS:
	case TOK_COMMA:
	case TOK_OPEN_BRACE:
	case TOK_CLOSE_BRACE:
	case TOK_OPEN_BRACKET:
	case TOK_CLOSE_BRACKET:
		next_char();
		break;

	// Terminators
	case TOK_COMMENT:
		skip_to_eol();
		break;

	case TOK_COMMENT_BLOCK:
		read_tok_comment_block();
		break;

	case TOK_EOL:
		// TODO: handle m_token_ident?
		next_char();
		break;

	case TOK_EOF:
		// Nothing to do
		break;

	default:
		DUCT_DEBUG_ASSERT(
			false,
			"Something has gone horribly wrong:"
			" unhandled token type!"
		);
		DUCT_SP_THROWF_(
			"Unhandled token type: '%s'",
			get_token_name(m_token)
		);
		break;
	}
	// Special case: when number and floating-point tokens only contain
	// signs or periods
	switch (m_token.type()) {
	case TOK_INTEGER:
		if (m_token.buffer().compare(s_set_sign)) {
			m_token.set_type(TOK_STRING);
		}
		break;
	case TOK_FLOATING:
		if (m_token.buffer().compare(s_set_sign) ||
			m_token.buffer().compare(CHAR_DECIMALPOINT)
		) {
			m_token.set_type(TOK_STRING);
		}
		break;
	default:
		break;
	}
	handle_token();
}

void
ScriptParser::handle_token() {
	switch (m_token.type()) {
	// Value or name
	case TOK_STRING:
	case TOK_STRING_QUOTED:
	case TOK_INTEGER:
	case TOK_FLOATING:
	case TOK_LITERAL_TRUE:
	case TOK_LITERAL_FALSE:
	case TOK_LITERAL_NULL:
		if (
			in_scope(VarType::array) &&
			!m_states.test_any(State::comma | State::open_array)
		) {
			DUCT_SP_THROW_(
				"Expected comma separator before value token in array scope"
			);
		} else if (m_states.test(State::equals)) {
			// Make named value
			make_value();
		} else if (in_scope(VarType::array)) {
			make_ident();
		} else if (in_scope(VarType::identifier)) {
			if (!m_token_ident.is_null()) {
				// Make nameless value with ident
				make_nameless_value();
			}
			make_ident();
		} else {
			if (!m_token_ident.is_null()) {
				// Have ident and last functor was whitespace: make
				// identifier with current ident
				make_collection(VarType::identifier);
			}
			make_ident();
		}
		break;

	// Scope/functors
	case TOK_EQUALS:
		if (in_scope(VarType::array)) {
			DUCT_SP_THROW_("Unexpected equality sign within array scope");
		} else if (m_token_ident.is_null()) {
			DUCT_SP_THROW_("Expected name, got equality sign");
		} else if (m_states.test(State::equals)) {
			DUCT_SP_THROW_(
				"Expected value after equality sign, got equality sign");
		} else if (m_states.test(State::comma)) {
			DUCT_SP_THROW_("Expected value after comma, got equality sign");
		} else {
			m_states.enable(State::equals);
		}
		break;

	case TOK_COMMA:
		if (!in_scope(VarType::array)) {
			DUCT_SP_THROW_("Unexpected comma in non-array scope");
		} else if (m_states.test(State::equals)) {
			DUCT_SP_THROW_("Expected value after equality sign, got comma");
		} else if (!m_token_ident.is_null()) {
			// Terminate ident into value
			make_nameless_value();
			m_states.enable(State::comma);
		} else if (m_states.test(State::open_array)) {
			DUCT_SP_THROW_("Expected value before comma");
		} else if (m_states.test(State::comma)) {
			DUCT_SP_THROW_("Expected value after comma, got comma");
		} else {
			m_states.enable(State::comma);
		}
		break;

	case TOK_OPEN_BRACE:
		if (in_scope(VarType::array)) {
			DUCT_SP_THROW_("Unexpected open-brace in array scope");
		} else if (in_scope(VarType::identifier)) {
			DUCT_SP_THROW_(
				"Cannot make node on same line as identifier with children");
		} else if (!m_token_ident.is_null() && !m_states.test(State::equals)) {
			DUCT_SP_THROW_(
				"Unexpected open-brace after name; possibly missing equality"
				" sign or attempting to (illegally) add node to identifier"
			);
		} else {
			make_collection(VarType::node);
		}
		break;

	case TOK_CLOSE_BRACE:
		if (at_root()) {
			DUCT_SP_THROW_("Mismatched node brace");
		} else if (in_scope(VarType::array)) {
			DUCT_SP_THROW_("Unexpected close-brace in array scope");
		} else if (m_states.test(State::equals)) {
			DUCT_SP_THROW_(
				"Expected value after equality sign, got close-brace");
		} else if (m_states.test(State::comma)) {
			DUCT_SP_THROW_("Expected value after comma, got close-brace");
		} else {
			if (!m_token_ident.is_null()) {
				// Terminative on a yet-realized (empty) identifier
				make_collection(VarType::identifier, false);
			} else if (in_scope(VarType::identifier)) {
				// Pop from identifier to parent (node) scope
				pop();
			}
			// Pop from node to parent scope
			pop();
		}
		break;

	case TOK_OPEN_BRACKET:
		if (
			in_scope(VarType::array) &&
			!m_states.test_any(State::comma | State::open_array)
		) {
			DUCT_SP_THROW_(
				"Unexpected token after non-open-bracket and"
				" non-comma in array scope"
			);
		} else {
			if (!m_token_ident.is_null() && !m_states.test(State::equals)) {
				if (in_scope(VarType::identifier)) {
					// Terminative on nameless value in identifier
					make_nameless_value();
				} else {
					// Terminative on a yet-realized (empty) identifier
					make_collection(VarType::identifier);
				}
			}
			make_collection(VarType::array);
			m_states.enable(State::open_array);
		}
		break;

	case TOK_CLOSE_BRACKET:
		if (at_root()) {
			DUCT_SP_THROW_("Mismatched array bracket");
		} else if (!in_scope(VarType::array)) {
			DUCT_SP_THROWF_(
				"Unexpected close-bracket in %s scope",
				var_type_name(current_collection().type())
			);
		} else if (m_states.test(State::equals)) {
			DUCT_SP_THROW_(
				"Expected value after equality sign, got close-bracket");
		} else if (m_states.test(State::comma)) {
			DUCT_SP_THROW_("Expected value after comma, got close-bracket");
		} else {
			if (!m_token_ident.is_null()) {
				// Terminate ident into value
				make_nameless_value();
			}
			// Pop from array to parent scope
			pop();
			m_states.disable(State::open_array);
		}
		break;

	// Terminators
	case TOK_COMMENT:
	case TOK_COMMENT_BLOCK:
		// Do nothing
		break;

	case TOK_EOL:
	case TOK_EOF:
		finish();
		break;

	default:
		DUCT_DEBUG_ASSERTF(
			false, "Unhandled token of type \'%s\'",
			get_token_name(m_token)
		);
		break;
	}
}

void
ScriptParser::read_tok_integer() {
	while (CHAR_EOF != m_curchar) {
		if (CHAR_QUOTE == m_curchar) {
			DUCT_SP_THROW_("Unexpected quotation mark");
		} else if (CHAR_SLASH == m_curchar) {
			if (CHAR_SLASH == peek_char() || CHAR_ASTERISK == m_peekchar) {
				// Comment or comment block terminate
				break;
			} else {
				m_token.set_type(TOK_STRING);
				read_tok_string();
				return;
			}
		} else if (s_set_terminator.contains(m_curchar)) {
			// Single terminators
			break;
		} else if (s_set_numeral.contains(m_curchar)) {
			m_token.buffer().push_back(m_curchar);
		} else if (CHAR_DECIMALPOINT == m_curchar) {
			m_token.buffer().push_back(m_curchar);
			m_token.set_type(TOK_FLOATING);
			next_char();
			read_tok_floating();
			return;
		} else {
			m_token.set_type(TOK_STRING);
			read_tok_string();
			return;
		}
		next_char();
	}
}

void
ScriptParser::read_tok_floating() {
	while (CHAR_EOF != m_curchar) {
		if (CHAR_QUOTE == m_curchar) {
			DUCT_SP_THROW_("Unexpected quotation mark");
		} else if (CHAR_SLASH == m_curchar) {
			if (CHAR_SLASH == peek_char() || CHAR_ASTERISK == m_peekchar) {
				// Comment or comment block terminate
				break;
			} else {
				m_token.set_type(TOK_STRING);
				read_tok_string();
				return;
			}
		} else if (s_set_terminator.contains(m_curchar)) {
			// Single terminators
			break;
		} else if (s_set_numeral.contains(m_curchar)) {
			m_token.buffer().push_back(m_curchar);
		} else {
			m_token.set_type(TOK_STRING);
			read_tok_string();
			return;
		}
		next_char();
	}
}

void
ScriptParser::read_tok_literal(
	char32 const match_str[],
	unsigned const length
) {
	unsigned index = 0;
	while (CHAR_EOF != m_curchar) {
		if (CHAR_QUOTE == m_curchar) {
			DUCT_SP_THROW_("Unexpected quotation mark");
		} else if (
			// Single terminators
			s_set_terminator.contains(m_curchar) ||
			// Comment or comment block terminate
			(CHAR_SLASH == m_curchar
			&& (CHAR_SLASH == peek_char() || CHAR_ASTERISK == m_peekchar))
		) {
			if (length > index) {
				// Haven't yet checked all characters in match string;
				// must be a normal string
				m_token.set_type(TOK_STRING);
			}
			break;
		} else if (length <= index || match_str[index] != m_curchar) {
			// Already overrun length or char does not match current
			m_token.set_type(TOK_STRING);
			read_tok_string();
			return;
		} else { // Haven't overrun and char matches current
			m_token.buffer().push_back(m_curchar);
			++index;
		}
		next_char();
	}
}

void
ScriptParser::read_tok_string() {
	while (CHAR_EOF != m_curchar) {
		if (CHAR_QUOTE == m_curchar) {
			DUCT_SP_THROW_("Unexpected quotation mark");
		} else if (CHAR_BACKSLASH == m_curchar) {
			char32 const
				cp = StringUtils::get_escape_char(next_char(), s_esc_pair);
			if (CHAR_EOF == m_curchar || CHAR_NEWLINE == m_curchar) {
				DUCT_SP_THROW_("Expected escape sequence, got EOL/EOF");
			} else if (CHAR_NULL == cp) {
				DUCT_SP_THROWF_("Unexpected escape sequence: '%c' (0x%X)",
					m_curchar, m_curchar);
			} else {
				m_token.buffer().push_back(cp);
			}
		} else if (
			// Single terminators
			s_set_terminator.contains(m_curchar) ||
			// Comment or comment block terminate
			(CHAR_SLASH == m_curchar
			&& (CHAR_SLASH == peek_char() || CHAR_ASTERISK == m_peekchar))
		) {
			break;
		} else {
			m_token.buffer().push_back(m_curchar);
		}
		next_char();
	}
}

void
ScriptParser::read_tok_string_quoted() {
	bool eol_reached = false;
	next_char(); // Skip the first character (it will be the initial quote)
	while (CHAR_QUOTE != m_curchar) {
		if (CHAR_EOF == m_curchar) {
			DUCT_SP_THROW_("Encountered EOF whilst reading quoted string");
		} else if (CHAR_BACKSLASH == m_curchar) {
			char32 const
				cp = StringUtils::get_escape_char(next_char(), s_esc_pair);
			if (CHAR_EOF == m_curchar || CHAR_NEWLINE == m_curchar) {
				DUCT_SP_THROW_("Expected escape sequence, got EOL/EOF");
			} else if (CHAR_NULL == cp) {
				DUCT_SP_THROWF_("Unexpected escape sequence: '%c' (0x%X)",
					m_curchar, m_curchar);
			} else {
				m_token.buffer().push_back(cp);
			}
		} else {
			if (!eol_reached) {
				m_token.buffer().push_back(m_curchar);
			}
			if (CHAR_NEWLINE == m_curchar) {
				eol_reached = true;
			} else if (eol_reached && !s_set_whitespace.contains(m_curchar)) {
				eol_reached = false;
				m_token.buffer().push_back(m_curchar);
			}
		}
		next_char();
	}
}

void
ScriptParser::read_tok_comment_block() {
	next_char(); // Skip the first character (it will be an asterisk)
	if (CHAR_EOF != m_curchar) {
		while (skip_to(CHAR_ASTERISK)) {
			if (CHAR_SLASH == next_char()) {
				// Get the next character, otherwise the discern_token()
				// call will try to handle the slash
				next_char();
				return;
			}
		}
	}
	DUCT_SP_THROW_("Unexpected EOF whilst reading comment block");
}

bool
ScriptParser::at_root() const noexcept {
	DUCT_DEBUG_ASSERT(
		0 != m_stack.size(),
		"Something has gone horribly wrong: the stack shouldn't be empty"
	);
	return 1 >= m_stack.size();
}

Var&
ScriptParser::current_collection() noexcept {
	DUCT_DEBUG_ASSERT(
		0 < m_stack.size(),
		"Something has gone horribly wrong: stack is empty!"
	);
	DUCT_DEBUG_ASSERT(
		nullptr != m_stack.back(),
		"Something has gone horribly wrong: stack.back() is null!"
	);
	return *(m_stack.back());
}

void
ScriptParser::push(
	Var& collection
) {
	DUCT_DEBUG_ASSERT(
		collection.is_type_of(VarMask::collection),
		"Something has gone horribly wrong:"
		" cannot push a non-collection"
	);
	DUCT_DEBUG_ASSERT(
		0 == m_stack.size() ||
		!collection.is_type(VarType::node) ||
		!in_scope(VarType::identifier),
		"Something has gone horribly wrong:"
		" cannot push a node whilst in identifier scope"
	);
	//DUCT_DEBUGF("at %lu, pushing %s",
	//	static_cast<unsigned long>(m_stack.size()),
	//	var_type_name(collection.type())
	//);
	if (0 < m_stack.size()) {
		DUCT_DEBUG_ASSERT(
			(&collection) != m_stack.back(),
			"Something has gone horribly wrong:"
			" pushing same collection"
		);
	}
	m_stack.push_back(&collection);
}

void
ScriptParser::pop() {
	DUCT_DEBUG_ASSERT(
		1 < m_stack.size(),
		"Something has gone horribly wrong:"
		" the root node cannot be popped"
	);
	m_stack.pop_back();
}

void
ScriptParser::throwex(
	ScriptParserException&& e
) {
	reset();
	throw std::move(e);
}

void
ScriptParser::make_ident() {
	DUCT_DEBUG_ASSERT(
		m_token_ident.is_null(),
		"Something has gone horribly wrong:"
		" already have name"
	);
	DUCT_DEBUG_ASSERT(
		!m_states.test(State::equals),
		"Something has gone horribly wrong:"
		" should not have State::equals here"
	);
	m_token_ident = m_token;
	m_states.remove(State::comma | State::open_array);
}

void
ScriptParser::make_collection(
	VarType const type,
	bool push_collection
) {
	DUCT_DEBUG_ASSERT(
		in_scope(VarType::node) ||
		VarType::array == type,
		"Something has gone horribly wrong:"
		" cannot make a node or an identifier whilst"
		" in an identifier or array scope"
	);
	if (m_token_ident.is_null()) {
		// Unnamed collection; must not be an identifier
		DUCT_DEBUG_ASSERT(VarType::identifier != type,
			"Something has gone horribly wrong:"
			" cannot make a nameless identifier"
		);
		current_collection().emplace_back(type);
		m_states.remove(State::comma | State::open_array);
	} else { // Named collection
		DUCT_DEBUG_ASSERT(
			VarType::identifier != type ||
			!m_states.test(State::equals),
			"Something has gone horribly wrong:"
			" cannot have equality sign when making an identifier"
		);
		current_collection().emplace_back(
			m_token_ident.buffer()
				.to_string<detail::var_config::name_type>(),
			type
		);
		m_states.remove(State::equals | State::comma | State::open_array);
		m_token_ident.reset(NULL_TOKEN, true);
	}
	if (push_collection) {
		push(current_collection().back());
	}
}

void
ScriptParser::make_value() {
	/*DUCT_DEBUG_ASSERT(in_scope(VarMask::collection),
		"Something has gone horribly wrong:"
		" attempted to make a named value whilst in a non-node scope"
	);*/
	DUCT_DEBUG_ASSERT(!m_token_ident.is_null(),
		"Something has gone horribly wrong:"
		" must have name to make named value"
	);
	DUCT_DEBUG_ASSERT(m_states.test(State::equals),
		"Something has gone horribly wrong:"
		" must have State::equals to make a value"
	);
	DUCT_DEBUG_ASSERT(!m_states.test(State::comma),
		"Something has gone horribly wrong:"
		" should not have State::comma here"
	);

	auto& coll = current_collection();
	switch (m_token.type()) {
	case TOK_STRING:
	case TOK_STRING_QUOTED:
		coll.emplace_back(
			m_token.buffer().to_string<detail::var_config::string_type>()
		); break;

	case TOK_INTEGER:
		coll.emplace_back(
			m_token.buffer().to_arithmetic<detail::var_config::integer_type>()
		); break;

	case TOK_FLOATING:
		coll.emplace_back(
			m_token.buffer().to_arithmetic<detail::var_config::decimal_type>()
		); break;

	case TOK_LITERAL_TRUE:
		coll.emplace_back(true);
		break;

	case TOK_LITERAL_FALSE:
		coll.emplace_back(false);
		break;

	case TOK_LITERAL_NULL:
		coll.emplace_back(VarType::null);
		break;
	}
	coll.back().set_name(
		m_token_ident.buffer().to_string<detail::var_config::name_type>()
	);
	m_states.remove(State::equals);
	m_token_ident.reset(NULL_TOKEN, true);
}

void
ScriptParser::make_nameless_value(
	signed override_type
) {
	/*DUCT_DEBUG_ASSERT(m_token_ident.is_null(),
		"Something has gone horribly wrong:"
		" name should be empty here"
	);*/
	DUCT_DEBUG_ASSERT(!m_states.test(State::equals),
		"Something has gone horribly wrong:"
		" should not have State::equals here"
	);
	DUCT_DEBUG_ASSERT(!in_scope(VarType::node),
		"Something has gone horribly wrong:"
		" can only make a nameless value when in non-node scope"
	);
	Token& token
		= m_token_ident.is_null()
		? m_token
		: m_token_ident
	;
	if (NULL_TOKEN == override_type) {
		override_type = token.type();
	}
	auto& coll = current_collection();
	switch (override_type) {
	case TOK_STRING:
	case TOK_STRING_QUOTED:
		coll.emplace_back(
			token.buffer().to_string<detail::var_config::string_type>()
		); break;

	case TOK_INTEGER:
		coll.emplace_back(
			token.buffer().to_arithmetic<detail::var_config::integer_type>()
		); break;

	case TOK_FLOATING:
		coll.emplace_back(
			token.buffer().to_arithmetic<detail::var_config::decimal_type>()
		); break;

	case TOK_LITERAL_TRUE:
		coll.emplace_back(true);
		break;

	case TOK_LITERAL_FALSE:
		coll.emplace_back(false);
		break;

	case TOK_LITERAL_NULL:
		coll.emplace_back(VarType::null);
		break;
	}
	if (!m_token_ident.is_null()) {
		m_token_ident.reset(NULL_TOKEN, true);
	}
	m_states.remove(State::comma | State::open_array);
}

#undef DUCT_SP_THROWF_NO_INFO_
#undef DUCT_SP_THROW_NO_INFO_
#undef DUCT_SP_THROWF_
#undef DUCT_SP_THROW_
