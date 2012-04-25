/**
@file config.hpp
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

duct++ configuration header.
*/

#ifndef DUCT_CONFIG_HPP_
#define DUCT_CONFIG_HPP_

#include <assert.h>
#include <stdint.h>

// Prevent ICU from auto-using the icu namespace
#define U_USING_ICU_NAMESPACE 0
#include <unicode/uversion.h>

namespace duct {

#ifdef DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

// Platform
/**
	Equal to the platform bitness (32 or 64).
*/
#define DUCT_PLATFORM_BITNESS 32 or 64
/**
	Defined when the platform is 32-bit (32==DUCT_PLATFORM_BITNESS).
*/
#define DUCT_PLATFORM_BITNESS_32
/**
	Defined when the platform is 64-bit (64==DUCT_PLATFORM_BITNESS).
*/
#define DUCT_PLATFORM_BITNESS_64

/** 
	Defined when the system is Windows; equivalent to DUCT_PLATFORM_BITNESS.
*/
#define DUCT_PLATFORM_WINDOWS DUCT_PLATFORM_BITNESS
/**
	Defined when the system is Linux; equivalent to DUCT_PLATFORM_BITNESS.
*/
#define DUCT_PLATFORM_LINUX DUCT_PLATFORM_BITNESS
/**
	Defined when the system is Mac OS; equivalent to DUCT_PLATFORM_BITNESS.
*/
#define DUCT_PLATFORM_MACOS DUCT_PLATFORM_BITNESS

// Exports
/**
	Import/export attributes for Windows (blank when building static or on UNIX-based systems).
*/
#define DUCT_API

/**
	Ensures bswap_16 is implemented (Linux byteswap.h).
	@see bswap_32(), bswap_64()
*/
#define bswap_16(x)
/**
	Ensures bswap_32 is implemented (Linux byteswap.h).
	@see bswap_16(), bswap_64()
*/
#define bswap_32(x)
/**
	Ensures bswap_64 is implemented (Linux byteswap.h).
	@see bswap_16(), bswap_32()
*/
#define bswap_64(x)

// Byte order
/* This byte order stuff was lifted from PhysFS, which was lifted from SDL. http://www.libsdl.org/ */
/**
	Little endian.
	@see DUCT_BYTEORDER, DUCT_BIG_ENDIAN
*/
#define DUCT_LITTLE_ENDIAN	1234
/**
	Big endian.
	@see DUCT_BYTEORDER, DUCT_LITTLE_ENDIAN
*/
#define DUCT_BIG_ENDIAN		4321

/**
	System byteorder (equals either DUCT_LITTLE_ENDIAN or DUCT_BIG_ENDIAN).
*/
#define DUCT_BYTEORDER DUCT_LITTLE_ENDIAN or DUCT_BIG_ENDIAN
/**
	Defined when DUCT_BYTEORDER==DUCT_LITTLE_ENDIAN.
	@see DUCT_BYTEORDER_BE
*/
#define DUCT_BYTEORDER_LE
/**
	Defined when DUCT_BYTEORDER==DUCT_BIG_ENDIAN.
	@see DUCT_BYTEORDER_LE
*/
#define DUCT_BYTEORDER_BE

#else // Quick, actually define everything while Doxygen isn't looking!

// Platform
#if (defined(__WORDSIZE) && (__WORDSIZE == 64)) || defined(__arch64__) || defined(__LP64__) || defined(_M_X64) || defined(__ppc64__) || defined(__x86_64__)
	#define DUCT_PLATFORM_BITNESS 64
	#define DUCT_PLATFORM_BITNESS_64
#elif (defined(__i386__) || defined(__ppc__))
	#define DUCT_PLATFORM_BITNESS 32
	#define DUCT_PLATFORM_BITNESS_32
#else
	#error "Unable to determine platform bitness"
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
	#define DUCT_PLATFORM_WINDOWS	DUCT_PLATFORM_BITNESS
#elif defined(linux) || defined(__linux)
	#define DUCT_PLATFORM_LINUX		DUCT_PLATFORM_BITNESS
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)
	#define DUCT_PLATFORM_MACOS		DUCT_PLATFORM_BITNESS
#else // Unsupported
	#error "This operating system is not supported by duct++"
#endif

// Exports
#if defined(DUCT_PLATFORM_WINDOWS)
	#ifdef DUCT_DYNAMIC
		#ifdef DUCT_EXPORT
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
	// Not needed for UNIX-based
	#define DUCT_API
#endif

#ifdef DUCT_PLATFORM_LINUX
	#include <byteswap.h>
#endif

#ifndef bswap_16
	#define bswap_16(x)	\
		((((x)>>8)&0xff)|(((x)&0xff)<<8))
#endif

#ifndef bswap_32
	#define bswap_32(x)	\
		((((x)&0xff000000)>>24)|(((x)&0x00ff0000)>>8)|(((x)&0x0000ff00)<<8)|(((x)&0x000000ff)<<24))
#endif

#ifndef bswap_64
	#define bswap_64(x)	\
		(((((x)&0xff00000000000000ULL) >> 56) | (((x)&0x00ff000000000000ULL) >> 40) | \
		  (((x)&0x0000ff0000000000ULL) >> 24) | (((x)&0x000000ff00000000ULL) >>  8) | \
		  (((x)&0x00000000ff000000ULL) <<  8) | (((x)&0x0000000000ff0000ULL) << 24) | \
		  (((x)&0x000000000000ff00ULL) << 40) | (((x)&0x00000000000000ffULL) << 56)))
#endif

// Byte order
/* This byte order stuff was lifted from PhysFS, which was lifted from SDL. http://www.libsdl.org/ */
#define DUCT_LITTLE_ENDIAN	1234
#define DUCT_BIG_ENDIAN		4321

#if	defined(__i386__) || defined(__ia64__) || defined(WIN32) || \
	(defined(__alpha__) || defined(__alpha)) || \
	defined(__arm__) || defined(ARM) || \
	(defined(__mips__) && defined(__MIPSEL__)) || \
	defined(__SYMBIAN32__) || \
	defined(__x86_64__) || \
	defined(__LITTLE_ENDIAN__)
	
	#define DUCT_BYTEORDER	DUCT_LITTLE_ENDIAN
	#define DUCT_BYTEORDER_LE
#else
	#define DUCT_BYTEORDER	DUCT_BIG_ENDIAN
	#define DUCT_BYTEORDER_BE
#endif

#endif // DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

} // namespace duct

#endif // DUCT_CONFIG_HPP_
