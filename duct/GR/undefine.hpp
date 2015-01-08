/**
@file
@brief Undefine all GR macros.
@details

This will also undefine the header guards for <duct/GR/common.hpp>,
<duct/GR/throw.hpp>, and <duct/GR/ceformat.hpp> (but not
<duct/GR/Error.hpp>) so that they can be reused within the same TU.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

/** @cond INTERNAL */

#undef DUCT_GR_COMMON_HPP_
#undef DUCT_GR_STR_LIT
#undef DUCT_GR_ROOT_NAMESPACE
#undef DUCT_GR_SCOPE_PREFIX
#undef DUCT_GR_SCOPE_CLASS
#undef DUCT_GR_SCOPE_FUNC
#undef DUCT_GR_SCOPE_CLASS_STR
#undef DUCT_GR_SCOPE_CLASS_STR_LIT
#undef DUCT_GR_SCOPE_FUNC_STR
#undef DUCT_GR_SCOPE_FUNC_STR_LIT
#undef DUCT_GR_SCOPE_FQN_STR
#undef DUCT_GR_SCOPE_FQN_STR_LIT
#undef DUCT_GR_MSG_IMPL_
#undef DUCT_GR_MSG
#undef DUCT_GR_MSG_CLASS
#undef DUCT_GR_MSG_FUNC
#undef DUCT_GR_MSG_FQN

#undef DUCT_GR_CEFORMAT_HPP_
#undef DUCT_GR_DEF_CEFMT_IMPL_
#undef DUCT_GR_THROW_CEFMT_IMPL_
#undef DUCT_GR_STR_LIT_CEFMT_
#undef DUCT_GR_MSG_CEFMT_IMPL_
#undef DUCT_GR_DEF_CEFMT
#undef DUCT_GR_DEF_CEFMT_CLASS
#undef DUCT_GR_DEF_CEFMT_FUNC
#undef DUCT_GR_DEF_CEFMT_FQN
#undef DUCT_GR_THROW_CEFMT

#undef DUCT_GR_THROW_HPP_
#undef DUCT_GR_THROW_IMPL_
#undef DUCT_GR_THROW_OSTR_IMPL_
#undef DUCT_GR_THROW
#undef DUCT_GR_THROW_OSTR
#undef DUCT_GR_THROW_CLASS
#undef DUCT_GR_THROW_FUNC
#undef DUCT_GR_THROW_FQN

/** @endcond */ // INTERNAL
