
## Building

The tests require premake 4.4+[1] and the latest precore[2].

Once those are in-place, the steps are roughly:

1. Set the `PRECORE_ROOT` environment variable to precore's root
directory.
2. Run `premake4 [options] ACTION` where `ACTION` is `clean` or your
system's toolchain (e.g., `gmake` or `xcode`).
3. Build with system toolchain.

Clang can be used with `--clang` and the stdlib can optionally be
selected with `--stdlib=name`. If `--clang` is used, the default
stdlib will be `stdc++` on Linux, and `c++` on Mac OS. 

## Links

1. [premake](http://industriousone.com/premake)
2. [precore](https://github.com/komiga/precore)
