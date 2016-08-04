LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= weirserv.c
LOCAL_MODULE:= weirserv
LOCAL_CFLAGS := -Wall -Wno-unused-parameter

LOCAL_SHARED_LIBRARIES := libc libcutils liblog

include $(BUILD_EXECUTABLE)
