
## duct++ — it's the bee's knees

**duct++** is a general-purpose C++11 header library which supplies:

* Compiler and platform detection macros.
* Debug message and assertion macros.
* Endian byte-swapping.
* Unicode encoding and string conversion helpers.
* `iostream` helpers for binary and textual data (integrates with the C++
  Standard Library).
* Generic token parsing.
* CVar-esque system with validation, and a markup language.

All it needs is a shiny C++11-compliant compiler and standard library.
That's not too much to ask for, is it? *Is it, Microsoft?*

## Building

Being a header library, duct++ is not compiled directly. However, all features
support Clang 3.3+ with libc++ (SVN head). GCC with libstdc++ is only partially
supported (as of 4.7.3) due to issues in ISO compliance (see *Defects* below).

The tests can be compiled using plash's standard project protocol:
http://komiga.com/pp-cpp

## Defects

The following headers require corrections in libstdc++ (4.7.3):

1. `<duct/IO/multistream.hpp>`:
   `std::vector<T>::assign()` is missing the rvalue ref variant.

## License

duct++ carries the MIT license, meaning you can do whatever you want to with it.

```
Copyright (c) 2010-2014 Tim Howard

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
```
