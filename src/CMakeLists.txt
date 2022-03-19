##======---------------------------------------------------------------======##
#                                                                             #
# Copyright 2022 Evan Cox <evanacox00@gmail.com>                              #
#                                                                             #
# Use of this source code is governed by a BSD-style license that can be      #
# found in the LICENSE.txt file at the root of this project, or at the        #
# following link: https://opensource.org/licenses/BSD-3-Clause                #
#                                                                             #
##======---------------------------------------------------------------======##

add_library(frt
        ./platform/new.cc
        ./runtime/ubsan.cc
        ./runtime/assert.cc
        ./runtime/failures.cc
        ./sync/spin_mutex.cc
        ./sync/ticket_mutex.cc
        ./core/memory.cc)
frt_configure_target(frt)
target_include_directories(frt PUBLIC ../include)
