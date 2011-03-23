/**
@file arghandling.hpp
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

duct++ argument handling.
*/

#ifndef _DUCT_ARGHANDLING_HPP
#define _DUCT_ARGHANDLING_HPP

#include <duct/config.hpp>
#include <duct/variables.hpp>

namespace duct {

/**
	Parse the given arguments.
	The expected value does <em>not</em> (by default) contain the first argument of standard arguments (the application location).
	You can override this by passing fullargs as true. If you leave it as false, the root identifier's name will be set to the first argument (which should be the application location with standard arguments).
	@returns: The root identifier containing the parsed arguments, or NULL if the given argument count is less than 1.
	@param optarglimit Limits how many arguments can be given to an option (options start with "--" or "-"). If set to -1, there is no limit.
	Single-dash options (e.g. "-a") are not parsed for arguments, whereas double-dash options (e.g. "--foo bar") will be.
*/
DUCT_API Identifier* parseArgs(int argc, const char** argv, bool fullargs=true, int optarglimit=1);

} // namespace duct

#endif // _DUCT_ARGHANDLING_HPP

