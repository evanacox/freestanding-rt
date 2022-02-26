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

namespace frt {
  struct InputIteratorTag {};
  struct ForwardIteratorTag : InputIteratorTag {};
  struct BidirectionalIteratorTag : ForwardIteratorTag {};
  struct RandomIteratorTag : BidirectionalIteratorTag {};
  struct ContiguousIteratorTag : RandomIteratorTag {};
  struct OutputIteratorTag {};

  namespace internal {
    using InputTag = InputIteratorTag;
    using ForwardTag = ForwardIteratorTag;
    using BidirectionTag = BidirectionalIteratorTag;
    using RandomTag = RandomIteratorTag;
    using ContiguousTag = ContiguousIteratorTag;
    using OutputTag = OutputIteratorTag;
  } // namespace internal
} // namespace frt