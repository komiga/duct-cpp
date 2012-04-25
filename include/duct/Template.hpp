/**
@file Template.hpp
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

duct++ Template class.
*/

// TODO: Copy constructors for Template, GArray and StringArray

#ifndef DUCT_TEMPLATE_HPP_
#define DUCT_TEMPLATE_HPP_

#include <duct/config.hpp>
#include <duct/Variables.hpp>
#include <duct/GArray.hpp>
#include <duct/UniStrArray.hpp>

namespace duct {

// Forward declarations
class Template;

/**
	Variable type layout.
	A type layout is an array of variable types that a #Template uses to match variable types.
*/
typedef GArray<unsigned int> VTypeLayout;

/**
	Template class.
*/
class DUCT_API Template {
public:
	/**
		Constructor.
	*/
	Template();
	/**
		Constructor with identity, layout and options.
		@param iden The template's identity. The given array is owned by the template.
		@param layout The template's layout.
		@param casesens Whether name comparison is case-sensitive.
		@param infinitism The infinitism variable type. See setInfinitism().
	*/
	Template(StringArray* iden, VTypeLayout* layout, bool casesens=false, unsigned int infinitism=VARTYPE_NONE);
	/**
		Destructor.
	*/
	~Template();
	/**
		Set the template's variable-identity.
		NOTE: The template owns the given pointer (it will be destroyed by the template upon destruction).
		@returns Nothing.
		@param iden The new identity. Can be NULL.
	*/
	void setIdentity(StringArray* identity);
	/**
		Get the template's variable-identity.
		@returns The template's identity.
	*/
	StringArray const* getIdentity() const;
	/**
		Set the variable type layout.
		NOTE: The template owns the given pointer (it will be destroyed by the template upon destruction).
		@returns Nothing.
		@param types The new variable type array. Can be NULL.
	*/
	void setLayout(VTypeLayout* layout);
	/**
		Get the template's variable type layout.
		@returns The template's layout.
	*/
	VTypeLayout const* getLayout() const;
	/**
		Set the template's infinitism type (types for flexible-template matching).
		Infinitism can be a combination of #VariableTypes, or equal to VARTYPE_ANY or VARTYPE_NONE (implying template flexibility is off).
		@returns Nothing.
		@param infinitism The new infinitsm type.
	*/
	void setInfinitism(unsigned int infinitism);
	/**
		Get the template's infinitism variable type.
		@returns The template's infinitism type.
	*/
	unsigned int getInfinitism() const;
	/**
		Set the identifier matching case-sensitivity.
		@returns Nothing.
		@param casesens Whether name-matching is case sensitive.
	*/
	void setCaseSensitive(bool casesens);
	/**
		Get the template's identifier matching case-sensitivity.
		@returns true if name-matching is case-sensitive, or false if name-matching is not case sensitive.
	*/
	bool getCaseSensitive() const;
	/**
		Validate the given identifier against the template.
		@returns true if the given identifier matched the template, or false if either it did not match or the given identifier was NULL.
		@param identifier The identifier to test.
	*/
	bool validateIdentifier(Identifier const* identifier) const;
	/**
		Validate the given value variable against the template.
		@returns true if the given value matched the template, or false if either it did not match or the given value was NULL.
		@param value The value to test.
	*/
	bool validateValue(ValueVariable const* value) const;
	/**
		Check if the given variable matches the template's identity.
		@returns true if the given variable matched the template's identity, or false if either it did not match or the given variable was NULL.
		@param variable The variable to validate.
	*/
	bool validateIdentity(Variable const* variable) const;
	/**
		Compact a sequence of value variables matching the template into identifiers.
		@returns The number of identifiers created and added (0 if either the no matches were found, or the given collection is NULL or had no children).
		@param name The name to give new identifiers.
		@param sequential Whether value sequence is sequential.
		If true, the comparison will require the template's layout to be matched in sequence (false will allow non-matching values in a sequence).
	*/
	unsigned int compactCollection(CollectionVariable* collection, icu::UnicodeString const& name, bool sequential=true) const;
	/**
		Rename all matching identifiers to the given name.
		@returns The number of identifiers that were renamed.
		@param collection The collection to operate on.
		@param name New name.
	*/
	unsigned int renameIdentifiers(CollectionVariable* collection, icu::UnicodeString const& name) const;
	/**
		Rename all matching values to the given name.
		@returns The number of values that were renamed.
		@param collection The collection to operate on.
		@param name New name.
	*/
	unsigned int renameValues(CollectionVariable* collection, icu::UnicodeString const& name) const;
	/**
		Get the first matching variable from the given begin-end pair.
		@returns The first matching variable, or NULL if a matching variable was not found.
		@param begin The beginning iterator.
		@param end The ending iterator.
		@param type The variable type(s) to validate. If equal to 0 or if VARTYPE_NODE is present, only the identity will be validated.
		@see validateIdentifier()
		@see validateValue()
		@see validateIdentity()
	*/
	template <typename iterator_type>
	Variable* getMatchingVariable(iterator_type begin, iterator_type end, unsigned int type) const {
		for (iterator_type iter=begin; iter!=end; ++iter) {
			Variable* variable=*iter;
			if (type&variable->getType()) {
				if (variable->getType()==VARTYPE_IDENTIFIER && validateIdentifier(dynamic_cast<Identifier*>(variable))) {
					return variable;
				} else if (variable->getType()&VARTYPE_VALUE && validateValue(dynamic_cast<ValueVariable*>(variable))) {
					return variable;
				} else if (variable->getType()==VARTYPE_NODE && validateIdentity(variable)) {
					return variable;
				}
			} else if (0==type && validateIdentity(variable)) {
				return variable;
			}
		}
		return NULL;
	}
	/**
		Get the first matching identifier from the given collection.
		@returns The first matching identifier, or NULL if the given collection either was NULL or did not contain a matching identifier.
		@param collection The collection to operate on.
		@param reverse Whether to start searching at the bottom of the collection.
		@see validateIdentifier()
	*/
	Identifier* getMatchingIdentifier(CollectionVariable const* collection, bool reverse=false) const;
	/**
		Get the first matching value variable from the given collection.
		@returns The first matching value variable, or NULL if the given collection either was NULL or did not contain a matching value.
		@param collection The collection to operate on.
		@param reverse Whether to start searching at the bottom of the collection.
		@see validateValue()
	*/
	ValueVariable* getMatchingValue(CollectionVariable const* collection, bool reverse=false) const;
	
protected:
	StringArray* m_iden;
	VTypeLayout* m_layout;
	bool m_casesens;
	unsigned int m_infinitism;
};

} // namespace duct

#endif // DUCT_TEMPLATE_HPP_
