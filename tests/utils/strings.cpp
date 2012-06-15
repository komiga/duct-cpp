
#include <duct/debug.hpp>
#include <duct/string.hpp>
#include <duct/EncodingUtils.hpp>

#include <cstdlib>
#include <cassert>
#include <cstdio>

#define init_decode(data_, offset_) \
	begin=data_; \
	end=data_+offset_; \
	next=nullptr; \
	cp=duct::CHAR_SENTINEL-1
// -

#define print_decode(data_, enc_) \
	printf(" next: %lu  end: %lu  cp: %d 0x%X  required: %u\n", next-data_, end-data_, cp, cp, duct:: enc_ ::required(cp))
	//printf(" non-surr: %d  non-max: %d ; 3.2-nc: %d  FFFE: %d\n", cp<0xD800 || cp>0xDFFF, cp<=0x10FFFF, cp>=0xFDD0 && cp<=0xFDEF, (cp&0xFFFE)==0xFFFE && cp<=0x10FFFF)
// -

#define run_decode(enc_, data_, offset_) \
	init_decode(data_, offset_); \
	next=duct:: enc_ ::decode(begin, end, cp, duct::CHAR_SENTINEL); \
	print_decode(data_, enc_)
// -

#define init_encode(cp_) \
	next=nullptr; \
	output=buffer; \
	cp=cp_
// -

#define print_encode(enc_) \
	printf(" next: %lu  cp: %d 0x%X  required: %u  enc: [", next-buffer, cp, cp, duct:: enc_ ::required(cp)); \
	for (auto it_=buffer; next>it_; ++it_) { \
		printf("0x%X%s", *it_, next==it_+1 ? "" : ", "); \
	} puts("]")
// -

#define run_encode(enc_, cp_) \
	init_encode(cp_); \
	next=duct:: enc_ ::encode(cp, output, 0); \
	print_encode(enc_)
// -

#define do_series(x_) (0==series || x_==series)

static duct::char8_strict const utf8_diay[]			={0xC3,0xBF}; // U+FF; ÿ
static duct::char8_strict const utf8_hirigana[]		={0xE3,0x81,0x82}; // U+3042; あ
static duct::char8_strict const utf8_olditalic[]	={0xF0,0x90,0x8C,0x82}; // U+10302; 𐌂
static duct::char8_strict const utf8_highest[]		={0xF4,0x8F,0xBF,0xBD}; // U+10FFFD
static duct::char8_strict const utf8_invalid1[]		={0xEF,0xBF,0xBF}; // U+FFFF
static duct::char8_strict const utf8_invalid2[]		={0xF4,0x8F,0xBF,0xBE}; // U+10FFFE

static duct::char16_strict const utf16_diay[]		={0x00FF};
static duct::char16_strict const utf16_hirigana[]	={0x3042};
static duct::char16_strict const utf16_olditalic[]	={0xD800,0xDF02};
static duct::char16_strict const utf16_highest[]	={0xDBFF,0xDFFD};
static duct::char16_strict const utf16_invalid1[]	={0xFFFF};
static duct::char16_strict const utf16_invalid2[]	={0xDBFF,0xDFFE};

static duct::char32_strict const utf32_diay[]		={0x0000FF};
static duct::char32_strict const utf32_hirigana[]	={0x003042};
static duct::char32_strict const utf32_olditalic[]	={0x010302};
static duct::char32_strict const utf32_highest[]	={0x10FFFD};
static duct::char32_strict const utf32_invalid1[]	={0x00FFFF};
static duct::char32_strict const utf32_invalid2[]	={0x10FFFE};

int main(int argc, char* argv[]) {
	DUCT_ASSERT(argc>1, "requires series argument (pass 0 for all)");
	unsigned int
		series=atoi(argv[1]),
		offset=argc>2 ? atoi(argv[2]) : 4;
	duct::char32 cp;
	if (do_series(1)) {
		puts("# UTF-8");
		duct::char8_strict buffer[8];
		duct::char8_strict* output;
		duct::char8_strict const *begin, *end, *next;
		run_decode(UTF8Utils, utf8_diay, offset);
		run_decode(UTF8Utils, utf8_hirigana, offset);
		run_decode(UTF8Utils, utf8_olditalic, offset);
		run_decode(UTF8Utils, utf8_highest, offset);
		run_decode(UTF8Utils, utf8_invalid1, offset);
		run_decode(UTF8Utils, utf8_invalid2, offset);
		// encode
		run_encode(UTF8Utils, utf32_diay[0]);
		run_encode(UTF8Utils, utf32_hirigana[0]);
		run_encode(UTF8Utils, utf32_olditalic[0]);
		run_encode(UTF8Utils, utf32_highest[0]);
		run_encode(UTF8Utils, utf32_invalid1[0]);
		run_encode(UTF8Utils, utf32_invalid2[0]);
	}
	if (do_series(2)) {
		puts("# UTF-16");
		duct::char16_strict buffer[2];
		duct::char16_strict* output;
		duct::char16_strict const *begin, *end, *next;
		run_decode(UTF16Utils, utf16_diay, offset);
		run_decode(UTF16Utils, utf16_hirigana, offset);
		run_decode(UTF16Utils, utf16_olditalic, offset);
		run_decode(UTF16Utils, utf16_highest, offset);
		run_decode(UTF16Utils, utf16_invalid1, offset);
		run_decode(UTF16Utils, utf16_invalid2, offset);
		// encode
		run_encode(UTF16Utils, utf32_diay[0]);
		run_encode(UTF16Utils, utf32_hirigana[0]);
		run_encode(UTF16Utils, utf32_olditalic[0]);
		run_encode(UTF16Utils, utf32_highest[0]);
		run_encode(UTF16Utils, utf32_invalid1[0]);
		run_encode(UTF16Utils, utf32_invalid2[0]);
	}
	if (do_series(3)) {
		puts("# UTF-32");
		duct::char32_strict buffer[1];
		duct::char32_strict* output;
		duct::char32_strict const *begin, *end, *next;
		run_decode(UTF32Utils, utf32_diay, offset);
		run_decode(UTF32Utils, utf32_hirigana, offset);
		run_decode(UTF32Utils, utf32_olditalic, offset);
		run_decode(UTF32Utils, utf32_highest, offset);
		run_decode(UTF32Utils, utf32_invalid1, offset);
		run_decode(UTF32Utils, utf32_invalid2, offset);
		// encode
		run_encode(UTF32Utils, utf32_diay[0]);
		run_encode(UTF32Utils, utf32_hirigana[0]);
		run_encode(UTF32Utils, utf32_olditalic[0]);
		run_encode(UTF32Utils, utf32_highest[0]);
		run_encode(UTF32Utils, utf32_invalid1[0]);
		run_encode(UTF32Utils, utf32_invalid2[0]);
	}
	return 0;
}
