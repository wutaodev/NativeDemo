//
// Created by 吴涛 on 2018/4/3.
//

#ifndef NATIVEDEMO_NATIVE_H
#define NATIVEDEMO_NATIVE_H

#endif //NATIVEDEMO_NATIVE_H

#include <jni.h>
#include <android/log.h>

#if 1

#define MODULE_NAME  "FFFFFFFFFFFFFFFF"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, MODULE_NAME, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, MODULE_NAME, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, MODULE_NAME, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,MODULE_NAME, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,MODULE_NAME, __VA_ARGS__)

#else

#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#define LOGF(...)
#endif


JNIEXPORT void JNICALL
Java_com_example_wutao_nativedemo_NativeUtils_setCallback1(JNIEnv *env, jclass type, jobject callback);

JNIEXPORT void JNICALL
Java_com_example_wutao_nativedemo_NativeUtils_setCallback2(JNIEnv *env, jclass type, jobject callback);