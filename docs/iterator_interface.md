# `frt::IteratorInterface`

FRT defines an interface for quickly building standards-compliant iterators without all the normal boilerplate necessary
to do so.

Through the use of CRTP and a few basic operations being provided by the user, the boilerplate for a standards-compliant
iterator can be generated.

```cpp
template <typename Derived,
    typename Type,
    typename Value,
    typename Reference = Value&,
    typename Pointer = Value*,
    typename DiffType = frt::isize>
class IteratorInterface;
```

## Template Parameters

* `Derived`: The actual iterator type, used for CRTP
* `Tag`: Some (FRT) iterator tag type, e.g `frt::InputIteratorTag` or `frt::ContiguousIteratorTag`
* `Value`: The `value_type` for the iterator, e.g `int` or `frt::ubyte`
* `Reference`: The result type of `operator*`, usually `ValueType&` but not always
* `Pointer`: The result type of `operator->`, usually `ValueType*` but not always
* `DiffType`: The type used to measure distances between iterators, probably `frt::isize`.

## Basic Usage

An iterator class that wants to use `frt::IteratorInterface` simply needs to inherit from it (pubically), and give a
tag/value type.

```cpp
class ArrayIterator 
  : public frt::IteratorInterface<ArrayIterator, frt::ForwardIteratorTag, int> {
public:
  ArrayIterator() noexcept = default;

  ArrayIterator(int index, int* array) noexcept : index_{index}, array_{array} {}

  bool eq(ArrayIterator other) const noexcept {
    return other.index_ == index_ 
           && other.array_ == array_;
  }

  int& value() const noexcept {
    return array_[index_];
  } 

  void next() const noexcept {
    ++index_;
  }

private:
  int index_ = 0;
  int* array_ = nullptr;
};
```

That tag type is used to tell FRT what type of iterator to generate boilerplate for, while the value (and any other
template parameters) are used to further specialize the output.

### `frt::ProxyIteratorInterface`

`ProxyIteratorInterface` is an alias of `IteratorInterface` with slightly different defaults, specifically it makes
the `Reference` type the same as the `Value` type. Useful for when `value`
needs to return an actual value. Example: a zip iterator returning a `Pair<T&, U&>`.

## Requirements

A type deriving from `IteratorInterface` must implement a certain set of basic operations that `IteratorInterface` can
use through CRTP, meaning all of these operations **must**
be `public`. These are subject to change and should not be called by any users, these operations (besides the special
member functions) should only be called by FRT library code.

Depending on the properties of the resulting iterator (and the corresponding iterator concept that one wants to fulfill)
, different tag types and different operations need to be provided.

---
**NOTE**

If you want to use the iterator with a sentinel type, you can provide that `operator==` overload yourself. The library
won't prevent you from doing so, but it won't do any magic for you.

---

### Every Iterator

Every iterator must implement move operations, and an `eq` (or `cmp`) method. An iterator that can't be compared to
another is not particularly useful, nor is one that cannot be moved or returned from anything.

```cpp
// every iterator needs to be move-constructible
Derived(Derived&&) /* hopefully noexcept */; 

// every iterator needs to be move-assignable 
Derived& operator=(Derived&&) /* hopefully noexcept */;

// can just be in terms of a defaulted `operator==` if you like
// if `cmp` is present, this is not necessary. 
bool eq(const Derived& rhs) const;
```

### `input_iterator` Operations

Input iterators simply need to yield a value and be able to be incremented.

`next` is roughly translated into `operator++`, while `value` is roughly translated into `operator*`.

```cpp
// return the value that the iterator yields, aka what `operator*` should return
Reference value() const; 

// move the iterator to the next value, aka what `operator++` should do 
void next(); 
```

### `TestForwardIterator` Operations

Forward iterators are effectively just input iterators that also fulfill `frt::Regular`. Since FRT requires all
iterators be `frt::Movable`
and `frt::EqualityComparable`, the only new thing is copy operations.

