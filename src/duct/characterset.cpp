/**
@file characterset.cpp
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

duct++ CharacterSet implementation.
*/

#include <duct/debug.hpp>
#include <duct/characterset.hpp>

namespace duct {

// class CharacterRange implementation

CharacterRange::CharacterRange(UChar32 start, unsigned int length) {
	_start=start;
	_end=start+length;
}

CharacterRange::CharacterRange(const CharacterRange& range) {
	_start=range._start;
	_end=range._end;
}

void CharacterRange::setStart(UChar32 start) {
	_start=start;
}

UChar32 CharacterRange::start() const {
	return _start;
}

void CharacterRange::setEnd(UChar32 end) {
	_end=end;
}

UChar32 CharacterRange::end() const {
	return _end;
}

bool CharacterRange::contains(UChar32 c) const {
	return (_start==c || (c>=_start && c<=_end));
}

int CharacterRange::findInString(const UnicodeString& str, unsigned int from) const {
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

int CharacterRange::findLastInString(const UnicodeString& str, int from) const {
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

int CharacterRange::compare(const CharacterRange& other) const {
	int sd=_end-_start;
	int od=other._end-other._start;
	if (sd<od) {
		return -1;
	} else if (sd>od) {
		return 1;
	}
	if (_start<other._start) {
		return -1;
	} else if (_start>other._start) {
		return 1;
	}
	return 0;
}

bool CharacterRange::intersects(const CharacterRange& other) const {
	if (compare(other)==0) {
		return true;
	}
	if (_end==(other._start-1)) {
		return true;
	} else if ((_start-1)==other._end) {
		return true;
	}
	return !(_start>other._end || _end<other._start);
}

// class CharacterSet implementation

CharacterSet::CharacterSet(const UnicodeString& str) {
	addRangesWithString(str);
}

CharacterSet::CharacterSet(UChar32 start, unsigned int length) {
	addRange(start, length);
}

CharacterSet::CharacterSet(UChar32 character) {
	addRange(character, 0);
}

CharacterSet::CharacterSet(const CharacterSet& set) {
	_ranges=set._ranges;
}

RangeVec::iterator CharacterSet::begin() {
	return _ranges.begin();
}

RangeVec::iterator CharacterSet::end() {
	return _ranges.end();
}

RangeVec::const_iterator CharacterSet::begin() const {
	return _ranges.begin();
}

RangeVec::const_iterator CharacterSet::end() const {
	return _ranges.end();
}

bool CharacterSet::contains(UChar32 c) const {
	for (RangeVec::const_iterator iter=begin(); iter!=end(); ++iter) {
		if ((*iter).contains(c)) {
			return true;
		}
	}
	return false;
}

bool CharacterSet::contains(const CharacterRange& range) const {
	for (RangeVec::const_iterator iter=begin(); iter!=end(); ++iter) {
		if (range.compare((*iter))==0) {
			return true;
		}
	}
	return false;
}

int CharacterSet::findInString(const UnicodeString& str, unsigned int from) const {
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

int CharacterSet::findLastInString(const UnicodeString& str, int from) const {
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

void CharacterSet::addRangesWithString(const UnicodeString& str) {
	const UChar32 CHAR_DASH=0x2D;
	const UChar32 CHAR_ESCAPE=0x5C;
	
	int lastchar=-1;
	int chr;
	bool isrange=false;
	bool escape=false;
	for (int i=0; i<str.length(); ++i) {
		chr=str.charAt(i);
		if (escape) {
			escape=false;
		} else if (chr==CHAR_ESCAPE) {
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
	bool empty=_ranges.empty();
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
		_ranges.push_back(range);
	}
}

CharacterSet& CharacterSet::initWithWhitespace() {
	addRange('\t', 1); // \t and \n
	addRange('\r', 0);
	addRange(' ', 0);
	return *this;
}

CharacterSet& CharacterSet::initWithAlphanumeric() {
	addRange('A', 26);
	addRange('a', 26);
	addRange('0', 10);
	return *this;
}

CharacterSet& CharacterSet::initWithLetters() {
	addRange('A', 26);
	addRange('a', 26);
	return *this;
}

CharacterSet& CharacterSet::initWithUppercaseLetters() {
	addRange('A', 26);
	return *this;
}

CharacterSet& CharacterSet::initWithLowercaseLetters() {
	addRange('a', 26);
	return *this;
}

CharacterSet& CharacterSet::initWithNumbers() {
	addRange('0', 10);
	return *this;
}

CharacterSet& CharacterSet::initWithNewline() {
	addRange('\n', 0);
	return *this;
}

} // namespace duct

