##======---------------------------------------------------------------======##
#                                                                             #
# Copyright 2022 Evan Cox <evanacox00@gmail.com>                              #
#                                                                             #
# Use of this source code is governed by a BSD-style license that can be      #
# found in the LICENSE.txt file at the root of this project, or at the        #
# following link: https://opensource.org/licenses/BSD-3-Clause                #
#                                                                             #
##======---------------------------------------------------------------======##

include(FetchContent)

function(frt_get_gtest)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/main.zip
    )

    # hack for windows, see GTest tutorial
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
endfunction()

set(FRT_TESTS_CORE core/bit.cc core/iterators/iterator_traits.cc)
set(FRT_TESTS_TYPES types/concepts.cc
        types/invoke.cc
        types/basic.cc
        types/traits/other.cc
        types/traits/common_type.cc
        types/traits/invoke_result.cc
        types/traits/cv.cc
        types/traits/array.cc
        types/traits/logical.cc
        types/traits/common_reference.cc)
set(FRT_TESTS_UTILITY utility/swap.cc utility/defer.cc utility/as.cc types/traits/convertible_to.cc)

if (${FRT_TESTS})
    message(STATUS "Configuring tests")

    # 'playground' executable
    add_executable(frt_playground ./playground.cc)
    frt_configure_target(frt_playground)
    target_link_libraries(frt_playground frt)
    target_include_directories(frt_playground PRIVATE ./)

    # download GTest
    frt_get_gtest()
    enable_testing()

    # create test suite
    add_executable(frt_tests ${FRT_TESTS_CORE} ${FRT_TESTS_TYPES} ${FRT_TESTS_UTILITY})
    frt_configure_target(frt_tests)
    target_link_libraries(frt_tests frt gtest_main gmock)
    target_include_directories(frt_tests PRIVATE ./)

    # discover tests so we can use it with CTest
    include(GoogleTest)
    gtest_discover_tests(frt_tests)
endif ()