```cpp
// everything that `input_iterator` requires, along with:

// needs to be `Regular`
Derived(const Derived&);

// needs to be `Regular`
Derived& operator=(const Derived&); 
```

### `TestBidirectionalIterator` Operations

Bidirectional iterators are effectively just forward iterators with
`prev` added on top. `prev` is used to generate `operator--` overloads, and will return the iterator to its previous
state.

Simply put, `a.next(), a.prev()` should leave the iterator as if it hadn't been changed at all.

```cpp
// everything that `TestForwardIterator` requires, along with:

// move to the previous element, opposite of `.next()`
void prev(); 
```

### `TestRandomAccessIterator` Operations

Random access iterators are able to drop much of the previous requirements, and instead use `move` for any move
operations.

`move(1)` is used for `operator++`, and `move(-1)` is used for
`operator--`. `move(n)` is used for `it + n`, `move(-n)` for
`it - n`, etc.

`distance_to` is also used, it calculates the number of steps that are needed to get to `other`. Formally,
if `it1.distance_to(it2)`
returns `n`, `it1.move(n)` must make the iterator equal to `it2`.

`value` has the same requirements as previous types, and the iterator still has to be `frt::Regular`.

`cmp` is the final requirement, it is used to implement comparisons for ordering (probably in the form of `<=>`, but it
may change). This must return `frt::StrongOrder`, and should just return the result of `<=>`ing state that matters. If
this exists, `eq` is no longer required

```cpp
// needs to be `Regular`
Derived(const Derived&);

// needs to be `Regular`
Derived& operator=(const Derived&); 

// OPTIONAL: if it doesnt exist, `operator==` is in terms of 
// `cmp` instead, so it isn't required. only implement when necessary
bool eq(const Derived& rhs) const;

// return the value that the iterator yields, aka what `operator*` should return
Reference value() const; 

// move the iterator by a possibly negative value. 
// `next` can be implemented as `move(1)` while `prev`
// can be implemented as `move(-1)`
void move(DiffType n);

// returns the number of steps to get to `other`, aka
// what number to `move` by to get an iterator equal to `other`
DiffType distance_to(const Derived& other) const;

// preferably just `<=>`s member and returns
frt::StrongOrder cmp(const Derived& other) const;
```

### `TestContiguousIterator` Operations

Contiguous iterators are effectively random iterators with the requirement that their values be stored contiguously, and
because of this, the requirement for `value` is dropped in favor of an `address` member function.

`address` must return a `Pointer` that can be dereferenced, incremented, deincremented, compared and whatnot separately
from the iterator type. Because of this, `operator*` returns `*address()` by default. `address`
will also be used to generate `operator->`.

`value` can still be provided if the user desires (and it will be used in `operator*` if it exists), but prefer
using `address` exclusively when possible.

```cpp
// type needs to be move-constructible
Derived(Derived&&) /* hopefully noexcept */; 

// type needs to be move-assignable 
Derived& operator=(Derived&&) /* hopefully noexcept */;

// needs to be `Regular`
Derived(const Derived&);

// needs to be `Regular`
Derived& operator=(const Derived&); 

// OPTIONAL: if it doesnt exist, `operator==` is in terms of 
// `cmp` instead, so it isn't required. only implement when necessary
bool eq(const Derived& rhs) const;

// returns a pointer to the object in the contiguous storage, 
// pointer should be suitable for `operator->` and for indexing
// by itself. `*address()` is the current element 
Pointer address() const; 

// OPTIONAL
//
// this will be used if it exists, if it doesn't exist, `*address()` is used
// as an alternative. If there's a good reason to have this, implement it 
Reference value() const;

// move the iterator by a possibly negative value. 
// `next` can be implemented as `move(1)` while `prev`
// can be implemented as `move(-1)`
void move(DiffType n);

// returns the number of steps to get to `other`, aka
// what number to `move` by to get an iterator equal to `other`
DiffType distance_to(const Derived& other) const;

// preferably just `<=>`s members and returns
frt::StrongOrder cmp(const Derived& other) const;
```