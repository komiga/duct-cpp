/**
@file ArgHandling.hpp
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

duct++ argument handling.
*/

#ifndef DUCT_ARGHANDLING_HPP_
#define DUCT_ARGHANDLING_HPP_

#include <duct/config.hpp>
#include <duct/Variables.hpp>
#include <duct/UniStrArray.hpp>

#include <list>

namespace duct {

// Forward declarations
//enum CallType;
class ArgumentHandler;
class ArgImpl;

/**
	ArgImpl* list.
*/
typedef std::list<ArgImpl*> ArgImplList;

/**
	ArgImpl call type enum.
	Flags 0x08 and 0x10 are reserved.
*/
enum CallType {
	/** Initialized state (meaningless in operation). */
	CALLTYPE_NONE=0x00,
	/** Option type (e.g. '--help'). */
	CALLTYPE_OPTION=0x01,
	/** Command type (e.g. 'help'). */
	CALLTYPE_COMMAND=0x02,
	/** Switch type (e.g. "-a"). */
	CALLTYPE_SWITCH=0x04,
	/** Reserved type 0x08. */
	_CALLTYPE_RESERVED2=0x08,
	/** Reserved type 0x10. */
	_CALLTYPE_RESERVED3=0x10
};

/**
	Parse the given arguments.
	The expected value does <em>not</em> (by default) contain the first argument of standard arguments (the application location).
	You can override this by passing fullargs as true. If you leave it as false, the root identifier's name will be set to the first argument (which should be the application location with standard arguments).
	@returns: The root identifier containing the parsed arguments, or NULL if the given argument count is less than 1.
	@param optarglimit Limits how many arguments can be given to an option (options start with "--" or "-"). If set to -1, there is no limit.
	Single-dash options (e.g. "-a") are not parsed for arguments, whereas double-dash options (e.g. "--foo bar") will be.
*/
DUCT_API Identifier* parseArgs(int argc, char const** argv, bool fullargs=true, int optarglimit=1);

/**
	Argument handler.
	This class takes ownership of all ArgImpl pointers given to it.
*/
class DUCT_API ArgumentHandler {
public:
	/**
		Constructor.
	*/
	ArgumentHandler();
	/**
		Destructor.
	*/
	~ArgumentHandler();
	/**
		Get the begin iterator for the argument handler.
		@returns The begin iterator.
	*/
	ArgImplList::iterator begin();
	ArgImplList::const_iterator begin() const;
	/**
		Get the end iterator for the argument handler.
		@returns The end iterator.
	*/
	ArgImplList::iterator end();
	ArgImplList::const_iterator end() const;
	/**
		Find an implementation with the given alias.
		@returns The iterator for the implementation with the given alias, or end() if the given alias was not found.
		@param alias The alias to search for.
	*/
	ArgImplList::iterator find(icu::UnicodeString const& alias);
	ArgImplList::const_iterator find(icu::UnicodeString const& alias) const;
	/**
		Add an implementation to the handler.
		The handler takes ownership of the given pointer.
		@returns true if the implementation was added, or false if the implementation was either null or had an empty alias array.
		@param impl The implementation to add.
	*/
	bool addImpl(ArgImpl* impl);
	/**
		Get the argument implementation with the given alias.
		@returns The argument implementation with the given alias, or NULL if there is no argument implementation with the given alias.
		@param alias The alias to search for.
	*/
	ArgImpl* getImpl(icu::UnicodeString const& alias);
	/**
		Clear the handler.
		This will free all argument implementations in the handler.
		@returns Nothing.
	*/
	void clear();
	
protected:
	ArgImplList m_list;
};

/**
	Argument implementation.
*/
class DUCT_API ArgImpl {
public:
	/**
		Aliases constructor.
	*/
	ArgImpl();
	/**
		Destructor.
	*/
	virtual ~ArgImpl();
	/**
		Set the implementation's current call type.
		@returns Nothing.
		@param calltype The new call type.
	*/
	void setCallType(unsigned int calltype);
	/**
		Get the current call type.
		@returns The implementation's current call type.
	*/
	unsigned int getCallType() const;
	/**
		Get the implementation's aliases.
		@returns The implementation's aliases.
	*/
	StringArray& getAliases();
	/**
		Set the current arguments.
		The implementation does not take ownership of the given identifier.
		@returns Nothing.
		@param args The argument identifier.
	*/
	void setArgs(Identifier* args);
	/**
		Get the current arguments.
		@returns The implementation's current arguments.
	*/
	Identifier* getArgs();
	/**
		Check if the implementation has the given alias.
		@returns true if the implementation has the given alias, or false if it does not.
		@param alias The alias to search for.
	*/
	bool hasAlias(icu::UnicodeString const& alias) const;
	/**
		Check the current arguments for errors.
		@returns 0 on success, or some error code on failure.
	*/
	virtual int checkErrors()=0;
	/**
		Execute the argument.
		@returns 0 on success, or some error code on failure.
	*/
	virtual int execute()=0;
	/**
		Get the implementation's usage string.
		@returns The implementation's usage string.
	*/
	virtual icu::UnicodeString const& getUsage() const=0;
	
protected:
	unsigned int m_calltype;
	StringArray m_aliases;
	Identifier* m_args;
};

} // namespace duct

#endif // DUCT_ARGHANDLING_HPP_
