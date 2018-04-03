//
// Created by 吴涛 on 2018/4/3.
//

#include "native.h"
#include <pthread.h>

JavaVM* java_vm = NULL;

void callbackRunnable(void *callback);

JNIEXPORT void JNICALL
Java_com_example_wutao_nativedemo_NativeUtils_setCallback1(JNIEnv *env, jclass type, jobject callback) {
    //通过传进来的对象找到该类
    jclass javaClass = (*env)->GetObjectClass(env, callback);
    if (javaClass == 0) {
        return;
    }
    //获取要回调的方法ID,回调java方法
    jmethodID javaCallbackId = (*env)->GetMethodID(env, javaClass, "callbackForNative", "(I)V");
    (*env)->CallVoidMethod(env, callback, javaCallbackId, 123);
}

JNIEXPORT void JNICALL
Java_com_example_wutao_nativedemo_NativeUtils_setCallback2(JNIEnv *env, jclass type, jobject callback) {
    pthread_t pthread;
    pthread_attr_t pthreadAttr; // 线程属性

    jobject g_callback = (*env)->NewGlobalRef(env, callback);// 生成全局引用

    pthread_attr_init(&pthreadAttr);  //初始化线程属性
    pthread_attr_setdetachstate(&pthreadAttr, PTHREAD_CREATE_DETACHED);

    pthread_create(&pthread,&pthreadAttr,callbackRunnable,g_callback);// 创建线程
}

void callbackRunnable(void *callback) {
    JNIEnv *env = NULL;
    int ret = 0;

    for (int i = 0; i < 10000; ++i) {
        ret++;
    }

    (*java_vm)->AttachCurrentThread(java_vm,&env,NULL);

    jclass javaClass = (*env)->GetObjectClass(env, callback);
    if (javaClass == 0) {
        return;
    }
    //获取要回调的方法ID,回调java方法
    jmethodID javaCallbackId = (*env)->GetMethodID(env, javaClass, "callbackForNative", "(I)V");
    (*env)->CallVoidMethod(env, callback, javaCallbackId, ret);

    // 删除全局引用
    (*env)->DeleteGlobalRef(env,callback);
    (*java_vm)->DetachCurrentThread(java_vm);

    //释放线程资源
    pthread_exit(NULL);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv *env;

    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        return -1;
    }
    java_vm = vm;

    return JNI_VERSION_1_4;
}