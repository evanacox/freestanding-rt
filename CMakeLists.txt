##======---------------------------------------------------------------======##
#                                                                             #
# Copyright 2022 Evan Cox <evanacox00@gmail.com>                              #
#                                                                             #
# Use of this source code is governed by a BSD-style license that can be      #
# found in the LICENSE.txt file at the root of this project, or at the        #
# following link: https://opensource.org/licenses/BSD-3-Clause                #
#                                                                             #
##======---------------------------------------------------------------======##

cmake_minimum_required(VERSION 3.20)
project(libfrt)

function(frt_configure_target TARGET)
    list(APPEND FRT_WARNINGS)

    if (${MSVC})
        list(APPEND FRT_WARNINGS /W3)
    else ()
        list(APPEND FRT_WARNINGS
                -Wall
                -Wextra
                -Wcast-qual
                -Wconversion-null
                -Wmissing-declarations
                -Woverlength-strings
                -Wpointer-arith
                -Wunused-local-typedefs
                -Wunused-result
                -Wvarargs
                -Wvla
                -Wwrite-strings
                -Werror
                -ftemplate-backtrace-limit=20)
    endif ()

    # enable warnings and warnings-as-errors in development
    # there's no reason not to, and its off by default for normal use
    if (FRT_DEV)
        target_compile_options(${TARGET} PRIVATE ${FRT_WARNINGS})
        message(STATUS "adding ${FRT_WARNINGS} to ${TARGET}")
    endif ()

    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        target_compile_definitions(${TARGET} PUBLIC FRT_DEBUG)
    endif ()

    target_compile_features(${TARGET} PUBLIC cxx_std_20)
endfunction()

add_library(libfrt
        src/rt.cc
        src/runtime/ubsan.cc
        src/runtime/assert_runtime.cc
        src/sync/spin_mutex.cc
        src/core/memory.cc)
frt_configure_target(libfrt)
target_include_directories(libfrt PUBLIC include)