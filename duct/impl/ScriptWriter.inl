/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
*/

namespace {
static CharacterSet const
	s_set_number_inner{"0-9."},
	s_set_number_front{"0-9.\\-+"},
	s_set_req_quotation{"\n\t ,=[]{}\"\'"};
static char const
	*const s_esc_quotes_alone[]{"\"\'", "\"\'"},
	*const s_esc_whitespace[]{"\n\r\t\"\'", "nrt\"\'"},
	*const s_esc_control[]{",=[]{}\"\'", ",=[]{}\"\'"},
	*const s_esc_all[]{"\n\r\t,=[]{}\"\'", "nrt,=[]{}\"\'"};
} // anonymous namespace

// class ScriptWriter implementation

bool
ScriptWriter::write(
	std::ostream& dest,
	Var const& source,
	bool const treat_as_root,
	unsigned const tab_level
) const {
	if (dest.good()) {
		if (VarType::node != source.get_type() || !treat_as_root) {
			m_stream_ctx.write_char(dest, CHAR_TAB, tab_level);
		}
		switch (source.get_type()) {
		case VarType::null:
		case VarType::string:
		case VarType::integer:
		case VarType::floatp:
		case VarType::boolean:
			return write_value(dest, source, true);
		case VarType::array:
			return write_array(dest, source, true);
		case VarType::node:
			return write_node(dest, source, treat_as_root, tab_level);
		case VarType::identifier:
			return write_identifier(dest, source);
		}
	}
	return false;
}

template<
	class StringT,
	class StringU
>
bool
ScriptWriter::write_string(
	std::ostream& dest,
	StringT const& str,
	bool const is_name
) const {
	char32 first_cp = CHAR_NULL;
	auto dec_iter = str.cbegin();
	if (!str.empty()) {
		dec_iter = StringU::decode(dec_iter, str.cend(), first_cp, CHAR_NULL);
	}
	bool const add_quotation =
		// Whether to always quote based on token type
		0 != (has_flags(is_name ? Flags::name_quote : Flags::value_string_quote))
		// Empty string must be quoted
		|| str.empty()
		// Whitespace and control characters require quotation
		||(str.cend() != s_set_req_quotation.find(str, str.cbegin()))
		// If parseable as a number token, must be quoted
		||(
			s_set_number_inner.sequence_matches<StringU>(
				dec_iter, str.cend()
			)
			&& (1u >= StringUtils::unit_occurrences(CHAR_DECIMALPOINT, str)
				&& s_set_number_front.contains(first_cp)
			)
		)
	;
	StringUtils::EscapeablePair esc_pair;
	auto const&
	str_esc_pair =
		(add_quotation
			? (m_flags.test(Flags::escape_whitespace)
				// Whitespace is always escaped
				? s_esc_whitespace
				// Don't have to escape control characters when quoted
				: s_esc_quotes_alone)
			// Everything must be escaped
			: (s_esc_all)
		);
	esc_pair.first  = str_esc_pair[0u];
	esc_pair.second = str_esc_pair[1u];

	StringT normalized;
	// A little overhead for quotations and escaped characters
	normalized.reserve(str.size() + (add_quotation ? 2u : 0u) + 20u);
	if (add_quotation) {
		normalized.append(1, CHAR_QUOTE);
	}
	StringUtils::escape_string(normalized, str, esc_pair, false, false);
	if (add_quotation) {
		normalized.append(1, CHAR_QUOTE);
	}
	m_stream_ctx.write_string(dest, normalized);
	return dest.good();
}

bool
ScriptWriter::write_value(
	std::ostream& dest,
	Var const& var,
	bool const with_name
) const {
	if (with_name) {
		write_string(dest, var.get_name(), true);
		m_stream_ctx.write_char(dest, CHAR_EQUALSIGN);
	}
	switch (var.get_type()) {
	case VarType::string:
		write_string(dest, var.get_string_ref(), false);
		break;

	case VarType::null:
	case VarType::integer:
	case VarType::floatp:
	case VarType::boolean: {
		// All non-VarType::string values are representable in single
		// UTF-8 code units. Also, the target encoding is most likely
		// going to be UTF-8, so this is optimal.
		auto const vstr = var.get_as_str<u8string>();
		m_stream_ctx.write_string(dest, vstr);
		break;
	}
	default: break;
	}
	return dest.good();
}

bool
ScriptWriter::write_array(
	std::ostream& dest,
	Var const& var,
	bool const with_name
) const {
	if (with_name && !var.get_name().empty()) {
		write_string(dest, var.get_name(), true);
		m_stream_ctx.write_char(dest, CHAR_EQUALSIGN);
	}
	m_stream_ctx.write_char(dest, CHAR_OPENBRACKET);
	for (auto it = var.cbegin(); var.cend() != it; ++it) {
		switch (it->get_type()) {
		case VarType::identifier:
		case VarType::node:
			// TODO: Throw exception
			return false;

		case VarType::array:
			if (!write_array(dest, *it, false)) {
				return false;
			}
			break;

		default:
			if (!write_value(dest, *it, false)) {
				return false;
			}
			break;
		}
		if (var.cend() != it + 1) {
			m_stream_ctx.write_char(dest, CHAR_COMMA);
			m_stream_ctx.write_char(dest, ' ');
		}
	}
	m_stream_ctx.write_char(dest, CHAR_CLOSEBRACKET);
	return dest.good();
}

bool
ScriptWriter::write_node(
	std::ostream& dest,
	Var const& var,
	bool const treat_as_root,
	unsigned tab_level
) const {
	if (!treat_as_root) {
		//m_stream_ctx.write_char(dest, CHAR_TAB, tab_level);
		if (!var.get_name().empty()) {
			write_string(dest, var.get_name(), true);
			m_stream_ctx.write_char(dest, CHAR_EQUALSIGN);
		}
		m_stream_ctx.write_char(dest, CHAR_OPENBRACE);
		if (!var.empty()) {
			m_stream_ctx.write_char(dest, CHAR_NEWLINE);
		}
		++tab_level;
	}
	for (auto it = var.cbegin(); var.cend() != it;  ++ it) {
		if (!write(dest, *it, false, tab_level)) {
			return false;
		}
		if (!treat_as_root || var.cend() != it + 1) {
			m_stream_ctx.write_char(dest, CHAR_NEWLINE);
		}
	}
	if (!treat_as_root) {
		--tab_level;
		if (!var.empty()) {
			m_stream_ctx.write_char(dest, CHAR_TAB, tab_level);
		}
		m_stream_ctx.write_char(dest, CHAR_CLOSEBRACE);
	}
	return dest.good();
}

bool
ScriptWriter::write_identifier(
	std::ostream& dest,
	Var const& var
) const {
	write_string(dest, var.get_name(), true);
	if (0u < var.size()) {
		m_stream_ctx.write_char(dest, ' ');
	}
	for (auto it = var.cbegin(); var.cend() != it; ++it) {
		switch (it->get_type()) {
		case VarType::identifier:
		case VarType::node:
			// TODO: Throw exception
			return false;

		case VarType::array:
			if (!write_array(dest, *it, !it->get_name().empty())) {
				return false;
			}
			break;

		default:
			if (!write_value(dest, *it, !it->get_name().empty())) {
				return false;
			}
			break;
		}
		if (var.cend() != it + 1) {
			m_stream_ctx.write_char(dest, ' ');
		}
	}
	return dest.good();
}
