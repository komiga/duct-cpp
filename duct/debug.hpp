/**
@file
@brief Debug utilities.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include "./config.hpp"

#include <cstdlib>
#include <cstdio>

namespace duct {

/**
	@addtogroup debug
	@{
*/

#ifdef DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI
	/**
		@ingroup config

		When defined, force all #DUCT_DEBUG and #DUCT_DEBUG_ASSERT
		macros to be defined (regardless of @c NDEBUG presence).
	*/
	#define DUCT_CONFIG_FORCE_DEBUG_MACROS
#endif

/**
	@name Non-debug assertion

	@note These macros mimic @c assert(), and will @c std::abort()
	the program if @a expr evaluates to @c false.

	@note These macros are always defined.

	@sa DUCT_DEBUG_ASSERT(),
		DUCT_DEBUG_ASSERTE(),
		DUCT_DEBUG_ASSERTF(),
		DUCT_DEBUG_ASSERTP(),
		DUCT_DEBUG_ASSERTPF()
*/ /// @{
/**
	Assertion with message.
	@param expr Expression to evaluate.
	@param mesg Message.
*/
#define DUCT_ASSERT(expr, mesg) (						\
	(expr) ? static_cast<void>(0) : (					\
		std::fprintf(									\
			stderr, "assertion failure: " mesg			\
			"\n in %s:%d: %s: Assertion: `" #expr "`\n",\
			__FILE__, __LINE__, DUCT_FUNC_SIG			\
		),												\
		std::abort()									\
	)													\
)

/**
	Assertion with expression.
	@param expr Expression to evaluate.
*/
#define DUCT_ASSERTE(expr) (					\
	(expr) ? static_cast<void>(0) : (			\
		std::fprintf(							\
			stderr, "assertion failure "		\
			"in %s:%d: %s: `" #expr "`\n",		\
			__FILE__, __LINE__, DUCT_FUNC_SIG	\
		),										\
		std::abort()							\
	)											\
)

/**
	Assertion with formatted message.

	@param expr Expression to evaluate.
	@param format Format string.
	@param ... Format arguments.
*/
#define DUCT_ASSERTF(expr, format, ...) (					\
	(expr) ? static_cast<void>(0) : (						\
		std::fprintf(										\
			stderr, "assertion failure: " format			\
			"\n in %s:%d: %s: Assertion: `" #expr "`\n",	\
			__VA_ARGS__, __FILE__, __LINE__, DUCT_FUNC_SIG	\
		),													\
		std::abort()										\
	)														\
)

/**
	Assertion with pointer and message.

	@param expr Expression to evaluate.
	@param p Pointer.
	@param mesg Message.
*/
#define DUCT_ASSERTP(expr, p, mesg) \
	DUCT_ASSERTF(expr, "[%p] " mesg, static_cast<void const* const>(p))

/**
	Assertion with pointer and formatted message.

	@param expr Expression to evaluate.
	@param p Pointer.
	@param format Format string.
	@param ... Format arguments.
*/
#define DUCT_ASSERTPF(expr, p, format, ...)	\
	DUCT_ASSERTF(expr, "[%p] " format,		\
		static_cast<void const* const>(p),	\
		__VA_ARGS__							\
	)
/// @}

#if (defined(NDEBUG) && !defined(DUCT_CONFIG_FORCE_DEBUG_MACROS)) \
||	(defined(DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI))
/** @name Message */ /// @{
	/**
		Print debug message.

		@param mesg Debug message.
	*/
	#define DUCT_DEBUG(mesg)
	/**
		Print formatted debug message.

		@param format Format string.
		@param ... Format arguments.
	*/
	#define DUCT_DEBUGF(format, ...)
	/**
		Print debug message with no newline.

		@param mesg Debug message.
	*/
	#define DUCT_DEBUGN(mesg)
	/**
		Print formatted debug message with no newline.

		@param format Format string.
		@param ... Format arguments.
	*/
	#define DUCT_DEBUGNF(format, ...)
/// @}

/** @name Message with function signature */ /// @{
	/**
		Print debug message with function signature.

		@param mesg Debug message.
	*/
	#define DUCT_DEBUGC(mesg)
	/**
		Print formatted debug message with function signature.

		@param format Format string.
		@param ... Format arguments.
	*/
	#define DUCT_DEBUGCF(format, ...)
	/**
		Print debug message with no newline and function signature.

		@param mesg Debug message.
	*/
	#define DUCT_DEBUGNC(mesg)
	/**
		Print formatted debug message with no newline and function
		signature.

		@param format Format string.
		@param ... Format arguments.
	*/
	#define DUCT_DEBUGNCF(format, ...)
/// @}

/** @name Message with function signature and pointer */ /// @{
	/**
		Print debug message with function signature and pointer.

		@param mesg Debug message.
		@param p Pointer.
	*/
	#define DUCT_DEBUGCP(p, mesg)
	/**
		Print formatted debug message with function signature and
		pointer.

		@param p Pointer.
		@param format Format string.
		@param ... Format arguments.
	*/
	#define DUCT_DEBUGCPF(p, format, ...)
	/**
		Print debug message with no newline, function signature, and
		pointer.

		@param p Pointer.
		@param mesg Debug message.
	*/
	#define DUCT_DEBUGNCP(p, mesg)
	/**
		Print formatted debug message with no newline, function
		signature, and pointer.

		@param p Pointer.
		@param format Format string.
		@param ... Format arguments.
	*/
	#define DUCT_DEBUGNCPF(p, format, ...)
