
#include <duct/char.hpp>
#include <duct/EncodingUtils.hpp>

using duct::char8;
using duct::char16;
using duct::char32;

/**
	Dynamic interface to EncodingUtils.
*/
class DynamicEncoder /*final*/ : duct::traits::restrict_copy {
private:
	duct::Encoding m_encoding;

public:
	DynamicEncoder(duct::Encoding const encoding)
		: m_encoding{encoding}
	{}

	void set_encoding(duct::Encoding const encoding)
		{ m_encoding=encoding; }
	duct::Encoding get_encoding() const
		{ return m_encoding; }

	template<typename RandomAccessIt>
	RandomAccessIt decode(
		RandomAccessIt pos, RandomAccessIt const end,
		char32& output,
		char32 const replacement=duct::CHAR_NULL
	) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:
			return duct::UTF8Utils::decode(pos, end, output, replacement);
		case duct::Encoding::UTF16:
			return duct::UTF16Utils::decode(pos, end, output, replacement);
		case duct::Encoding::UTF32:
			return duct::UTF32Utils::decode(pos, end, output, replacement);
	}}

	template<typename OutputIterator>
	OutputIterator encode(
		char32 input,
		OutputIterator output,
		char32 const replacement=duct::CHAR_NULL
	) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:
		return duct::UTF8Utils::encode(input, output, replacement);
		case duct::Encoding::UTF16:
		return duct::UTF16Utils::encode(input, output, replacement);
		case duct::Encoding::UTF32:
		return duct::UTF32Utils::encode(input, output, replacement);
	}}

	template<typename RandomAccessIt>
	RandomAccessIt next(RandomAccessIt const from, RandomAccessIt const end) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:
			return duct::UTF8Utils::next(from, end);
		case duct::Encoding::UTF16:
			return duct::UTF16Utils::next(from, end);
		case duct::Encoding::UTF32:
			return duct::UTF32Utils::next(from, end);
	}}

	template<typename RandomAccessIt>
	RandomAccessIt prev(RandomAccessIt from, RandomAccessIt const begin) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:
			return duct::UTF8Utils::prev(from, begin);
		case duct::Encoding::UTF16:
			return duct::UTF16Utils::prev(from, begin);
		case duct::Encoding::UTF32:
			return duct::UTF32Utils::prev(from, begin);
	}}

	unsigned required_first(char32 const first) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:
			return duct::UTF8Utils::required_first(
				static_cast<char8>(first));
		case duct::Encoding::UTF16:
			return duct::UTF16Utils::required_first(
				static_cast<char16>(first));
		case duct::Encoding::UTF32:
			return duct::UTF32Utils::required_first(first);
	}}

	unsigned required_first_whole(char32 const first) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:
			return duct::UTF8Utils::required_first_whole(
				static_cast<char8>(first));
		case duct::Encoding::UTF16:
			return duct::UTF16Utils::required_first_whole(
				static_cast<char16>(first));
		case duct::Encoding::UTF32:
			return duct::UTF32Utils::required_first_whole(first);
	}}

	unsigned required(char32 const c) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:
			return duct::UTF8Utils::required(static_cast<char8>(c));
		case duct::Encoding::UTF16:
			return duct::UTF16Utils::required(static_cast<char16>(c));
		case duct::Encoding::UTF32:
			return duct::UTF32Utils::required(c);
	}}

	template<typename RandomAccessIt>
	std::size_t count(
		RandomAccessIt begin,
		RandomAccessIt const end, bool const count_incomplete=false) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:
			return duct::UTF8Utils::count(begin, end, count_incomplete);
		case duct::Encoding::UTF16:
			return duct::UTF16Utils::count(begin, end, count_incomplete);
		case duct::Encoding::UTF32:
			return duct::UTF32Utils::count(begin, end, count_incomplete);
	}}

	static DynamicEncoder const& utf8_static() {
		static DynamicEncoder const s_de_utf8{duct::Encoding::UTF8};
		return s_de_utf8;
	}
	static DynamicEncoder const& utf16_static() {
		static DynamicEncoder const s_de_utf16{duct::Encoding::UTF16};
		return s_de_utf16;
	}
	static DynamicEncoder const& utf32_static() {
		static DynamicEncoder const s_de_utf32{duct::Encoding::UTF32};
		return s_de_utf32;
	}
};

signed main() {
	DynamicEncoder u8{duct::Encoding::UTF8};
	DynamicEncoder u16{duct::Encoding::UTF16};
	DynamicEncoder u32{duct::Encoding::UTF32};
	return 0;
}
