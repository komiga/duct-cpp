/**
@file config.hpp
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

duct++ configuration header. Mostly taken from SFML.
*/

#ifndef _DUCT_CONFIG_HPP
#define _DUCT_CONFIG_HPP

#include <assert.h>
//#include <stdint.h>

namespace duct {

/**
	duct++ version.
*/
//const char* VERSION = "0.01"; // TODO: multiple definition errors with this line

// Platform
#if defined(_WIN32) || defined(__WIN32__)
	// Windows
	#define DUCT_PLATFORM_WINDOWS
#elif defined(linux) || defined(__linux)
	// Linux
	#define DUCT_PLATFORM_LINUX
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)
	// MacOS
	#define DUCT_PLATFORM_MACOS
#else // Unsupported
	#error This operating system is not supported by duct++.
#endif

// Exports
#if defined(DUCT_PLATFORM_WINDOWS)
	#ifdef DUCT_DYNAMIC
		#ifdef DUCT_EXPORTS
			// Library
			#define DUCT_API __declspec(dllexport)
		#else
			// Client
			#define DUCT_API __declspec(dllimport)
		#endif
	#else
		// Not needed needed for static build
		#define DUCT_API
	#endif
#else
	// Not needed for unix
	#define DUCT_API
#endif

#ifdef DUCT_PLATFORM_LINUX
	#include <byteswap.h>
#endif

#ifndef bswap_16
	#define bswap_16(x)	\
		((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))
#endif

#ifndef bswap_32
	#define bswap_32(x)	\
		((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24))
#endif

/* This byteorder stuff was lifted from PhysFS, which was lifted from SDL. http://www.libsdl.org/ */
#define DUCT_LITTLE_ENDIAN	1234
#define DUCT_BIG_ENDIAN	4321

#if	defined(__i386__) || defined(__ia64__) || defined(WIN32) || \
	(defined(__alpha__) || defined(__alpha)) || \
	defined(__arm__) || defined(ARM) || \
	(defined(__mips__) && defined(__MIPSEL__)) || \
	defined(__SYMBIAN32__) || \
	defined(__x86_64__) || \
	defined(__LITTLE_ENDIAN__)
	#define DUCT_BYTEORDER	DUCT_LITTLE_ENDIAN
#else
	#define DUCT_BYTEORDER	DUCT_BIG_ENDIAN
#endif

/*
// Define portable fixed-size types
#include <limits.h>
// 8-bit integer types (byte)
#if UCHAR_MAX == 0xFF
	typedef signed		char Byte;
	typedef unsigned	char UByte;
#else
	#error No 8-bit integer type for this platform
#endif

// 16-bit integer types (short)
#if USHRT_MAX == 0xFFFF
	typedef signed		short Short;
	typedef unsigned	short UShort;
#elif UINT_MAX == 0xFFFF
	typedef signed		int Short;
	typedef unsigned	int UShort;
#elif ULONG_MAX == 0xFFFF
	typedef signed		long Short;
	typedef unsigned	long UShort;
#else
	#error No 16-bit integer type for this platform
#endif

// 32-bit integer types (int)
#if USHRT_MAX == 0xFFFFFFFF
	typedef signed		short Int;
	typedef unsigned	short UInt;
#elif UINT_MAX == 0xFFFFFFFF
	typedef signed		int Int;
	typedef unsigned	int UInt;
#elif ULONG_MAX == 0xFFFFFFFF
	typedef signed		long Int;
	typedef unsigned	long UInt;
#else
	#error No 32-bit integer type for this platform
#endif

// 64-bit integer types (long)
#if ULONG_MAX == 0xFFFFFFFFFFFFFFFF
	typedef signed		long int Long;
	typedef unsigned	long int ULong;
#elif ULLONG_MAX == 0xFFFFFFFFFFFFFFFF
	typedef signed		long long int Long;
	typedef unsigned	long long int ULong;
#else
	#error No 64-bit integer type for this platform
#endif
*/

} // namespace duct

#endif // _DUCT_CONFIG_HPP

