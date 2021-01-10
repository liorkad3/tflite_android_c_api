#include <jni.h>
#include <android/log.h>
#include <android/asset_manager_jni.h>
#include "TfliteNet.h"


extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_tflitebenchmark_TfModel_createInstance(
        JNIEnv *env,
        jobject thiz,
        jobject assetManager,
        jstring model_name,
        jboolean is_gpu,
        jint num_threads){

        const char *modelName = env->GetStringUTFChars(model_name, nullptr);
        AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
        AAsset* file = AAssetManager_open(mgr, modelName, AASSET_MODE_BUFFER);
        if (!file){
                __android_log_print(ANDROID_LOG_DEBUG, "TfModelJni", "load model failed");
            return -1;
        }
        size_t fileSize = AAsset_getLength(file);
        char* fileContent = new char[fileSize];

        int ret = AAsset_read(file, fileContent, fileSize);
        if (ret != fileSize){
                __android_log_print(ANDROID_LOG_DEBUG, "TfModelJni", "read model failed");
            return -1;
        }

        auto* net = new TfliteNet(is_gpu, num_threads);
        TfLiteStatus status = net->init(fileContent, fileSize);
        if (status != kTfLiteOk){
                return -1;
        }
        // release files resources
        AAsset_close(file);
        delete[] fileContent;

        return (jlong) net;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_tflitebenchmark_TfModel_release(JNIEnv *env, jobject thiz, jlong net_address) {
        auto * net = (TfliteNet*)net_address;
        net->release();
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_tflitebenchmark_TfModel_bench(JNIEnv *env, jobject thiz, jlong net_address) {
        auto * net = (TfliteNet*)net_address;
        net->benchmark();
}