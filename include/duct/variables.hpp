/**
@file variables.hpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010 Tim Howard

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

duct++ Variable classes.
*/

#ifndef _DUCT_VARIABLES_HPP
#define _DUCT_VARIABLES_HPP

#include <duct/config.hpp>
#include <unicode/uchar.h>
#include <unicode/unistr.h>
#include <list>

namespace duct {

/**
	Base Variable types.
	0x1 through 0x80 are reserved types.
*/
enum VariableType {
	/**
		#IntVariable.
	*/
	VARTYPE_INTEGER = 0x1,
	/**
		#StringVariable.
	*/
	VARTYPE_STRING = 0x2,
	/**
		#FloatVariable.
	*/
	VARTYPE_FLOAT = 0x4,
	/**
		#BoolVariable.
	*/
	VARTYPE_BOOL = 0x8,
	/**
		Reserved type 0x10.
	*/
	_VARTYPE_RESERVED0 = 0x10,
	/**
		Reserved type 0x20.
	*/
	_VARTYPE_RESERVED1 = 0x20,
	/**
		#Identifier.
	*/
	VARTYPE_IDENTIFIER = 0x40,
	/**
		#Node.
	*/
	VARTYPE_NODE = 0x80,
	/**
		No variable type.
		Special variable type (means 'no variable type'). Alias for 0x0 (no flags).
		@see VARTYPE_ANY.
	*/
	VARTYPE_NONE = 0x0,
	/**
		Special value for variable searching.
		Means 'any variable type'.
		See CollectionVariable.findWithType().
	*/
	VARTYPE_ANY = 0xFFFFFFFF,
	/**
		Special type for ValueVariables.
	*/
	VARTYPE_VALUE = VARTYPE_INTEGER | VARTYPE_STRING | VARTYPE_FLOAT | VARTYPE_BOOL
};

/**
	Variable value/name format.
	Formatting flags for Variable names and ValueVariable values.
*/
enum ValueFormat {
	/**
		Value quote-always format flag.
		This flag is for any variable type. The output will always have quotes around it.
	*/
	FMT_VALUE_QUOTE_ALWAYS = 0x1,
	
	/**
		String quote-whitespace format flag.
		This format will quote a string containing whitespace or newlines. e.g. "foo bar~t" -> "\"foo bar~t\"".
	*/
	FMT_STRING_QUOTE_WHITESPACE = 0x10,
	/**
		String quote-empty format flag.
		This format will quote an empty string. e.g. "" -> "\"\"".
	*/
	FMT_STRING_QUOTE_EMPTY = 0x20,
	/**
		String quote-bool format flag.
		This format will quote a string if it equals "true" or "false" as a type safeguard. e.g. "true" -> "\"true\"".
	*/
	FMT_STRING_SAFE_BOOL = 0x40,
	/**
		String quote-number format flag.
		This format will quote a string if it is a number as a type safeguard. e.g. "1234.5678" -> "\"1234.5678\"".
	*/
	FMT_STRING_SAFE_NUMBER = 0x80,
	/**
		String safe format flag.
		Consists of #FMT_STRING_SAFE_BOOL and #FMT_STRING_SAFE_NUMBER
	*/
	FMT_STRING_SAFE = FMT_STRING_SAFE_BOOL | FMT_STRING_SAFE_NUMBER,
	/**
		Default string format flag.
		Consists of #FMT_STRING_SAFE, #FMT_STRING_QUOTE_WHITESPACE and #FMT_STRING_QUOTE_EMPTY.
	*/
	FMT_STRING_DEFAULT = FMT_STRING_SAFE | FMT_STRING_QUOTE_WHITESPACE | FMT_STRING_QUOTE_EMPTY,
	
	/**
		Float truncate format flag.
		This format will remove repeated numbers at the end of a float. e.g. "0.123400000" -> "0.1234".
	*/
	FMT_FLOAT_TRUNCATE = 0x100,
	/**
		Default float format flag.
		Consists of #FMT_FLOAT_TRUNCATE.
	*/
	FMT_FLOAT_DEFAULT = FMT_FLOAT_TRUNCATE,
	
