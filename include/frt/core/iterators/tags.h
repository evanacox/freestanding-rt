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

#ifdef FRT_HAVE_STDLIB
#include <iterator>
#endif

namespace frt {
#ifdef FRT_HAVE_STDLIB
  using InputIteratorTag = std::input_iterator_tag;
  using ForwardIteratorTag = std::forward_iterator_tag;
  using BidirectionalIteratorTag = std::bidirectional_iterator_tag;
  using RandomIteratorTag = std::random_access_iterator_tag;
  using ContiguousIteratorTag = std::contiguous_iterator_tag;
  using OutputIteratorTag = std::output_iterator_tag;
#else
  struct InputIteratorTag {};
  struct ForwardIteratorTag : InputIteratorTag {};
  struct BidirectionalIteratorTag : ForwardIteratorTag {};
  struct RandomAccessIteratorTag : BidirectionalIteratorTag {};
  struct ContiguousIteratorTag : RandomAccessIteratorTag {};
  struct OutputIteratorTag {};
#endif

  namespace internal {
    using InputTag = InputIteratorTag;
    using ForwardTag = ForwardIteratorTag;
    using BidirectionTag = BidirectionalIteratorTag;
    using RandomTag = RandomAccessIteratorTag;
    using ContiguousTag = ContiguousIteratorTag;
    using OutputTag = OutputIteratorTag;
  } // namespace internal
} // namespace frt