/// @}

/** @name Print function signature */ /// @{
	/**
		Print function signature.
	*/
	#define DUCT_DEBUG_CALLED()
	/**
		Print function signature with pointer.

		@param p Pointer.
	*/
	#define DUCT_DEBUG_CALLEDP(p)
/// @}

/**
	@name Debug assertion
	@note These route to the non-debug assertion macros.
	@sa DUCT_ASSERT(),
		DUCT_ASSERTE(),
		DUCT_ASSERTF(),
		DUCT_ASSERTP(),
		DUCT_ASSERTPF()
*/ /// @{
	/** @copydoc DUCT_ASSERT() */
	#define DUCT_DEBUG_ASSERT(expr, mesg)
	/** @copydoc DUCT_ASSERTE() */
	#define DUCT_DEBUG_ASSERTE(expr)
	/** @copydoc DUCT_ASSERTF() */
	#define DUCT_DEBUG_ASSERTF(expr, format, ...)
	/** @copydoc DUCT_ASSERTP() */
	#define DUCT_DEBUG_ASSERTP(expr, p, mesg)
	/** @copydoc DUCT_ASSERTPF() */
	#define DUCT_DEBUG_ASSERTPF(expr, p, format, ...)
/// @}

#else
	#define DUCT_DEBUG_PREFIX_ "debug: "

	// Debug
	#define DUCT_DEBUG(mesg) \
		std::printf(DUCT_DEBUG_PREFIX_ mesg "\n")

	#define DUCT_DEBUGF(format, ...) \
		std::printf(DUCT_DEBUG_PREFIX_ format "\n", __VA_ARGS__)

	// - no newline
	#define DUCT_DEBUGN(mesg) \
		std::printf(DUCT_DEBUG_PREFIX_ mesg)

	#define DUCT_DEBUGNF(format, ...) \
		std::printf(DUCT_DEBUG_PREFIX_ format, __VA_ARGS__)

	// - signature
	#define DUCT_DEBUGC(mesg) \
		DUCT_DEBUGF("in %s: " mesg, DUCT_FUNC_SIG)

	#define DUCT_DEBUGCF(format, ...)	\
		DUCT_DEBUGF("in %s: " format,	\
			DUCT_FUNC_SIG,				\
			__VA_ARGS__					\
		)

	// - signature and no newline
	#define DUCT_DEBUGNC(mesg) \
		DUCT_DEBUGNF("in %s: " mesg, DUCT_FUNC_SIG)
	
	#define DUCT_DEBUGNCF(format, ...)	\
		DUCT_DEBUGNF("in %s: " format,	\
			DUCT_FUNC_SIG,				\
			__VA_ARGS__					\
		)

	// - signature and pointer
	#define DUCT_DEBUGCP(p, mesg)				\
		DUCT_DEBUGF("[%p] in %s: " mesg,		\
			static_cast<void const* const>(p),	\
			DUCT_FUNC_SIG						\
		)

	#define DUCT_DEBUGCPF(p, format, ...)		\
		DUCT_DEBUGF("[%p] in %s: " format,		\
			static_cast<void const* const>(p),	\
			DUCT_FUNC_SIG,						\
			__VA_ARGS__							\
		)

	// - signature and pointer and no newline
	#define DUCT_DEBUGNCP(p, mesg)				\
		DUCT_DEBUGNF("[%p] in %s: " mesg,		\
			static_cast<void const* const>(p),	\
			DUCT_FUNC_SIG						\
		)

	#define DUCT_DEBUGNCPF(p, format, ...)		\
		DUCT_DEBUGNF("[%p] in %s: " format,		\
			static_cast<void const* const>(p),	\
			DUCT_FUNC_SIG,						\
			__VA_ARGS__							\
		)

	// Call
	#define DUCT_DEBUG_CALLED() \
		DUCT_DEBUGF("called: %s", DUCT_FUNC_SIG)

	#define DUCT_DEBUG_CALLEDP(p)				\
		DUCT_DEBUGF("called: [%p] %s",			\
			static_cast<void const* const>(p),	\
			DUCT_FUNC_SIG						\
		)

	// Assert
	#define DUCT_DEBUG_ASSERT(expr, mesg) \
		DUCT_ASSERT(expr, mesg)

	// Assert
	#define DUCT_DEBUG_ASSERTE(expr) \
		DUCT_ASSERTE(expr)

	#define DUCT_DEBUG_ASSERTF(expr, format, ...) \
		DUCT_ASSERTF(expr, format, __VA_ARGS__)

	// - pointer
	#define DUCT_DEBUG_ASSERTP(expr, p, mesg) \
		DUCT_ASSERTP(expr, p, mesg)

	#define DUCT_DEBUG_ASSERTPF(expr, p, format, ...) \
		DUCT_ASSERTPF(expr, p, format, __VA_ARGS__)

#endif

/** @} */ // end doc-group debug

} // namespace duct
