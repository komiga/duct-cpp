/**
@file genericarray.cpp
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
#include <duct/genericarray.hpp>

namespace duct {

// class GArray implementation
/*
template <class T>
GArray<T>::GArray() : _size(0), _data(NULL), _releasecontainer(true) {
}

template <class T>
GArray<T>::GArray(T element) : _size(1), _data(new T[1]), _releasecontainer(true) {
	_data[0]=element;
}

template <class T>
GArray<T>::GArray(T* data, int size, bool isstatic) : _size(0), _data(NULL), _releasecontainer(true) {
	set(data, size, isstatic!=true);
}

template <class T>
GArray<T>::GArray(size_t num, ...) : _size(0), _data(NULL), _releasecontainer(true) {
	va_list ap;
	va_start(ap, num);
	setVL(num, ap);
	va_end(ap);
}

template <class T>
GArray<T>::~GArray() {
	release();
}

template <class T>
size_t GArray<T>::size() const {
	return _size;
}

template <class T>
T* GArray<T>::data() {
	return _data;
}

template <class T>
const T* GArray<T>::data() const {
	return _data;
}

template <class T>
void GArray<T>::set(T element) {
	release();
	_data=new T[_size=1];
	_data[0]=element;
	_releasecontainer=true;
}

template <class T>
void GArray<T>::set(T* data, int size) {
	release();
	if (size<0) {
		int i=0;
		while (data[i]!=NULL) {
			++i;
		}
		size=i;
	}
	_size=size;
	_data=data;
	_releasecontainer=true;
}

template <class T>
void GArray<T>::setStatic(T* data, int size) {
	release();
	if (size<0) {
		int i=0;
		while (data[i]!=NULL) {
			++i;
		}
		size=i;
	}
	_size=size;
	_data=data;
	_releasecontainer=false;
}

template <class T>
void GArray<T>::setVL(size_t num, va_list ap) {
	release();
	_size=num;
	_data=new T[_size];
	for (unsigned int i=0; i<num; ++i) {
		_data[i]=va_arg(ap, T);
	}
	_releasecontainer=true;
}

template <class T>
void GArray<T>::setV(size_t num, ...) {
	va_list ap;
	va_start(ap, num);
	setVL(num, ap);
	va_end(ap);
}

template <class T>
void GArray<T>::release() {
	if (_data) {
		for (unsigned int i=0; i<_size; ++i) {
			if (_data[i]) {
				delete _data[i];
				_data[i]=NULL;
			}
		}
		if (_releasecontainer)
			delete[] _data;
		_data=NULL;
	}
	_size=0;
}
*/

} // namespace duct

