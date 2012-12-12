/**
@file impl/ScriptParser.inl

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

namespace {
enum {
	TOK_STRING=1,
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

enum {
	STATE_EQUALS=1<<0,
	STATE_COMMA=1<<1,
	STATE_OPEN_ARRAY=1<<2
};

char const* get_token_name(Token const& token) {
	static char const* const s_names[]={
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
	if (token.is_type(NULL_TOKEN)) {
		return "NULL_TOKEN";
	} else if ((sizeof(s_names)/sizeof(*s_names))>=static_cast<unsigned int>(token.get_type()) && 1<=token.get_type()) {
		return s_names[token.get_type()-1];
	} else {
		return "TOK_UNKNOWN";
	}
}

static CharacterSet const s_set_whitespace{"\t "};
static CharacterSet const s_set_sign{"\\-+"};
static CharacterSet const s_set_numeral{"0-9"};
static CharacterSet const s_set_terminator{"\n\t ,=[]{}"}; // Linefeed, s_set_whitespace, and functors
static StringUtils::EscapeablePair const s_esc_pair{"\n\r\t,=[]{}\"\'\\", "nrt,=[]{}\"\'\\"};

static char32 const s_lit_true[]={'t','r','u','e'};
static char32 const s_lit_false[]={'f','a','l','s','e'};
static char32 const s_lit_null[]={'n','u','l','l'};
} // anonymous namespace

#define DUCT_SP_THROW__(fmt_) throwex({DUCT_FUNC_NAME, &m_token, this, fmt_})
#define DUCT_SP_THROWF__(fmt_, ...) throwex({DUCT_FUNC_NAME, &m_token, this, fmt_, __VA_ARGS__})
#define DUCT_SP_THROW_NO_INFO__(fmt_) throwex({DUCT_FUNC_NAME, nullptr, nullptr, fmt_})
#define DUCT_SP_THROWF_NO_INFO__(fmt_, ...) throwex({DUCT_FUNC_NAME, nullptr, nullptr, fmt_, __VA_ARGS__})

// class ScriptParserException implementation

ScriptParserException::ScriptParserException(char const scope[], Token const* token, ScriptParser const* const parser, char const fmt[], ...)
	: std::exception()
	, m_scope(scope)
	, m_token(token)
	, m_parser(parser)
{
	char formatted_message[256];
	int start_line=-1, start_column=-1;
	int end_line=-2, end_column=-2;
	va_list args;
	va_start(args, fmt);
	vsnprintf(formatted_message, 256, fmt, args);
	va_end(args);
	if (m_parser && !m_token) {
		m_token=&(m_parser->get_token());
	}
	if (m_token) {
		start_line=m_token->get_line(); start_column=m_token->get_column();
	}
	if (m_parser) {
		end_line=m_parser->get_line(); end_column=m_parser->get_column();
	}
	if (start_line==end_line && start_column==end_column) {
		snprintf(m_message, 512, "(%s) at [%d:%d]: %s", m_scope, start_line, start_column, formatted_message);
	} else if (-1!=start_line) {
		snprintf(m_message, 512, "(%s) from [%d:%d] to [%d:%d]: %s", m_scope, start_line, start_column, end_line, end_column, formatted_message);
	} else {
		snprintf(m_message, 512, "(%s): %s", m_scope, formatted_message);
	}
}

// class ScriptParser implementation

void ScriptParser::reset() {
	Parser::reset();
	m_stack.clear();
	m_varname.clear();
	clear_all_states();
}

void ScriptParser::finish() {
	if (has_states(STATE_EQUALS)) {
		DUCT_SP_THROW__("Expected value, got EOL/EOF");
	} else if (m_token.is_type(TOK_EOF) && has_states(STATE_COMMA)) {
		DUCT_SP_THROW__("Expected value, got EOF");
	} else if (!m_varname.empty()) { // No-child identifier
		make_collection(VARTYPE_IDENTIFIER, false);
	} else if (in_scope(VARTYPE_IDENTIFIER)) { // Terminates identifier
		pop();
	}
}

void ScriptParser::skip_whitespace() {
	while (CHAR_EOF!=m_curchar && s_set_whitespace.contains(m_curchar)) {
		next_char();
	}
}

bool ScriptParser::process(Variable& node, std::istream& stream) {
	if (initialize(stream)) {
		node.morph(VARTYPE_NODE, false); // Make sure variable is a node
		push(node);
		while (parse())
		{}
		finish();
		if (!at_root()) {
			DUCT_SP_THROWF_NO_INFO__("Unclosed collection at EOF: %lu deep in %s scope",
				static_cast<unsigned long>(m_stack.size()), detail::get_vartype_name(m_stack.back()->get_type()));
		}
		reset();
		return true;
	} else {
		return false;
	}
}

bool ScriptParser::parse() {
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

void ScriptParser::discern_token() {
	m_token.reset(NULL_TOKEN, false);
	m_token.set_position(m_line, m_column);
	switch (m_curchar) {
	case CHAR_QUOTE: m_token.set_type(TOK_STRING_QUOTED); break;
	case CHAR_DECIMALPOINT:
		m_token.set_type(TOK_FLOATING);
		m_token.get_buffer().push_back(m_curchar); // Add the decimal
		break;
	case CHAR_PLUS:
	case CHAR_DASH:
		m_token.set_type(TOK_INTEGER);
		m_token.get_buffer().push_back(m_curchar); // Add sign
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
		if (CHAR_SLASH==peek_char()) {
			DUCT_SP_THROW__("Encountered unexpected end of block comment");
		}
		m_token.set_type(TOK_STRING);
		break;
	case CHAR_SLASH:
		if (CHAR_SLASH==peek_char()) {
			m_token.set_type(TOK_COMMENT);
		} else if (CHAR_ASTERISK==m_peekchar) {
			m_token.set_type(TOK_COMMENT_BLOCK);
		} else {
			m_token.set_type(TOK_STRING);
		}
		break;
	case CHAR_NEWLINE: m_token.set_type(TOK_EOL); break;
	case CHAR_EOF: m_token.set_type(TOK_EOF); break;

	default: // None of the above; guess whether it's a string or an integer depending on first character
		m_token.set_type((s_set_numeral.contains(m_curchar)) ? TOK_INTEGER : TOK_STRING);
		break;
	}
}

void ScriptParser::read_token() {
	//DUCT_DEBUGCF("token-type:%s line:%d, col:%d\n", get_token_name(m_token), m_token.get_line(), m_token.get_column());
	switch (m_token.get_type()) {
	case TOK_STRING_QUOTED:
		read_tok_string_quoted();
		next_char();
		break;
	case TOK_STRING:
		read_tok_string();
		break;
	case TOK_INTEGER:
		if (CHAR_PLUS==m_curchar || CHAR_DASH==m_curchar) {
			next_char(); // Move out of sign
		}
		read_tok_integer();
		break;
	case TOK_FLOATING:
		next_char(); // Move out of decimal
		read_tok_floating();
		break;
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
		next_char();
		break;
	case TOK_EOF:
		// Nothing to do
		break;

	default:
		DUCT_DEBUG_ASSERT(false, "Something has gone horribly wrong: unhandled token type!");
		DUCT_SP_THROWF__("Unhandled token type: '%s'", get_token_name(m_token));
		break;
	}
	// Special case: when number and floating-point tokens only contain signs or periods
	switch (m_token.get_type()) {
	case TOK_INTEGER:
		if (m_token.get_buffer().compare(s_set_sign)) {
			m_token.set_type(TOK_STRING);
		}
		break;
	case TOK_FLOATING:
		if (m_token.get_buffer().compare(s_set_sign) || m_token.get_buffer().compare(CHAR_DECIMALPOINT)) {
			m_token.set_type(TOK_STRING);
		}
		break;
	default:
		break;
	}
	handle_token();
}

void ScriptParser::handle_token() {
	switch (m_token.get_type()) {
	// Value or name
	case TOK_STRING:
	case TOK_STRING_QUOTED:
	case TOK_INTEGER:
	case TOK_FLOATING:
	case TOK_LITERAL_TRUE:
	case TOK_LITERAL_FALSE:
	case TOK_LITERAL_NULL:
		if (in_scope(VARTYPE_ARRAY) && !has_states_any(STATE_COMMA|STATE_OPEN_ARRAY)) {
			DUCT_SP_THROW__("Unexpected token after non-open-bracket and non-comma in array scope");
		} else if (has_states(STATE_EQUALS)) { // Make named value in node scope
			make_value();
		} else if (in_scope(VARTYPE_IDENTIFIER|VARTYPE_ARRAY)) { // Already in a non-node scope; make nameless value
			make_nameless_value();
		} else if (!m_varname.empty()) { // Have name and last functor was whitespace: make identifier with child
			make_collection(VARTYPE_IDENTIFIER);
			make_nameless_value(); // Make nameless value in identifier scope
		} else { // First token before functor; make name
			make_name();
		}
		break;

	// Scope/functors
	case TOK_EQUALS:
		if (!in_scope(VARTYPE_NODE)) {
			DUCT_SP_THROW__("Unexpected equality sign within non-node scope");
		} else if (m_varname.empty()) {
			DUCT_SP_THROW__("Expected name, got equality sign");
		} else if (has_states(STATE_EQUALS)) {
			DUCT_SP_THROW__("Expected value after equality sign, got equality sign");
		} else if (has_states(STATE_COMMA)) {
			DUCT_SP_THROW__("Expected value after comma, got comma");
		} else {
			assign_states(STATE_EQUALS);
		}
		break;
	case TOK_COMMA:
		if (!in_scope(VARTYPE_ARRAY)) {
			DUCT_SP_THROW__("Unexpected comma in non-array scope");
		} else if (has_states(STATE_EQUALS)) {
			DUCT_SP_THROW__("Expected value after equality sign, got comma");
		} else if (has_states(STATE_OPEN_ARRAY)) {
			DUCT_SP_THROW__("Expected value before comma");
		} else if (has_states(STATE_COMMA)) {
			DUCT_SP_THROW__("Expected value after comma, got comma");
		} else {
			assign_states(STATE_COMMA);
		}
		break;
	case TOK_OPEN_BRACE:
		if (in_scope(VARTYPE_ARRAY)) {
			DUCT_SP_THROW__("Unexpected open-brace in array scope");
		} else if (in_scope(VARTYPE_IDENTIFIER)) {
			DUCT_SP_THROW__("Cannot make node on same line as identifier with children");
		} else if (!m_varname.empty() && !has_states(STATE_EQUALS)) {
			DUCT_SP_THROW__("Unexpected open-brace after name; possibly missing equality sign or attempting to (illegally) add node to identifier");
		} else {
			make_collection(VARTYPE_NODE);
		}
		break;
	case TOK_CLOSE_BRACE:
		if (at_root()) {
			DUCT_SP_THROW__("Mismatched node brace");
		} else if (in_scope(VARTYPE_ARRAY)) {
			DUCT_SP_THROW__("Unexpected close-brace in array scope");
		} else if (has_states(STATE_EQUALS)) {
			DUCT_SP_THROW__("Expected value after equality sign, got close-brace");
		} else if (has_states(STATE_COMMA)) {
			DUCT_SP_THROW__("Expected value after comma, got close-brace");
		} else {
			if (!m_varname.empty()) { // Acts terminatively on a yet-realized identifier
				make_collection(VARTYPE_IDENTIFIER, false);
			} else if (in_scope(VARTYPE_IDENTIFIER)) {
				pop(); // Pop from identifier to parent (node) scope
			}
			pop(); // Pop from node to parent scope
		}
		break;
	case TOK_OPEN_BRACKET:
		if (in_scope(VARTYPE_ARRAY) && !has_states_any(STATE_COMMA|STATE_OPEN_ARRAY)) {
			DUCT_SP_THROW__("Unexpected token after non-open-bracket and non-comma in array scope");
		} else {
			if (!m_varname.empty() && !has_states(STATE_EQUALS)) { // Acts constructively on a yet-realized identifier
				make_collection(VARTYPE_IDENTIFIER);
			}
			make_collection(VARTYPE_ARRAY);
			assign_states(STATE_OPEN_ARRAY);
		}
		break;
	case TOK_CLOSE_BRACKET:
		if (at_root()) {
			DUCT_SP_THROW__("Mismatched array bracket");
		} else if (!in_scope(VARTYPE_ARRAY)) {
			DUCT_SP_THROWF__("Unexpected close-bracket in %s scope", detail::get_vartype_name(get_current_collection().get_type()));
		} else if (has_states(STATE_EQUALS)) {
			DUCT_SP_THROW__("Expected value after equality sign, got close-bracket");
		} else if (has_states(STATE_COMMA)) {
			DUCT_SP_THROW__("Expected value after comma, got close-bracket");
		} else {
			pop(); // Pop from array to parent scope
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
		DUCT_DEBUG_ASSERTF(false, "Unhandled token of type \'%s\'", get_token_name(m_token));
		break;
	}
}

void ScriptParser::read_tok_integer() {
	while (CHAR_EOF!=m_curchar) {
		if (CHAR_QUOTE==m_curchar) {
			DUCT_SP_THROW__("Unexpected quotation mark");
		} else if (CHAR_SLASH==m_curchar) {
			if (CHAR_SLASH==peek_char() || CHAR_ASTERISK==m_peekchar) {
				break;
			} else {
				m_token.set_type(TOK_STRING);
				read_tok_string();
				return;
			}
		} else if (s_set_terminator.contains(m_curchar)) { // All single terminators
			break;
		} else if (s_set_numeral.contains(m_curchar)) {
			m_token.get_buffer().push_back(m_curchar);
		} else if (CHAR_DECIMALPOINT==m_curchar) {
			m_token.get_buffer().push_back(m_curchar);
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

void ScriptParser::read_tok_floating() {
	while (CHAR_EOF!=m_curchar) {
		if (CHAR_QUOTE==m_curchar) {
			DUCT_SP_THROW__("Unexpected quotation mark");
		} else if (CHAR_SLASH==m_curchar) {
			if (CHAR_SLASH==peek_char() || CHAR_ASTERISK==m_peekchar) { // Comment or comment block terminates
				break;
			} else {
				m_token.set_type(TOK_STRING);
				read_tok_string();
				return;
			}
		} else if (s_set_terminator.contains(m_curchar)) { // All single terminators
			break;
		} else if (s_set_numeral.contains(m_curchar)) {
			m_token.get_buffer().push_back(m_curchar);
		} else {
			m_token.set_type(TOK_STRING);
			read_tok_string();
			return;
		}
		next_char();
	}
}

void ScriptParser::read_tok_literal(char32 const match_str[], unsigned int const length) {
	unsigned int index=0;
	while (CHAR_EOF!=m_curchar) {
		if (CHAR_QUOTE==m_curchar) {
			DUCT_SP_THROW__("Unexpected quotation mark");
		} else if (s_set_terminator.contains(m_curchar) // All single terminators
				||(CHAR_SLASH==m_curchar && (CHAR_SLASH==peek_char() || CHAR_ASTERISK==m_peekchar))) { // Comment or comment block terminate
			if (length>index) { // Haven't yet checked all characters in match string; must be a normal string
				m_token.set_type(TOK_STRING);
			}
			break;
		} else if (length<=index || match_str[index]!=m_curchar) { // Already overrun length or char does not match current
			m_token.set_type(TOK_STRING);
			read_tok_string();
			return;
		} else { // Haven't overrun and char matches current
			m_token.get_buffer().push_back(m_curchar);
			++index;
		}
		next_char();
	}
}

void ScriptParser::read_tok_string() {
	while (CHAR_EOF!=m_curchar) {
		if (CHAR_QUOTE==m_curchar) {
			DUCT_SP_THROW__("Unexpected quotation mark");
		} else if (CHAR_BACKSLASH==m_curchar) {
			char32 const cp=StringUtils::get_escape_char(next_char(), s_esc_pair);
			if (CHAR_EOF==m_curchar || CHAR_NEWLINE==m_curchar) {
				DUCT_SP_THROW__("Expected escape sequence, got EOL/EOF");
			} else if (CHAR_NULL==cp) {
				DUCT_SP_THROWF__("Unexpected escape sequence: '%c' (0x%X)", m_curchar, m_curchar);
			} else {
				m_token.get_buffer().push_back(cp);
			}
		} else if (s_set_terminator.contains(m_curchar) // All single terminators
				||(CHAR_SLASH==m_curchar && (CHAR_SLASH==peek_char() || CHAR_ASTERISK==m_peekchar))) { // Comment or comment block terminate
			break;
		} else {
			m_token.get_buffer().push_back(m_curchar);
		}
		next_char();
	}
}

void ScriptParser::read_tok_string_quoted() {
	bool eol_reached=false;
	next_char(); // Skip the first character (it will be the initial quote)
	while (CHAR_QUOTE!=m_curchar) {
		if (CHAR_EOF==m_curchar) {
			DUCT_SP_THROW__("Encountered EOF whilst reading quoted string");
		} else if (CHAR_BACKSLASH==m_curchar) {
			char32 const cp=StringUtils::get_escape_char(next_char(), s_esc_pair);
			if (CHAR_EOF==m_curchar || CHAR_NEWLINE==m_curchar) {
				DUCT_SP_THROW__("Expected escape sequence, got EOL/EOF");
			} else if (CHAR_NULL==cp) {
				DUCT_SP_THROWF__("Unexpected escape sequence: '%c' (0x%X)", m_curchar, m_curchar);
			} else {
				m_token.get_buffer().push_back(cp);
			}
		} else {
			if (!eol_reached) {
				m_token.get_buffer().push_back(m_curchar);
			}
			if (CHAR_NEWLINE==m_curchar) {
				eol_reached=true;
			} else if (eol_reached && !s_set_whitespace.contains(m_curchar)) {
				eol_reached=false;
				m_token.get_buffer().push_back(m_curchar);
			}
		}
		next_char();
	}
}

void ScriptParser::read_tok_comment_block() {
	next_char(); // Skip the first character (it will be an asterisk)
	if (CHAR_EOF!=m_curchar) {
		while (skip_to(CHAR_ASTERISK)) {
			if (CHAR_SLASH==next_char()) {
				next_char(); // Get the next character, otherwise the discern_token() call will try to handle the slash
				return;
			}
		}
	}
	DUCT_SP_THROW__("Unexpected EOF whilst reading comment block");
}

bool ScriptParser::at_root() const {
	DUCT_DEBUG_ASSERT(0!=m_stack.size(), "Something has gone horribly wrong: the stack shouldn't be empty");
	return 1>=m_stack.size();
}

bool ScriptParser::in_scope(unsigned int const types) {
	return (get_current_collection().get_type()&types);
}

Variable& ScriptParser::get_current_collection() {
	DUCT_DEBUG_ASSERT(0<m_stack.size(), "Something has gone horribly wrong: stack is empty!");
	DUCT_DEBUG_ASSERT(nullptr!=m_stack.back(), "Something has gone horribly wrong: stack.back() is null!");
	return *(m_stack.back());
}

void ScriptParser::push(Variable& collection) {
	DUCT_DEBUG_ASSERT(collection.is_class(VARCLASS_COLLECTION), "Something has gone horribly wrong: cannot push a non-collection");
	DUCT_DEBUG_ASSERT(0==m_stack.size() || (!collection.is_type(VARTYPE_NODE) || !in_scope(VARTYPE_IDENTIFIER)), "Something has gone horribly wrong: cannot push a node whilst in identifier scope");
	//DUCT_DEBUGF("at %lu, pushing %s", static_cast<unsigned long>(m_stack.size()), detail::get_vartype_name(collection.get_type()));
	if (0<m_stack.size()) {
		DUCT_DEBUG_ASSERT((&collection)!=m_stack.back(), "Something has gone horribly wrong: pushing same collection");
	}
	m_stack.push_back(&collection);
}

void ScriptParser::pop() {
	DUCT_DEBUG_ASSERT(1<m_stack.size(), "Something has gone horribly wrong: the root node cannot be popped");
	m_stack.pop_back();
}

void ScriptParser::throwex(ScriptParserException&& e) {
	reset();
	throw e;
}

void ScriptParser::make_name() {
	DUCT_DEBUG_ASSERT(m_varname.empty(), "Something has gone horribly wrong: already have name");
	DUCT_DEBUG_ASSERT(!has_states(STATE_EQUALS), "Something has gone horribly wrong: should not have STATE_EQUALS here");
	m_varname.assign(m_token.get_buffer().cache());
	remove_states(STATE_COMMA);
}

void ScriptParser::make_collection(VariableType const type, bool push_collection) {
	DUCT_DEBUG_ASSERT(in_scope(VARTYPE_NODE) || VARTYPE_ARRAY==type, "Something has gone horribly wrong: cannot make a node or an identifier whilst in an identifier or array scope");
	if (m_varname.empty()) { // Unnamed collection; must not be an identifier
		DUCT_DEBUG_ASSERT(VARTYPE_IDENTIFIER!=type, "Something has gone horribly wrong: cannot make a nameless identifier");
		get_current_collection().emplace_back(type);
		remove_states(STATE_COMMA|STATE_OPEN_ARRAY);
	} else { // Named collection
		DUCT_DEBUG_ASSERT(VARTYPE_IDENTIFIER!=type || !has_states(STATE_EQUALS), "Something has gone horribly wrong: cannot have equality sign when making an identifier");
		get_current_collection().emplace_back(m_varname, type);
		remove_states(STATE_EQUALS|STATE_COMMA|STATE_OPEN_ARRAY);
		m_varname.clear();
	}
	if (push_collection) {
		push(get_current_collection().get_children().back());
	}
}

void ScriptParser::make_value() {
	DUCT_DEBUG_ASSERT(in_scope(VARTYPE_NODE), "Something has gone horribly wrong: attempted to make a named value whilst in a non-node scope");
	DUCT_DEBUG_ASSERT(has_states(STATE_EQUALS), "Something has gone horribly wrong: must have STATE_EQUALS to make a node value");
	DUCT_DEBUG_ASSERT(!has_states(STATE_COMMA), "Something has gone horribly wrong: should not have STATE_COMMA here");
	switch (m_token.get_type()) {
	case TOK_STRING:
	case TOK_STRING_QUOTED:
		get_current_collection().emplace_back(m_varname, m_token.get_buffer().to_string<detail::var_config::string_type>());
		break;
	case TOK_INTEGER: get_current_collection().emplace_back(m_varname, m_token.get_buffer().to_arithmetic<detail::var_config::int_type>()); break;
	case TOK_FLOATING: get_current_collection().emplace_back(m_varname, m_token.get_buffer().to_arithmetic<detail::var_config::float_type>()); break;
	case TOK_LITERAL_TRUE: get_current_collection().emplace_back(m_varname, true); break;
	case TOK_LITERAL_FALSE: get_current_collection().emplace_back(m_varname, false); break;
	case TOK_LITERAL_NULL: get_current_collection().emplace_back(m_varname, VARTYPE_NULL); break;
	}
	remove_states(STATE_EQUALS);
	m_varname.clear();
}

void ScriptParser::make_nameless_value(int override_type) {
	DUCT_DEBUG_ASSERT(m_varname.empty(), "Something has gone horribly wrong: name should be empty here");
	DUCT_DEBUG_ASSERT(!has_states(STATE_EQUALS), "Something has gone horribly wrong: should not have STATE_EQUALS here");
	DUCT_DEBUG_ASSERT(!in_scope(VARTYPE_NODE), "Something has gone horribly wrong: can only make a nameless value when in non-node scope");
	if (NULL_TOKEN==override_type) {
		override_type=m_token.get_type();
	}
	switch (override_type) {
	case TOK_STRING:
	case TOK_STRING_QUOTED:
		get_current_collection().emplace_back(m_token.get_buffer().to_string<detail::var_config::string_type>()); break;
		break;
	case TOK_INTEGER: get_current_collection().emplace_back(m_token.get_buffer().to_arithmetic<detail::var_config::int_type>()); break;
	case TOK_FLOATING: get_current_collection().emplace_back(m_token.get_buffer().to_arithmetic<detail::var_config::float_type>()); break;
	case TOK_LITERAL_TRUE: get_current_collection().emplace_back(true); break;
	case TOK_LITERAL_FALSE: get_current_collection().emplace_back(false); break;
	case TOK_LITERAL_NULL: get_current_collection().emplace_back(VARTYPE_NULL); break;
	}
	remove_states(STATE_COMMA|STATE_OPEN_ARRAY);
}

#undef DUCT_SP_THROWF_NO_INFO__
#undef DUCT_SP_THROW_NO_INFO__
#undef DUCT_SP_THROWF__
#undef DUCT_SP_THROW__
