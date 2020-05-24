cmake_minimum_required(VERSION 2.6)

project(mcrun)

set( ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../)
set( COMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../00.Common/common )
set( MC_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src )

IF(${ARCH} STREQUAL "centos-x86_64")
include_directories( 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/include/ 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/ 
${ROOT_DIR}/00.Common/protobuf/src/protobuf-2.6.1/src/  
${ROOT_DIR}/00.Common/protobuf/proto_define/ 
${ROOT_DIR}/00.Common/common 
${ROOT_DIR}/00.Common/mediacommon 
${ROOT_DIR}/01.BaseLib/netlib/include 
${MC_SOURCE_DIR}/crypt_4_0_sdk/centos )
link_directories( 
${ROOT_DIR}/01.BaseLib/netlib/lib/ 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/lib/linux/64bit_X86-64/ 
${MC_SOURCE_DIR}/crypt_4_0_sdk/centos/x86_64)
ENDIF()

IF(${ARCH} STREQUAL "centos-aarch64")
include_directories( 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/include/ 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/ 
${ROOT_DIR}/00.Common/protobuf/src/protobuf-2.6.1/src/  
${ROOT_DIR}/00.Common/protobuf/proto_define/ 
${ROOT_DIR}/00.Common/common 
${ROOT_DIR}/00.Common/mediacommon 
${ROOT_DIR}/01.BaseLib/netlib/include 
${MC_SOURCE_DIR}/crypt_4_0_sdk/centos )
link_directories( 
${ROOT_DIR}/01.BaseLib/netlib/lib/ 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/lib/linux/centos-aarch64/ 
${MC_SOURCE_DIR}/crypt_4_0_sdk/centos/aarch64)
ENDIF()

IF(${ARCH} STREQUAL "kylin-aarch64")
include_directories( 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/include/ 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/ 
${ROOT_DIR}/00.Common/protobuf/src/protobuf-2.6.1/src/  
${ROOT_DIR}/00.Common/protobuf/proto_define/ 
${ROOT_DIR}/00.Common/common 
${ROOT_DIR}/00.Common/mediacommon 
${ROOT_DIR}/01.BaseLib/netlib/include 
${MC_SOURCE_DIR}/crypt_4_0_sdk/kylin )
link_directories( 
${ROOT_DIR}/01.BaseLib/netlib/lib/ 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/lib/linux/kylin-aarch64/ 
${MC_SOURCE_DIR}/crypt_4_0_sdk/kylin/aarch64)
ENDIF()

IF(${ARCH} STREQUAL "kylin-x86_64")
include_directories( 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/include/ 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/ 
${ROOT_DIR}/00.Common/protobuf/src/protobuf-2.6.1/src/  
${ROOT_DIR}/00.Common/protobuf/proto_define/ 
${ROOT_DIR}/00.Common/common 
${ROOT_DIR}/00.Common/mediacommon 
${ROOT_DIR}/01.BaseLib/netlib/include 
${MC_SOURCE_DIR}/crypt_4_0_sdk/kylin )
link_directories( 
${ROOT_DIR}/01.BaseLib/netlib/lib/ 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/lib/linux/kylin-x86_64/ 
${MC_SOURCE_DIR}/crypt_4_0_sdk/kylin/x86_64)
ENDIF()

message(${CMAKE_CURRENT_SOURCE_DIR})

file ( GLOB_RECURSE lib_source_files 
"${MC_SOURCE_DIR}/asyncthread.cpp" 
"${MC_SOURCE_DIR}/clientConnect.cpp" 
"${MC_SOURCE_DIR}/deviceManager.cpp" 
"${MC_SOURCE_DIR}/devmgrConnect.cpp" 
"${MC_SOURCE_DIR}/hardwareInfo.cpp" 
"${MC_SOURCE_DIR}/ini_configuration.cpp" 
"${MC_SOURCE_DIR}/MD5.cpp" 
"${MC_SOURCE_DIR}/sr_common.cpp" 
"${MC_SOURCE_DIR}/sr_decrypt.cpp"
"${MC_SOURCE_DIR}/sr_mpdevprocess.cpp" 
"${MC_SOURCE_DIR}/sr_tersmsgprocess.cpp" 
"${MC_SOURCE_DIR}/SRProtoConnBase.cpp" 
"${MC_SOURCE_DIR}/terminalMgr.cpp" 
"${MC_SOURCE_DIR}/thread.cpp" 
"${MC_SOURCE_DIR}/thread_mutex.cpp" 
"${MC_SOURCE_DIR}/timer_manager.cpp" 
"${MC_SOURCE_DIR}/MC.cpp")
file ( GLOB_RECURSE common_source_files 
"${COMMON_SOURCE_DIR}/BaseThread.cpp" 
"${COMMON_SOURCE_DIR}/LpConfig.c" 
"${COMMON_SOURCE_DIR}/SRProtoParser.cpp" 
"${COMMON_SOURCE_DIR}/TraceLog.cpp" 
"${COMMON_SOURCE_DIR}/Common.c" 
"${COMMON_SOURCE_DIR}/Utils.cpp" )
add_executable(mcrun 
${lib_source_files} 
${common_source_files}  
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/mc_ter.pb.cc 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/mc_ter_plus.pb.cc 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/mc_gw.pb.cc 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/mc_netmp.pb.cc 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/mc_mp.pb.cc 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/mc_crs.pb.cc 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/mc_relayserver.pb.cc 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/device_ser.pb.cc )

set( CMAKE_BUILE_TYPE DEBUG )

add_definitions(-g -Wall -DLINUX -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function) 
target_link_libraries(mcrun rt)
target_link_libraries(mcrun pthread)
target_link_libraries(mcrun netlib)
target_link_libraries(mcrun CryptoWrapper)
target_link_libraries(mcrun protobuf protoc)