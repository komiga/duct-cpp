/**
@file Variable.hpp
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

Multi-type variable class.

@defgroup variable Variable constructs
*/

#ifndef DUCT_VARIABLE_HPP_
#define DUCT_VARIABLE_HPP_

#include <duct/config.hpp>
#include <duct/string.hpp>
#include <duct/StringUtils.hpp>
#include <duct/detail/vartype.hpp>

#include <sstream>
#include <vector>
#include <cassert>

namespace duct {

// Forward declarations
class Variable;

/**
	@addtogroup variable
	@{
*/

/**
	Multi-type variable.
	@warning This class is relatively heavy since it serves the purposes of every variable type — use it wisely.
*/
class Variable /*final*/ {
public:
	/** Children vector. */
	typedef std::vector<Variable> vector_type;
	/** Children iterator.  */
	typedef vector_type::iterator iterator;
	/** Children const iterator.  */
	typedef vector_type::const_iterator const_iterator;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Construct nameless @c VARTYPE_NULL.
	*/
	Variable()
		: m_type(VARTYPE_NULL)
		, m_name()
		, m_children()
		, m_strv()
		, m_intv(0)
	{}
	/**
		Construct named with type (default value).
		@param name Name.
		@param type Type.
	*/
	Variable(u8string const& name, VariableType const type)
		: m_type(type)
		, m_name(name)
		, m_children()
		, m_strv()
		, m_intv(0)
	{}
	/**
		Construct nameless with type (default value).
		@param type Type.
	*/
	explicit Variable(VariableType const type)
		: m_type(type)
		, m_name()
		, m_children()
		, m_strv()
		, m_intv(0)
	{}
	/**
		Construct named @c VARTYPE_STRING with value.
		@param name Name.
		@param value Value.
	*/
	Variable(u8string const& name, u8string const& value)
		: m_type(VARTYPE_STRING)
		, m_name(name)
		, m_children()
		, m_strv(value)
		, m_intv()
	{}
	/**
		Construct nameless @c VARTYPE_STRING with value.
		@param value Value.
	*/
	explicit Variable(u8string const& value)
		: m_type(VARTYPE_STRING)
		, m_name()
		, m_children()
		, m_strv(value)
		, m_intv()
	{}
	/**
		Construct named @c VARTYPE_INTEGER with value.
		@param name Name.
		@param value Value.
	*/
	Variable(u8string const& name, int const value)
		: m_type(VARTYPE_INTEGER)
		, m_name(name)
		, m_children()
		, m_strv()
		, m_intv(value)
	{}
	/**
		Construct nameless @c VARTYPE_INTEGER with value.
		@param value Value.
	*/
	explicit Variable(int const value)
		: m_type(VARTYPE_INTEGER)
		, m_name()
		, m_children()
		, m_strv()
		, m_intv(value)
	{}
	/**
		Construct named @c VARTYPE_FLOAT with value.
		@param name Name.
		@param value Value.
	*/
	Variable(u8string const& name, float const value)
		: m_type(VARTYPE_FLOAT)
		, m_name(name)
		, m_children()
		, m_strv()
		, m_floatv(value)
	{}
	/**
		Construct nameless @c VARTYPE_FLOAT with value.
		@param value Value.
	*/
	explicit Variable(float const value)
		: m_type(VARTYPE_FLOAT)
		, m_name()
		, m_children()
		, m_strv()
		, m_floatv(value)
	{}
	/**
		Construct named @c VARTYPE_BOOL with value.
		@param name Name.
		@param value Value.
	*/
	Variable(u8string const& name, bool const value)
		: m_type(VARTYPE_BOOL)
		, m_name(name)
		, m_children()
		, m_strv()
		, m_boolv(value)
	{}
	/**
		Construct nameless @c VARTYPE_BOOL with value.
		@param value Value.
	*/
	explicit Variable(bool const value)
		: m_type(VARTYPE_BOOL)
		, m_name()
		, m_children()
		, m_strv()
		, m_boolv(value)
	{}

