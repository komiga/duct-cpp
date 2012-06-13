
#include <duct/char.hpp>
#include <duct/EncodingUtils.hpp>

typedef duct::char8 char8;
typedef duct::char16 char16;
typedef duct::char32 char32;

/**
	Dynamic interface to EncodingUtils.
*/
class DynamicEncoder /*final*/ {
private:
	DUCT_DISALLOW_COPY_AND_ASSIGN(DynamicEncoder);

public:
	DynamicEncoder(duct::Encoding const encoding)
		: m_encoding(encoding)
	{}

	inline void set_encoding(duct::Encoding const encoding) { m_encoding=encoding; }
	inline duct::Encoding get_encoding() const { return m_encoding; }

	template<typename InputIterator>
	inline InputIterator decode(InputIterator pos, InputIterator const end, char32& output, char32 const replacement=duct::CHAR_NULL) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:		return duct::UTF8Utils::decode(pos, end, output, replacement);
		case duct::Encoding::UTF16:		return duct::UTF16Utils::decode(pos, end, output, replacement);
		case duct::Encoding::UTF32:		return duct::UTF32Utils::decode(pos, end, output, replacement);
	}}

	template<typename OutputIterator>
	inline OutputIterator encode(char32 input, OutputIterator output, char32 const replacement=duct::CHAR_NULL) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:		return duct::UTF8Utils::encode(input, output, replacement);
		case duct::Encoding::UTF16:		return duct::UTF16Utils::encode(input, output, replacement);
		case duct::Encoding::UTF32:		return duct::UTF32Utils::encode(input, output, replacement);
	}}

	template<typename RandomAccessIterator>
	inline RandomAccessIterator next(RandomAccessIterator const from, RandomAccessIterator const end) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:		return duct::UTF8Utils::next(from, end);
		case duct::Encoding::UTF16:		return duct::UTF16Utils::next(from, end);
		case duct::Encoding::UTF32:		return duct::UTF32Utils::next(from, end);
	}}

	template<typename RandomAccessIterator>
	inline RandomAccessIterator prev(RandomAccessIterator from, RandomAccessIterator const begin) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:		return duct::UTF8Utils::prev(from, begin);
		case duct::Encoding::UTF16:		return duct::UTF16Utils::prev(from, begin);
		case duct::Encoding::UTF32:		return duct::UTF32Utils::prev(from, begin);
	}}

	inline unsigned int required_first(char32 const first) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:		return duct::UTF8Utils::required_first(static_cast<char8>(first));
		case duct::Encoding::UTF16:		return duct::UTF16Utils::required_first(static_cast<char16>(first));
		case duct::Encoding::UTF32:		return duct::UTF32Utils::required_first(first);
	}}

	inline unsigned int required_first_whole(char32 const first) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:		return duct::UTF8Utils::required_first_whole(static_cast<char8>(first));
		case duct::Encoding::UTF16:		return duct::UTF16Utils::required_first_whole(static_cast<char16>(first));
		case duct::Encoding::UTF32:		return duct::UTF32Utils::required_first_whole(first);
	}}

	inline unsigned int required(char32 const c) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:		return duct::UTF8Utils::required(static_cast<char8>(c));
		case duct::Encoding::UTF16:		return duct::UTF16Utils::required(static_cast<char16>(c));
		case duct::Encoding::UTF32:		return duct::UTF32Utils::required(c);
	}}

	template<typename RandomAccessIterator>
	inline std::size_t count(RandomAccessIterator begin, RandomAccessIterator const end, bool const count_incomplete=false) {
		switch (m_encoding) {
		case duct::Encoding::UTF8:		return duct::UTF8Utils::count(begin, end, count_incomplete);
		case duct::Encoding::UTF16:		return duct::UTF16Utils::count(begin, end, count_incomplete);
		case duct::Encoding::UTF32:		return duct::UTF32Utils::count(begin, end, count_incomplete);
	}}

	inline static DynamicEncoder const& utf8_static() {
		static DynamicEncoder const s_de_utf8(duct::Encoding::UTF8);
		return s_de_utf8;
	}
	inline static DynamicEncoder const& utf16_static() {
		static DynamicEncoder const s_de_utf16(duct::Encoding::UTF16);
		return s_de_utf16;
	}
	inline static DynamicEncoder const& utf32_static() {
		static DynamicEncoder const s_de_utf32(duct::Encoding::UTF32);
		return s_de_utf32;
	}

private:
	duct::Encoding m_encoding;
};

int main(int argc, char* argv[]) {
	DynamicEncoder u8(duct::Encoding::UTF8);
	DynamicEncoder u16(duct::Encoding::UTF16);
	DynamicEncoder u32(duct::Encoding::UTF32);
	return 0;
}
