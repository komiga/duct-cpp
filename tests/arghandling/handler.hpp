
#include <duct/arghandling.hpp>

#ifndef __TESTS_HANDLER_HPP__
#define __TESTS_HANDLER_HPP__

using namespace duct;

class HelpImpl : public ArgImpl {
public:
	HelpImpl();
	int checkErrors();
	int execute();
	icu::UnicodeString const& getUsage() const;
};

class TestImpl : public ArgImpl {
public:
	TestImpl();
	int checkErrors();
	int execute();
	icu::UnicodeString const& getUsage() const;
};

#endif // __TESTS_HANDLER_HPP__