	/**
		Boolean string format flag.
		Convert the boolean value to a string ("true", "false"). e.g. False -> "false", True -> "true".
	*/
	FMT_BOOL_STRING = 0x1000,
	/**
		Default boolean format flag.
		Consists of #FMT_BOOL_STRING.
	*/
	FMT_BOOL_DEFAULT = FMT_BOOL_STRING,
	
	/**
		Default name format flag.
		Consists of #FMT_STRING_SAFE, #FMT_STRING_QUOTE_WHITESPACE and #FMT_STRING_QUOTE_EMPTY.
	*/
	FMT_NAME_DEFAULT = FMT_STRING_SAFE | FMT_STRING_QUOTE_WHITESPACE | FMT_STRING_QUOTE_EMPTY,
	
	/**
		Default int format flag.
		Unset flag (no formatting).
	*/
	FMT_INTEGER_DEFAULT = 0,
	
	/**
		Default format flag for any variable.
		Consists of all default format flags: #FMT_STRING_DEFAULT, #FMT_FLOAT_DEFAULT, #FMT_BOOL_DEFAULT and #FMT_INTEGER_DEFAULT.
	*/
	FMT_ALL_DEFAULT = FMT_STRING_DEFAULT | FMT_FLOAT_DEFAULT | FMT_BOOL_DEFAULT | FMT_INTEGER_DEFAULT,
	
	/**
		No-format flag.
	*/
	FMT_NONE = 0
};

// Forward declarations
class CollectionVariable;
class ValueVariable;

/**
	Variable class.
*/
class DUCT_API Variable {
public:
	virtual ~Variable();
	
	/**
		Set the variable's name.
		@returns Nothing.
		@param name The new name.
	*/
	virtual void setName(const UnicodeString& name);
	/**
		Get the variable's name.
		@returns The variable's name.
	*/
	virtual const UnicodeString& getName() const;
	/**
		Get the variable's name with the given format.
		@returns Nothing.
		@param result The variable to store the formatted name.
		@param format The formatting flags.
	*/
	virtual void getNameFormatted(UnicodeString& result, unsigned int format = FMT_NAME_DEFAULT) const;
	
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
	virtual unsigned int getType() const = 0;
	/**
		Get the variable's type as a string.
		e.g. "string", "int", etc.
		@returns A const char pointer to the variable's type.
	*/
	virtual const char* getTypeAsString() const = 0;
	
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
	static signed int stringToBool(const UnicodeString& source);
	
	/**
		Convert the given string into a ValueVariable.
		If the given source string is empty, an empty StringVariable will be returned with the given variable name.
		@returns The value, or NULL if an error occurred.
		@param source The string to convert.
		@param varname The variable name for the resulting value.
		@param type Optional type-shortcut. Possible values for base ValueVariable types can be found in the #VariableType enum.
	*/
	static ValueVariable* stringToValue(const UnicodeString& source, const UnicodeString& varname, unsigned int type = VARTYPE_NONE);
	/**
		Convert the given string into a ValueVariable.
		If the given source string is empty, an empty StringVariable will be returned.
		@returns The value, or NULL if an error occurred.
		@param source The string to convert.
		@param type Optional type-shortcut. Possible values for base ValueVariable types can be found in the #VariableType enum.
	*/
	static ValueVariable* stringToValue(const UnicodeString& source, unsigned int type = VARTYPE_NONE);
	
protected:
	UnicodeString _name;
	CollectionVariable* _parent;
};

/**
	Value variable.
*/
class DUCT_API ValueVariable : public Variable {
public:
	virtual ~ValueVariable();
	
