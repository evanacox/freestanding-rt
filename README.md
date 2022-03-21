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

* has an 8-bit `char` type
* uses two's complement (this is also a requirement for C++20)

## License

All library headers and source code are licensed under the BSD-3 license. Full text can be found in
the [LICENSE](./LICENSE) file at the root of this project.

Note that the **test suite** (in `./tests/`) includes some code that is licensed under different conditions (portions
are modified code from the LLVM libc++ test suite). The test suite code is completely independent of the library and is
not included, linked against, or used in any way whatsoever by the main library, and is not even built unless a CMake
option has been explicitly set to `ON`.

Any test files that use modified LLVM code have a notice at the top of the file clearly stating that, and they have the
LLVM license file header in addition to the standard FRT one. The LLVM (and therefore libc++) are licensed under the
Apache License v2.0 with an LLVM-specific exception, the full license can be found [here](https://llvm.org/LICENSE.txt).
The LLVM-specific exception is at the bottom of the page.