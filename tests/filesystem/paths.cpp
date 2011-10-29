
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <duct/filesystem.hpp>

std::string std_path, std_result;
UnicodeString icu_path, icu_result;
bool std_success=false, icu_success=false;

char const* cs_orig="";
size_t cs_orig_size=0;

void clear() {
	std_result.clear();
	std_success=false;
	icu_result.remove();
	icu_success=false;
}

void assign(char const* value) {
	cs_orig=value;
	cs_orig_size=strlen(cs_orig);
	std_path.assign(value);
	icu_path=UnicodeString(value);
	clear();
	printf("assign: '%s'\n", cs_orig);
}

void assign(std::string const& value) {
	assign(value.c_str());
}

#define NOTE_NAME printf("%-26s: ", name)
#define SPACE_STR "                                                          "

void note_results(char const* name) {
	NOTE_NAME;
	printf("std: '");
	std::cout<<std_result;
	printf("'%.*s (%d) icu: '", std_result.size()>cs_orig_size ? 0 : int(cs_orig_size-std_result.size()), SPACE_STR, std_success);
	std::cout<<icu_result;
	printf("'%.*s (%d)", icu_result.length()>int(cs_orig_size) ? 0 : int(cs_orig_size-icu_result.length()), SPACE_STR, icu_success);
	printf("%s\n", std_success==icu_success ? "" : "  NOT EQUAL!");
	clear();
}

void note_success(char const* name) {
	NOTE_NAME;
	/*printf("std: %.*s (%d) icu: %.*s (%d) %s\n",
		int(cs_orig_size), SPACE_STR, std_success,
		int(cs_orig_size), SPACE_STR, icu_success,
		std_success==icu_success ? "" : "  NOT EQUAL!");*/
	printf("std: (%d) icu: (%d) %s\n", std_success, icu_success, std_success==icu_success ? "" : "  NOT EQUAL!");
	clear();
}

#define TEST_HAS_EXT(name_) \
	std_success=duct::FileSystem::pathHasExtension(std_path);\
	icu_success=duct::FileSystem::pathHasExtension(icu_path);\
	note_success(name_);
#define TEST_HAS_LEFT(name_, allow_leading) \
	std_success=duct::FileSystem::pathHasLeftPart(std_path, allow_leading);\
	icu_success=duct::FileSystem::pathHasLeftPart(icu_path, allow_leading);\
	note_success(name_);
#define TEST_HAS_RIGHT(name_, allow_leading) \
	std_success=duct::FileSystem::pathHasRightPart(std_path, allow_leading);\
	icu_success=duct::FileSystem::pathHasRightPart(icu_path, allow_leading);\
	note_success(name_);
#define TEST_HAS_NAME(name_) \
	std_success=duct::FileSystem::pathHasFilename(std_path);\
	icu_success=duct::FileSystem::pathHasFilename(icu_path);\
	note_success(name_);
#define TEST_HAS_DIR(name_) \
	std_success=duct::FileSystem::pathHasDirectory(std_path);\
	icu_success=duct::FileSystem::pathHasDirectory(icu_path);\
	note_success(name_);

#define TEST_HAS_PARTS(left_name, right_name, allow_leading) \
	TEST_HAS_LEFT(left_name, allow_leading);\
	TEST_HAS_RIGHT(right_name, allow_leading);

#define TEST_HAS(ext_name, name_name, dir_name) \
	TEST_HAS_EXT(ext_name);\
	TEST_HAS_NAME(name_name);\
	TEST_HAS_DIR(dir_name);

#define EXTRACT_EXT(name_, include_period) \
	std_success=duct::FileSystem::extractFileExtension(std_path, std_result, include_period);\
	icu_success=duct::FileSystem::extractFileExtension(icu_path, icu_result, include_period);\
	note_results(name_);
#define EXTRACT_LEFT(name_, allow_leading) \
	std_success=duct::FileSystem::extractFileLeftPart(std_path, std_result, allow_leading);\
	icu_success=duct::FileSystem::extractFileLeftPart(icu_path, icu_result, allow_leading);\
	note_results(name_);
#define EXTRACT_RIGHT(name_, include_period, allow_leading) \
	std_success=duct::FileSystem::extractFileRightPart(std_path, std_result, include_period, allow_leading);\
	icu_success=duct::FileSystem::extractFileRightPart(icu_path, icu_result, include_period, allow_leading);\
	note_results(name_);
#define EXTRACT_NAME(name_, with_ext) \
	std_success=duct::FileSystem::extractFilename(std_path, std_result, with_ext);\
	icu_success=duct::FileSystem::extractFilename(icu_path, icu_result, with_ext);\
	note_results(name_);
#define EXTRACT_DIR(name_, trailing_slash) \
	std_success=duct::FileSystem::extractFileDirectory(std_path, std_result, trailing_slash);\
	icu_success=duct::FileSystem::extractFileDirectory(icu_path, icu_result, trailing_slash);\
	note_results(name_);

#define EXTRACT_PARTS(left_name, right_name, allow_leading) \
	EXTRACT_LEFT(left_name, allow_leading);\
	EXTRACT_RIGHT(right_name, true, allow_leading);

int main(int argc, char** argv) {
	for (int i=1; i<argc; ++i) {
		printf("-----------------\n");
		std::string arg_path;
		duct::FileSystem::normalizePath(std::string(argv[i]), arg_path);
		assign(arg_path);
		TEST_HAS_PARTS("test_has_left", "test_has_right", true);
		TEST_HAS_PARTS("test_has_left_na", "test_has_right_na", false);
		TEST_HAS("test_has_ext", "test_has_name", "test_has_dir");
		EXTRACT_EXT("extract_ext", false);
		EXTRACT_NAME("extract_name", false);
		EXTRACT_NAME("extract_name+ext", true);
		EXTRACT_PARTS("extract_left", "extract_right", true);
		EXTRACT_PARTS("extract_left_na", "extract_right_na", false);
		EXTRACT_DIR("extract_dir", false);
		EXTRACT_DIR("extract_dir+trail", true);
	}
	return 0;
}

