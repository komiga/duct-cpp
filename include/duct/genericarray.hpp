/**
@file genericarray.hpp
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

@section DESCRIPTION

duct++ GArray class.
*/

#ifndef _DUCT_GENERICARRAY_HPP
#define _DUCT_GENERICARRAY_HPP

#include <duct/config.hpp>
#include <duct/debug.hpp>

#include <stdarg.h>

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
	GArray()
		: m_size(0), m_data(NULL), m_static(false)
	{/* Do nothing*/};
	/**
		Constructor with single element.
		@param element The single element to initialize with.
	*/
	GArray(T element)
		: m_size(1), m_data(new T[1]), m_static(false)
	{
		m_data[0]=element;
	};
	/**
		Array data constructor.
		@param data The data pointer.
		@param size The number of elements in the data pointer. If -1, the pointer must be NULL-terminated.
		@param isstatic Whether the given data pointer is a static array. If true, the given pointer will not be freed.
	*/
	GArray(T* data, int size=-1, bool isstatic=false)
		: m_size(0), m_data(NULL), m_static(false)
	{
		set(data, size, isstatic);
	};
	/**
		Variadic constructor with elements.
		@param num The number of arguments.
		@param ... The elements of type T to initialize with.
	*/
	GArray(size_t num, ...)
		: m_size(0), m_data(NULL), m_static(false)
	{
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
		return m_size;
	};
	/**
		Get the array's data pointer.
		@returns The array's data pointer.
	*/
	T* data() {
		return m_data;
	};
	T const* data() const {
		return m_data;
	};
	/**
		Set the array to the given element.
		@returns Nothing.
		@param element The element to set the array to (single-element).
	*/
	void set(T element) {
		release();
		m_data=new T[m_size=1];
		m_data[0]=element;
		m_static=false;
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
		m_size=size;
		m_data=data;
		m_static=false;
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
		m_size=size;
		m_data=data;
		m_static=true;
	};
	/**
		Set the array's data from the given va_list.
		@returns Nothing.
		@param num The number of arguments in the list.
		@param ap The variadic argument list.
	*/
	void setVL(size_t num, va_list ap) {
		release();
		m_size=num;
		m_data=new T[m_size];
		for (unsigned int i=0; i<num; ++i) {
			m_data[i]=va_arg(ap, T);
		}
		m_static=false;
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
		if (m_data && !m_static) {
			delete[] m_data;
		}
		m_data=NULL;
		m_size=0;
	};
	/**
		Subscript operator.
		Bounds are not checked.
		@returns Reference to the element at the given index.
		@param index The index to retrieve.
	*/
	T& operator[](int const index) {
		debug_assertp(m_data!=NULL, this, "m_data==NULL");
		return m_data[index];
	};
	T const& operator[](int const index) const {
		debug_assertp(m_data!=NULL, this, "m_data==NULL");
		return m_data[index];
	};
	T& operator[](unsigned int const index) {
		debug_assertp(m_data!=NULL, this, "m_data==NULL");
		return m_data[index];
	};
	T const& operator[](unsigned int const index) const {
		debug_assertp(m_data!=NULL, this, "m_data==NULL");
		return m_data[index];
	};
	
protected:
	size_t m_size;
	T* m_data;
	bool m_static;
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
		if (this->m_data) {
			for (unsigned int i=0; i<this->m_size; ++i) {
				if (this->m_data[i]) {
					delete this->m_data[i];
					this->m_data[i]=NULL;
				}
			}
		}
	};
	virtual void release() {
		if (this->m_data) {
			for (unsigned int i=0; i<this->m_size; ++i) {
				if (this->m_data[i]) {
					delete this->m_data[i];
					this->m_data[i]=NULL;
				}
			}
			if (!this->m_static) {
				delete[] this->m_data;
			}
		}
		this->m_data=NULL;
		this->m_size=0;
	};
};

} // namespace duct

#endif // _DUCT_GENERICARRAY_HPP

