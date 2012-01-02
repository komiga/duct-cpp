/**
@file csvtemplate.cpp
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
#include <duct/csvtemplate.hpp>

#include <stdio.h>

namespace duct {

CSVTemplate::CSVTemplate(size_t count, const CSVTRecord* layout) {
	setLayout(count, layout);
}

void CSVTemplate::setLayout(size_t count, const CSVTRecord* layout) {
	_count=count;
	_layout=layout;
}

const CSVTRecord* CSVTemplate::getLayout() const {
	return _layout;
}

size_t CSVTemplate::getCount() const {
	return _count;
}

bool CSVTemplate::validate(const CSVRow& row, int startindex) const {
	if (!_layout && _count!=row.inRange(startindex, startindex+_count)) {
		//printf("%s: count mismatch; _c: %lu inr: %lu\n", __PRETTY_FUNCTION__, _count, row.inRange(startindex, startindex+_count));
		return false;
	}
	if (_layout) {
		int index=startindex;
		CSVRecordMap::const_iterator iter;
		CSVRecordMap::const_iterator end=row.end();
		while ((size_t)index<_count) {
			iter=row.find(index);
			if (iter!=end) {
				if (iter->second && !(_layout[index].type&iter->second->getType())) {
					//printf("debug: %s: [%d] non-matching type\n", __PRETTY_FUNCTION__, index);
					return false;
				} else if (!iter->second && !_layout[index].nullable) { // not nullable; is null
					//printf("debug: %s: [%d] not nullable; null\n", __PRETTY_FUNCTION__, index);
					return false;
				}
			} else if (!_layout[index].nullable) { // not nullable; is unset
				//printf("debug: %s: [%d] not nullable; unset\n", __PRETTY_FUNCTION__, index);
				return false;
			}
			index++;
		}
	} // else count==rangecount and that's all that the template requires for validation
	return true;
}

size_t CSVTemplate::removeRows(CSVMap& map, bool matching) const {
	size_t count=0;
	CSVRowMap::iterator iter;
	CSVRowMap::iterator end=map.end();
	for (iter=map.begin(); iter!=end; ++iter) {
		if (validate(*iter->second)==matching) {
			map.erase(iter);
			count++;
		}
	}
	return count;
}

} // namespace duct

