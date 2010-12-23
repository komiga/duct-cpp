/**
@file characterset.hpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010 Tim Howard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

@section DESCRIPTION

duct++ CharacterSet class.
*/

#ifndef _DUCT_CHARACTERSET_HPP
#define _DUCT_CHARACTERSET_HPP

#include <duct/config.hpp>
#include <unicode/unistr.h>
#include <vector>

namespace duct {

class DUCT_API CharacterRange {
public:
	/**
		Constructor with range.
		@param start The start of the range.
		@param length The length of the range.
	*/
	CharacterRange(UChar32 start, unsigned int length);
	/**
		Copy constructor.
		@param range The range to copy from.
	*/
	CharacterRange(const CharacterRange& range);
	
	/**
		Set the start of the range.
		@returns Nothing.
		@param start The new start of the range.
	*/
	void setStart(UChar32 start);
	/**
		Get the start of the range.
		@returns The start of the range.
	*/
	UChar32 start() const;
	
	/**
		Set the end of the range.
		@returns Nothing.
		@param end The new end of the range.
	*/
	void setEnd(UChar32 end);
	/**
		Get the end of the range.
		@returns The end of the range.
	*/
	UChar32 end() const;
	
	/**
		Check if the range contains the given character.
		@returns true if the character was in the range, or false if it was not.
		@param c The character to test.
	*/
	bool contains(UChar32 c) const;
	
	/**
		Find the first matching character in the given string.
		@returns The index of the first matching character in the string, or -1 if either <i>from</i> was greater than or equal to the string's length or there were no matching characters in the string.
		@param str The string to search.
		@param from Optional start index.
	*/
	int findInString(const UnicodeString& str, unsigned int from = 0) const;
	/**
		Find the last matching character in the given string.
		@returns The index of the last matching character in the string, or -1 if there were no matching characters in the string.
		@param str The string to search.
		@param from Optional start index. If -1, the last index will be used. Iteration is backwards.
	*/
	int findLastInString(const UnicodeString& str, int from = -1) const;
	
	/**
		Compare the given range with this range.
		@returns -1 if the given range is greater than this, 1 if this range is greater than the given, or 0 if they are the same.
		@param other The range to test.
	*/
	int compare(const CharacterRange& other) const;
	/**
		Check if the given range intersects with this range.
		@returns true if the ranges intersect, or false if they do not.
		@param other The range to test.
	*/
	bool intersects(const CharacterRange& other) const;
	
protected:
	UChar32 _start;
	UChar32 _end;
};

/**
	CharacterSet range vector.
*/
typedef std::vector<CharacterRange> RangeVec;

class DUCT_API CharacterSet {
public:
	/**
		Constructor with string ranges.
	*/
	CharacterSet(const UnicodeString& str);
	/**
		Constructor with single range.
	*/
	CharacterSet(UChar32 start, unsigned int length);
	/**
		Constructor with character.
	*/
	CharacterSet(UChar32 character);
	/**
		Copy constructor.
	*/
	CharacterSet(const CharacterSet& set);
	
	/**
		Begin range iterator.
		@returns The beginning iterator for the set's ranges.
	*/
	RangeVec::iterator begin();
	/**
		End range iterator.
		@returns The end iterator for the set's ranges.
	*/
	RangeVec::iterator end();
	
	/**
		const begin range iterator.
		@returns The beginning iterator for the set's ranges.
	*/
	RangeVec::const_iterator begin() const;
	/**
		const end range iterator.
		@returns The end iterator for the set's ranges.
	*/
	RangeVec::const_iterator end() const;
	
	/**
		Check if the set contains the given character.
		@returns true if the character was in the set's ranges, or false if it was not.
		@param c The character to test.
	*/
	bool contains(UChar32 c) const;
	
	/**
		Check if the set contains the given range.
		@returns true if the character was in the set's ranges, or false if it was not.
		@param range The range to test.
	*/
	bool contains(const CharacterRange& range) const;
	
	/**
		Find the first matching character in the given string.
		@returns The index of the first matching character in the string, or -1 if either <i>from</i> was greater than or equal to the string's length or there were no matching characters in the string.
		@param str The string to search.
		@param from Optional begin index.
	*/
	int findInString(const UnicodeString& str, unsigned int from = 0) const;
	/**
		Find the last matching character in the given string.
		@returns The index of the last matching character in the string, or -1 if there were no matching characters in the string.
		@param str The string to search.
		@param from Optional begin index. If -1, the last index will be used. Iteration is backwards.
	*/
	int findLastInString(const UnicodeString& str, int from = -1) const;
	
	/**
		Add the given string ranges to the set.
		@returns Nothing.
		@param str The string ranges to add.
	*/
	void addRangesWithString(const UnicodeString& str);
	/**
		Add the given integer range.
		@returns Nothing.
		@param begin The start of the range.
		@param length The length of the range.
	*/
	void addRange(UChar32 begin, unsigned int length);
	
	//void toString(UnicodeString& str) const;
	
	/**
		Initialize the set with whitespace ranges.
		Includes \t, \r, \n, and space.
		@returns A reference to itself.
	*/
	CharacterSet& initWithWhitespace();
	/**
		Initialize the set with alphanumeric ranges.
		Includes A-Z, a-z, and 0-9.
		@returns A reference to itself.
	*/
	CharacterSet& initWithAlphanumeric();
	/**
		Initialize the set with letter ranges.
		Includes A-Z and a-z.
		@returns A reference to itself.
	*/
	CharacterSet& initWithLetters();
	/**
		Initialize the set with upper-case letters ranges.
		Includes A-Z.
		@returns A reference to itself.
	*/
	CharacterSet& initWithUppercaseLetters();
	/**
		Initialize the set with lower-case letters ranges.
		Includez a-z.
		@returns A reference to itself.
	*/
	CharacterSet& initWithLowercaseLetters();
	/**
		Initialize the set with number ranges.
		Includes 0-9.
		@returns A reference to itself.
	*/
	CharacterSet& initWithNumbers();
	/**
		Initialize the set with newline.
		Includes \n.
		@returns A reference to itself.
	*/
	CharacterSet& initWithNewline();
	
protected:
	RangeVec _ranges;
};

} // namespace duct

#endif // _DUCT_CHARACTERSET_HPP

