/**
@file Var.hpp
@brief Var class.

@author Timothy Howard
@copyright 2010-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include "./config.hpp"
#include "./aux.hpp"
#include "./string.hpp"
#include "./debug.hpp"
#include "./utility.hpp"
#include "./VarType.hpp"
#include "./detail/var.hpp"
#include "./StringUtils.hpp"

#include <utility>

namespace duct {

// Forward declarations
class Var;

/**
	@addtogroup var
	@{
*/

/**
	Multi-type variable.

	@warning This class is relatively heavy since it serves the
	purposes of every variable type — use it wisely.
*/
class Var final {
public:
/** @name Types */ /// @{
	/** Children vector. */
	using vector_type = aux::vector<Var>;
	/** Children iterator.  */
	using iterator = vector_type::iterator;
	/** @copydoc iterator  */
	using const_iterator = vector_type::const_iterator;
	/** Initializer list. */
	using initializer_list_type = std::initializer_list<Var>;
/// @}

private:
	VarType m_type{VarType::null};
	detail::var_config::name_type m_name{};
	vector_type m_children{};
	detail::var_config::string_type m_strv{};

	// NB: union is zero-initialized.
	// GCC 4.7.0 (and probably other versions) has a defect which does
	// not allow a ctor to be `=default` when a union member is
	// aggregate-intialized.
	union {
		detail::var_config::int_type m_intv;
		detail::var_config::float_type m_floatv;
		detail::var_config::bool_type m_boolv;
	};

public:
/** @name Constructors and operators */ /// @{
	/**
		Construct nameless @c VarType::null.
	*/
	Var() = default;

	/**
		Construct named with type (default value).

		@param name Name.
		@param type Type.
	*/
	Var(
		detail::var_config::name_type name,
		VarType const type
	)
		: m_type(type)
		, m_name(std::move(name))
	{}

	/**
		Construct nameless with type (default value).

		@param type Type.
	*/
	explicit
	Var(
		VarType const type
	)
		: m_type(type)
	{}

	/**
		Construct nameless with collection type and children.

		@warning An assertion will fail if @a type is not a
		collection type.
		@warning In ductScript, identifiers must have a name.

		@param type Type.
		@param ilist Initializer list.
	*/
	Var(
		VarType const type,
		initializer_list_type ilist
	)
		: m_type(type)
		, m_children(std::move(ilist))
	{
		DUCT_ASSERTE(is_type_of(VarMask::collection));
	}

	/**
		Construct named with collection type and children.

		@warning An assertion will fail if @a type is not a
		collection type.
		@warning In ductScript, identifiers must have a name.

		@param name Name.
		@param type Type.
		@param ilist Initializer list.
	*/
	Var(
		detail::var_config::name_type name,
		VarType const type,
		initializer_list_type ilist
	)
		: m_type(type)
		, m_name(std::move(name))
		, m_children(std::move(ilist))
	{
		DUCT_ASSERTE(is_type_of(VarMask::collection));
	}

	/**
		Construct named @c VarType::string with value.

		@param name Name.
		@param value Value.
	*/
	Var(
		detail::var_config::name_type name,
		detail::var_config::string_type value
	)
		: m_type(VarType::string)
		, m_name(std::move(name))
		, m_strv(std::move(value))
	{}

	/**
		Construct nameless @c VarType::string with value.

		@param value Value.
	*/
	explicit
	Var(
		detail::var_config::string_type value
	)
		: m_type(VarType::string)
		, m_strv(std::move(value))
	{}

	/**
		Construct named @c VarType::integer with value.

		@param name Name.
		@param value Value.
	*/
	Var(
		detail::var_config::name_type name,
		detail::var_config::int_type const value
	)
		: m_type(VarType::integer)
		, m_name(std::move(name))
		, m_intv(value)
	{}

	/**
		Construct nameless @c VarType::integer with value.

		@param value Value.
	*/
	explicit
	Var(
		detail::var_config::int_type const value
	)
		: m_type(VarType::integer)
		, m_intv(value)
	{}

	/**
		Construct named @c VarType::floatp with value.

		@param name Name.
		@param value Value.
	*/
	Var(
		detail::var_config::name_type name,
		detail::var_config::float_type const value
	)
		: m_type(VarType::floatp)
		, m_name(std::move(name))
		, m_floatv(value)
	{}
	/**
		Construct nameless @c VarType::floatp with value.

		@param value Value.
	*/
	explicit
	Var(
		detail::var_config::float_type const value
	)
		: m_type(VarType::floatp)
		, m_floatv(value)
	{}

