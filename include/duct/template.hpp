/**
@file template.hpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010-2011 Tim Howard

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

#ifndef _DUCT_TEMPLATE_HPP
#define _DUCT_TEMPLATE_HPP

#include <duct/config.hpp>
#include <duct/variables.hpp>
#include <duct/genericarray.hpp>
#include <duct/unistrarray.hpp>

namespace duct {

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
		NOTE: The template owns the given pointer (it will be destroyed by the template upon deconstruction).
		@returns Nothing.
		@param iden The new identity. Can be NULL.
	*/
	void setIdentity(StringArray* identity);
	/**
		Get the template's variable-identity.
		@returns The template's identity.
	*/
	const StringArray* getIdentity() const;
	/**
		Set the variable type layout.
		NOTE: The template owns the given pointer (it will be destroyed by the template upon deconstruction).
		@returns Nothing.
		@param types The new variable type array. Can be NULL.
	*/
	void setLayout(VTypeLayout* layout);
	/**
		Get the template's variable type layout.
		@returns The template's layout.
	*/
	const VTypeLayout* getLayout() const;
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
	const unsigned int& getInfinitism() const;
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
	bool validateIdentifier(const Identifier* identifier) const;
	/**
		Validate the given value variable against the template.
		@returns true if the given value matched the template, or false if either it did not match or the given value was NULL.
		@param value The value to test.
	*/
	bool validateValue(const ValueVariable* value) const;
	/**
		Check if the given variable matches the template's identity.
		@returns true if the given variable matched the template's identity, or false if either it did not match or the given variable was NULL.
		@param variable The variable to validate.
	*/
	bool validateIdentity(const Variable* variable) const;
	/**
		Compact a sequence of value variables matching the template into identifiers.
		@returns The number of identifiers created and added (0 if either the no matches were found, or the given collection is NULL or had no children).
		@param name The name to give new identifiers.
		@param sequential Whether value sequence is sequential.
		If true, the comparison will require the template's layout to be matched in sequence (false will allow non-matching values in a sequence).
	*/
	unsigned int compactCollection(CollectionVariable* collection, const UnicodeString& name, bool sequential=true) const;
	/**
		Rename all matching identifiers to the given name.
		@returns The number of identifiers that were renamed.
		@param collection The collection to operate on.
		@param name New name.
	*/
	unsigned int renameIdentifiers(CollectionVariable* collection, const UnicodeString& name) const;
	/**
		Rename all matching values to the given name.
		@returns The number of values that were renamed.
		@param collection The collection to operate on.
		@param name New name.
	*/
	unsigned int renameValues(CollectionVariable* collection, const UnicodeString& name) const;
	/**
		Get the first matching identifier from the given collection.
		@returns The first matching identifier, or NULL if the given collection either was NULL or did not contain a matching identifier.
		@param collection The collection to operate on.
		@see validateIdentifier()
	*/
	Identifier* getMatchingIdentifier(const CollectionVariable* collection) const;
	/**
		Get the first matching value variable from the given collection.
		@returns The first matching value variable, or NULL if the given collection either was NULL or did not contain a matching value.
		@param collection The collection to operate on.
		@see validateValue()
	*/
	ValueVariable* getMatchingValue(const CollectionVariable* collection) const;
	
protected:
	StringArray* _iden;
	VTypeLayout* _layout;
	bool _casesens;
	unsigned int _infinitism;
};

} // namespace duct

#endif // _DUCT_TEMPLATE_HPP

