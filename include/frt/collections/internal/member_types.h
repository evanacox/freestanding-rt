//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#pragma once

#include "../../core/iterators.h"
#include "../../types/basic.h"

namespace frt::internal {
  template <typename T, typename Iter, typename ConstIter> struct ContainerTypes {
    using value_type = T;
    using size_type = frt::usize;
    using difference_type = frt::isize;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = Iter;
    using const_iterator = ConstIter;
    using reverse_iterator = ReverseIterator<iterator>;
    using const_reverse_iterator = ReverseIterator<const_iterator>;
  };

  struct IteratorInit {};

  inline constexpr IteratorInit iterator_init{};
} // namespace frt::internal

#define FRT_USING_CONTAINER_TYPES(Base)                                                                                \
  using value_type = typename Base::value_type;                                                                        \
  using size_type = typename Base::size_type;                                                                          \
  using difference_type = typename Base::difference_type;                                                              \
  using reference = typename Base::reference;                                                                          \
  using const_reference = typename Base::const_reference;                                                              \
  using pointer = typename Base::pointer;                                                                              \
  using const_pointer = typename Base::const_pointer;                                                                  \
  using reverse_iterator = typename Base::reverse_iterator;                                                            \
  using const_reverse_iterator = typename Base::const_reverse_iterator
