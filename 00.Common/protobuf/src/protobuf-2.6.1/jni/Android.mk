include $(CLEAR_VARS)

USE_POC_THIRD_PARTY := 1
USE_POC_API :=1

#-----------------------------Third-party libraries -------------------------------
ifeq ($(USE_POC_THIRD_PARTY),1)
include $(callserver-root-dir)/Android.mk
endif
#----------------------------Third-party libraries end----------------------------


#----------------------------MediaCore and CallServer----------------------------

#include $(callserver-root-dir)/srengine/dependant/android_so/Android.mk

#----------------------------MediaCore and CallServer end----------------------------

#----------------------------Third-party libraries end----------------------------

#---------------------------------------API------------------------------------

ifeq ($(USE_POC_API),1)
#include $(callserver-root-dir)/callserver/Android.mk
endif
#---------------------------------------API end--------------------------------