	/**
		Set the variable's value from the given string.
		@returns Nothing.
		@param value The string to set the variable from.
	*/
	virtual void setFromString(const UnicodeString& source) = 0;
	/**
		Get the variable's value as a string with the given format.
		@returns Nothing.
		@param result The variable to store the formatted value.
		@param format The formatting flags.
	*/
	virtual void getValueFormatted(UnicodeString& result, unsigned int format = FMT_ALL_DEFAULT) const = 0;
	/**
		Get an unformatted string conversion of the variable.
		@returns The variable's value as an unformatted string.
		@param result The variable to store the result.
		@param append Whether to append or replace the result string. Default is false.
	*/
	virtual void valueAsString(UnicodeString& result, bool append = false) const = 0;
};

/**
	Variable pointer list.
*/
typedef std::list<Variable*> VarList;

/**
	Collection variable.
	WARNING: All variables added to the collection are <em>owned</em> by the collection.
	Variables will be deallocated when removed and when the collection gets destroyed.
	As such, you should avoid passing statically-allocated variables, and should avoid deallocation of variables passed to the collection.
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
	/**
		Get the collection's child list.
		@returns The collection's child list.
	*/
	const VarList& getChildren() const;
	/**
		Get the number of children in the collection.
		@returns The number of children in the collection.
	*/
	size_t getChildCount() const;
	
	/**
		Get the const start iterator for the collection.
		@returns Start of Variable iterator for the collection.
	*/
	VarList::const_iterator begin() const;
	/**
		Get the const end iterator for the collection.
		@returns End of Variable iterator for the collection.
	*/
	VarList::const_iterator end() const;
	/**
		Get the start iterator for the collection.
		@returns Start of Variable iterator for the collection.
	*/
	VarList::iterator begin();
	/**
		Get the end iterator for the collection.
		@returns End of Variable iterator for the collection.
	*/
	VarList::iterator end();
	
	/**
		Find the given variable.
		@returns The iterator for the given variable, or the end iterator if the variable was not found.
		@param variable The variable to search for.
	*/
	VarList::iterator find(Variable* variable);
	/**
		Find the given variable.
		@returns The iterator for the given variable, or the end iterator if the variable was not found.
		@param variable The variable to search for.
		@param iter Beginning iterator.
	*/
	VarList::iterator find(Variable* variable, VarList::iterator iter);
	
	/**
		Find the given variable type.
		@returns The iterator for the first matching variable, or the end iterator if the type was not found.
		@param type The variable type to search for. VARTYPE_ALL will result in the first variable being returned (if the collection has children). See #VariableType.
	*/
	VarList::iterator findWithType(unsigned int type);
	/**
		Find the given variable type.
		@returns The iterator for the first matching variable, or the end iterator if the type was not found.
		@param type The variable type to search for. VARTYPE_ALL will result in the first variable being returned (if the collection has children). See #VariableType.
		@param iter Beginning iterator.
	*/
	VarList::iterator findWithType(unsigned int type, VarList::iterator iter);
	
	/**
		Find a variable with the given name and type.
		@returns The iterator for the variable with the given name and type, or the end iterator if the variable was not found.
		@param name The name to search for.
		@param casesens Case-sensitive name checking. Case-sensitivity is on by default.
		@param type Optional variable-type to search for. VARTYPE_ANY is the default.
	*/
	VarList::iterator findWithName(const UnicodeString& name, bool casesens = true, unsigned int type = VARTYPE_ANY);
	/**
		Find a variable with the given name.
		@returns The iterator for the variable with the given name and type, or the end iterator if the variable was not found.
		@param name The name to search for.
		@param iter Beginning iterator.
		@param casesens Case-sensitive name checking. Case-sensitivity is on by default.
		@param type Optional variable-type to search for. VARTYPE_ANY is the default.
	*/
	VarList::iterator findWithName(const UnicodeString& name, VarList::iterator iter, bool casesens = true, unsigned int type = VARTYPE_ANY);
	
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
		@returns true if the variable was added, or false if the given variable was NULL.
		@param variable The variable to add.
	*/
	bool addVariable(Variable* variable);
	
