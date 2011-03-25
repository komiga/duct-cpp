/**
@file arghandling.cpp
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

duct++ argument handling implementation.
*/

#include <duct/arghandling.hpp>

namespace duct {

Identifier* parseArgs(int argc, const char** argv, bool fullargs, int optarglimit) {
	if (argc<1)
		return NULL;
	Identifier* root=new Identifier();
	int i=0, length=argc-1;
	if (fullargs)
		root->setName(UnicodeString(argv[i++]));
	UnicodeString arg;
	Identifier* sub;
	bool subset=false;
	optarglimit=(optarglimit==-1) ? length : optarglimit;
	for (; i<=length; ++i) {
		arg=UnicodeString(argv[i]);
		sub=new Identifier(arg, NULL);
		if (arg.length()>0 && arg[0]=='-') {
			if (arg.length()>1 && arg[1]=='-') {
				int lim=(length<(i+optarglimit)) ? length : (i+optarglimit);
				i++;
				while (i<=length) {
					arg=UnicodeString(argv[i]);
					if (arg.length()>0 || arg[0]!='-') {
						sub->add(Variable::stringToValue(arg));
						i++;
						if (i>lim) {
							i--;
							break;
						}
					} else {
						i--;
						break;
					}
				}
			}
			root->add(sub);
		} else {
			if (!subset) {
				root->add(sub);
				root=sub;
				subset=true;
			} else {
				root->add(Variable::stringToValue(arg));
				delete sub;
				sub=NULL;
			}
		}
	}
	while (root->getParent()!=NULL) {
		root=(Identifier*)root->getParent();
	}
	return root;
}

} // namespace duct

