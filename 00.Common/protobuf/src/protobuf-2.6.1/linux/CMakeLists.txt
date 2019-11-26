cmake_minimum_required(VERSION 2.6)

project(libprotobuf)

set( SRC_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_SOURCE_DIR}/../src )

message(${CMAKE_CURRENT_SOURCE_DIR})

#file ( GLOB_RECURSE lib_source_files "${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.*" )
#file ( GLOB_RECURSE common_source_files "${COMMON_SOURCE_DIR}/*.cpp" )

add_library(protobuf STATIC  
					${SRC_SOURCE_DIR}/src/google/protobuf/io/coded_stream.cc                  
					${SRC_SOURCE_DIR}/src/google/protobuf/stubs/common.cc                     
					${SRC_SOURCE_DIR}/src/google/protobuf/stubs/stringprintf.cc               
					${SRC_SOURCE_DIR}/src/google/protobuf/descriptor.cc                       
					${SRC_SOURCE_DIR}/src/google/protobuf/descriptor.pb.cc                    
					${SRC_SOURCE_DIR}/src/google/protobuf/descriptor_database.cc              
					${SRC_SOURCE_DIR}/src/google/protobuf/dynamic_message.cc                  
					${SRC_SOURCE_DIR}/src/google/protobuf/extension_set.cc                    
					${SRC_SOURCE_DIR}/src/google/protobuf/extension_set_heavy.cc              
					${SRC_SOURCE_DIR}/src/google/protobuf/generated_message_reflection.cc     
					${SRC_SOURCE_DIR}/src/google/protobuf/generated_message_util.cc           
					${SRC_SOURCE_DIR}/src/google/protobuf/io/gzip_stream.cc                   
					${SRC_SOURCE_DIR}/src/google/protobuf/io/strtod.cc                        
					${SRC_SOURCE_DIR}/src/google/protobuf/compiler/importer.cc                
					${SRC_SOURCE_DIR}/src/google/protobuf/message.cc                          
					${SRC_SOURCE_DIR}/src/google/protobuf/message_lite.cc                     
					${SRC_SOURCE_DIR}/src/google/protobuf/stubs/once.cc                       
					${SRC_SOURCE_DIR}/src/google/protobuf/compiler/parser.cc                  
					${SRC_SOURCE_DIR}/src/google/protobuf/io/printer.cc                       
					${SRC_SOURCE_DIR}/src/google/protobuf/reflection_ops.cc                   
					${SRC_SOURCE_DIR}/src/google/protobuf/repeated_field.cc                   
					${SRC_SOURCE_DIR}/src/google/protobuf/service.cc                          
					${SRC_SOURCE_DIR}/src/google/protobuf/stubs/structurally_valid.cc         
					${SRC_SOURCE_DIR}/src/google/protobuf/stubs/strutil.cc                    
					${SRC_SOURCE_DIR}/src/google/protobuf/stubs/substitute.cc                 
					${SRC_SOURCE_DIR}/src/google/protobuf/text_format.cc                    
					${SRC_SOURCE_DIR}/src/google/protobuf/io/tokenizer.cc                     
					${SRC_SOURCE_DIR}/src/google/protobuf/unknown_field_set.cc                
					${SRC_SOURCE_DIR}/src/google/protobuf/wire_format.cc                      
					${SRC_SOURCE_DIR}/src/google/protobuf/wire_format_lite.cc                 
					${SRC_SOURCE_DIR}/src/google/protobuf/io/zero_copy_stream.cc              
					${SRC_SOURCE_DIR}/src/google/protobuf/io/zero_copy_stream_impl.cc         
					${SRC_SOURCE_DIR}/src/google/protobuf/io/zero_copy_stream_impl_lite.cc  
					)				

set( CMAKE_BUILE_TYPE DEBUG )
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb ")
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${target_dir}/lib)
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../lib )
set( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib )
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${target_dir}/bin))

add_definitions(-g -Wall -DLINUX -std=c++11 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function)