	/**
		Copy constructor.
		@param other Variable to copy.
	*/
	Variable(Variable const& other)
		: m_type(other.m_type)
		, m_name()
		, m_children()
		, m_strv()
		, m_intv(0)
	{
		this->operator=(other);
	}
	/**
		Copy operator.
		@note #reset() is called if @a other's type and the current type are both unequal and not collections.
		@param other Variable to copy.
	*/
	void operator=(Variable const& other) {
		if (other.m_type!=m_type && !((VARCLASS_COLLECTION&m_type) && (VARCLASS_COLLECTION&other.m_type))) {
			reset();
		}
		m_type=other.m_type;
		m_name=other.m_name;
		switch (other.m_type) {
		case VARTYPE_STRING: m_strv=other.m_strv; break;
		case VARTYPE_INTEGER: m_intv=other.m_intv; break;
		case VARTYPE_FLOAT: m_floatv=other.m_floatv; break;
		case VARTYPE_BOOL: m_boolv=other.m_boolv; break;
		case VARTYPE_ARRAY:
		case VARTYPE_NODE:
		case VARTYPE_IDENTIFIER:
			m_children.assign(other.m_children.cbegin(), other.m_children.cend());
			break;
		default: break;
		}
	}
/// @}

/** @name Properties */ /// @{
	/**
		Get type.
		@returns The current type.
	*/
	inline VariableType get_type() const { return m_type; }

	/**
		Set name.
		@param name New name.
	*/
	inline void set_name(u8string const& name) { m_name=name; }
	/**
		Get name.
		@returns The current name.
	*/
	inline u8string const& get_name() const { return m_name; }

	/**
		Test the variable's class.
		@returns @c true if the variable's type is of the given class.
		@param vclass Variable class to test against.
	*/
	inline bool is_class(VariableClass const vclass) const { return m_type&vclass; }
	/**
		Check if the variable is a @c VARTYPE_NULL.
		@returns @c true if variable is null.
	*/
	inline bool is_null() const { return VARTYPE_NULL==m_type; }
/// @}

/** @name Operations */ /// @{
	/**
		Change type to @c VARTYPE_NULL.
		Equivalent to @c morph(VARTYPE_NULL).
	*/
	inline void nullify() { morph(VARTYPE_NULL); }

	/**
		Reset @c VARCLASS_VALUE types to default value; clear children for @c VARCLASS_COLLECTION types.
		@note Does nothing when @c is_null()==true
	*/
	void reset() {
		switch (m_type) {
		case VARTYPE_STRING: m_strv.clear(); break;
		case VARTYPE_INTEGER: m_intv=0; break;
		case VARTYPE_FLOAT: m_floatv=0.0f; break;
		case VARTYPE_BOOL: m_boolv=false; break;
		case VARTYPE_ARRAY:
		case VARTYPE_NODE:
		case VARTYPE_IDENTIFIER:
			m_children.clear();
			break;
		default: break;
		}
	}

	/**
		Change type.
		@note #reset() is called if @a type and the current type are both unequal and not collections.
		@note If the desired type and the current type are both collections, children are kept.
		@returns
			@c true if the type was changed; or
			@c false if @c get_type()==type
		@param type New type.
	*/
	bool morph(VariableType const type) {
		if (type!=m_type) {
			if (!((VARCLASS_COLLECTION&m_type) && (VARCLASS_COLLECTION&type))) {
				reset();
			}
			m_type=type;
			return true;
		}
		return false;
	}

