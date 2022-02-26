# Generated Intrinsics & Customization

FRT will generate simple definitions for intrinsic(-ish) functions and types that are needed for use in the library.
These can be toggled on/off by defining macros when compiling the library itself and when compiling code that uses FRT
headers.

FRT also provides a way to override/customize those intrinsic types and functions if desired in your environment.

> Note: Some of the following is technically "UB" from opening up `std` and using reserved symbols. I argue that it's not
> actually UB, due to the fact that this library plays the role of an "implementation" where `std` doesn't exist :)

## Namespacing

Any code generated inside of `std` that is not directly implementing part of the `std` interface is placed inside
of `std::__frt`.

Any code generated that would interfere with global scope is namespaced with `__frt_`.

## `<compare>` and `FRT_GENERATE_DEFAULT_STD_COMPARE`

FRT uses three-way-comparison (`operator<=>`) internally very liberally, and thus needs to ensure that
three-way-comparison is actually supported in the target environment. Some freestanding C++ 'implementations' do not
include the `<compare>` header, thus FRT gives the option of generating an implementation for itself.

If this macro is defined, FRT generates a GCC/Clang-compatible version of `std::strong_ordering`, `std::weak_ordering`
and `std::partial_ordering` inside of `platform/compare.h`. No other parts of `<compare>` are generated, only those
types and their associated overloads/constants and whatnot.

If this macro is not defined, `<compare>` is included. It must contain functional forms of the above 3 types at a
minimum.

The constant `frt::generated_compare` can be queried at build time to see which mode the library is built in.

## `mem{cpy, move, set, cmp}` and `FRT_GENERATE_DEFAULT_MEM_INTRINS`

GCC requires that the 4 core memory functions (`memcpy`, `memmove`, `memset` and `memcmp`) exist at link-time and have
the "usual" signatures/behavior, and this is basically impossible to completely disable. For convenience, FRT can
generate functional implementations of these functions if an optimized set for your environment are not available.

If `FRT_GENERATE_DEFAULT_MEM_INTRINS` is defined, the 4 memory functions are generated as calls to the
associated `__frt_mem_*` functions.

If it is not defined, an implementation for all 4 of those symbols must be provided at link-time.

The constant `frt::generated_memory_intrinsics` can be queried at build time to see which mode the library is built in.

## `__frt_assert_fail` and `FRT_GENERATE_DEFAULT_ASSERT_FAIL`

`__frt_assert_fail` is a library intrinsic called whenever the library detects an assertion failure internally. The
function has the following signature:

```cpp
extern "C" [[noreturn]] void __frt_assert_fail(const char* /* non-null null-terminated condition string */, 
                                               const char* /* non-null null-terminated message string */, 
                                               frt::CSourceLocation) noexcept;
```

If `FRT_GENERATE_DEFAULT_ASSERT_FAIL` is defined, FRT will generate a default implementation of this function that
contains a call to `__builtin_trap`.

If it is not defined, an implementation for the symbol must be provided at link-time.

The constant `frt::generated_assert_fail` can be queried at build time to see which mode the library is built in.

## `__frt_tried_alloc` and `FRT_GENERATE_DEFAULT_TRIED_THROW`

`__frt_tried_alloc` is a library intrinsic called whenever the library tries to allocate memory from an allocator, and
the allocator determines this should not happen. This is not to be confused with allocation *failure*, this is meant for
situations like "stack allocator is exhausted but more memory was requested."

The function has the following signature:

```cpp
extern "C" [[noreturn]] void __frt_tried_alloc(const char* /* non-null null-terminated message string */, 
                                             frt::CSourceLocation) noexcept;
```

If `FRT_GENERATE_DEFAULT_ASSERT_FAIL` is defined, FRT will generate a default implementation of this function that
contains a call to `__builtin_trap`.

If it is not defined, an implementation for the symbol must be provided at link-time.

The constant `frt::generated_tried_alloc` can be queried at build time to see which mode the library is built in.

## `__frt_bounds_fail` and `FRT_GENERATE_DEFAULT_BOUNDS_FAIL`

`__frt_bounds_fail` is a library intrinsic called whenever the library is executing bounds-checked code and an
out-of-bounds situation is encountered.

The function has the following signature:

```cpp
extern "C" [[noreturn]] void __frt_bounds_fail(const char* /* non-null null-terminated message string */, 
                                             frt::CSourceLocation) noexcept;
```

If `FRT_GENERATE_DEFAULT_BOUNDS_FAIL` is defined, FRT will generate a default implementation of this function that
contains a call to `__builtin_trap`.

If it is not defined, an implementation for the symbol must be provided at link-time.

The constant `frt::generated_bounds_fail` can be queried at build time to see which mode the library is built in.
