/**
@file genericarray.hpp
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

duct++ GArray class.
*/

#ifndef _DUCT_GENERICARRAY_HPP
#define _DUCT_GENERICARRAY_HPP

#include <stdarg.h>
#include <duct/config.hpp>
#include <duct/debug.hpp>

namespace duct {

/**
	Generic non-flexible managed array.
	This class takes ownership of given data pointers.
	@see GPArray
*/
template <class T>
class DUCT_API GArray {
public:
	/**
		Constructor.
	*/
	GArray() : _size(0), _data(NULL), _static(false) {
	};
	/**
		Constructor with single element.
		@param element The single element to initialize with.
	*/
	GArray(T element) : _size(1), _data(new T[1]), _static(false) {
		_data[0]=element;
	};
	/**
		Array data constructor.
		@param data The data pointer.
		@param size The number of elements in the data pointer. If -1, the pointer must be NULL-terminated.
		@param isstatic Whether the given data pointer is a static array. If true, the given pointer will not be freed.
	*/
	GArray(T* data, int size=-1, bool isstatic=false) : _size(0), _data(NULL), _static(false) {
		set(data, size, isstatic);
	};
	/**
		Variadic constructor with elements.
		@param num The number of arguments.
		@param ... The elements of type T to initialize with.
	*/
	GArray(size_t num, ...) : _size(0), _data(NULL), _static(false) {
		va_list ap;
		va_start(ap, num);
		setVL(num, ap);
		va_end(ap);
	};
	/**
		Destructor.
	*/
	virtual ~GArray() {
		release();
	};
	/**
		Get the array's size.
		@returns The array's size.
	*/
	size_t size() const {
		return _size;
	};
	/**
		Get the array's data pointer.
		@returns The array's data pointer.
	*/
	T* data() {
		return _data;
	};
	const T* data() const {
		return _data;
	};
	/**
		Set the array to the given element.
		@returns Nothing.
		@param element The element to set the array to (single-element).
	*/
	void set(T element) {
		release();
		_data=new T[_size=1];
		_data[0]=element;
		_static=false;
	};
	/**
		Set the array data.
		The given data is not copied - the array owns the given data pointer.
		@returns Nothing.
		@param data The array data.
		@param size The number of elements in the array. If -1, the array must be NULL-terminated.
	*/
	void set(T* data, int size=-1) {
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
		_static=false;
	};
	/**
		Set the array data to the given static-array data pointer.
		The given pointer will not be freed.
		@returns Nothing.
		@param data The data pointer.
		@param size The number of elements in the array. If -1, the pointer must be NULL-terminated.
	*/
	void setStatic(T* data, int size=-1) {
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
		_static=true;
	};
	/**
		Set the array's data from the given va_list.
		@returns Nothing.
		@param num The number of arguments in the list.
		@param ap The variadic argument list.
	*/
	void setVL(size_t num, va_list ap) {
		release();
		_size=num;
		_data=new T[_size];
		for (unsigned int i=0; i<num; ++i) {
			_data[i]=va_arg(ap, T);
		}
		_static=false;
	};
	/**
		Set the array's data from the given arugments.
		@returns Nothing.
		@param num The number of variadic arguments.
		@param ... The elements of type T to use.
	*/
	void setV(size_t num, ...) {
		va_list ap;
		va_start(ap, num);
		setVL(num, ap);
		va_end(ap);
	};
	/**
		Release the array's data.
		@returns Nothing.
	*/
	virtual void release() {
		if (_data && !_static) {
			delete[] _data;
		}
		_data=NULL;
		_size=0;
	};
	/**
		Subscript operator.
		Bounds are not checked.
		@returns Reference to the element at the given index.
		@param index The index to retrieve.
	*/
	T& operator[](const int index) {
		debug_assertp(_data!=NULL, this, "_data==NULL");
		return _data[index];
	};
	const T& operator[](const int index) const {
		debug_assertp(_data!=NULL, this, "_data==NULL");
		return _data[index];
	};
	T& operator[](const unsigned int index) {
		debug_assertp(_data!=NULL, this, "_data==NULL");
		return _data[index];
	};
	const T& operator[](const unsigned int index) const {
		debug_assertp(_data!=NULL, this, "_data==NULL");
		return _data[index];
	};
	
protected:
	size_t _size;
	T* _data;
	bool _static;
};

/**
	Generic non-flexible managed array for pointers.
	This class takes ownership of given data pointers and elements.
	Static data pointers will not be freed, but their contents will be.
	@see GArray
*/
template <class T>
class DUCT_API GPArray : public GArray<T> {
public:
	virtual ~GPArray() {
		if (this->_data) {
			for (unsigned int i=0; i<this->_size; ++i) {
				if (this->_data[i]) {
					delete this->_data[i];
					this->_data[i]=NULL;
				}
			}
		}
	};
	virtual void release() {
		if (this->_data) {
			for (unsigned int i=0; i<this->_size; ++i) {
				if (this->_data[i]) {
					delete this->_data[i];
					this->_data[i]=NULL;
				}
			}
			if (!this->_static) {
				delete[] this->_data;
			}
		}
		this->_data=NULL;
		this->_size=0;
	};
};

} // namespace duct

#endif // _DUCT_GENERICARRAY_HPP