	/**
		Change type and set value.
		@note #reset() is called if the type is changed.
		@returns
			@c true if the type was changed — @c reset() is called and value is set; or
			@c false if @c get_type()==type — value is not set unless @c force_set==true
		@tparam T Value type; inferred from @a value.
		@param value New value; the new variable type is inferred.
		@param force_set Whether to set the value if the variable is already of the desired type; @c false by default.
	*/
	template<typename T>
	bool morph(T const& value, bool const force_set=false) {
		static_assert(true==detail::is_valtype<T>::value, "value type does not have a corresponding VariableType");
		VariableType const type=(VariableType)detail::type_to_valtype<T>::value;
		if (type!=m_type) {
			reset();
			m_type=type;
			set(value);
			return true;
		} else {
			if (force_set) {
				set(value);
			}
			return false;
		}
	}
	/**
		Change type and set string value (move).
		@note #reset() is called if the type is changed.
		@returns
			@c true if the type was changed — @c reset() is called and value is set; or
			@c false if @c get_type()==VARTYPE_STRING — value is not set unless @c force_set==true
		@param value New value.
		@param force_set Whether to set the value if the variable is already of the desired type; @c false by default.
	*/
	bool morph(detail::var_config::string_type&& value, bool const force_set=false) {
		if (VARTYPE_STRING!=m_type) {
			reset();
			m_type=VARTYPE_STRING;
			set(std::move(value));
			return true;
		} else {
			if (force_set) {
				set(std::move(value));
			}
			return false;
		}
	}
/// @}

/** @name Comparison */ /// @{
	/**
		Name and value equality to another variable.
		@returns @c true if name and value of @c this and name and value of @a other are equivalent.
		@param other Variable to compare against.
	*/
	bool equals(Variable const& other) const {
		return 0==compare(other);
	}
	/**
		Value equality to another variable.
		@returns @c true if value of @c this and value of @a other are equivalent.
		@param other Variable to compare against.
	*/
	bool equals_value(Variable const& other) const {
		return 0==compare_value(other);
	}

	/**
		Name and value comparison to another variable.
		@returns A value which is
			@c <0 if name or value of @c this is less than name or value of @a other
			@c 0 if name or value of @c this is equal to name or value of @a other
			@c >0 if name or value of @c this is greater than name or value of @a other
		@param other Variable to compare against.
	*/
	int compare(Variable const& other) const {
		int const diff=m_name.compare(other.m_name);
		return (0!=diff)
			? diff
			: compare_value(other);
	}

	/**
		Value comparison to another variable.
		@returns A value which is
			@c <0 if value of @c this is less than value of @a other
			@c 0 if value of @c this is equal to value of @a other
			@c >0 if value of @c this is greater than value of @a other
		@param other Variable to compare against.
	*/
	int compare_value(Variable const& other) const {
		switch (m_type|other.m_type) {
		case VARTYPE_NULL: return 0;
		case VARTYPE_STRING: return m_strv.compare(other.m_strv);
		case VARTYPE_INTEGER: return m_intv-other.m_intv;
		case VARTYPE_FLOAT: return (m_floatv>other.m_floatv) ? 1 : ((m_floatv<other.m_floatv) ? -1 : 0);
		case VARTYPE_BOOL: return m_boolv-other.m_boolv;
		case VARTYPE_ARRAY:
		case VARTYPE_NODE:
		case VARTYPE_IDENTIFIER: {
			auto const sdiff=size()-other.size();
			if (0!=sdiff || 0==size()) { // If not equivalent or both 0
				return sdiff;
			} else { // If equivalent and not 0
				int vdiff;
				for (auto ti=m_children.cbegin(), oi=other.m_children.cbegin(); m_children.cend()!=ti; ++ti, ++oi) {
					vdiff=(*ti).compare_value(*oi);
					if (0!=vdiff) {
						return vdiff;
					}
				}
				return 0; // All children equivalent
			}
		}
		default: return m_type-other.m_type; // Bitwise OR of both types equals not a single type, therefore: types are unequal
		}
	}
/// @}

/**
	@name Value set/get
	@warning All <em>value</em> get/set methods (except for @c get_as_str() — it has a different rule) are type-strict; an assertion will fail if @c get_type() does not equal:
	-# the type of assignment; or
	-# the type of retrieval.
	@{
*/
	/** @cond INTERNAL */
	#define DUCT_V_set_value_() assert(DUCT_V_TYPE_==m_type); DUCT_V_FIELD_=value
	#define DUCT_V_get_value_() assert(DUCT_V_TYPE_==m_type); return DUCT_V_FIELD_
	#define DUCT_V_TYPE_ VARTYPE_STRING
	#define DUCT_V_FIELD_ m_strv
	/** @endcond */

