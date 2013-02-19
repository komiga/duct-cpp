/**
@file config.hpp
@brief Configuration.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef DUCT_CONFIG_HPP_
#define DUCT_CONFIG_HPP_

#include <cstdint>

/**
	@addtogroup config
	@{
*/

#ifdef DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/**
	@name Platform
	@{
*/

/** OS: Windows. */
#define DUCT_FLAG_PLATFORM_WINDOWS	0x00000001
/** OS: Linux. */
#define DUCT_FLAG_PLATFORM_LINUX	0x00000002
/** OS: Mac OS. */
#define DUCT_FLAG_PLATFORM_MACOS	0x00000004

/** Processor model: 32-bit (x86). */
#define DUCT_FLAG_PLATFORM_MODEL_32	0x00000100
/** Processor model: 64-bit (x86-64). */
#define DUCT_FLAG_PLATFORM_MODEL_64	0x00000200

/**
	Platform flags.
	Defined to a bit-wise OR of one OS:
	-# #DUCT_FLAG_PLATFORM_WINDOWS
	-# #DUCT_FLAG_PLATFORM_LINUX
	-# #DUCT_FLAG_PLATFORM_MACOS

	and one model:
	-# #DUCT_FLAG_PLATFORM_MODEL_32 (x86 processor)
	-# #DUCT_FLAG_PLATFORM_MODEL_64 (x86-64 processor)
*/
#define DUCT_PLATFORM system_dependent

/**
	Equals one of:
	-# #DUCT_FLAG_PLATFORM_WINDOWS
	-# #DUCT_FLAG_PLATFORM_LINUX
	-# #DUCT_FLAG_PLATFORM_MACOS
*/
#define DUCT_PLATFORM_SYSTEM system_dependent

/**
	Defined when:
	@code (DUCT_PLATFORM_SYSTEM==DUCT_FLAG_PLATFORM_WINDOWS) @endcode
*/
#define DUCT_PLATFORM_SYSTEM_WINDOWS
/**
	Defined when:
	@code (DUCT_PLATFORM_SYSTEM==DUCT_FLAG_PLATFORM_LINUX) @endcode
*/
#define DUCT_PLATFORM_SYSTEM_LINUX
/**
	Defined when:
	@code (DUCT_PLATFORM_SYSTEM==DUCT_FLAG_PLATFORM_MACOS) @endcode
*/
#define DUCT_PLATFORM_SYSTEM_MACOS

/**
	Platform processor model: 32-bit (x86) or 64-bit (x86-64).
	Equals one of:
	-# #DUCT_FLAG_PLATFORM_MODEL_32 (x86 processor)
	-# #DUCT_FLAG_PLATFORM_MODEL_64 (x86-64 processor)
*/
#define DUCT_PLATFORM_MODEL system_dependent
/**
	Defined when the platform is 32-bit:
	@code (DUCT_PLATFORM_MODEL==DUCT_FLAG_PLATFORM_MODEL_32) @endcode
*/
#define DUCT_PLATFORM_MODEL_32
/**
	Defined when the platform is 64-bit:
	@code (DUCT_PLATFORM_MODEL==DUCT_FLAG_PLATFORM_MODEL_64) @endcode
*/
#define DUCT_PLATFORM_MODEL_64

// Compiler
/**
	Compiler.
	See <duct/config.hpp> for possible values.
*/
#define DUCT_COMPILER system_dependent

/** @} */ // end of name-group Platform

/**
	@name Endian
	@{
*/

/**
	Ensures bswap_16 is defined (Linux @c <byteswap.h>).
	@sa bswap_32(), bswap_64()
*/
#define bswap_16(x)
/**
	Ensures bswap_32 is defined (Linux @c <byteswap.h>).
	@sa bswap_16(), bswap_64()
*/
#define bswap_32(x)
/**
	Ensures bswap_64 is defined (Linux @c <byteswap.h>).
	@sa bswap_16(), bswap_32()
*/
#define bswap_64(x)

/**
	Little endian.
	@sa DUCT_BYTEORDER, DUCT_ENDIAN_BIG
*/
#define DUCT_ENDIAN_LITTLE	1234
/**
	Big endian.
	@sa DUCT_BYTEORDER, DUCT_ENDIAN_LITTLE
*/
#define DUCT_ENDIAN_BIG		4321

/**
	System byteorder.
	Equals one of:
	-# #DUCT_ENDIAN_LITTLE (little-endian architecture)
	-# #DUCT_ENDIAN_BIG (big-endian architecture)
*/
#define DUCT_BYTEORDER
/**
	Defined when: @code (DUCT_BYTEORDER==DUCT_ENDIAN_LITTLE) @endcode
	@sa DUCT_BYTEORDER_BE
*/
#define DUCT_BYTEORDER_LE
/**
	Defined when: @code (DUCT_BYTEORDER==DUCT_ENDIAN_BIG) @endcode
	@sa DUCT_BYTEORDER_LE
*/
#define DUCT_BYTEORDER_BE

/** @} */ // end of name-group Endian

/**
	@name Class utilities
	@deprecated These are deprecated. They have been replaced by explicit
	in-class exposition and base-class @ref restrictors "restrictors".
	Both macros currently use the @c =delete; notation instead of relying on
	private protection.
	@{
*/

/**
	Disallow copy and copy assignment for the given typename.
*/
#define DUCT_DISALLOW_COPY_AND_ASSIGN(TypeName)
/**
	Disallow all construction for the given typename.
*/
#define DUCT_DISALLOW_ALL_CONSTRUCTION(TypeName)

/** @} */ // end of name-group Class utilities

/**
	@name etc
	@{
*/

/**
	Allocator class for all auxiliary stdlib specializations.
	@note Defaults to @c std::allocator.
*/
#define DUCT_CONFIG_ALLOCATOR

/**
	Detailed function signature.
	Alias to either:
	-# @c __FUNCSIG__ (Windows)
	-# @c __PRETTY_FUNCTION__ (UNIX-based)
*/
#define DUCT_FUNC_SIG system_dependent
/**
	Function name.
	Alias to @c __FUNCTION__.
*/
#define DUCT_FUNC_NAME __FUNCTION__

/** @} */ // end of name-group etc

#else // Quick, actually define everything while Doxygen isn't looking!

// Platform
#define DUCT_FLAG_PLATFORM_WINDOWS		0x00000001
#define DUCT_FLAG_PLATFORM_LINUX		0x00000002
#define DUCT_FLAG_PLATFORM_MACOS		0x00000004

#define DUCT_FLAG_PLATFORM_MODEL_32		0x00000100
#define DUCT_FLAG_PLATFORM_MODEL_64		0x00000200

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
	#define DUCT_PLATFORM_SYSTEM DUCT_FLAG_PLATFORM_WINDOWS
#elif defined(__linux) || defined(linux)
	#define DUCT_PLATFORM_SYSTEM DUCT_FLAG_PLATFORM_LINUX
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)
	#define DUCT_PLATFORM_SYSTEM DUCT_FLAG_PLATFORM_MACOS
