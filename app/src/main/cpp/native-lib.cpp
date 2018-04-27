#include <jni.h>
#include <string>
#include "Log.h"
#include "string.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_dreamliner_signjni_AESJniUtil_stringFromJNI(JNIEnv *env, jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_dreamliner_signjni_AESJniUtil_encode(JNIEnv *env, jclass jclss1, jstring content) {
    jstring encodeResult = env->NewStringUTF("");
    jclass aes_util_class = env->FindClass("com/dreamliner/signjni/AESUtil");
    if (NULL == aes_util_class) {
        return encodeResult;
    }
    jmethodID methodId = env->GetStaticMethodID(aes_util_class, "encode", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
    encodeResult = (jstring) env->CallStaticObjectMethod(aes_util_class, methodId, content, env->NewStringUTF("fead3de8601643e6"));
    return encodeResult;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_dreamliner_signjni_AESJniUtil_decode(JNIEnv *env, jclass jclss1, jstring content) {
    jstring decodeResult = env->NewStringUTF("");
    jclass aes_util_class = env->FindClass("com/dreamliner/signjni/AESUtil");
    if (NULL == aes_util_class) {
        return decodeResult;
    }
    jmethodID methodId = env->GetStaticMethodID(aes_util_class, "decode", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
    decodeResult = (jstring) env->CallStaticObjectMethod(aes_util_class, methodId, content, env->NewStringUTF("fead3de8601643e6"));

    env->DeleteLocalRef(aes_util_class);

    return decodeResult;
}

static int verifySign(JNIEnv *env);

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return JNI_ERR;
    }
    if (verifySign(env) == JNI_OK) {
        return JNI_VERSION_1_4;
    }
    return JNI_ERR;
}

static jobject getApplication(JNIEnv *env) {
    jobject application = NULL;
    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
    if (activity_thread_clz != NULL) {
        jmethodID currentApplication = env->GetStaticMethodID(
                activity_thread_clz, "currentApplication", "()Landroid/app/Application;");
        if (currentApplication != NULL) {
            application = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);
        } else {
            LOGE("Cannot find method: currentApplication() in ActivityThread.");
        }
        env->DeleteLocalRef(activity_thread_clz);
    } else {
        LOGE("Cannot find class: android.app.ActivityThread");
    }
    return application;
}

static const char *SIGN =
        "3082038b30820273a0030201020204693dc2b0300d06092a864886f70d01010b05003076310b300906035504061302383631123010060355040813094775616e67446f6e6731123010060355040713094775616e675a686f7531133011060355040a130a447265616d6c696e657231133011060355040b130a447265616d6c696e6572311530130603550403130c4368656e5a686f6e674a696e301e170d3138303432373034343531305a170d3433303432313034343531305a3076310b300906035504061302383631123010060355040813094775616e67446f6e6731123010060355040713094775616e675a686f7531133011060355040a130a447265616d6c696e657231133011060355040b130a447265616d6c696e6572311530130603550403130c4368656e5a686f6e674a696e30820122300d06092a864886f70d01010105000382010f003082010a0282010100d1a9ce667d3ff37fd54a755a8613025f14e88fb6bb819824d06226e994cf6e121e9db82e66756e2854f4c4562ccdba561823019890053e35749996d7fccb2a5965e62090fa5cb73505de311193aec46ce488bb03f0f7d4219ac7a48459d05e74233e3d2ee908896a8e850e705fea4db4f5b1183df25169c868c753c5d53f33adddbea2fab57f50101121f7742bddbdd15f1f6f797ff8d95dd35adadc8f44b1ad809e3b99b323047942a79c681643c29a12d38feaec4341e7a3fd966082515d7e6480b0ee60366efdbdd79eae1ec1eab31eca87e02d11030ce11d531309afa7274ba8e4110eef18d285cda902bbb5309d424b2a90c5b7c8d81c9a06adf533c72b0203010001a321301f301d0603551d0e04160414939c00d4494ba47a3463955a9ae996bb3a1d7d2d300d06092a864886f70d01010b05000382010100be15be4f07a3af191f76eafddaffc8e502cf7a4f37a5fa08a3d0dbbf5ba3cc53defd0fd97138a0377ac325215f113969341a9679a81916b40afa1934d219c499fcbe6f16e396454390865318a8225c2ad6f82f0ff275226bb821fbe68fdfebb0a343da2a26dd36924dcfd14cf832d2e2a65b05ecbcc7f7957ee57e08f37e94ba6e13577fb9f7045b7939190388abe490735f7b47546a5f384753a7727c9de27b2232d4be2a54b4371e2dd53e3ea3f1df1c3838fe35fbd546142527679466916e380fa7abc6e75da07eb39c186c5efcdb604bd9dc584caef96bf295891b85c06424e498fe8d357aa7d2f18651d845980572ca57da687ef91d833e3ad83944be49";

static int verifySign(JNIEnv *env) {
    // Application object
    jobject application = getApplication(env);
    if (application == NULL) {
        return JNI_ERR;
    }
    // Context(ContextWrapper) class
    jclass context_clz = env->GetObjectClass(application);
    // getPackageManager()
    jmethodID getPackageManager = env->GetMethodID(context_clz, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    // android.content.pm.PackageManager object
    jobject package_manager = env->CallObjectMethod(application, getPackageManager);
    // PackageManager class
    jclass package_manager_clz = env->GetObjectClass(package_manager);
    // getPackageInfo()
    jmethodID getPackageInfo = env->GetMethodID(package_manager_clz, "getPackageInfo",
                                                "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    // context.getPackageName()
    jmethodID getPackageName = env->GetMethodID(context_clz, "getPackageName", "()Ljava/lang/String;");
    // call getPackageName() and cast from jobject to jstring
    jstring package_name = (jstring) (env->CallObjectMethod(application, getPackageName));
    // PackageInfo object
    jobject package_info = env->CallObjectMethod(package_manager, getPackageInfo, package_name, 64);
    // class PackageInfo
    jclass package_info_clz = env->GetObjectClass(package_info);
    // field signatures
    jfieldID signatures_field = env->GetFieldID(package_info_clz, "signatures", "[Landroid/content/pm/Signature;");
    jobject signatures = env->GetObjectField(package_info, signatures_field);
    jobjectArray signatures_array = (jobjectArray) signatures;
    jobject signature0 = env->GetObjectArrayElement(signatures_array, 0);
    jclass signature_clz = env->GetObjectClass(signature0);

    jmethodID toCharsString = env->GetMethodID(signature_clz, "toCharsString", "()Ljava/lang/String;");
    // call toCharsString()
    jstring signature_str = (jstring) (env->CallObjectMethod(signature0, toCharsString));

    // release
    env->DeleteLocalRef(application);
    env->DeleteLocalRef(context_clz);
    env->DeleteLocalRef(package_manager);
    env->DeleteLocalRef(package_manager_clz);
    env->DeleteLocalRef(package_name);
    env->DeleteLocalRef(package_info);
    env->DeleteLocalRef(package_info_clz);
    env->DeleteLocalRef(signatures);
    env->DeleteLocalRef(signature0);
    env->DeleteLocalRef(signature_clz);

    const char *sign = env->GetStringUTFChars(signature_str, NULL);
    if (sign == NULL) {
        return JNI_ERR;
    }
    int result = strcmp(sign, SIGN);
    env->ReleaseStringUTFChars(signature_str, sign);
    env->DeleteLocalRef(signature_str);
    if (result == 0) {
        return JNI_OK;
    }
    return JNI_ERR;
}
