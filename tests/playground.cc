//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#include "frt/frt.h"
#include <iostream>
#include <vector>

int main() {
  std::vector<int> vec{1, 2, 3, 4, 5};

  static_assert(std::indirectly_readable<frt::Projected<decltype(vec)::iterator, frt::Identity>>);
  static_assert(frt::IndirectlyReadable<frt::Projected<decltype(vec)::iterator, frt::Identity>>);

  std::cout << frt::any_of(vec, [](int x) {
    return x > 5;
  });
}