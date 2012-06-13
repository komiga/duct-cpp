
#include <duct/char.hpp>
#include <duct/string.hpp>
#include <duct/detail/string_traits.hpp>
#include <duct/IO.hpp>

#include <iostream>
#include <cstdio>

static duct::char8_strict const utf8_data[]={
	0xE3,0x81,0x82,	// U+3042
	0xE3,0x80,0x82,	// U+3002
	0xE2,0x80,0xA6,	// U+2026
	0xC3,0xBF,		// U+00FF
	0xC2,0xB9,		// U+00B9
	0xC2,0xB0,		// U+00B0
	'z','x','c','v',// "zxcv"
	0xC2,0xAC,		// U+00AC
	0xC2,0xA5,		// U+00A5
	0xC2,0xAC		// U+00AC
};

static duct::char16_strict const utf16_data[]={
	0x3042,
	0x3002,
	0x2026,
	0x00FF,
	0x00B9,
	0x00B0,
	'z','x','c','v',
	0x00AC,
	0x00A5,
	0x00AC
};

static duct::char32_strict const utf32_data[]={
	0x3042,
	0x3002,
	0x2026,
	0x00FF,
	0x00B9,
	0x00B0,
	'z','x','c','v',
	0x00AC,
	0x00A5,
	0x00AC
};

template<class fromU, class stringT>
void read_s(stringT& str, void const* data, std::size_t const size) {
	duct::IO::StreamContext ctx(fromU::id, duct::Endian::SYSTEM);
	duct::IO::mem_istreambuf sb(data, size*fromU::char_size);
	std::istream stream(&sb);
	assert(stream.good());
	ctx.read_string(stream, str, static_cast<std::streamsize>(size), duct::CHAR_NULL);
	assert(stream.good() || stream.eof());
	stream.rdbuf(nullptr);
	printf("String [size: %lu bsize: %lu len: %lu]: |", size, size*fromU::char_size, str.size());
	std::cout<<str<<"|\n";
	str.clear();
}

template<class toU, class stringT>
void write_s(stringT& str, typename toU::strict_char_type const* data, std::size_t const size) {
	typename toU::strict_char_type out_buffer[512];
	duct::IO::StreamContext ctx(toU::id, duct::Endian::SYSTEM);
	duct::IO::mem_ostreambuf sb(out_buffer, sizeof(out_buffer));
	std::ostream stream(&sb);
	assert(stream.good());
	std::size_t written_size=ctx.write_string(stream, str, duct::CHAR_NULL);
	printf("size: %lu  written_size: %lu\n", size, written_size);
	assert(stream.good() || stream.eof());
	assert(written_size==size);
	stream.flush();
	stream.rdbuf(nullptr);
	bool kosher=true;
	typename toU::strict_char_type const
		*di=data,
		*wi=out_buffer;
	for (; data+size!=di && out_buffer+written_size!=wi; ++di, ++wi) {
		if (*di!=*wi) {
			printf("di: %lu, %u 0x%X  wi: %lu, %u 0x%X\n", di-data, *di, *di, wi-out_buffer, *wi, *wi);
			kosher=false;
		}
	}
	if (kosher) {
		puts("She's kosher, cap'n.");
	} else {
		puts("Something has gone horribly wrong!");
	}
}

int main(int argc, char* argv[]) {
	duct::u8string str;
	puts("Reading");
	read_s<duct::UTF8Utils>(str, utf8_data, sizeof(utf8_data)/sizeof(*utf8_data));
	read_s<duct::UTF16Utils>(str, utf16_data, sizeof(utf16_data)/sizeof(*utf16_data));
	read_s<duct::UTF32Utils>(str, utf32_data, sizeof(utf32_data)/sizeof(*utf32_data));

	puts("\nWriting");
	str.assign(reinterpret_cast<char const*>(utf8_data), sizeof(utf8_data));
	write_s<duct::UTF8Utils>(str, utf8_data, sizeof(utf8_data)/sizeof(*utf8_data));
	write_s<duct::UTF16Utils>(str, utf16_data, sizeof(utf16_data)/sizeof(*utf16_data));
	write_s<duct::UTF32Utils>(str, utf32_data, sizeof(utf32_data)/sizeof(*utf32_data));
	std::cout<<std::endl;
	return 0;
}
