# Android NDK — Java 与 C/C++ 的互相调用
> 本文使用的 native 代码以 c语言为例，c++代码类似。
这里主要介绍 C 调 Java 的两种方法，Java 调 C 比较简单。

[原文链接](https://www.jianshu.com/p/cdfcfc7489bb)

## Java 调 C
1. 加载 so 库,"native-lib"为库名。
```java
static {
        System.loadLibrary("native-lib");
    }
```
2. Java native方法
``` java
public native void test();
```
写一个带 native 关键字的方法，写完之后 Android Studio 会提示红色错误，直接 alt + enter 解决报错，然后会在配置好 jni 环境下会在 .c 的文件下直接生成类似下面的代码。
```java
Java_com_immusician_unity_UnityPlayerActivity_test(JNIEnv *env, jobject instance) {
}
```
方法格式：以下划线连接  "Java" + "包名" + "native方法所在的类名" + "对应的native方法名"
然后就可以这个方法体内写相应的代码逻辑了，至此 Java 调 C 就介绍完了。当然上述步骤也可以反过来完成，只不过c的方法没办法自动生成，容易写错，所以推荐按上述步骤来写，是不是很简单呢。

## C调Java

#### 方法一
可以在 Java native 方法的参数列表中传入一个 Java 对象，然后 C 里面
通过对象调方法回调 Java ，和 Java 里面常见的 setCallback 是一个道理。
具体步骤如下：
1. 调用 setCallback 时传入实现了该接口的实例。
``` java
//自定义的一个接口
public interface OnNativeCallback {
    void callbackForNative(int i);
}

...

//这里可以不是static，为了方便就这样写了
public native static void setCallback1(OnNativeCallback callback);
```
2. 在native方法中回调 Java
```c
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
```
上面的调用步骤就相当于 Java 中对象调方法，当然也可以改成直接调用静态方法，那样可以不传对象，步骤也差不多，但是这种方法更符合 Java 的思维方式；这里有些地方不明白的可以看我下一片文章。

#### 方法二
方法二是方法一的延伸，方法一只适合在当前线程中回调，就会有局限性，所以研究了一种在多线程中回调的方法，基本步骤和方法一中差别不大，只是涉及了 native中的线程的使用，对 pthread 线程不熟悉的可以查看我这篇文章[Android NDK — Native 线程 pthread](https://www.jianshu.com/p/34d88df0cfe0)。
1. 同方法一步骤一
2. 在子线程中回调
```c
JavaVM* java_vm = NULL;

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

//当.so库加载的时候会调用，用于获取JavaVM
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv *env;

    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        return -1;
    }
    java_vm = vm;

    return JNI_VERSION_1_4;
}

```
以上就是c 回调 java 的两种方法，基本上适用于大部分情况了，如果有补充的，欢迎留言。