#else
	#error "Unsupported or unrecognized operating system"
#endif

#if (DUCT_PLATFORM_SYSTEM==DUCT_FLAG_PLATFORM_WINDOWS)
	#define DUCT_PLATFORM_SYSTEM_WINDOWS
#elif (DUCT_PLATFORM_SYSTEM==DUCT_FLAG_PLATFORM_LINUX)
	#define DUCT_PLATFORM_SYSTEM_LINUX
#elif (DUCT_PLATFORM_SYSTEM==DUCT_FLAG_PLATFORM_MACOS)
	#define DUCT_PLATFORM_SYSTEM_MACOS
#endif

#if (defined(__WORDSIZE) && (__WORDSIZE == 64)) \
	|| defined(__arch64__) || defined(__LP64__) \
	|| defined(_M_X64) || defined(__ppc64__) \
	|| defined(__x86_64__)
	#define DUCT_PLATFORM_MODEL DUCT_FLAG_PLATFORM_MODEL_64
	#define DUCT_PLATFORM_MODEL_64
#elif (defined(__i386__) || defined(__ppc__))
	#define DUCT_PLATFORM_MODEL DUCT_FLAG_PLATFORM_MODEL_32
	#define DUCT_PLATFORM_MODEL_32
#else
	#error "Unable to determine platform bitness"
