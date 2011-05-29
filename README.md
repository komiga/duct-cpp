duct++ is an official [duct](http://duct.komiga.com) implementation in C++.

This library has only been tested on Linux (Ubuntu 10.10, 32-bit and 64-bit) and Windows (Visual Studio 2008, 32-bit). It *should* work on Mac OS.

Dependencies:

* [ICU (International Components for Unicode)](http://site.icu-project.org) (duct++ uses ICU 4.4.2 - see the downloads list on the GitHub page)
* [msinttypes](http://code.google.com/p/msinttypes) (headers; required for `stdint.h` when compiling with MSVC 9.0)