	/**
		Set value.
		@param value New value.
		@sa morph(T, bool)
	*/
	void set(detail::var_config::string_type const& value) { DUCT_V_set_value_(); }
	/** @copydoc set(detail::var_config::string_type const&) */
	void set(detail::var_config::string_type&& value) { assert(DUCT_V_TYPE_==m_type); DUCT_V_FIELD_=std::move(value); }
	/**
		Get string value.
		@returns The current string value.
	*/
	detail::var_config::string_type get_string() const { DUCT_V_get_value_(); }
	/** @copydoc get_string() */
	detail::var_config::string_type& get_string_ref() { DUCT_V_get_value_(); }
	/** @copydoc get_string() */
	detail::var_config::string_type const& get_string_cref() const { DUCT_V_get_value_(); }

	/** @cond INTERNAL */
	#undef DUCT_V_TYPE_
	#undef DUCT_V_FIELD_
	#define DUCT_V_TYPE_ VARTYPE_INTEGER
	#define DUCT_V_FIELD_ m_intv
	/** @endcond */

	/** @copydoc Variable::set(detail::var_config::string_type const&) */
	void set(detail::var_config::int_type const value) { DUCT_V_set_value_(); }
	/**
		Get int value.
		@returns The current int value.
	*/
	detail::var_config::int_type get_int() const { DUCT_V_get_value_(); }
	/** @copydoc get_int() */
	detail::var_config::int_type& get_int_ref() { DUCT_V_get_value_(); }

	/** @cond INTERNAL */
	#undef DUCT_V_TYPE_
	#undef DUCT_V_FIELD_
	#define DUCT_V_TYPE_ VARTYPE_FLOAT
	#define DUCT_V_FIELD_ m_floatv
	/** @endcond */

	/** @copydoc Variable::set(detail::var_config::string_type const&) */
	void set(detail::var_config::float_type const value) { DUCT_V_set_value_(); }
	/**
		Get float value.
		@returns The current float value.
	*/
	detail::var_config::float_type get_float() const { DUCT_V_get_value_(); }
	/** @copydoc get_float() */
	detail::var_config::float_type& get_float_ref() { DUCT_V_get_value_(); }

	/** @cond INTERNAL */
	#undef DUCT_V_TYPE_
	#undef DUCT_V_FIELD_
	#define DUCT_V_TYPE_ VARTYPE_BOOL
	#define DUCT_V_FIELD_ m_boolv
	/** @endcond */
	/** @copydoc Variable::set(detail::var_config::string_type const&) */
	void set(detail::var_config::bool_type const value) { DUCT_V_set_value_(); }
	/**
		Get boolean value.
		@returns The current boolean value.
	*/
	detail::var_config::bool_type get_bool() const { DUCT_V_get_value_(); }
	/** @copydoc get_bool() */
	detail::var_config::bool_type& get_bool_ref() { DUCT_V_get_value_(); }

	/** @cond INTERNAL */
	#undef DUCT_V_TYPE_
	#undef DUCT_V_FIELD_
	#undef DUCT_V_get_value_
	#undef DUCT_V_set_value_
	/** @endcond */

