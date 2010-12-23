/**
@file debug.hpp
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

duct++ debug functions.
*/

#ifndef _DUCT_DEBUG_HPP
#define _DUCT_DEBUG_HPP

#include <duct/config.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#ifdef NDEBUG
	#define debug_print(mesg)
	#define debug_printp(p, mesg)
	#define debug_print_source(mesg)
	#define debug_printp_source(p, mesg)
	#define debug_called()
	#define debug_calledp(p)
	#define debug_assert(expr, mesg)
	#define debug_assertp(expr, p, mesg)
#else
	namespace duct {
		
		// Debug functions
		void __debug_assert(const char* __assertion, const char* __file, int __line, const char* __function, const char* __mesg);
		void __debug_assertp(const char* __assertion, const char* __file, int __line, const char* __function, const void* __p, const char* __mesg);
		
	} // namespace duct
	
	/**
		@def debug_print(mesg)
		Print a debug message.
		The output format is "debug: %s\n", where %s is the message.
		
		This macro is blank-defined if NDEBUG is defined at the time of debug.hpp being included.
		@returns Nothing.
		@param mesg Message to print.
	*/
	#define debug_print(mesg)	\
		(printf("debug: %s\n", mesg))
	
	/**
		@def debug_printp(p, mesg)
		Helper macro for printing object-specific information.
		The output format is "debug: [%p] %s\n", where %s is the message.
		
		This macro is blank-defined if NDEBUG is defined at the time of debug.hpp being included.
		@returns Nothing.
		@param p Pointer to print with message.
		@param mesg Message to print.
	*/
	#define debug_printp(p, mesg)	\
		(printf("debug: [%p] %s\n", (const void*)p, mesg))
	
	/**
		@def debug_print_source(mesg)
		Helper macro for printing caller-specific information.
		The output format is "debug: from %s: %s\n", where the first %s is the function being called, and the second is the message.
		
		This macro is blank-defined if NDEBUG is defined at the time of debug.hpp being included.
		@returns Nothing.
		@param mesg Message to print.
	*/
	#ifdef DUCT_PLATFORM_WINDOWS
		#define debug_print_source(mesg)	\
			(printf("debug: from %s: %s\n", _FUNCTION_, mesg))
	#else
		#define debug_print_source(mesg)	\
			(printf("debug: from %s: %s\n", __PRETTY_FUNCTION__, mesg))
	#endif
	
	/**
		@def debug_printp_source(p, mesg)
		Helper macro for printing object- and caller- specific information.
		The output format is "debug: [%p] from %s: %s\n", where %p is the pointer, the first %s is the function being called, and the second is the message.
		
		This macro is blank-defined if NDEBUG is defined at the time of debug.hpp being included.
		@returns Nothing.
		@param p Pointer to print with message.
		@param mesg Message to print.
	*/
	#ifdef DUCT_PLATFORM_WINDOWS
		#define debug_printp_source(p, mesg)	\
			(printf("debug: [%p] from %s: %s\n", (const void*)p, _FUNCTION_, mesg))
	#else
		#define debug_printp_source(p, mesg)	\
			(printf("debug: [%p] from %s: %s\n", (const void*)p, __PRETTY_FUNCTION__, mesg))
	#endif
	
	/**
		@def debug_called()
		Debug message 'function called'.
		Helper macro for outputting a message to note that a function has been called.
		The output format is "debug_called: %s\n", where %s is the name of the function that was called.
		
		This macro is blank-defined if NDEBUG is defined at the time of debug.hpp being included.
		@returns Nothing.
	*/
	#ifdef DUCT_PLATFORM_WINDOWS
		#define debug_called()	\
			(printf("debug_called: %s\n", _FUNCTION_))
	#else
		#define debug_called()	\
			(printf("debug_called: %s\n", __PRETTY_FUNCTION__))
	#endif
	
	/**
		@def debug_calledp(p)
		Debug message 'function called' with pointer.
		Helper macro for outputting a message to note that a function has been called (with pointer).
		The output format is "debug_calledp: [%p] %s\n", where %s is the name of the function that was called.
		
		This macro is blank-defined if NDEBUG is defined at the time of debug.hpp being included.
		@returns Nothing.
		@param p Pointer to print with message.
	*/
	#ifdef DUCT_PLATFORM_WINDOWS
		#define debug_calledp(p)	\
			(printf("debug_calledp: [%p] %s\n", (const void*)p, __FUNCTION__))
	#else
		#define debug_calledp(p)	\
			(printf("debug_calledp: [%p] %s\n", (const void*)p, __PRETTY_FUNCTION__))
	#endif
	
	/**
		@def debug_assert(expr, mesg)
		Assertion with message.
		This macro mimics assert(), and aborts the program if the assertion fails.
		
		This macro is blank-defined if NDEBUG is defined at the time of debug.hpp being included.
		@returns Nothing.
		@param expr Expression to evaluate.
		@param mesg Message to print.
	*/
	#ifdef DUCT_PLATFORM_WINDOWS
		#define debug_assert(expr, mesg)	\
			((expr) ? void(0) : duct::__debug_assert(#expr, __FILE__, __LINE__, __FUNCTION__, mesg))
	#else
		#define debug_assert(expr, mesg)	\
			((expr) ? void(0) : duct::__debug_assert(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__, mesg))
	#endif
	
	/**
		@def debug_assertp(expr, p, mesg)
		Assertion with message and pointer.
		Helper macro for object-specific assertions.
		
		This macro mimics assert(), and aborts the program if the assertion fails.
		This macro is blank-defined if NDEBUG is defined at the time of debug.hpp being included.
		@returns Nothing.
		@param expr Expression to evaluate.
		@param p Pointer to print with message.
		@param mesg Message to print.
	*/
	#ifdef DUCT_PLATFORM_WINDOWS
		#define debug_assertp(expr, p, mesg)	\
			((expr) ? void(0) : duct::__debug_assertp(#expr, __FILE__, __LINE__, __FUNCTION__, p, mesg))
	#else
		#define debug_assertp(expr, p, mesg)	\
			((expr) ? void(0) : duct::__debug_assertp(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__, p, mesg))
	#endif
	
#endif // if NDEBUG -> else

#endif // __DUCT_DEBUG_HPP

