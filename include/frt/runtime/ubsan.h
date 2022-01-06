//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021 Evan Cox <evanacox00@gmail.com>. All rights reserved.      //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#pragma once

#include "../types/basic.h"

namespace frt::ubsan {
  using ValueHandle = frt::usize;

  extern "C" {
    struct UBSanSourceInfo {
      const char* filename;
      //
    };

    struct UBSanTypeDescriptor {
      frt::u16 kind;
      frt::u16 info;
      char type_name[]; // the rest of the name follows the struct
    };

    struct UBSanTypeMismatchData {
      UBSanSourceInfo location;
      UBSanTypeDescriptor* type;
      frt::usize alignment;
      frt::byte type_check_kind;
    };

    ///
    ///
    /// \param data
    /// \param value_handle
    void __ubsan_handle_type_mismatch(UBSanTypeMismatchData* data, ValueHandle value_handle);
  }
} // namespace frt::ubsan