	/**
		Get value as string.
		@returns The current value as a string.
		@sa get_as_str(stringT&)
	*/
	inline detail::var_config::string_type get_as_str() const {
		return get_as_str<detail::var_config::string_type>();
	}
	/**
		Get value as string.
		@returns The current value as a string.
		@tparam stringT String type to convert to.
		@sa get_as_str(stringT&)
	*/
	template<class stringT>
	stringT get_as_str() const {
		stringT str;
		get_as_str(str);
		return str;
	}
	/**
		Get value as string with output parameter.
		@warning An assertion will fail if the variable is not a @c VARCLASS_VALUE type nor a @c VARTYPE_NULL.
		@note
		- A @c VARTYPE_NULL will result in @c "null".
		- A @c VARTYPE_BOOL will result in either @c "false" or @c "true".
		@tparam stringT String type to convert to; inferred from @a out_str.
		@param[out] out_str Output value.
	*/
	template<class stringT>
	void get_as_str(stringT& out_str) const {
		assert(m_type&(VARCLASS_VALUE|VARTYPE_NULL));
		switch (m_type) {
		case VARTYPE_NULL: out_str="null"; break;
		case VARTYPE_STRING: StringUtils::convert(out_str, m_strv); break;
		case VARTYPE_BOOL: out_str=(m_boolv) ? "true" : "false"; break;
		default: {
			std::stringstream stream;
			switch (m_type) {
			case VARTYPE_INTEGER: stream<<m_intv; break;
			case VARTYPE_FLOAT: stream<<std::showpoint<<m_floatv; break;
			default: return;
			}
			u8string temp;
			stream>>temp;
			StringUtils::convert(out_str, temp);
		}}
	}
/** @} */

/**
	@name Children
	@warning An assertion will fail for all methods defined in this group if @c is_class(VARCLASS_COLLECTION)==false.
	@{
*/
	/**
		Get beginning child iterator.
		@returns The beginning child iterator.
	*/
	iterator begin() { assert(is_class(VARCLASS_COLLECTION)); return m_children.begin(); }
	/** @copydoc begin() */
	const_iterator cbegin() const { assert(is_class(VARCLASS_COLLECTION)); return m_children.cbegin(); }

	/**
		Get ending child iterator.
		@returns The ending child iterator.
	*/
	iterator end() { assert(is_class(VARCLASS_COLLECTION)); return m_children.end(); }
	/** @copydoc end() */
	const_iterator cend() const { assert(is_class(VARCLASS_COLLECTION)); return m_children.cend(); }

	/**
		Get number of children.
		@returns The current number of children.
	*/
	inline vector_type::size_type size() const { assert(is_class(VARCLASS_COLLECTION)); return m_children.size(); }

	/**
		Set children.
		@param new_children New child collection.
	*/
	inline void set_children(vector_type const& new_children) { assert(is_class(VARCLASS_COLLECTION)); m_children=new_children; }
	/** @copydoc set_children(vector_type const&) */
	inline void set_children(vector_type&& new_children) { assert(is_class(VARCLASS_COLLECTION)); m_children=std::move(new_children); }

	/**
		Get children.
		@returns The current child collection.
	*/
	inline vector_type const& get_children() const { assert(is_class(VARCLASS_COLLECTION)); return m_children; }

	/**
		Append to end of child collection.
		@returns @c *this.
		@param var Variable to append.
	*/
	Variable& push_back(Variable const& var) {
		assert(is_class(VARCLASS_COLLECTION));
		m_children.push_back(var);
		return *this;
	}
	/** @copydoc push_back(Variable const&) */
	Variable& push_back(Variable&& var) {
		assert(is_class(VARCLASS_COLLECTION));
		m_children.push_back(std::move(var));
		return *this;
	}

	/**
		Emplace to end of child collection.
		@returns @c *this.
		@tparam ...Args_ Parameter pack for constructor; see @c Variable().
		@param args Parameter pack for constructor.
	*/
	template<class ...Args_>
	Variable& emplace_back(Args_&&... args) {
		assert(is_class(VARCLASS_COLLECTION));
		m_children.emplace_back(std::forward<Args_>(args)...);
		return *this;
	}
/** @} */

private:
	VariableType m_type;
	u8string m_name;
	vector_type m_children;
	detail::valtype_to_type<VARTYPE_STRING>::type m_strv;
	union {
		detail::valtype_to_type<VARTYPE_INTEGER>::type m_intv;
		detail::valtype_to_type<VARTYPE_FLOAT>::type m_floatv;
		detail::valtype_to_type<VARTYPE_BOOL>::type m_boolv;
	};
};

/** @} */ // end doc-group variable

} // namespace duct

#endif // DUCT_VARIABLE_HPP_
