/**
@file variables.hpp
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

Implements component parts:
<ul>
	<li>Variable framework</li>
</ul>
*/

#ifndef _DUCT_VARIABLES_HPP
#define _DUCT_VARIABLES_HPP

#include <duct/config.hpp>

#include <list>
#include <unicode/uchar.h>
#include <unicode/unistr.h>

namespace duct {

/**
	Base Variable types.
	00x1 through 0x80 are reserved types.
*/
enum VariableType {
	/**
		#IntVariable.
	*/
	VARTYPE_INTEGER=0x01,
	/**
		#StringVariable.
	*/
	VARTYPE_STRING=0x02,
	/**
		#FloatVariable.
	*/
	VARTYPE_FLOAT=0x04,
	/**
		#BoolVariable.
	*/
	VARTYPE_BOOL=0x08,
	/**
		Reserved type 0x10.
	*/
	VARTYPE_RESERVED0=0x10,
	/**
		Reserved type 0x20.
	*/
	VARTYPE_RESERVED1=0x20,
	/**
		#Identifier.
	*/
	VARTYPE_IDENTIFIER=0x40,
	/**
		#Node.
	*/
	VARTYPE_NODE=0x80,
	/**
		No variable type.
		Special variable type (means 'no variable type'). Alias for 0x00 (no flags).
		@see VARTYPE_ANY.
	*/
	VARTYPE_NONE=0x00,
	/**
		Special value for variable searching.
		Means 'any variable type'.
		See CollectionVariable.findWithType().
	*/
	VARTYPE_ANY=0xFFFFFFFF,
	/**
		Special type for ValueVariables.
	*/
	VARTYPE_VALUE=VARTYPE_INTEGER|VARTYPE_STRING|VARTYPE_FLOAT|VARTYPE_BOOL,
	/**
		Special type for CollectionVariables.
	*/
	VARTYPE_COLLECTION=VARTYPE_IDENTIFIER|VARTYPE_NODE
};

/**
	Variable value/name format.
	Formatting flags for Variable names and ValueVariable values.
*/
enum ValueFormat {
	/**
		No-format flag.
	*/
	FMT_NONE=0x00,
	/**
		Value quote-always format flag.
		This flag is for any variable type. The output will always have quotes around it.
	*/
	FMT_VALUE_QUOTE_ALWAYS=0x01,
	/**
		String quote-whitespace format flag.
		This format will quote a string containing whitespace or linefeed/carriage-return characters. e.g. "foo bar\t" -> "\"foo bar\t\"".
	*/
	FMT_STRING_QUOTE_WHITESPACE=0x10,
	/**
		String quote-empty format flag.
		This format will quote an empty string. e.g. "" -> "\"\"".
	*/
	FMT_STRING_QUOTE_EMPTY=0x20,
	/**
		String quote-control format flag.
		This format will quote a string containing the following characters: '{', '}', '='.
	*/
	FMT_STRING_QUOTE_CONTROL=0x40,
	/**
		String quote-always format flag.
		This format will always quote any string.
	*/
	FMT_STRING_QUOTE_ALWAYS=0x80,
	/**
		String quote-bool format flag.
		This format will quote a string if it equals "true" or "false" as a type safeguard. e.g. "true" -> "\"true\"".
	*/
	FMT_STRING_SAFE_BOOL=0x0100,
	/**
		String quote-number format flag.
		This format will quote a string if it is a number as a type safeguard. e.g. "1234.5678" -> "\"1234.5678\"".
	*/
	FMT_STRING_SAFE_NUMBER=0x0200,
	/**
		String escape-newline format flag.
		This format will replace the following characters with escape sequences, if the string is not surrounded in quotes: '\n' '\r'
	*/
	FMT_STRING_ESCAPE_NEWLINE=0x1000,
	/**
		String escape-control format flag.
		This format will replace the following characters with escape sequences, if the string is not surrounded in quotes: '{', '}', '='.
	*/
	FMT_STRING_ESCAPE_CONTROL=0x2000,
	/**
		String escape-other format flag.
		This format will replace the following characters with escape sequences: '\t', '\"', '\'.
	*/
	FMT_STRING_ESCAPE_OTHER=0x4000,
	/**
		String escape-all format flag.
		Consists of #FMT_STRING_ESCAPE_NEWLINE, #FMT_STRING_ESCAPE_CONTROL and #FMT_STRING_ESCAPE_OTHER.
	*/
	FMT_STRING_ESCAPE_ALL=FMT_STRING_ESCAPE_NEWLINE|FMT_STRING_ESCAPE_CONTROL|FMT_STRING_ESCAPE_OTHER,
	/**
		String safe format flag.
		Consists of #FMT_STRING_SAFE_BOOL, #FMT_STRING_SAFE_NUMBER, #FMT_STRING_ESCAPE_OTHER and #FMT_STRING_QUOTE_CONTROL.
	*/
	FMT_STRING_SAFE=FMT_STRING_SAFE_BOOL|FMT_STRING_SAFE_NUMBER|FMT_STRING_ESCAPE_OTHER|FMT_STRING_QUOTE_CONTROL,
	/**
		Default string format flag.
		Consists of #FMT_STRING_SAFE, #FMT_STRING_QUOTE_WHITESPACE and #FMT_STRING_QUOTE_EMPTY.
	*/
	FMT_STRING_DEFAULT=FMT_STRING_SAFE|FMT_STRING_QUOTE_WHITESPACE|FMT_STRING_QUOTE_EMPTY,
	/**
		Boolean quote format flag.
		Converts the boolean value to a string ("true", "false"). e.g. false -> "false", true -> "true".
	*/
	FMT_BOOL_QUOTE=0x010000,
	/**
		Default boolean format flag.
		Unset flag (no formatting).
	*/
	FMT_BOOL_DEFAULT=FMT_NONE,
	/**
		Default name format flag.
		Consists of #FMT_STRING_SAFE, #FMT_STRING_QUOTE_WHITESPACE and #FMT_STRING_QUOTE_EMPTY.
	*/
	FMT_NAME_DEFAULT=FMT_STRING_SAFE|FMT_STRING_QUOTE_WHITESPACE|FMT_STRING_QUOTE_EMPTY,
	/**
		Default int format flag.
		Unset flag (no formatting).
	*/
	FMT_INTEGER_DEFAULT=FMT_NONE,
	/**
		Default float format flag.
		Unset flag (no formatting).
	*/
	FMT_FLOAT_DEFAULT=FMT_NONE,
	/**
		Default format flag for any variable.
		Consists of all default format flags: #FMT_STRING_DEFAULT, #FMT_FLOAT_DEFAULT, #FMT_BOOL_DEFAULT and #FMT_INTEGER_DEFAULT.
	*/
	FMT_ALL_DEFAULT=FMT_STRING_DEFAULT|FMT_FLOAT_DEFAULT|FMT_BOOL_DEFAULT|FMT_INTEGER_DEFAULT
};

// Forward declarations
class CollectionVariable;
class ValueVariable;

/**
	Variable class.
*/
class DUCT_API Variable {
public:
	/**
		Destructor.
	*/
	virtual ~Variable();
	/**
		Set the variable's name.
		@returns Nothing.
		@param name The new name.
	*/
	virtual void setName(const icu::UnicodeString& name);
	/**
		Get the variable's name.
		@returns The variable's name.
	*/
	virtual const icu::UnicodeString& getName() const;
	/**
		Get the variable's name with the given format.
		@returns Nothing.
		@param result The variable to store the formatted name.
		@param format The formatting flags.
	*/
	virtual void getNameFormatted(icu::UnicodeString& result, unsigned int format=FMT_NAME_DEFAULT) const;
	/**
		Set the variable's parent.
		@returns The variable's parent.
		@param parent The new parent.
	*/
	virtual void setParent(CollectionVariable* parent);
	/**
		Get the variable's parent.
		@returns The variable's parent.
	*/
	virtual CollectionVariable* getParent() const;
	/**
		Get the variable's type.
		Return values for base Variable types can be found in the enum #VariableType.
		@returns The variable's type.
	*/
	virtual unsigned int getType() const=0;
	/**
		Get the variable's type as a string.
		e.g. "string", "int", etc.
		@returns A const char pointer to the variable's type.
	*/
	virtual const char* getTypeName() const=0;
	/**
		Get a clone of the variable.
		Clones are exactly identical to their source, except that the Parent property is <em>always</em> NULL.
		@returns A clone of the variable.
	*/
	virtual Variable* clone() const=0;
	/**
		Convert the given variable to a boolean.
		@returns 1 if the given variable was able to convert to 'true', 0 if the given variable was able to convert to 'false', or -1 if the variable was not able to convert to a boolean value.
		@param source The variable to test.
	*/
	static signed int variableToBool(Variable* source);
	/**
		Convert the given string to a boolean.
		This will match for "true", "1", "false" and "0". Case sensitivity is off.
		@returns 1 if the given variable was able to convert to 'true', 0 if the given variable was able to convert to 'false', or -1 if the variable was not able to convert to a boolean value.
		@param source The string to test.
	*/
	static signed int stringToBool(const icu::UnicodeString& source);
	/**
		Convert the given string into a ValueVariable.
		If the given source string is empty, an empty StringVariable will be returned with the given variable name.
		@returns The value, or NULL if an error occurred.
		@param source The string to convert.
		@param varname The variable name for the resulting value.
		@param type Optional type-shortcut. Possible values for base ValueVariable types can be found in the #VariableType enum.
	*/
	static ValueVariable* stringToValue(const icu::UnicodeString& source, const icu::UnicodeString& varname, unsigned int type=VARTYPE_NONE);
	/**
		Convert the given string into a ValueVariable.
		If the given source string is empty, an empty StringVariable will be returned.
		@returns The value, or NULL if an error occurred.
		@param source The string to convert.
		@param type Optional type-shortcut. Possible values for base ValueVariable types can be found in the #VariableType enum.
	*/
	static ValueVariable* stringToValue(const icu::UnicodeString& source, unsigned int type=VARTYPE_NONE);
	
protected:
	icu::UnicodeString _name;
	CollectionVariable* _parent;
};

/**
	Value variable.
*/
class DUCT_API ValueVariable : public Variable {
public:
	/**
		Destructor.
	*/
	virtual ~ValueVariable();
	/**
		Set the variable's value from the given string.
		@returns Nothing.
		@param value The string to set the variable from.
	*/
	virtual void setFromString(const icu::UnicodeString& source)=0;
	/**
		Get the variable's value as a string with the given format.
		@returns Nothing.
		@param result The variable to store the formatted value.
		@param format The formatting flags.
	*/
	virtual void getValueFormatted(icu::UnicodeString& result, unsigned int format=FMT_ALL_DEFAULT) const=0;
	/**
		Get an unformatted string conversion of the variable.
		@returns The variable's value as an unformatted string.
		@param result The variable to store the result.
		@param append Whether to append or replace the result string. Default is false.
	*/
	virtual void valueAsString(icu::UnicodeString& result, bool append=false) const=0;
};

/**
	Variable pointer list.
*/
typedef std::list<Variable*> VarList;

/**
	Integer variable.
*/
class DUCT_API IntVariable : public ValueVariable {
public:
	/**
		Constructor with value.
		@param value The value to initialize with.
		@param parent The parent to initialize with.
	*/
	IntVariable(int value=0, CollectionVariable* parent=NULL);
	/**
		Constructor with name and value.
		@param value The value to initialize with.
		@param name The name to initialize with.
		@param parent The parent to initialize with.
	*/
	IntVariable(int value, const icu::UnicodeString& name, CollectionVariable* parent=NULL);
	/**
		Set the variable's value.
		@returns Nothing.
		@param value The new value.
	*/
	virtual void set(int value);
	/**
		Get the variable's value.
		@returns The variable's value.
	*/
	virtual int get() const;
	virtual void setFromString(const icu::UnicodeString& source);
	virtual void getValueFormatted(icu::UnicodeString& result, unsigned int format=FMT_ALL_DEFAULT) const;
	virtual void valueAsString(icu::UnicodeString& result, bool append=false) const;
	virtual unsigned int getType() const;
	virtual const char* getTypeName() const;
	virtual Variable* clone() const;
	
protected:
	int _value;
};

/**
	String variable.
*/
class DUCT_API StringVariable : public ValueVariable {
public:
	/**
		Constructor with parent.
		@param parent The parent to initialize with.
	*/
	StringVariable(CollectionVariable* parent=NULL);
	/**
		Constructor with value and parent.
		@param value The value to initialize with.
		@param parent The parent to initialize with.
	*/
	StringVariable(const icu::UnicodeString& value, CollectionVariable* parent=NULL);
	/**
		Constructor with name, value and parent.
		@param value The value to initialize with.
		@param name The name to initialize with.
		@param parent The parent to initialize with.
	*/
	StringVariable(const icu::UnicodeString& value, const icu::UnicodeString& name, CollectionVariable* parent=NULL);
	/**
		Check if the string is numerical.
		@returns True if the string is numerical.
		@param allowdecimal Whether to allow a decimal point.
	*/
	bool isNumeric(bool allowdecimal=true) const;
	/**
		Set the variable's value.
		@returns Nothing.
		@param value The new value.
	*/
	virtual void set(const icu::UnicodeString& value);
	/**
		Get the variable's value.
		@returns The variable's value.
	*/
	virtual const icu::UnicodeString& get() const;
	virtual void setFromString(const icu::UnicodeString& source);
	virtual void getValueFormatted(icu::UnicodeString& result, unsigned int format=FMT_ALL_DEFAULT) const;
	virtual void valueAsString(icu::UnicodeString& result, bool append=false) const;
	virtual unsigned int getType() const;
	virtual const char* getTypeName() const;
	virtual Variable* clone() const;
	
protected:
	icu::UnicodeString _value;
};

/**
	Float variable.
*/
class DUCT_API FloatVariable : public ValueVariable {
public:
	/**
		Constructor with value and parent.
		@param value The value to initialize with.
		@param parent The parent to initialize with.
	*/
	FloatVariable(float value=0.0, CollectionVariable* parent=NULL);
	/**
		Constructor with name, value and parent.
		@param value The value to initialize with.
		@param name The name to initialize with.
		@param parent The parent to initialize with.
	*/
	FloatVariable(float value, const icu::UnicodeString& name, CollectionVariable* parent=NULL);
	/**
		Set the variable's value.
		@returns Nothing.
		@param value The new value.
	*/
	virtual void set(float value);
	/**
		Get the variable's value.
		@returns The variable's value.
	*/
	virtual float get() const;
	virtual void setFromString(const icu::UnicodeString& source);
	virtual void getValueFormatted(icu::UnicodeString& result, unsigned int format=FMT_ALL_DEFAULT) const;
	virtual void valueAsString(icu::UnicodeString& result, bool append=false) const;
	virtual unsigned int getType() const;
	virtual const char* getTypeName() const;
	virtual Variable* clone() const;
	
protected:
	float _value;
};

/**
	Boolean variable.
*/
class DUCT_API BoolVariable : public ValueVariable {
public:
	/**
		Constructor with value and parent.
		@param value The value to initialize with.
		@param parent The parent to initialize with.
	*/
	BoolVariable(bool value=false, CollectionVariable* parent=NULL);
	/**
		Constructor with name, value and parent.
		@param value The value to initialize with.
		@param name The name to initialize with.
		@param parent The parent to initialize with.
	*/
	BoolVariable(bool value, const icu::UnicodeString& name, CollectionVariable* parent=NULL);
	/**
		Set the variable's value.
		@returns Nothing.
		@param value The new value.
	*/
	virtual void set(bool value);
	/**
		Get the variable's value.
		@returns The variable's value.
	*/
	virtual bool get() const;
	virtual void setFromString(const icu::UnicodeString& source);
	virtual void getValueFormatted(icu::UnicodeString& result, unsigned int format=FMT_ALL_DEFAULT) const;
	virtual void valueAsString(icu::UnicodeString& result, bool append=false) const;
	virtual unsigned int getType() const;
	virtual const char* getTypeName() const;
	virtual Variable* clone() const;
	
protected:
	bool _value;
};

// forward declarations
class Identifier;
class Node;

/**
	Collection variable.
	WARNING: All variables added to the collection are <em>owned</em> by the collection.
	Variables will be destroyed when removed and when the collection gets destroyed.
	As such, you should avoid passing statically-allocated variables, and should avoid destroying variables which are passed to the collection.
*/
class DUCT_API CollectionVariable : public Variable {
public:
	/**
		Destructor.
	*/
	virtual ~CollectionVariable();
	/**
		Get the collection's child list.
		@returns The collection's child list.
	*/
	VarList& getChildren();
	const VarList& getChildren() const;
	/**
		Get the number of children in the collection.
		@returns The number of children in the collection.
	*/
	size_t getChildCount() const;
	/**
		Get the start iterator for the collection.
		@returns Start Variable iterator for the collection.
	*/
	VarList::iterator begin();
	VarList::const_iterator begin() const;
	/**
		Get the end iterator for the collection.
		@returns End Variable iterator for the collection.
	*/
	VarList::iterator end();
	VarList::const_iterator end() const;
	/**
		Find the given variable.
		@returns The iterator for the given variable, or the end iterator if the variable was not found.
		@param variable The variable to search for.
	*/
	VarList::iterator find(const Variable* variable);
	VarList::const_iterator find(const Variable* variable) const;
	/**
		Find the given variable starting from iter.
		@returns The iterator for the given variable, or the end iterator if the variable was not found.
		@param variable The variable to search for.
		@param iter Beginning iterator.
	*/
	VarList::iterator find(const Variable* variable, VarList::iterator iter);
	VarList::const_iterator find(const Variable* variable, VarList::const_iterator iter) const;
	/**
		Get the iterator at the given index if it matches the given type.
		@returns The iterator for the variable at the index if it matches the given type, or the end iterator if either the index was invalid or the variable at the index did not match the type.
		@param index The index to retrieve.
		@param type The variable type to search for. See #VariableType.
	*/
	VarList::iterator findAt(int index, unsigned int type=VARTYPE_ANY);
	VarList::const_iterator findAt(int index, unsigned int type=VARTYPE_ANY) const;
	/**
		Find the given variable type.
		@returns The iterator for the first matching variable, or the end iterator if the type was not found.
		@param type The variable type to search for. VARTYPE_ALL will result in the first variable being returned (if the collection has children). See #VariableType.
	*/
	VarList::iterator findWithType(unsigned int type);
	VarList::const_iterator findWithType(unsigned int type) const;
	/**
		Find the given variable type starting from iter.
		@returns The iterator for the first matching variable, or the end iterator if the type was not found.
		@param type The variable type to search for. VARTYPE_ALL will result in the first variable being returned (if the collection has children). See #VariableType.
		@param iter Beginning iterator.
	*/
	VarList::iterator findWithType(unsigned int type, VarList::iterator iter);
	VarList::const_iterator findWithType(unsigned int type, VarList::const_iterator iter) const;
	/**
		Find a variable with the given name and type.
		@returns The iterator for the variable with the given name and type, or the end iterator if the variable was not found.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
		@param type Optional variable-type to search for. VARTYPE_ANY is the default.
	*/
	VarList::iterator findWithName(const icu::UnicodeString& name, bool casesens=true, unsigned int type=VARTYPE_ANY);
	VarList::const_iterator findWithName(const icu::UnicodeString& name, bool casesens=true, unsigned int type=VARTYPE_ANY) const;
	/**
		Find a variable with the given name and type, starting from iter.
		@returns The iterator for the variable with the given name and type, or the end iterator if the variable was not found.
		@param name The name to search for.
		@param iter Beginning iterator.
		@param casesens Whether to use case-sensitive name comparison.
		@param type Optional variable-type to search for. VARTYPE_ANY is the default.
	*/
	VarList::iterator findWithName(const icu::UnicodeString& name, VarList::iterator iter, bool casesens=true, unsigned int type=VARTYPE_ANY);
	VarList::const_iterator findWithName(const icu::UnicodeString& name, VarList::const_iterator iter, bool casesens=true, unsigned int type=VARTYPE_ANY) const;
	/**
		Remove the given iterator and delete its variable.
		@returns Nothing.
		@param position The iterator to erase.
	*/
	void erase(VarList::iterator position);
	/**
		Remove all variables from the collection.
		@returns Nothing.
	*/
	void clear();
	/**
		Add the given variable to the list.
		If the variable was added, its Parent property is set to this.
		@returns true on success, or false if the given variable was NULL.
		@param variable The variable to add.
	*/
	bool add(Variable* variable);
	/**
		Insert the given variable before the given index.
		If the variable was inserted, its Parent property is set to this.
		@returns true on success, or false if either the index was invalid or the variable was NULL.
		@param index The index to insert at.
		@param variable The variable to insert.
	*/
	bool insertBefore(int index, Variable* variable);
	/**
		Insert the given variable before the given target.
		If the variable was inserted, its Parent property is set to this.
		@returns true on success, or false if either variable was NULL or target was either NULL or not found.
		@param variable The variable to insert.
		@param target The variable to insert after.
	*/
	bool insertBefore(Variable* variable, Variable* target);
	/**
		Insert the given variable after the given index.
		If the variable was inserted, its Parent property is set to this.
		@returns true on success, or false if either the index was invalid or the variable was NULL.
		@param index The index to insert at.
		@param variable The variable to insert.
	*/
	bool insertAfter(int index, Variable* variable);
	/**
		Insert the given variable after the given target.
		If the variable was inserted, its Parent property is set to this.
		@returns true on success, or false if either variable was NULL or target was either NULL or not found.
		@param variable The variable to insert.
		@param target The variable to insert after.
	*/
	bool insertAfter(Variable* variable, Variable* target);
	/**
		Remove the variable at the given index.
		@returns true if the variable at the index was removed, or false if the index was invalid.
		@param index The index to remove.
	*/
	bool remove(int index, unsigned int type=VARTYPE_ANY);
	/**
		Remove the given variable from the list.
		@returns true if the variable was removed, or false if the given variable was not found.
		@param variable The variable to remove.
	*/
	bool remove(const Variable* variable);
	/**
		Remove the given variable type from the list.
		This will only remove the first matching variable found.
		@returns true if a variable with the given type was removed, or false if the type was not found.
		@param type The variable type to remove. See #VariableType.
	*/
	bool remove(unsigned int type);
	/**
		Remove a variable with the given name and type.
		@returns true if a variable with the given name was removed, or false if the name was not found.
		@param name The name of the variable to remove.
		@param casesens Whether to use case-sensitive name comparison.
		@param type Optional variable-type to search for. VARTYPE_ANY is the default.
	*/
	bool remove(const icu::UnicodeString& name, bool casesens=true, unsigned int type=VARTYPE_ANY);
	/**
		Get a variable with the given name and type.
		@returns The first matching variable, or NULL if no children matched the parameters.
		@param name The variable name to search for.
		@param casesens Whether to use case-sensitive name comparison.
		@param type Optional variable-type to search for. VARTYPE_ANY is the default.
	*/
	Variable* get(const icu::UnicodeString& name, bool casesens=true, unsigned int type=VARTYPE_ANY);
	const Variable* get(const icu::UnicodeString& name, bool casesens=true, unsigned int type=VARTYPE_ANY) const;
	/**
		Get the variable at the given index.
		@returns The variable at the given index, or NULL if either the given index was out of bounds or the variable at the given index did not match the given variable type.
		@param index The index to retrieve.
		@param type Variable type to retrieve. If the given variable type does not match the variable type at the given index, NULL will be returned.
	*/
	Variable* get(int index, unsigned int type=VARTYPE_ANY);
	const Variable* get(int index, unsigned int type=VARTYPE_ANY) const;
	/**
		Get the integer at the given index.
		@returns The IntVariable at the index, or NULL if either the index was invalid or the variable at the index wasn't the correct type.
		@param index The index to retrieve.
	*/
	IntVariable* getInt(int index);
	const IntVariable* getInt(int index) const;
	/**
		Get the integer matching the given name.
		@returns The first matching IntVariable, or NULL if no children matched the parameters.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
	*/
	IntVariable* getInt(const icu::UnicodeString& name, bool casesens=true);
	const IntVariable* getInt(const icu::UnicodeString& name, bool casesens=true) const;
	/**
		Get the integer value at the given index.
		@returns true on success, or false if either the index was invalid or the variable at the index wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param index The index to retrieve.
	*/
	bool getIntValue(int& result, int index) const;
	/**
		Get the integer value matching the given name.
		@returns true on success, or false if no children matched the parameters.
		@param result The result. This is not modified if the retrieval failed.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
	*/
	bool getIntValue(int& result, const icu::UnicodeString& name, bool casesens=true) const;
	/**
		Get the string at the given index.
		@returns The StringVariable at the index, or NULL if either the index was invalid or the variable at the index wasn't the correct type.
		@param index The index to retrieve.
	*/
	StringVariable* getString(int index);
	const StringVariable* getString(int index) const;
	/**
		Get the string matching the given name.
		@returns The first matching StringVariable, or NULL if no children matched the parameters.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
	*/
	StringVariable* getString(const icu::UnicodeString& name, bool casesens=true);
	const StringVariable* getString(const icu::UnicodeString& name, bool casesens=true) const;
	/**
		Get the string value at the given index.
		The returned string should not be destroyed.
		@returns The string at the index, or NULL if either the index was invalid or the variable at the index wasn't the correct type.
		@param index The index to retrieve.
	*/
	const icu::UnicodeString* getStringValue(int index) const;
	/**
		Get the string value at the given index.
		@returns true on success, or false if either the index was invalid or the variable at the index wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param index The index to retrieve.
	*/
	bool getStringValue(icu::UnicodeString& result, int index) const;
	/**
		Get the string value matching the given name.
		The returned string should not be destroyed.
		@returns true on success, or false if no children matched the parameters.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
	*/
	const icu::UnicodeString* getStringValue(const icu::UnicodeString& name, bool casesens=true) const;
	/**
		Get the string value matching the given name.
		@returns true on success, or false if no children matched the parameters.
		@param result The result. This is not modified if the retrieval failed.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
	*/
	bool getStringValue(icu::UnicodeString& result, const icu::UnicodeString& name, bool casesens=true) const;
	/**
		Get the float at the given index.
		@returns The FloatVariable at the index, or NULL if either the index was invalid or the variable at the index wasn't the correct type.
		@param index The index to retrieve.
	*/
	FloatVariable* getFloat(int index);
	const FloatVariable* getFloat(int index) const;
	/**
		Get the float matching the given name.
		@returns The first matching FloatVariable, or NULL if no children matched the parameters.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
	*/
	FloatVariable* getFloat(const icu::UnicodeString& name, bool casesens=true);
	const FloatVariable* getFloat(const icu::UnicodeString& name, bool casesens=true) const;
	/**
		Get the float value at the given index.
		@returns true on success, or false if either the index was invalid or the variable at the index wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param index The index to retrieve.
	*/
	bool getFloatValue(float& result, int index) const;
	/**
		Get the float value matching the given name.
		@returns true on success, or false if no children matched the parameters.
		@param result The result. This is not modified if the retrieval failed.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
	*/
	bool getFloatValue(float& result, const icu::UnicodeString& name, bool casesens=true) const;
	/**
		Get the bool at the given index.
		@returns The BoolVariable at the index, or NULL if either the index was invalid or the variable at the index wasn't the correct type.
		@param index The index to retrieve.
	*/
	BoolVariable* getBool(int index);
	const BoolVariable* getBool(int index) const;
	/**
		Get the bool matching the given name.
		@returns The first matching BoolVariable, or NULL if no children matched the parameters.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
	*/
	BoolVariable* getBool(const icu::UnicodeString& name, bool casesens=true);
	const BoolVariable* getBool(const icu::UnicodeString& name, bool casesens=true) const;
	/**
		Get the bool value at the given index.
		@returns true on success, or false if either the index was invalid or the variable at the index wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param index The index to retrieve.
	*/
	bool getBoolValue(bool& result, int index) const;
	/**
		Get the bool value matching the given name.
		@returns true on success, or false if no children matched the parameters.
		@param result The result. This is not modified if the retrieval failed.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
	*/
	bool getBoolValue(bool& result, const icu::UnicodeString& name, bool casesens=true);
	/**
		Get the variable at the given index as a string.
		@returns true on success, or false if either the index was invalid or the variable at the index wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param index The index to retrieve.
		@param type Optional type. Since this is only for ValueVariables, the format is masked with VARTYPE_VALUE before searching.
	*/
	bool getAsString(icu::UnicodeString& result, int index, unsigned int type=VARTYPE_VALUE) const;
	/**
		Get the variable matching the given parameters.
		@returns true on success, or false if no variables matched the parameters.
		@param result The result. This is not modified if the retrieval failed.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
	*/
	bool getAsString(icu::UnicodeString& result, const icu::UnicodeString& name, bool casesens=true, unsigned int type=VARTYPE_VALUE) const;
	/**
		Get the identifier at the given index.
		@returns The identifier at the index, or NULL if either index was invalid or the variable at the index was not an identifier.
		@param index The index to retrieve.
	*/
	Identifier* getIdentifier(int index);
	const Identifier* getIdentifier(int index) const;
	/**
		Get the first identifier matching the given name.
		@returns The identifier matching name, or NULL if no identifier matched the name.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
	*/
	Identifier* getIdentifier(const icu::UnicodeString& name, bool casesens=true);
	const Identifier* getIdentifier(const icu::UnicodeString& name, bool casesens=true) const;
	/**
		Get the node at the given index.
		@returns The node at the index, or NULL if either index was invalid or the variable at the index was not a node.
		@param index The index to retrieve.
	*/
	Node* getNode(int index);
	const Node* getNode(int index) const;
	/**
		Get the first node matching the given name.
		@returns The node matching name, or NULL if no node matched the name.
		@param name The name to search for.
		@param casesens Whether to use case-sensitive name comparison.
	*/
	Node* getNode(const icu::UnicodeString& name, bool casesens=true);
	const Node* getNode(const icu::UnicodeString& name, bool casesens=true) const;
	/**
		Add a clone of every variable in this collection to the given collection.
		Deriving classes should call this after cloning.
		@returns Nothing.
	*/
	void cloneChildren(CollectionVariable& dest) const;
	
protected:
	VarList _children;
};

/**
	Identifier.
	Statement-like collection.
*/
class DUCT_API Identifier : public CollectionVariable {
public:
	/**
		Constructor with parent.
		@param parent The parent to initialize with.
	*/
	Identifier(CollectionVariable* parent=NULL);
	/**
		Constructor with name.
		@param name The name to initialize with.
		@param parent The parent to initialize with.
	*/
	Identifier(const icu::UnicodeString& name, CollectionVariable* parent=NULL);
	virtual unsigned int getType() const;
	virtual const char* getTypeName() const;
	virtual Variable* clone() const;
};

/**
	Node.
	Group-like collection.
*/
class DUCT_API Node : public CollectionVariable {
public:
	/**
		Constructor with parent.
		@param parent The parent to initialize with.
	*/
	Node(CollectionVariable* parent=NULL);
	/**
		Constructor with name.
		@param name The name to initialize with.
		@param parent The parent to initialize with.
	*/
	Node(const icu::UnicodeString& name, CollectionVariable* parent=NULL);
	virtual unsigned int getType() const;
	virtual const char* getTypeName() const;
	virtual Variable* clone() const;
};

} // namespace duct

#endif // _DUCT_VARIABLES_HPP