#endif

#define DUCT_PLATFORM	DUCT_PLATFORM_SYSTEM | DUCT_PLATFORM_MODEL

// Compiler
// This stuff was gracefully swiped from GLM (http://glm.g-truc.net)

// Visual C++
#define DUCT_FLAG_COMPILER_VC			0x01000000
#define DUCT_FLAG_COMPILER_VC2			0x01000010
#define DUCT_FLAG_COMPILER_VC4			0x01000020
#define DUCT_FLAG_COMPILER_VC5			0x01000030
#define DUCT_FLAG_COMPILER_VC6			0x01000040
#define DUCT_FLAG_COMPILER_VC2002		0x01000050
#define DUCT_FLAG_COMPILER_VC2003		0x01000060
#define DUCT_FLAG_COMPILER_VC2005		0x01000070
#define DUCT_FLAG_COMPILER_VC2008		0x01000080
#define DUCT_FLAG_COMPILER_VC2010		0x01000090
#define DUCT_FLAG_COMPILER_VC2011		0x010000A0

// GCC
#define DUCT_FLAG_COMPILER_GCC			0x02000000
#define DUCT_FLAG_COMPILER_GCC_LLVM		0x02000001
#define DUCT_FLAG_COMPILER_GCC_CLANG	0x02000002
#define DUCT_FLAG_COMPILER_GCC30		0x02000010
#define DUCT_FLAG_COMPILER_GCC31		0x02000020
#define DUCT_FLAG_COMPILER_GCC32		0x02000030
#define DUCT_FLAG_COMPILER_GCC33		0x02000040
#define DUCT_FLAG_COMPILER_GCC34		0x02000050
#define DUCT_FLAG_COMPILER_GCC35		0x02000060
#define DUCT_FLAG_COMPILER_GCC40		0x02000070
#define DUCT_FLAG_COMPILER_GCC41		0x02000080
#define DUCT_FLAG_COMPILER_GCC42		0x02000090
#define DUCT_FLAG_COMPILER_GCC43		0x020000A0
#define DUCT_FLAG_COMPILER_GCC44		0x020000B0
#define DUCT_FLAG_COMPILER_GCC45		0x020000C0
#define DUCT_FLAG_COMPILER_GCC46		0x020000D0
#define DUCT_FLAG_COMPILER_GCC47		0x020000E0
#define DUCT_FLAG_COMPILER_GCC48		0x020000F0
#define DUCT_FLAG_COMPILER_GCC49		0x02000100
#define DUCT_FLAG_COMPILER_GCC50		0x02000200

// Clang
#define DUCT_FLAG_COMPILER_CLANG		0x30000000
#define DUCT_FLAG_COMPILER_CLANG26		0x30000010
#define DUCT_FLAG_COMPILER_CLANG27		0x30000020
#define DUCT_FLAG_COMPILER_CLANG28		0x30000030
#define DUCT_FLAG_COMPILER_CLANG29		0x30000040
#define DUCT_FLAG_COMPILER_CLANG30		0x30000050
#define DUCT_FLAG_COMPILER_CLANG31		0x30000060
#define DUCT_FLAG_COMPILER_CLANG32		0x30000070
#define DUCT_FLAG_COMPILER_CLANG33		0x30000080

// LLVM GCC
#define DUCT_FLAG_COMPILER_LLVM_GCC		0x40000000

