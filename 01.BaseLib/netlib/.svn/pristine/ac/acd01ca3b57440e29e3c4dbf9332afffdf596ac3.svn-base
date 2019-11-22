
LOCAL_PATH:= $(call my-dir)
TOP_PATH:= $(LOCAL_PATH)/../../
SRC_PATH:= $(TOP_PATH)/src/
COMM_PATH:= $(TOP_PATH)/../../00.Common/common/

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
		  $(SRC_PATH)/EpollEventLoop.cpp\
		  $(SRC_PATH)/EventLoop.cpp\
		  $(SRC_PATH)/EventLoopMgr.cpp\
		  $(SRC_PATH)/NetLibCommon.cpp \
		  $(SRC_PATH)/SelectEventLoop.cpp \
		  $(SRC_PATH)/Socket.cpp \
		  $(SRC_PATH)/TcpActiveConn.cpp \
		  $(SRC_PATH)/TcpConn.cpp \
		  $(SRC_PATH)/TcpListenner.cpp \
		  $(SRC_PATH)/TcpListenSocket.cpp \
		  $(SRC_PATH)/TcpPassiveConn.cpp \
		  $(SRC_PATH)/TcpSocket.cpp \
		  $(SRC_PATH)/TcpTransLib.cpp \
		  $(SRC_PATH)/UdpSocket.cpp \
		  $(SRC_PATH)/UdpTrans.cpp \
		  $(SRC_PATH)/UdpTransLib.cpp \
		  $(COMM_PATH)/BaseThread.cpp \
		  $(COMM_PATH)/Utils.cpp \
		  $(COMM_PATH)/TraceLog.cpp \

LOCAL_C_INCLUDES += \
	$(SRC_PATH)/ \
	$(TOP_PATH)/include/ \
	$(TOP_PATH)/interface/ \
	$(COMM_PATH)/ \

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog 

LOCAL_CPPFLAGS := -DHAMMER_TIME=1 \
		  -DHASHNAMESPACE=__gnu_cxx \
		  -DHASH_NAMESPACE=__gnu_cxx \
		  -DDISABLE_DYNAMIC_CAST \
		  -D_REENTRANT \
		  -DANDROID \
		  -DEXPAT_RELATIVE_PATH \
		  -DFEATURE_ENABLE_SSL \
		  -DHAVE_OPENSSL_SSL_H=1 \
		  -DFEATURE_ENABLE_PSTN \
		  -DHAVE_WEBRTC_VOICE=1 \
		  -DLOGGING=1 \
		  -DRAYCOM_ANDROID \

LOCAL_CFLAGS := -fexpensive-optimizations -fpermissive -pthread -DHAVE_NEON=1 \
		-mfpu=neon -mfloat-abi=softfp -flax-vector-conversions -fPIC -D__STDC_CONSTANT_MACROS -Wno-sign-compare -Wno-switch 

LOCAL_MODULE:= libnetlib

include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)

