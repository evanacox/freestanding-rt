# `libfrt`

A "runtime"/utility library for freestanding C++ projects, includes basic utilities for use in freestanding environments

It provides various templates, types, utility functions, and can even conditionally provide things like UBSan stubs or
semi-decent `mem{cpy|move|set|cmp}` implementations if certain macros are defined.

## Compiler Support

Currently, `libfrt` only supports GCC/Clang-based compilers due to the intrinsics being used, and because of the liberal
use of C++20 features.

## Platform Support

Nothing is required of the platform in order to compile, besides the following headers:

* `<limits.h>`
* `<stddef.h>`
* `<stdint.h>`

## Architecture Support

`libfrt` supports any architecture that fulfills the following requirements:

* has support from GCC/Clang's `__atomic_*` builtins
* has an 8-bit `char` type
* has support for `{u}int64_t`, whether emulated or native (although it won't use them internally, it will use
  whatever `size_t` is)

## License

Licensed under the BSD-3 license. Full text can be found in the [LICENSE](./LICENSE)
file at the root of this project.