#ifdef DUCT_COMPILER
// Visual C++
#elif defined(_MSC_VER)
	#if _MSC_VER == 900
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_VC2
	#elif _MSC_VER == 1000
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_VC4
	#elif _MSC_VER == 1100
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_VC5
	#elif _MSC_VER == 1200
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_VC6
	#elif _MSC_VER == 1300
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_VC2002
	#elif _MSC_VER == 1310
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_VC2003
	#elif _MSC_VER == 1400
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_VC2005
	#elif _MSC_VER == 1500
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_VC2008
	#elif _MSC_VER == 1600
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_VC2010
	#elif _MSC_VER == 1700
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_VC2011
	#else//_MSC_VER
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_VC
	#endif//_MSC_VER

// Clang
#elif defined(__clang__)
	#if(__clang_major__ == 2) && (__clang_minor__ == 6)
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_CLANG26
	#elif(__clang_major__ == 2) && (__clang_minor__ == 7)
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_CLANG27
	#elif(__clang_major__ == 2) && (__clang_minor__ == 8)
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_CLANG28
	#elif(__clang_major__ == 2) && (__clang_minor__ == 9)
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_CLANG29
	#elif(__clang_major__ == 3) && (__clang_minor__ == 0)
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_CLANG30
	#elif(__clang_major__ == 3) && (__clang_minor__ == 1)
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_CLANG31
	#elif(__clang_major__ == 3) && (__clang_minor__ == 2)
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_CLANG32
	#elif(__clang_major__ == 3) && (__clang_minor__ == 3)
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_CLANG33
	#else
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_CLANG
	#endif

// G++
#elif defined(__GNUC__) || defined(__MINGW32__)
	#if defined (__llvm__)
		#define DUCT_GCC_FLAG_EXTRA__ DUCT_FLAG_COMPILER_GCC_LLVM
	#elif defined (__clang__)
		#define DUCT_GCC_FLAG_EXTRA__ DUCT_FLAG_COMPILER_GCC_CLANG
	#else
		#define DUCT_GCC_FLAG_EXTRA__ 0
	#endif

	#if   (__GNUC__ == 3) && (__GNUC_MINOR__ == 2)
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_GCC32
	#elif (__GNUC__ == 3) && (__GNUC_MINOR__ == 3)
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_GCC33
	#elif (__GNUC__ == 3) && (__GNUC_MINOR__ == 4)
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_GCC34
	#elif (__GNUC__ == 3) && (__GNUC_MINOR__ == 5)
		#define DUCT_COMPILER DUCT_FLAG_COMPILER_GCC35
	#elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 0)
		#define DUCT_COMPILER (DUCT_FLAG_COMPILER_GCC40 | DUCT_GCC_FLAG_EXTRA__)
	#elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 1)
		#define DUCT_COMPILER (DUCT_FLAG_COMPILER_GCC41 | DUCT_GCC_FLAG_EXTRA__)
	#elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 2)
		#define DUCT_COMPILER (DUCT_FLAG_COMPILER_GCC42 | DUCT_GCC_FLAG_EXTRA__)
	#elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 3)
		#define DUCT_COMPILER (DUCT_FLAG_COMPILER_GCC43 | DUCT_GCC_FLAG_EXTRA__)
	#elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 4)
		#define DUCT_COMPILER (DUCT_FLAG_COMPILER_GCC44 | DUCT_GCC_FLAG_EXTRA__)
	#elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 5)
		#define DUCT_COMPILER (DUCT_FLAG_COMPILER_GCC45 | DUCT_GCC_FLAG_EXTRA__)
	#elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 6)
		#define DUCT_COMPILER (DUCT_FLAG_COMPILER_GCC46 | DUCT_GCC_FLAG_EXTRA__)
	#elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 7)
		#define DUCT_COMPILER (DUCT_FLAG_COMPILER_GCC47 | DUCT_GCC_FLAG_EXTRA__)
	#elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 8)
		#define DUCT_COMPILER (DUCT_FLAG_COMPILER_GCC48 | DUCT_GCC_FLAG_EXTRA__)
	#elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 9)
		#define DUCT_COMPILER (DUCT_FLAG_COMPILER_GCC49 | DUCT_GCC_FLAG_EXTRA__)
	#elif (__GNUC__ == 5) && (__GNUC_MINOR__ == 0)
		#define DUCT_COMPILER (DUCT_FLAG_COMPILER_GCC50 | DUCT_GCC_FLAG_EXTRA__)
	#else
		#define DUCT_COMPILER (DUCT_FLAG_COMPILER_GCC | DUCT_GCC_FLAG_EXTRA__)
	#endif