	/**
		Remove the given variable from the list.
		@returns true if the variable was removed, or false if the given variable was not found.
		@param variable The variable to remove.
	*/
	bool removeVariable(Variable* variable);
	/**
		Remove the given variable type from the list.
		This will only remove the first matching variable found.
		@returns true if a variable with the given type was removed, or false if the type was not found.
		@param type The variable type to remove. See #VariableType.
	*/
	bool removeVariableWithType(unsigned int type);
	/**
		Remove a variable with the given name and type.
		@returns true if a variable with the given name was removed, or false if the name was not found.
		@param name The name of the variable to remove.
		@param casesens Case-sensitive name checking. Default is true.
		@param type Optional variable-type to search for. VARTYPE_ANY is the default.
	*/
	bool removeVariableWithName(const UnicodeString& name, bool casesens = true, unsigned int type = VARTYPE_ANY);
	
	/**
		Get a variable with the given name and type.
		@returns The first variable matching the given name, or NULL if the name was not found.
		@param name The variable name to search for.
		@param casesens Case-sensitive name checking. Default is true.
		@param type Optional variable-type to search for. VARTYPE_ANY is the default.
	*/
	Variable* getVariableWithName(const UnicodeString& name, bool casesens = true, unsigned int type = VARTYPE_ANY);
	
	/**
		Get the variable at the given index.
		@returns The variable at the given index, or NULL if either the given index was out of bounds or the variable at the given index did not match the given variable type.
		@param index The index to retrieve.
		@param type Variable type to retrieve. If the given variable type does not match the variable type at the given index, NULL will be returned.
	*/
	Variable* get(int index, unsigned int type = VARTYPE_ANY);
	const Variable* get(int index, unsigned int type = VARTYPE_ANY) const;
	
	/**
		Get an integer from the given index.
		@returns true if the integer at the given index was retrieved, or false if the given index was either out-of-bounds or the variable was not an IntVariable.
		@param index The index to retrieve.
		@param result The result. This is not changed if the retrieval failed.
	*/
	bool getInt(int index, int& result) const;
	
	/**
		Get a string from the given index.
		@returns The string at the given index, or NULL if the given index was either out-of-bounds or the variable was not a StringVariable.
		@param index The index to retrieve.
	*/
	const UnicodeString* getString(int index) const;
	/**
		Get a string from the given index.
		@returns true if the integer at the given index was retrieved, or false if the given index was either out-of-bounds or the variable was not an IntVariable.
		@param index The index to retrieve.
		@param result The result. This is not changed if the retrieval failed.
	*/
	bool getString(int index, UnicodeString& result) const;
	
	/**
		Get the variable at the given index as a string.
		@returns true if the variable at the given index was retrieved, or false if the given index was out-of-bounds.
		@param index The index to retrieve.
		@param result The result. This is not changed if the retrieval failed.
		@param format Optional formatting flags. Default is none.
	*/
	bool getAsString(int index, UnicodeString& result, unsigned int format = FMT_NONE) const;
	
	/**
		Get a float from the given index.
		@returns true if the float at the given index was retrieved, or false if the given index was either out-of-bounds or the variable was not a FloatVariable.
		@param index The index to retrieve.
		@param result The result. This is not changed if the retrieval failed.
	*/
	bool getFloat(int index, float& result) const;
	
