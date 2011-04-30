/**
@file unistrarray.cpp
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
*/

#include <duct/debug.hpp>
#include <duct/unistrarray.hpp>

namespace duct {

// class StringArray implementation

StringArray::StringArray() : _size(0), _data(NULL), _releasecontainer(true) {
}

StringArray::StringArray(const char** data, int size) : _size(0), _data(NULL), _releasecontainer(true) {
	setFromCStrings(data, size);
}

StringArray::StringArray(UnicodeString** data, int size, bool _releasecontainer) : _size(0), _data(NULL), _releasecontainer(true) {
	set(data, size, _releasecontainer);
}

StringArray::~StringArray() {
	release();
}

unsigned int StringArray::getSize() const {
	return _size;
}

UnicodeString** StringArray::get() {
	return _data;
}

void StringArray::release() {
	if (_data) {
		for (unsigned int i=0; i<_size; ++i) {
			if (_data[i]) {
				delete _data[i];
				_data[i]=NULL;
			}
		}
		if (_releasecontainer)
			delete [] _data;
		_data=NULL;
	}
	_size=0;
}

void StringArray::set(UnicodeString** data, int size, bool releasecontainer) {
	if (size<0) {
		int i=0;
		while (data[i]!=NULL) {
			++i;
		}
		size=i;
	}
	_size=size;
	_data=data;
	_releasecontainer=releasecontainer;
}

void StringArray::setFromCStrings(const char** data, int size) {
	int i=0;
	if (size<0) {
		while (data[i]!=NULL) {
			++i;
		}
		size=i;
	}
	_size=size;
	_data=new UnicodeString*[size];
	for (i=0; i<size; ++i) {
		_data[i]=new UnicodeString(data[i]);
	}
	_releasecontainer=true;
}

void StringArray::setFromVCStrings(unsigned int num, va_list ap) {
	_size=num;
	_data=new UnicodeString*[_size];
	for (unsigned int i=0; i<num; ++i) {
		_data[i]=new UnicodeString(va_arg(ap, const char*));
	}
	_releasecontainer=true;
}

void StringArray::setFromVCStrings(unsigned int num, ...) {
	va_list ap;
	va_start(ap, num);
	setFromVCStrings(num, ap);
	va_end(ap);
}

void StringArray::setFromVUStrings(unsigned int num, va_list ap) {
	_size=num;
	_data=new UnicodeString*[_size];
	for (unsigned int i=0; i<num; ++i) {
		_data[i]=va_arg(ap, UnicodeString*);
	}
	_releasecontainer=true;
}

void StringArray::setFromVUStrings(unsigned int num, ...) {
	va_list ap;
	va_start(ap, num);
	setFromVUStrings(num, ap);
	va_end(ap);
}

UnicodeString* StringArray::operator[](const int index) {
	debug_assertp(_data==NULL, this, "_data==NULL");
	return _data[index];
}

const UnicodeString* StringArray::operator[](const int index) const {
	debug_assertp(_data==NULL, this, "_data==NULL");
	return _data[index];
}

StringArray* StringArray::withVCStrings(unsigned int num, ...) {
	StringArray* sa=new StringArray();
	va_list ap;
	va_start(ap, num);
	sa->setFromVCStrings(num, ap);
	va_end(ap);
	return sa;
}

StringArray* StringArray::withVUStrings(unsigned int num, ...) {
	StringArray* sa=new StringArray();
	va_list ap;
	va_start(ap, num);
	sa->setFromVUStrings(num, ap);
	va_end(ap);
	return sa;
}

} // namespace duct

