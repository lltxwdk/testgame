CUR_DIR := $(shell pwd)
OBJ_DIR := $(CUR_DIR)/
STATIC_DIR := $(CUR_DIR)/../../../build/linux/third-party/jsoncpp-0.10.7/lib/64/
#$(shell [ -d ${OBJ_DIR} ] || mkdir -p ${OBJ_DIR})
$(shell [ -d ${STATIC_DIR} ] || mkdir -p ${STATIC_DIR})

ver = debug
ifeq ($(ver), debug)
CFLAGS := -g -Wall -DLINUX
else
CFLAGS := -O2 -Wall -DLINUX  
endif

CC := g++  

LOCAL_SRC_FILES :=  \
      ./jsoncpp-0.10.7/src/lib_json/json_reader.cpp \
      ./jsoncpp-0.10.7/src/lib_json/json_value.cpp \
      ./jsoncpp-0.10.7/src/lib_json/json_writer.cpp \
      
LDFLAGS:= -shared -fPIC

CSRCS :=  \
	   $(LOCAL_SRC_FILES) \

COBJS := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(CSRCS)) 

INCDIR := \
	  -I./jsoncpp-0.10.7/include \
	  -I./jsoncpp-0.10.7/src/lib_json \

TARGET := $(STATIC_DIR)/libjsoncpp.a 

$(TARGET): $(COBJS)
	ar r $(TARGET) $^

$(COBJS):$(OBJ_DIR)/%.o:%.cpp
	$(CC) -c  $< -o $@ $(INCDIR) $(CFLAGS) 
clean:  
	rm -rf *.o  *.a  $(COBJS) $(TARGET)  
