cmake_minimum_required(VERSION 2.6)

project(devmgr)

set( ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../../../../../../)
set( COMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../../../../../../00.Common/common )
set( DEV_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../src/DevMgr )

include_directories( 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/include/ 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/ 
${ROOT_DIR}/00.Common/protobuf/src/protobuf-2.6.1/src/  
${ROOT_DIR}/00.Common/protobuf/proto_define/ 
${ROOT_DIR}/00.Common/common 
${ROOT_DIR}/01.BaseLib/netlib/include 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/redis-3.0.5/include  
${CMAKE_CURRENT_SOURCE_DIR}/../../../sredisdll/include/)

IF(${DBTYPE} STREQUAL "MYSQL_DB_DEF")
IF(${ARCH} STREQUAL "centos-x86_64")
include_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/mysql/64/include/)
ENDIF()
IF(${ARCH} STREQUAL "kylin-aarch64")
include_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/mysql/kylin-aarch64/include/)
ENDIF()
IF(${ARCH} STREQUAL "kylin-x86_64")
include_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/mysql/kylin-x86_64/include/)
ENDIF()
IF(${ARCH} STREQUAL "centos-aarch64")
include_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/mysql/centos-aarch64/include/)
ENDIF()
ENDIF()

IF(${DBTYPE} STREQUAL "SHENTONG_DB_DEF")
include_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/shentong/include/)
ENDIF()

IF(${ARCH} STREQUAL "centos-x86_64")
include_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libevent-2.1.8-stable/linux/64/include)
ENDIF()
IF(${ARCH} STREQUAL "kylin-aarch64")
include_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libevent-2.1.8-stable/linux/kylin-aarch64/include)
ENDIF()
IF(${ARCH} STREQUAL "kylin-x86_64")
include_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libevent-2.1.8-stable/linux/kylin-x86_64/include)
ENDIF()
IF(${ARCH} STREQUAL "centos-aarch64")
include_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libevent-2.1.8-stable/linux/centos-aarch64/include)
ENDIF()

link_directories( 
${ROOT_DIR}/01.BaseLib/netlib/lib/  
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/redis-3.0.5/lib/64/ 
${CMAKE_CURRENT_SOURCE_DIR}/../../../sredisdll/lib/64/)

IF(${ARCH} STREQUAL "centos-x86_64")
link_directories( 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/lib/linux/64bit_X86-64/ 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libevent-2.1.8-stable/linux/64/lib)
ENDIF()
IF(${ARCH} STREQUAL "kylin-aarch64")
link_directories( 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/lib/linux/kylin-aarch64/ 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libevent-2.1.8-stable/linux/kylin-aarch64/lib)
ENDIF()
IF(${ARCH} STREQUAL "kylin-x86_64")
link_directories( 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/lib/linux/kylin-x86_64/ 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libevent-2.1.8-stable/linux/kylin-x86_64/lib)
ENDIF()
IF(${ARCH} STREQUAL "centos-aarch64")
link_directories( 
${ROOT_DIR}/00.Common/protobuf/protobuf-interface/lib/linux/centos-aarch64/ 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/libevent-2.1.8-stable/linux/centos-aarch64/lib)
ENDIF()

IF(${DBTYPE} STREQUAL "MYSQL_DB_DEF")
IF(${ARCH} STREQUAL "centos-x86_64")
link_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/mysql/64/lib)
ENDIF()
IF(${ARCH} STREQUAL "kylin-aarch64")
link_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/mysql/kylin-aarch64/lib)
ENDIF()
IF(${ARCH} STREQUAL "kylin-x86_64")
link_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/mysql/kylin-x86_64/lib)
ENDIF()
IF(${ARCH} STREQUAL "centos-aarch64")
link_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/mysql/centos-aarch64/lib)
ENDIF()
ENDIF()

IF(${DBTYPE} STREQUAL "SHENTONG_DB_DEF")
IF(${ARCH} STREQUAL "centos-x86_64")
link_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/shentong/linux64/)
ENDIF()
IF(${ARCH} STREQUAL "kylin-aarch64")
link_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/shentong/arm64/)
ENDIF()
IF(${ARCH} STREQUAL "kylin-x86_64")
link_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/shentong/linux64/)
ENDIF()
IF(${ARCH} STREQUAL "centos-aarch64")
link_directories( 
${CMAKE_CURRENT_SOURCE_DIR}/../../../third-party/shentong/arm64/)
ENDIF()
ENDIF()

message(${CMAKE_CURRENT_SOURCE_DIR})

