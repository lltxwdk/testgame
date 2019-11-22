cmake_minimum_required(VERSION 2.6)

project(socketserver)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/../../../00.Common/common )

set(CMAKE_BUILE_TYPE DEBUG)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-g -Wall -DLINUX)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)
add_executable(test ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test_multithread_send.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test_normal.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test_echoserver.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../../../00.Common/common/Utils.cpp)
target_link_libraries(test netlib.a pthread  rt)
