cmake_minimum_required(VERSION 2.6)

project(libnetlib)

set( COMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../00.Common/common )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../../../../00.Common/common ${CMAKE_CURRENT_SOURCE_DIR}/../../include )

message(${CMAKE_CURRENT_SOURCE_DIR})

file ( GLOB_RECURSE lib_source_files "${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.*" )
file ( GLOB_RECURSE common_source_files "${COMMON_SOURCE_DIR}/*.cpp" )

add_library(netlib STATIC ${lib_source_files} ${COMMON_SOURCE_DIR}/BaseThread.cpp ${COMMON_SOURCE_DIR}/TraceLog.cpp)

#set( CMAKE_BUILE_TYPE DEBUG )
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb ")
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${target_dir}/lib)
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../lib )
set( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib )
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${target_dir}/bin))
set(CMAKE_VERBOSE_MAKEFILE ON)

IF(RELEASE EQUAL 1)
set( CMAKE_BUILD_TYPE RELEASE )
add_definitions(-c -O3 -Wall -fPIC -DLINUX -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function)
ELSE()
set( CMAKE_BUILD_TYPE DEBUG )
add_definitions(-g -Wall -fPIC -DLINUX -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function)
ENDIF()


