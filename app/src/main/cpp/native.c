//
// Created by 吴涛 on 2018/4/3.
// 对比三种方式线程的创建和退出
//

#include "native.h"
#include <pthread.h>
#include <malloc.h>

JavaVM* java_vm = NULL;
pthread_attr_t pthreadAttr; // 线程属性
pthread_t pthread2;

void callbackRunnable(void *callback);
void callbackRunnable2(void *callback);
void callbackRunnable3(void *callback);
void destroy();

JNIEXPORT void JNICALL
Java_com_example_wutao_nativedemo_NativeUtils_setCallback1(JNIEnv *env, jclass type, jobject callback) {
    // 通过传进来的对象找到该类
    jclass javaClass = (*env)->GetObjectClass(env, callback);
    if (javaClass == 0) {
        return;
    }
    // 获取要回调的方法ID,回调java方法
    jmethodID javaCallbackId = (*env)->GetMethodID(env, javaClass, "callbackForNative", "(I)V");
    (*env)->CallVoidMethod(env, callback, javaCallbackId, 123);
}

JNIEXPORT void JNICALL
Java_com_example_wutao_nativedemo_NativeUtils_setCallback2(JNIEnv *env, jclass type, jobject callback) {
    pthread_t pthread;


    jobject g_callback = (*env)->NewGlobalRef(env, callback);// 生成全局引用

    pthread_attr_init(&pthreadAttr);  //初始化线程属性
    pthread_attr_setdetachstate(&pthreadAttr, PTHREAD_CREATE_DETACHED);

    // 创建线程
    pthread_create(&pthread,&pthreadAttr,callbackRunnable,g_callback); //方式一 // 线程状态指定为unjoinable

    //pthread_create(&pthread2,NULL,callbackRunnable2,g_callback); //方式二 默认的状态是joinable

    //pthread_create(&pthread2,NULL,callbackRunnable3,g_callback); //方式三 默认的状态是joinable
}

JNIEXPORT void JNICALL
Java_com_example_wutao_nativedemo_NativeUtils_destroy(JNIEnv *env, jclass type) {

    destroy();
}

// 方式一
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
    // 获取要回调的方法ID,回调java方法
    jmethodID javaCallbackId = (*env)->GetMethodID(env, javaClass, "callbackForNative", "(I)V");
    (*env)->CallVoidMethod(env, callback, javaCallbackId, ret);

    // 删除全局引用
    (*env)->DeleteGlobalRef(env,callback);
    (*java_vm)->DetachCurrentThread(java_vm);

    // 释放线程资源
    pthread_attr_destroy(&pthreadAttr);
    pthread_exit(NULL); //！！！自动释放
}

// 方式二
void callbackRunnable2(void *callback) {
    JNIEnv *env = NULL;
    int ret = 0;
    char *memory = NULL;

    for (int i = 0; i < 20000; ++i) {
        ret++;
    }

    //！！！手动创建内存
    memory = (char *) calloc(1, sizeof(char)*1024);

    (*java_vm)->AttachCurrentThread(java_vm,&env,NULL);

    jclass javaClass = (*env)->GetObjectClass(env, callback);
    if (javaClass == 0) {
        return;
    }
    // 获取要回调的方法ID,回调java方法
    jmethodID javaCallbackId = (*env)->GetMethodID(env, javaClass, "callbackForNative", "(I)V");
    (*env)->CallVoidMethod(env, callback, javaCallbackId, ret);

    // 删除全局引用
    (*env)->DeleteGlobalRef(env,callback);
    (*java_vm)->DetachCurrentThread(java_vm);

    // 释放线程资源
    pthread_exit(memory); ///！！！手动释放
}

// 方式三
void callbackRunnable3(void *callback) {
    JNIEnv *env = NULL;
    int ret = 0;

    for (int i = 0; i < 30000; ++i) {
        ret++;
    }

    (*java_vm)->AttachCurrentThread(java_vm,&env,NULL);

    jclass javaClass = (*env)->GetObjectClass(env, callback);
    if (javaClass == 0) {
        return;
    }
    // 获取要回调的方法ID,回调java方法
    jmethodID javaCallbackId = (*env)->GetMethodID(env, javaClass, "callbackForNative", "(I)V");
    (*env)->CallVoidMethod(env, callback, javaCallbackId, ret);

    // 删除全局引用
    (*env)->DeleteGlobalRef(env,callback);
    (*java_vm)->DetachCurrentThread(java_vm);

    pthread_detach(pthread_self());// 将状态改为unjoinable状态，确保资源的释放

    // 释放线程资源
    pthread_exit(0); //！！！
}

// so库加载时调用
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv *env;

    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        return -1;
    }
    java_vm = vm;

    return JNI_VERSION_1_4;
}

void destroy(){
    void *pVoid = NULL;

    pthread_join(pthread2, &pVoid);// 结束线程
    if (pVoid != NULL) {
        free(pVoid); // 方式二线程结束释放内存
        LOGD("方式二释放资源");
    }
}
