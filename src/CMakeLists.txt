#=================================================================================================|
#
#   Copyright (c) 2011, Marsh Ray
#
#   Permission to use, copy, modify, and/or distribute this software for any
#   purpose with or without fee is hereby granted, provided that the above
#   copyright notice and this permission notice appear in all copies.
#
#   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
#   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
#   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
#   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
#   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
#   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
#   OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#=================================================================================================|

cmake_minimum_required (VERSION 2.6)

project(QAK)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	set(DEBREL debug)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
	set(DEBREL release)
else()
	message(FATAL_ERROR "need CMAKE_BUILD_TYPE defined")
endif()

#	Decide if we're going to use a precompiled header.
#if (    CMAKE_CXX_COMPILER MATCHES "g\\+\\+-4\\.[456789]\\.[0123456789]$"
#     OR CMAKE_CXX_COMPILER MATCHES "g\\+\\+-[56789]\\.[0123456789]\\.[0123456789]$" )
#	message(STATUS "Using precompiled headers with '${CMAKE_CXX_COMPILER}'")
#	set(USE_PRECOMPILED_HEADERS 1)
#else()
#	message(STATUS "Not using precompiled headers with '${CMAKE_CXX_COMPILER}'")
	unset(USE_PRECOMPILED_HEADERS)
#endif()
#
#if(${USE_PRECOMPILED_HEADERS})
#	#	Add the include directory via flags because we really need it to come first,
#	#	even before those include_directories(BEFORE) which may run later.
#	message(STATUS "CMAKE_CXX_FLAGS: '${CMAKE_CXX_FLAGS}'")
#	set(CMAKE_CXX_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/include-pch ${CMAKE_CXX_FLAGS}")
#	message(STATUS "CMAKE_CXX_FLAGS: '${CMAKE_CXX_FLAGS}'")
#endif()

enable_testing()

include(TestBigEndian)
TEST_BIG_ENDIAN(BIG_ENDIAN)
if (BIG_ENDIAN)
	add_definitions(-DQAK_BIG_ENDIAN)
else()
	add_definitions(-DQAK_LITTLE_ENDIAN)
endif()

#if (${USE_PRECOMPILED_HEADERS})
#	add_subdirectory (pch)
#endif()

add_subdirectory (libqak)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake "set(CTEST_OUTPUT_ON_FAILURE 1)\n")