file ( GLOB_RECURSE lib_source_files 
"${DEV_SOURCE_DIR}/AutoTrace.cpp" 
"${DEV_SOURCE_DIR}/utility.cpp" 
"${DEV_SOURCE_DIR}/MsgQueue.cpp" 
"${DEV_SOURCE_DIR}/thread.cpp" 
"${DEV_SOURCE_DIR}/thread_mutex.cpp" 
"${DEV_SOURCE_DIR}/asyncthread.cpp" 
"${DEV_SOURCE_DIR}/parser.cpp" 
"${DEV_SOURCE_DIR}/SRProtoConnBase.cpp" 
"${DEV_SOURCE_DIR}/DBconnector.cpp" 
"${DEV_SOURCE_DIR}/timer_manager.cpp" 
"${DEV_SOURCE_DIR}/list_conference.cpp" 
"${DEV_SOURCE_DIR}/list_confparticipant.cpp" 
"${DEV_SOURCE_DIR}/list_confreport.cpp" 
"${DEV_SOURCE_DIR}/list_device.cpp" 
"${DEV_SOURCE_DIR}/list_deviceconfdetail.cpp" 
"${DEV_SOURCE_DIR}/list_deviceconfig.cpp" 
"${DEV_SOURCE_DIR}/list_mediagroup.cpp" 
"${DEV_SOURCE_DIR}/list_userconfdetail.cpp" 
"${DEV_SOURCE_DIR}/list_confrecord.cpp"
"${DEV_SOURCE_DIR}/list_recordfile.cpp"
"${DEV_SOURCE_DIR}/list_conflivesetting.cpp"
"${DEV_SOURCE_DIR}/list_confliveinfo.cpp"
"${DEV_SOURCE_DIR}/list_confrollcallinfo.cpp"
"${DEV_SOURCE_DIR}/list_rollcalllist.cpp"
"${DEV_SOURCE_DIR}/list_confpollinfo.cpp"
"${DEV_SOURCE_DIR}/list_polllist.cpp"
"${DEV_SOURCE_DIR}/list_datadictionary.cpp"
"${DEV_SOURCE_DIR}/list_sysparameter.cpp"
"${DEV_SOURCE_DIR}/wrapper_msg.cpp" 
"${DEV_SOURCE_DIR}/ActiveConnect.cpp" 
"${DEV_SOURCE_DIR}/deviceConnect.cpp" 
"${DEV_SOURCE_DIR}/deviceConnMgr.cpp" 
"${DEV_SOURCE_DIR}/HttpConnMgr.cpp" 
"${DEV_SOURCE_DIR}/DevMgr.cpp")
file ( GLOB_RECURSE common_source_files 
"${COMMON_SOURCE_DIR}/BaseThread.cpp" 
"${COMMON_SOURCE_DIR}/Buffer.cc" 
"${COMMON_SOURCE_DIR}/LpConfig.c" 
"${COMMON_SOURCE_DIR}/PackDef.cc" 
"${COMMON_SOURCE_DIR}/ParsePdu.cc" 
"${COMMON_SOURCE_DIR}/SRProtoParser.cpp" 
"${COMMON_SOURCE_DIR}/TraceLog.cpp" 
"${COMMON_SOURCE_DIR}/Common.c" 
"${COMMON_SOURCE_DIR}/Utils.cpp"  )
add_executable(devmgr ${lib_source_files} ${common_source_files}  ${ROOT_DIR}/00.Common/protobuf/protobuf-interface/output/device_ser.pb.cc )

set( CMAKE_BUILE_TYPE DEBUG )

IF(${DBTYPE} STREQUAL "MYSQL_DB_DEF")
add_definitions(-g -Wall -DLINUX -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -DMYSQL_DB_DEF) 
ELSEIF(${DBTYPE} STREQUAL "SHENTONG_DB_DEF")
add_definitions(-g -Wall -DLINUX -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -DSHENTONG_DB_DEF) 
ENDIF()

target_link_libraries(devmgr rt)
target_link_libraries(devmgr pthread)
target_link_libraries(devmgr netlib)
target_link_libraries(devmgr protobuf protoc)
target_link_libraries(devmgr sredisdll hiredis)
#target_link_libraries(devmgr hiredis sredisdll)
target_link_libraries(devmgr dl)
IF(${DBTYPE} STREQUAL "MYSQL_DB_DEF")
target_link_libraries(devmgr mysqlclient)
ENDIF()
target_link_libraries(devmgr event)
target_link_libraries(devmgr event_openssl)
target_link_libraries(devmgr event_pthreads)

IF(${DBTYPE} STREQUAL "SHENTONG_DB_DEF")
target_link_libraries(devmgr acci aci) 
ENDIF()