#endif // compiler tree

#ifndef DUCT_COMPILER
	#error "Compiler unrecognized -- it might not be supported by duct++"
#endif

// Endian
#ifdef DUCT_PLATFORM_SYSTEM_LINUX
	#include <byteswap.h>
#endif

#ifndef bswap_16
#define bswap_16(x)	\
	((((x)>>8)&0xFFu)|(((x)&0xFFu)<<8))
#endif

#ifndef bswap_32
#define bswap_32(x)	\
	((((x)&0xFF000000u)>>24) | (((x)&0x00FF0000u)>>8 ) | \
	 (((x)&0x0000FF00u)<<8 ) | (((x)&0x000000FFu)<<24))
#endif

#ifndef bswap_64
#define bswap_64(x)	\
	((((x)&0xFF00000000000000ull)>>56) | (((x)&0x00FF000000000000ull)>>40) | \
	 (((x)&0x0000FF0000000000ull)>>24) | (((x)&0x000000FF00000000ull)>> 8) | \
	 (((x)&0x00000000FF000000ull)<< 8) | (((x)&0x0000000000FF0000ull)<<24) | \
	 (((x)&0x000000000000FF00ull)<<40) | (((x)&0x00000000000000FFull)<<56))
#endif

/*
	This byte order stuff was lifted from PhysFS, which was lifted from SDL.
	http://www.libsdl.org
*/
#define DUCT_ENDIAN_LITTLE	1234
#define DUCT_ENDIAN_BIG		4321

#if	defined(__i386__) || defined(__ia64__) || defined(_WIN32) || \
	(defined(__alpha__) || defined(__alpha)) || \
	defined(__arm__) || defined(ARM) || \
	(defined(__mips__) && defined(__MIPSEL__)) || \
	defined(__SYMBIAN32__) || \
	defined(__x86_64__) || \
	defined(__LITTLE_ENDIAN__)
	
	#define DUCT_BYTEORDER	DUCT_ENDIAN_LITTLE
	#define DUCT_BYTEORDER_LE
#else
	#define DUCT_BYTEORDER	DUCT_ENDIAN_BIG
	#define DUCT_BYTEORDER_BE
#endif

// Class utilities
#define DUCT_DISALLOW_COPY_AND_ASSIGN(TypeName)	\
	TypeName(TypeName const&)=delete;			\
	TypeName& operator=(TypeName const&)=delete
// -

#define DUCT_DISALLOW_ALL_CONSTRUCTION(TypeName)\
	TypeName()=delete;							\
	TypeName(TypeName const&)=delete;			\
	TypeName(TypeName&&)=delete;				\
	~TypeName()=delete;							\
	TypeName& operator=(TypeName const&)=delete;\
	TypeName& operator=(TypeName&&)=delete
// -

#ifndef DUCT_CONFIG_ALLOCATOR
	#define DUCT_CONFIG_ALLOCATOR std::allocator
#endif

// etc
#ifdef DUCT_PLATFORM_SYSTEM_WINDOWS
	#define DUCT_FUNC_SIG __FUNCSIG__
#else
	#define DUCT_FUNC_SIG __PRETTY_FUNCTION__
#endif

#define DUCT_FUNC_NAME __FUNCTION__

#endif // DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/** @} */ // end of doc-group config

#endif // DUCT_CONFIG_HPP_
