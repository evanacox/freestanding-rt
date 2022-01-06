# `libfrt`

A "runtime"/utility library for freestanding C++ projects, includes basic utilities for use in freestanding environments

It provides various templates, types, utility functions, and can even conditionally provide things like UBSan stubs or
semi-decent `mem{cpy|move|set|cmp}` implementations if certain macros are defined.

## Compiler Support

Currently, `libfrt` only supports modern versions of GCC/Clang-based compilers due to the intrinsics being used, and
because of the liberal use of C++20 features.

Some intrinsics needed only exist in C++20 mode, and the library uses concepts throughout. C++20 modules are not used
yet, but this may change once they become more stable.

## Platform Support

Nothing is required of the platform in order to compile, besides the following headers:

* `<limits.h>`
* `<stddef.h>`
* `<stdint.h>`

## Architecture Support

`libfrt` supports any architecture that fulfills the following requirements:

* has support from GCC/Clang's `__atomic_*` builtins
* has an 8-bit `char` type
* uses two's complement (this is also a requirement for C++20)
* has support for `__int128` (i.e. architecture is 64-bit)

## License

Licensed under the BSD-3 license. Full text can be found in the [LICENSE](./LICENSE)
file at the root of this project.