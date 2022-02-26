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
        message(STATUS "adding ${FRT_WARNINGS} to ${TARGET}")
        message(STATUS "adding FRT_GENERATE definitions to ${TARGET}")
        target_compile_options(${TARGET} PRIVATE ${FRT_WARNINGS})
        target_compile_definitions(${TARGET} PUBLIC
                FRT_GENERATE_DEFAULT_ASSERT_FAIL
                FRT_GENERATE_DEFAULT_TRIED_THROW
                FRT_GENERATE_DEFAULT_STD_COMPARE)
    endif ()

    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        target_compile_definitions(${TARGET} PUBLIC FRT_DEBUG)
    endif ()

    target_compile_features(${TARGET} PUBLIC cxx_std_20)
endfunction()

add_library(frt
        src/rt.cc
        src/runtime/ubsan.cc
        src/runtime/assert.cc
        src/runtime/failures.cc
        src/sync/spin_mutex.cc
        src/sync/ticket_mutex.cc
        src/core/memory.cc)
frt_configure_target(frt)
target_include_directories(frt PUBLIC include)