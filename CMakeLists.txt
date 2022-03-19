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

option(FRT_HAS_STDLIB "Whether or not to build assuming there is a C++ standard library" OFF)
option(FRT_HAS_NEW "Whether or not to generate placement new implementations" OFF)
option(FRT_HAS_COMPARE "Whether to build assuming there is a <compare> header" OFF)
option(FRT_HAS_MEMCPY "Whether to build assuming there is a `memcpy`/`memmove`/`memcmp`/`memset` implementation" OFF)
option(FRT_HAS_ASSERT_FAIL "Whether to build assuming there is a `__frt_assert_fail` implementation" OFF)
option(FRT_HAS_TRIED_THROW "Whether to build assuming there is a `__frt_tried_alloc` implementation" OFF)
option(FRT_HAS_BOUNDS_FAIL "Whether to build assuming there is a `__frt_bounds_fail` implementation" OFF)
option(FRT_DEV "Whether to build with the developer's personal settings" OFF)
option(FRT_TEST "Whether to build the test suite" OFF)

if (FRT_DEV)
    set(FRT_HAS_ASSERT_FAIL OFF)
    set(FRT_HAS_TRIED_THROW OFF)
    set(FRT_HAS_BOUNDS_FAIL OFF)
    set(FRT_HAS_MEMCPY ON)
    set(FRT_HAS_COMPARE ON)
    set(FRT_HAS_NEW ON)
    set(FRT_HAS_STDLIB OFF)
endif ()

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
    endif ()

    if (FRT_HAS_STDLIB)
        target_compile_definitions(${TARGET} PUBLIC FRT_HAVE_STDLIB)
    endif ()

    if (NOT FRT_HAS_NEW)
        target_compile_definitions(${TARGET} PUBLIC FRT_GENERATE_PLACEMENT_NEW)
    endif ()

    if (NOT FRT_HAS_COMPARE)
        target_compile_definitions(${TARGET} PUBLIC FRT_GENERATE_DEFAULT_STD_COMPARE)
    endif ()

    if (NOT FRT_HAS_MEMCPY)
        target_compile_definitions(${TARGET} PUBLIC FRT_GENERATE_DEFAULT_MEM_INTRINS)
    endif ()

    if (NOT FRT_HAS_ASSERT_FAIL)
        target_compile_definitions(${TARGET} PUBLIC FRT_GENERATE_DEFAULT_ASSERT_FAIL)
    endif ()

    if (NOT FRT_HAS_TRIED_THROW)
        target_compile_definitions(${TARGET} PUBLIC FRT_GENERATE_DEFAULT_TRIED_THROW)
    endif ()

    if (NOT FRT_HAS_BOUNDS_FAIL)
        target_compile_definitions(${TARGET} PUBLIC FRT_GENERATE_DEFAULT_BOUNDS_FAIL)
    endif ()

    target_compile_features(${TARGET} PUBLIC cxx_std_20)
endfunction()

add_subdirectory(src)
add_subdirectory(tests)