	/**
		Construct named @c VarType::boolean with value.

		@param name Name.
		@param value Value.
	*/
	Var(
		detail::var_config::name_type name,
		detail::var_config::bool_type const value
	)
		: m_type(VarType::boolean)
		, m_name(std::move(name))
		, m_boolv(value)
	{}

	/**
		Construct nameless @c VarType::boolean with value.

		@param value Value.
	*/
	explicit
	Var(
		detail::var_config::bool_type const value
	)
		: m_type(VarType::boolean)
		, m_boolv(value)
	{}

	/** Copy constructor. */
	Var(Var const&) = default;
	/** Move constructor. */
	Var(Var&&) = default;
	/** Destructor. */
	~Var() = default;
/// @}

/** @name Operators */ /// @{
	/**
		Copy assignment operator.

		@note #reset() is called if @a other's type and the current
		type are both unequal and not collections.

		@returns @c *this.
		@param other Var to copy.
	*/
	Var&
	operator=(
		Var const& other
	) {
		if (other.m_type != m_type
		&& !( var_type_is_of(m_type, VarMask::collection)
		   && var_type_is_of(other.m_type, VarMask::collection))
		) {
			reset();
		}
		m_type = other.m_type;
		m_name = other.m_name;
		switch (other.m_type) {
		case VarType::string: m_strv = other.m_strv; break;
		case VarType::integer: m_intv = other.m_intv; break;
		case VarType::floatp: m_floatv = other.m_floatv; break;
		case VarType::boolean: m_boolv = other.m_boolv; break;

		case VarType::array:
		case VarType::node:
		case VarType::identifier:
			m_children.assign(
				other.m_children.cbegin(),
				other.m_children.cend()
			);
			break;

		case VarType::null:
			break;
		}
		return *this;
	}
	/** Move assignment operator. */
	Var& operator=(Var&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Get type.

		@returns The current type.
		@sa morph(T const)
	*/
	VarType
	get_type() const noexcept {
		return m_type;
	}

	/**
		Set name.

		@returns @c *this.
		@param name New name.
	*/
	Var&
	set_name(
		detail::var_config::name_type name
	) {
		m_name.assign(std::move(name));
		return *this;
	}

	/**
		Get name.

		@returns The current name.
	*/
	detail::var_config::name_type const&
	get_name() const noexcept {
		return m_name;
	}

	/**
		Test type.

		@returns <code>get_type() == type</code>.
		@param type Type to test against.
	*/
	bool
	is_type(
		VarType const type
	) const noexcept {
		return type == m_type;
	}

	/**
		Test type against a mask.

		@returns @c true if the variable's type is of @a mask.
		@param mask Type mask to test against.
	*/
	bool
	is_type_of(
		VarMask const mask
	) const noexcept {
		return var_type_is_of(m_type, mask);
	}

	/**
		Check if the variable is null.

		@returns @c is_type(VarType::null).
	*/
	bool
	is_null() const noexcept {
		return is_type(VarType::null);
	}
/// @}

/** @name Operations */ /// @{
	/**
		Change type to @c VarType::null.

		Equivalent to @c morph(VarType::null).

		@returns @c *this.
	*/
	Var&
	nullify() noexcept {
		return morph(VarType::null);
	}

	/**
		Reset @c VarMask::value types to default value; clear children
		for @c VarMask::collection types.

		@note Does nothing when <code>is_null() == true</code>.

		@returns @c *this.
	*/
	Var&
	reset() noexcept {
		switch (m_type) {
		case VarType::string: m_strv.clear(); break;
		case VarType::integer: m_intv = 0; break;
		case VarType::floatp: m_floatv = 0.0f; break;
		case VarType::boolean: m_boolv = false; break;
		case VarType::array:
		case VarType::node:
		case VarType::identifier:
			m_children.clear();
			break;
		case VarType::null:
		default:
			break;
		}
		return *this;
	}

	/**
		Change type.

		@returns @c *this.
		@param type New type.
		@param discard_children Whether to discard children (#reset())
		when changing between @c VarMask::collection types; @c false
		by default.
	*/
	Var&
	morph(
		VarType const type,
		bool const discard_children = false
	) noexcept {
		if (type != m_type) {
			if (
				discard_children
			|| !( var_type_is_of(m_type, VarMask::collection)
			   && var_type_is_of(type, VarMask::collection))
			) {
				reset();
			}
			m_type = type;
		}
		return *this;
	}

	/**
		Change type to collection and set children.

		@note #reset() is called when the type is changed
		<em>only</em> if the current type is not a collection.

		@warning An assertion will fail if @a type is
		not a @c VarMask::collection.

		@returns @c *this.
		@param type New collection type.
		@param children New child collection.
	*/
	Var&
	morph(
		VarType const type,
		vector_type children
	) {
		DUCT_ASSERTE(var_type_is_of(type, VarMask::collection));
		if (type != m_type) {
			if (!var_type_is_of(m_type, VarMask::collection)) {
				reset();
			}
			m_type = type;
		}
		set_children(std::move(children));
		return *this;
	}

	/**
		Change type and set value.

		@note #reset() is called if the type is changed.

		@returns @c *this.
		@tparam T Value type; inferred from @a value.
		@param value New value; the new variable type is inferred.
	*/
	template<
		typename T
	>
	Var&
	morph(
		T const value
	) {
		static_assert(
			true == detail::is_valtype<T>::value,
			"T does not have a corresponding VarType"
		);
		static constexpr VarType const
			type = detail::type_to_valtype<T>::value;
		if (type != m_type) {
			reset();
			m_type = type;
		}
		assign(value);
		return *this;
	}

	/**
		Change type and set string value.

		@note #reset() is called if the type is changed.

		@returns @c *this.
		@param value New string value.
	*/
	Var&
	morph(
		detail::var_config::string_type value
	) {
		if (VarType::string != m_type) {
			reset();
			m_type = VarType::string;
		}
		assign(std::move(value));
		return *this;
	}
/// @}

/** @name Comparison */ /// @{
	/**
		Name and value equality to another variable.

		@returns @c true if name and value
		of @c this and name and value of @a other are equivalent.
		@param other Var to compare against.
	*/
	bool
	equals(
		Var const& other
	) const {
		return 0 == compare(other);
	}

	/**
		Value equality to another variable.

		@returns @c true if value of @c this and value
		of @a other are equivalent.
		@param other Var to compare against.
	*/
	bool
	equals_value(
		Var const& other
	) const {
		return 0 == compare_value(other);
	}

	/**
		Name and value comparison to another variable.

		@returns
		- @c <0 if name of @c this is less than name of @a other; or
		- @c >0 if name of @c this is greater than name of @a other;
		  or
		- the result of @c this->compare_value(other) if their names
		  are equal.
		@param other Var to compare against.
	*/
	signed
	compare(
		Var const& other
	) const {
		signed const diff = m_name.compare(other.m_name);
		return (0 != diff)
			? diff
			: compare_value(other);
	}

	/**
		Value comparison to another variable.

		@remarks Two empty collections of the same type will be equal.

		@returns

		For @c VarMask::value types:
		- @c <0 if value of @c this is less than value of @a other; or
		- @c 0 if value of @c this is equal to value of @a other; or
		- @c >0 if value of @c this is greater than value of @a other.

		For @c VarMask::collection types
		- of equal size:
			- @c 0 if their children are equal; or
			- @c compare_value() of the first non-equal pair.
		- of unequal size:
			- the difference between the size of @c this
			  and the size of @a other.

		For unequal types, the difference between the type
		of @c this and the type of @a other.
		@param other Var to compare against.
	*/
	signed
	compare_value(
		Var const& other
	) const {
		switch (enum_combine(m_type, other.m_type)) {
		case VarType::null: return 0;
		case VarType::string: return m_strv.compare(other.m_strv);
		case VarType::integer: return m_intv - other.m_intv;
		case VarType::floatp:
			return
				  (m_floatv > other.m_floatv)
					?  1
				: (m_floatv < other.m_floatv)
					? -1
				:  0
			;
		case VarType::boolean: return m_boolv - other.m_boolv;
		case VarType::array:
		case VarType::node:
		case VarType::identifier: {
			auto const sdiff = size() - other.size();
			if (0 != sdiff || 0 == size()) { // If not equivalent or both 0
				return sdiff;
			} else { // If equivalent and not 0
				signed vdiff;
				auto oi = other.m_children.cbegin();
				for (auto const& tv : m_children) {
					vdiff = tv.compare_value(*oi);
					if (0 != vdiff) {
						return vdiff;
					}
					++oi;
				}
				return 0; // All children equivalent
			}
		}
		// Bitwise OR of both types equals not a single type,
		// therefore: types are unequal
		default:
			return
				signed_cast(enum_cast(m_type)) -
				signed_cast(enum_cast(other.m_type))
			;
		}
	}
/// @}

/**
	@name Value set/get

	@warning All <em>value</em> get/set functions (except
	for @c get_as_str() — it has a different rule) are type-strict;
	an assertion will fail if @c get_type() does not equal:

	-# the type of assignment; or
	-# the type of retrieval.
	@{
*/
	/** @cond */
	#define DUCT_V_set_value_()			\
		DUCT_ASSERTE(DUCT_V_TYPE_ == m_type);	\
		DUCT_V_FIELD_ = value;			\
		return *this;
	#define DUCT_V_get_value_()			\
		DUCT_ASSERTE(DUCT_V_TYPE_ == m_type);	\
		return DUCT_V_FIELD_

	#define DUCT_V_TYPE_ VarType::string
	#define DUCT_V_FIELD_ m_strv
	/** @endcond */

	/**
		Set value.

		@returns @c *this.
		@param value New value.
		@sa morph(T const)
	*/
	Var&
	assign(
		detail::var_config::string_type value
	) {
		DUCT_ASSERTE(DUCT_V_TYPE_ == m_type);
		DUCT_V_FIELD_.assign(std::move(value));
		return *this;
	}

	/**
		Get string value.
		@returns The current string value.
	*/
	detail::var_config::string_type
	get_string() const {
		DUCT_V_get_value_();
	}

	/** @copydoc get_string() */
	detail::var_config::string_type&
	get_string_ref() noexcept {
		DUCT_V_get_value_();
	}

	/** @copydoc get_string() */
	detail::var_config::string_type const&
	get_string_ref() const noexcept {
		DUCT_V_get_value_();
	}

	/** @cond */
	#undef DUCT_V_TYPE_
	#undef DUCT_V_FIELD_
	#define DUCT_V_TYPE_ VarType::integer
	#define DUCT_V_FIELD_ m_intv
	/** @endcond */

	/** @copydoc Var::assign(detail::var_config::string_type) */
	Var&
	assign(
		detail::var_config::int_type const value
	) noexcept {
		DUCT_V_set_value_();
	}

	/**
		Get integer value.

		@returns The current integer value.
	*/
	detail::var_config::int_type
	get_int() const noexcept {
		DUCT_V_get_value_();
	}

	/** @copydoc get_int() */
	detail::var_config::int_type&
	get_int_ref() noexcept {
		DUCT_V_get_value_();
	}

	/** @cond */
	#undef DUCT_V_TYPE_
	#undef DUCT_V_FIELD_
	#define DUCT_V_TYPE_ VarType::floatp
	#define DUCT_V_FIELD_ m_floatv
	/** @endcond */

	/** @copydoc Var::assign(detail::var_config::string_type) */
	Var&
	assign(
		detail::var_config::float_type const value
	) noexcept {
		DUCT_V_set_value_();
	}

	/**
		Get floating-point value.

		@returns The current floating-point value.
	*/
	detail::var_config::float_type
	get_float() const noexcept {
		DUCT_V_get_value_();
	}

	/** @copydoc get_float() */
	detail::var_config::float_type&
	get_float_ref() noexcept {
		DUCT_V_get_value_();
	}

	/** @cond */
	#undef DUCT_V_TYPE_
	#undef DUCT_V_FIELD_
	#define DUCT_V_TYPE_ VarType::boolean
	#define DUCT_V_FIELD_ m_boolv
	/** @endcond */

	/** @copydoc Var::assign(detail::var_config::string_type) */
	Var&
	assign(
		detail::var_config::bool_type const value
	) noexcept {
		DUCT_V_set_value_();
	}

	/**
		Get boolean value.

		@returns The current boolean value.
	*/
	detail::var_config::bool_type
	get_bool() const noexcept {
		DUCT_V_get_value_();
	}

	/** @copydoc get_bool() */
	detail::var_config::bool_type&
	get_bool_ref() noexcept  {
		DUCT_V_get_value_();
	}

	/** @cond */
	#undef DUCT_V_TYPE_
	#undef DUCT_V_FIELD_
	#undef DUCT_V_get_value_
	#undef DUCT_V_set_value_
	/** @endcond */

	/**
		Get value as string.

		@returns The current value as a string.
		@sa get_as_str(StringT&) const
	*/
	detail::var_config::string_type
	get_as_str() const {
		return get_as_str<detail::var_config::string_type>();
	}

	/**
		Get value as string.

		@returns The current value as a string.
		@tparam StringT String type to convert to.
		@sa get_as_str(StringT&) const
	*/
	template<
		class StringT
	>
	StringT
	get_as_str() const {
		StringT str;
		get_as_str(str);
		return str;
	}

	/**
		Get value as string with output parameter.

		@warning An assertion will fail if the variable is not
		a @c VarMask::value type nor a @c VarType::null.

		@note
		- A @c VarType::null will result in @c "null".
		- A @c VarType::boolean will result in either @c "false"
		  or @c "true".

		@tparam StringT String type to convert to; inferred
		from @a out_str.
		@param[out] out_str Output value.
	*/
	template<
		class StringT
	>
	void
	get_as_str(
		StringT& out_str
	) const {
		DUCT_ASSERTE(enum_cast(m_type) & enum_cast(VarMask::value_nullable));
		switch (m_type) {
		case VarType::null: out_str = "null"; break;
		case VarType::string: StringUtils::convert(out_str, m_strv); break;
		case VarType::boolean: out_str = (m_boolv) ? "true" : "false"; break;
		default: {
			aux::stringstream stream;
			switch (m_type) {
			case VarType::integer: stream << m_intv; break;
			case VarType::floatp: stream << std::showpoint << m_floatv; break;
			default: return;
			}
			u8string temp;
			stream >> temp;
			StringUtils::convert(out_str, temp);
		}}
	}
/** @} */

/**
	@name Child properties and insertion

	@warning An assertion will fail for all functions defined in this
	group if <code>!is_type_of(VarMask::collection)</code>.
	@{
*/
	/**
		Get beginning child iterator.
	*/
	iterator
	begin() noexcept {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.begin();
	}

	/**
		Get beginning child iterator.
	*/
	const_iterator
	begin() const noexcept {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.cbegin();
	}

	/** @copydoc begin() */
	const_iterator
	cbegin() const noexcept {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.cbegin();
	}

	/**
		Get ending child iterator.
	*/
	iterator
	end() noexcept {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.end();
	}

	/**
		Get ending child iterator.
	*/
	const_iterator
	end() const noexcept {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.cend();
	}

	/** @copydoc end() */
	const_iterator
	cend() const noexcept {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.cend();
	}

	/**
		Check if the child collection is empty.

		@returns @c true if the child collection is empty.
	*/
	bool
	empty() const noexcept {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.empty();
	}

	/**
		Get number of children.

		@returns The current number of children.
	*/
	vector_type::size_type
	size() const noexcept {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.size();
	}

	/**
		Set children.

		@param new_children New child collection.
	*/
	void
	set_children(
		vector_type new_children
	) {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		m_children = std::move(new_children);
	}

	/**
		Get children.

		@returns The current child collection.
	*/
	vector_type&
	get_children() noexcept {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children;
	}

	/** @copydoc get_children() */
	vector_type const&
	get_children() const noexcept {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children;
	}

	/**
		Get first child.
	*/
	Var&
	front() {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.front();
	}

	/**
		Get first child.
	*/
	Var const&
	front() const {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.front();
	}

	/**
		Get last child.
	*/
	Var&
	back() {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.back();
	}

	/**
		Get last child.
	*/
	Var const&
	back() const {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.back();
	}

	/**
		Get child at index.

		@param index Child index.
	*/
	Var&
	at(
		std::size_t const index
	) {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.at(index);
	}

	/**
		Get child at index.

		@param index Child index.
	*/
	Var const&
	at(
		std::size_t const index
	) const {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		return m_children.at(index);
	}

	/**
		Erase child.

		@param pos Iterator to child.
	*/
	void
	erase(
		iterator const pos
	) {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		m_children.erase(pos);
	}

	/**
		Erase child.

		@param pos Iterator to child.
	*/
	void
	erase(
		const_iterator const pos
	) {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		m_children.erase(pos);
	}

	/**
		Append to end of child collection.

		@returns @c *this.
		@param var Var to append.
	*/
	Var&
	push_back(
		Var var
	) {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		m_children.push_back(std::move(var));
		return *this;
	}

	/**
		Emplace to end of child collection.

		@returns @c *this.
		@tparam ArgP Parameter pack for constructor;
		see @c Var().
		@param args Parameter pack for constructor.
	*/
	template<
		typename... ArgP
	>
	Var&
	emplace_back(ArgP&&... args) {
		DUCT_ASSERTE(is_type_of(VarMask::collection));
		m_children.emplace_back(std::forward<ArgP>(args)...);
		return *this;
	}
/** @} */
};

/** @} */ // end doc-group var

} // namespace duct
