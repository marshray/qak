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

include_directories(BEFORE ../include)

add_library( qak STATIC
	atomic.cxx
	mutex.cxx
	now.cxx
	stopwatch.cxx
)

target_link_libraries( qak
	rt ) # for clock_gettime used in now.cxx

add_executable(atomic__test atomic__test.cxx)
target_link_libraries(atomic__test qak)
add_test(atomic__test ${EXECUTABLE_OUTPUT_PATH}/atomic__test)

add_executable(fail__test fail__test.cxx)
target_link_libraries(fail__test qak)
add_test(fail__test ${EXECUTABLE_OUTPUT_PATH}/fail__test)

add_executable(min_max__test min_max__test.cxx)
target_link_libraries(min_max__test qak)
add_test(min_max__test ${EXECUTABLE_OUTPUT_PATH}/min_max__test)

add_executable(mutex__test mutex__test.cxx)
target_link_libraries(mutex__test qak)
add_test(mutex__test ${EXECUTABLE_OUTPUT_PATH}/mutex__test)

add_executable(now__test now__test.cxx)
target_link_libraries(now__test qak)
add_test(now__test ${EXECUTABLE_OUTPUT_PATH}/now__test)

add_executable(optional__test optional__test.cxx)
target_link_libraries(optional__test qak)
add_test(optional__test ${EXECUTABLE_OUTPUT_PATH}/optional__test)

add_executable(stopwatch__test stopwatch__test.cxx)
target_link_libraries(stopwatch__test qak)
add_test(stopwatch__test ${EXECUTABLE_OUTPUT_PATH}/stopwatch__test)

add_executable(vector__test vector__test.cxx)
target_link_libraries(vector__test qak)
add_test(vector__test ${EXECUTABLE_OUTPUT_PATH}/vector__test)
