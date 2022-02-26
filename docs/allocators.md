# Allocators

Allocators *mostly* follow the `std` design, with one exception: allocators act like values instead of acting like
references.

```cpp
auto f() {
    frt::Vec<int, frt::StackAllocator<int, 512>> v;
    
    return v;
}

void g() {
    // this is perfectly fine! `v`'s allocator owns its
    // own memory instead of keeping a reference to an allocator.
    auto v = f();
}
```

This gives the obvious advantages, but has an issue: how do you share allocators between multiple containers?

You simply use an allocator adapter that gives reference semantics!

```cpp
using Alloc = frt::StackAllocator<int, 4096>;

Alloc arena;

// both vectors use the same arena
frt::Vec<int, frt::AllocRef<Alloc>> v; 
frt::Vec<int, frt::AllocRef<Alloc>> v2;
```