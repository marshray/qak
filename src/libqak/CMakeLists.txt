#=================================================================================================|
#
#   Copyright (c) 2011-2012, Marsh Ray
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
	host_info.cxx
	mutex.cxx
	now.cxx
	permutation.cxx
	rptr.cxx
	rotate_sequence.cxx
	stopwatch.cxx
	static_data.cxx
	thread.cxx
	thread_group.cxx
	ucs.cxx
)
target_link_libraries( qak
	rt ) # for clock_gettime used in now.cxx

#	Test test_app itself first.
add_executable(test_app__test test_app__test.cxx)
target_link_libraries(test_app__test qak)
add_test(test_app__test ${EXECUTABLE_OUTPUT_PATH}/test_app__test)

#	Everything else in alphabetical order.

add_executable(atomic__test atomic__test.cxx)
target_link_libraries(atomic__test qak)
add_test(atomic__test ${EXECUTABLE_OUTPUT_PATH}/atomic__test)

add_executable(bitsizeof__test bitsizeof__test.cxx)
target_link_libraries(bitsizeof__test qak)
add_test(bitsizeof__test ${EXECUTABLE_OUTPUT_PATH}/bitsizeof__test)

add_executable(fail__test fail__test.cxx)
target_link_libraries(fail__test qak)
add_test(fail__test ${EXECUTABLE_OUTPUT_PATH}/fail__test)

add_executable(hash__test hash__test.cxx)
target_link_libraries(hash__test qak)
add_test(hash__test ${EXECUTABLE_OUTPUT_PATH}/hash__test)

add_executable(host_info__test host_info__test.cxx)
target_link_libraries(host_info__test qak)
add_test(host_info__test ${EXECUTABLE_OUTPUT_PATH}/host_info__test)

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

add_executable(permutation__test permutation__test.cxx)
target_link_libraries(permutation__test qak)
add_test(permutation__test ${EXECUTABLE_OUTPUT_PATH}/permutation__test)

add_executable(prng64__test prng64__test.cxx)
target_link_libraries(prng64__test qak)
add_test(prng64__test ${EXECUTABLE_OUTPUT_PATH}/prng64__test)

add_executable(rotate_sequence__test rotate_sequence__test.cxx)
target_link_libraries(rotate_sequence__test qak)
add_test(rotate_sequence__test ${EXECUTABLE_OUTPUT_PATH}/rotate_sequence__test)

add_executable(rptr__test rptr__test.cxx)
target_link_libraries(rptr__test qak)
add_test(rptr__test ${EXECUTABLE_OUTPUT_PATH}/rptr__test)

add_executable(stopwatch__test stopwatch__test.cxx)
target_link_libraries(stopwatch__test qak)
add_test(stopwatch__test ${EXECUTABLE_OUTPUT_PATH}/stopwatch__test)

add_executable(thread__test thread__test.cxx)
target_link_libraries(thread__test qak)
add_test(thread__test ${EXECUTABLE_OUTPUT_PATH}/thread__test)

add_executable(thread_group__test thread_group__test.cxx)
target_link_libraries(thread_group__test qak)
add_test(thread_group__test ${EXECUTABLE_OUTPUT_PATH}/thread_group__test)

add_executable(ucs__test ucs__test.cxx)
target_link_libraries(ucs__test qak)
add_test(ucs__test ${EXECUTABLE_OUTPUT_PATH}/ucs__test)

add_executable(vector__test vector__test.cxx)
target_link_libraries(vector__test qak)
add_test(vector__test ${EXECUTABLE_OUTPUT_PATH}/vector__test)
