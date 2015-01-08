/**
@file
@brief Type traits and helpers.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include "./config.hpp"

namespace duct {
namespace traits {

// Forward declarations
struct restrict_all;
struct restrict_copy;

/**
	@addtogroup traits
	@{
*/
/**
	@addtogroup restrictors
	@{
*/

/**
	Complete construction restrictor.
*/
struct restrict_all {
/** @name Constructors and destructor */ /// @{
	~restrict_all() = delete;
	restrict_all() = delete;
	restrict_all(restrict_all const&) = delete;
	restrict_all(restrict_all&&) = delete;
/// @}

/** @name Operators */ /// @{
	restrict_all& operator=(restrict_all const&) = delete;
	restrict_all& operator=(restrict_all&&) = delete;
/// @}
};

/**
	Copy construction restrictor.
*/
struct restrict_copy {
/** @name Constructors and destructor */ /// @{
	~restrict_copy() = default;
	restrict_copy() = default;
	restrict_copy(restrict_copy const&) = delete;
	restrict_copy(restrict_copy&&) = default;
/// @}

/** @name Operators */ /// @{
	restrict_copy& operator=(restrict_copy const&) = delete;
	restrict_copy& operator=(restrict_copy&&) = default;
/// @}
};

/** @} */ // end of doc-group restrictors
/** @} */ // end of doc-group traits

} // namespace traits
} // namespace duct
