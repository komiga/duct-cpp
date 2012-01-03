/**
@file characterset.cpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010-2012 Tim Howard

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
*/

#include <duct/debug.hpp>
#include <duct/characterset.hpp>
#include <duct/charutils.hpp>

namespace duct {

// class CharacterRange implementation

CharacterRange::CharacterRange(UChar32 start, unsigned int length)
	: m_start(start), m_end(start+length)
{/* Do nothing*/}

CharacterRange::CharacterRange(CharacterRange const& range)
	: m_start(range.m_start), m_end(range.m_end)
{/* Do nothing*/}

void CharacterRange::setStart(UChar32 start) {
	m_start=start;
}

UChar32 CharacterRange::start() const {
	return m_start;
}

void CharacterRange::setEnd(UChar32 end) {
	m_end=end;
}

UChar32 CharacterRange::end() const {
	return m_end;
}

bool CharacterRange::contains(UChar32 c) const {
	return (m_start==c || (c>=m_start && c<=m_end));
}

int CharacterRange::findInString(icu::UnicodeString const& str, unsigned int from) const {
	if (from>=(unsigned int)str.length()) {
		return -1;
	}
	for (int i=from; i<str.length(); ++i) {
		if (contains(str.charAt(i))) {
			return i;
		}
	}
	return -1;
}

int CharacterRange::findLastInString(icu::UnicodeString const& str, int from) const {
	if (from==-1) {
		from=(unsigned int)str.length()-1;
	}
	for (int i=from; i>-1; --i) {
		if (contains(str.charAt(i))) {
			return i;
		}
	}
	return -1;
}

int CharacterRange::compare(CharacterRange const& other) const {
	int sd=m_end-m_start;
	int od=other.m_end-other.m_start;
	if (sd<od) {
		return -1;
	} else if (sd>od) {
		return 1;
	}
	if (m_start<other.m_start) {
		return -1;
	} else if (m_start>other.m_start) {
		return 1;
	}
	return 0;
}

bool CharacterRange::intersects(CharacterRange const& other) const {
	if (compare(other)==0) {
		return true;
	}
	if (m_end==(other.m_start-1)) {
		return true;
	} else if ((m_start-1)==other.m_end) {
		return true;
	}
	return !(m_start>other.m_end || m_end<other.m_start);
}

// class CharacterSet implementation

CharacterSet::CharacterSet()
	: m_ranges()
{/* Do nothing*/}

CharacterSet::CharacterSet(icu::UnicodeString const& str)
	: m_ranges()
{
	addRangesWithString(str);
}

CharacterSet::CharacterSet(const char* str)
	: m_ranges()
{
	icu::UnicodeString ustr(str);
	addRangesWithString(ustr);
}

CharacterSet::CharacterSet(UChar32 start, unsigned int length)
	: m_ranges()
{
	addRange(start, length);
}

CharacterSet::CharacterSet(UChar32 character)
	: m_ranges()
{
	addRange(character, 0);
}

CharacterSet::CharacterSet(const CharacterSet& set)
	: m_ranges()
{
	m_ranges=set.m_ranges;
}

RangeVec::iterator CharacterSet::begin() {
	return m_ranges.begin();
}

RangeVec::iterator CharacterSet::end() {
	return m_ranges.end();
}

RangeVec::const_iterator CharacterSet::begin() const {
	return m_ranges.begin();
}

RangeVec::const_iterator CharacterSet::end() const {
	return m_ranges.end();
}

bool CharacterSet::contains(UChar32 c) const {
	for (RangeVec::const_iterator iter=begin(); iter!=end(); ++iter) {
		if ((*iter).contains(c)) {
			return true;
		}
	}
	return false;
}

bool CharacterSet::contains(CharacterRange const& range) const {
	for (RangeVec::const_iterator iter=begin(); iter!=end(); ++iter) {
		if (range.compare((*iter))==0) {
			return true;
		}
	}
	return false;
}

int CharacterSet::findInString(icu::UnicodeString const& str, unsigned int from) const {
	if (from>(unsigned int)str.length()) {
		return -1;
	}
	int i;
	for (RangeVec::const_iterator iter=begin(); iter!=end(); ++iter) {
		i=(*iter).findInString(str, from);
		if (i>-1) {
			return i;
		}
	}
	return -1;
}

int CharacterSet::findLastInString(icu::UnicodeString const& str, int from) const {
	if (from==-1) {
		from=str.length()-1;
	}
	int result=-1, i=0;
	for (RangeVec::const_iterator iter=begin(); iter!=end(); ++iter) {
		i=(*iter).findLastInString(str, from);
		if (i!=-1 && (i>result || result==-1)) {
			result=i;
		}
	}
	return result;
}

void CharacterSet::clear() {
	m_ranges.clear();
}

void CharacterSet::addRangesWithString(icu::UnicodeString const& str) {
	int lastchar=-1;
	int chr;
	bool isrange=false;
	bool escape=false;
	for (int i=0; i<str.length(); ++i) {
		chr=str.charAt(i);
		if (escape) {
			escape=false;
		} else if (chr==CHAR_BACKSLASH) {
			escape=true;
			continue;
		} else if (lastchar!=-1 && chr==CHAR_DASH && !isrange) {
			isrange=true;
			continue;
		}
		if (lastchar!=-1) {
			if (isrange) {
				if (chr==lastchar) {
					addRange(chr, 0);
				} else if (chr<lastchar) {
					addRange(chr, lastchar-chr);
				} else {
					addRange(lastchar, chr-lastchar);
				}
				lastchar=-1;
				isrange=false;
			} else {
				addRange(lastchar, 0);
				lastchar=chr;
			}
		} else {
			lastchar=chr;
		}
	}
	if (lastchar!=-1) {
		if (isrange) {
			//throw "Invalid range in string '"+str+"'";
			debug_printp(this, "Invalid range in string");
		}
		addRange(lastchar, 0);
	}
}

void CharacterSet::addRange(UChar32 start, unsigned int length) {
	bool empty=m_ranges.empty();
	CharacterRange range(start, length);
	if (empty || !contains(range)) { // try to avoid adding the same range twice
		if (!empty) {
			for (RangeVec::iterator iter=begin(); iter!=end(); ++iter) {
				CharacterRange& i=(*iter);
				if (range.intersects(i)) {
					if (range.start()<i.start()) {
						i.setStart(range.start());
					}
					if (i.end()<range.end()) {
						i.setEnd(range.end());
					}
					return;
				}
			}
		}
		m_ranges.push_back(range);
	}
}

void CharacterSet::addWhitespace() {
	addRange('\t', 1); // \t and \n
	addRange('\r', 0);
	addRange(' ', 0);
}

void CharacterSet::addAlphanumeric() {
	addRange('A', 26);
	addRange('a', 26);
	addRange('0', 10);
}

void CharacterSet::addLetters() {
	addRange('A', 26);
	addRange('a', 26);
}

void CharacterSet::addUppercaseLetters() {
	addRange('A', 26);
}

void CharacterSet::addLowercaseLetters() {
	addRange('a', 26);
}

void CharacterSet::addNumbers() {
	addRange('0', 10);
}

} // namespace duct

