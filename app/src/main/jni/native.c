#include <com_crankycoder_ndk1_AndroidNDK1SampleActivity.h>
#include <string.h>
#include <android/log.h>

#define DEBUG_TAG "NDK_AndroidNDK1SampleActivity"

JNIEXPORT void JNICALL Java_com_crankycoder_ndk1_AndroidNDK1SampleActivity_helloLog(JNIEnv * env, jobject this, jstring logThis)
{
    jboolean isCopy;
    const char * szLogThis = (*env)->GetStringUTFChars(env, logThis, &isCopy);

    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]", szLogThis);

    (*env)->ReleaseStringUTFChars(env, logThis, szLogThis);
}
