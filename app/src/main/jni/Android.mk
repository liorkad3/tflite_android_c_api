LOCAL_PATH := $(call my-dir)
PROJECT_PATH := ${LOCAL_PATH}/../../../..
# change this folder path to yours
TFLITE_PATH := ${PROJECT_PATH}/../external_libs/tflite/2.4.0

include $(CLEAR_VARS)
LOCAL_MODULE := tflite
LOCAL_SRC_FILES := $(TFLITE_PATH)/jni/$(TARGET_ARCH_ABI)/libtensorflowlite_jni.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := tflite-gpu
LOCAL_SRC_FILES := $(TFLITE_PATH)/jni/$(TARGET_ARCH_ABI)/libtensorflowlite_gpu_jni.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := demo
LOCAL_SRC_FILES := demo_jni.cpp TfliteNet.cpp

LOCAL_C_INCLUDES := $(TFLITE_PATH)/headers

LOCAL_SHARED_LIBRARIES := tflite tflite-gpu

#LOCAL_CFLAGS := -O2 -fvisibility=hidden -fomit-frame-pointer -fstrict-aliasing -ffunction-sections -fdata-sections -ffast-math
#LOCAL_CPPFLAGS := -O2 -fvisibility=hidden -fvisibility-inlines-hidden -fomit-frame-pointer -fstrict-aliasing -ffunction-sections -fdata-sections -ffast-math
#LOCAL_LDFLAGS += -Wl,--gc-sections

LOCAL_CFLAGS += -fopenmp
LOCAL_CPPFLAGS += -fopenmp
LOCAL_LDFLAGS += -static-openmp -fopenmp

LOCAL_LDLIBS := -llog -landroid

include $(BUILD_SHARED_LIBRARY)