	/**
		Get a bool from the given index.
		@returns true if the bool at the given index was retrieved, or false if the given index was either out-of-bounds or the variable was not a BoolVariable.
		@param index The index to retrieve.
		@param result The result. This is not changed if the retrieval failed.
	*/
	bool getBool(int index, bool& result) const;
	
protected:
	VarList _children;
};

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
	IntVariable(int value = 0, CollectionVariable* parent = NULL);
	/**
		Constructor with name and value.
		@param value The value to initialize with.
		@param name The name to initialize with.
		@param parent The parent to initialize with.
	*/
	IntVariable(int value, const UnicodeString& name, CollectionVariable* parent = NULL);
	
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
	
	virtual void setFromString(const UnicodeString& source);
	virtual void getValueFormatted(UnicodeString& result, unsigned int format = FMT_ALL_DEFAULT) const;
	virtual void valueAsString(UnicodeString& result, bool append = false) const;
	
	virtual unsigned int getType() const;
	virtual const char* getTypeAsString() const;
	
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
	StringVariable(CollectionVariable* parent = NULL);
	/**
		Constructor with value and parent.
		@param value The value to initialize with.
		@param parent The parent to initialize with.
	*/
	StringVariable(const UnicodeString& value, CollectionVariable* parent = NULL);
	/**
		Constructor with name, value and parent.
		@param value The value to initialize with.
		@param name The name to initialize with.
		@param parent The parent to initialize with.
	*/
	StringVariable(const UnicodeString& value, const UnicodeString& name, CollectionVariable* parent = NULL);
	
	/**
		Check if the string is numerical.
		@returns True if the string is numerical.
		@param allowdecimal Allow demical. Default is true.
	*/
	bool isNumeric(bool allowdecimal = true) const;
	
	/**
		Set the variable's value.
		@returns Nothing.
		@param value The new value.
	*/
	virtual void set(const UnicodeString& value);
	/**
		Get the variable's value.
		@returns The variable's value.
	*/
	virtual const UnicodeString& get() const;
	
	virtual void setFromString(const UnicodeString& source);
	virtual void getValueFormatted(UnicodeString& result, unsigned int format = FMT_ALL_DEFAULT) const;
	virtual void valueAsString(UnicodeString& result, bool append = false) const;
	
	virtual unsigned int getType() const;
	virtual const char* getTypeAsString() const;
	
protected:
	UnicodeString _value;
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
	FloatVariable(float value = 0.0, CollectionVariable* parent = NULL);
	/**
		Constructor with name, value and parent.
		@param value The value to initialize with.
		@param name The name to initialize with.
		@param parent The parent to initialize with.
	*/
	FloatVariable(float value, const UnicodeString& name, CollectionVariable* parent = NULL);
	
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
	
	virtual void setFromString(const UnicodeString& source);
	virtual void getValueFormatted(UnicodeString& result, unsigned int format = FMT_ALL_DEFAULT) const;
	virtual void valueAsString(UnicodeString& result, bool append = false) const;
	
	virtual unsigned int getType() const;
	virtual const char* getTypeAsString() const;
	
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
	BoolVariable(bool value = false, CollectionVariable* parent = NULL);
	/**
		Constructor with name, value and parent.
		@param value The value to initialize with.
		@param name The name to initialize with.
		@param parent The parent to initialize with.
	*/
	BoolVariable(bool value, const UnicodeString& name, CollectionVariable* parent = NULL);
	
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
	
	virtual void setFromString(const UnicodeString& source);
	virtual void getValueFormatted(UnicodeString& result, unsigned int format = FMT_ALL_DEFAULT) const;
	virtual void valueAsString(UnicodeString& result, bool append = false) const;
	
	virtual unsigned int getType() const;
	virtual const char* getTypeAsString() const;
	
protected:
	bool _value;
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
	Identifier(CollectionVariable* parent = NULL);
	/**
		Constructor with name.
		@param name The name to initialize with.
		@param parent The parent to initialize with.
	*/
	Identifier(const UnicodeString& name, CollectionVariable* parent = NULL);
	
	virtual unsigned int getType() const;
	virtual const char* getTypeAsString() const;
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
	Node(CollectionVariable* parent = NULL);
	/**
		Constructor with name.
		@param name The name to initialize with.
		@param parent The parent to initialize with.
	*/
	Node(const UnicodeString& name, CollectionVariable* parent = NULL);
	
	virtual unsigned int getType() const;
	virtual const char* getTypeAsString() const;
};

} // namespace duct

#endif // _DUCT_VARIABLES_HPP

