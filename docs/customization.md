# Generated Intrinsics & Customization

FRT will generate simple definitions for intrinsic(-ish) functions and types that are needed for use in the library.
These can be toggled on/off by setting CMake options when building the library with CMake, or by defining macros when
including the headers directly.

FRT also provides a way to override/customize those intrinsic types and functions if desired in your environment.

> Note: Some of the following is technically "UB" from opening up `std` and using reserved symbols. I argue that it's not
> actually UB, due to the fact that this library plays the role of an "implementation" where `std` doesn't exist :)

Every instance of `FRT_HAS_THING` is the name of a **CMake** variable that can be toggled `ON`/`OFF`.

#### Side note: Namespacing

Any code generated inside of `std` that is not directly implementing part of the `std` interface is placed inside
of `std::__frt`.

Any code generated that would interfere with global scope is namespaced with `__frt_`.

## `FRT_HAS_STDLIB`: Building with `std`

If you have a standard library implementation present, FRT can work with it.

Define the `FRT_HAVE_STDLIB` macro, and FRT will try to include C++ standard headers and use the types / functions
within. Parts of the library that are simply re-implementations of the stdlib will 'melt away' into wrappers.

## `FRT_HAS_COMPARE`: Whether to use `<compare>`

FRT uses three-way-comparison (`operator<=>`) internally very liberally, and thus needs to ensure that
three-way-comparison is actually supported in the target environment. Some freestanding C++ 'implementations' do not
include the `<compare>` header, thus FRT gives the option of generating an implementation for itself.

If the macro `FRT_GENERATE_DEFAULT_STD_COMPARE` is defined, FRT generates a GCC/Clang-compatible version
of `std::strong_ordering`, `std::weak_ordering`
and `std::partial_ordering` inside of `platform/compare.h`. No other parts of `<compare>` are generated, only those
types and their associated overloads/constants and whatnot.

If this macro is not defined, `<compare>` is included. It must contain functional forms of the above 3 types at a
minimum.

The constant `frt::generated_compare` can be queried at build time to see which mode the library is built in.

## `FRT_HAS_MEMCPY`: Whether to implement `mem{cpy, move, set, cmp}`

GCC requires that the 4 core memory functions (`memcpy`, `memmove`, `memset` and `memcmp`) exist at link-time and have
the "usual" signatures/behavior, and this is basically impossible to completely disable. For convenience, FRT can
generate functional implementations of these functions if an optimized set for your environment are not available.

If `FRT_GENERATE_DEFAULT_MEM_INTRINS` is defined, the 4 memory functions are generated as calls to the
associated `__frt_mem_*` functions.

If it is not defined, an implementation for all 4 of those symbols must be provided at link-time.

The constant `frt::generated_memory_intrinsics` can be queried at build time to see which mode the library is built in.

## `FRT_HAS_ASSERT_FAIL`: Whether ot implement `__frt_assert_fail`

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

## `FRT_HAS_TRIED_THROW`: Whether to implement `__frt_tried_alloc`

`__frt_tried_alloc` is a library intrinsic called whenever the library tries to allocate memory from an allocator, and
the allocator determines this should not happen. This is not to be confused with allocation *failure*, this is meant for
situations like "stack allocator is exhausted but more memory was requested."

The function has the following signature:

```cpp
extern "C" [[noreturn]] void __frt_tried_alloc(const char* /* non-null null-terminated message string */, 
                                             frt::CSourceLocation) noexcept;
```

If `FRT_GENERATE_DEFAULT_TRIED_THROW` is defined, FRT will generate a default implementation of this function that
contains a call to `__builtin_trap`.

If it is not defined, an implementation for the symbol must be provided at link-time.

The constant `frt::generated_tried_alloc` can be queried at build time to see which mode the library is built in.

## `FRT_HAS_BOUNDS_FAIL`: Whether to implement `__frt_bounds_fail`

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
