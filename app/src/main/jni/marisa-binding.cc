#include <jni.h>
#include <android/log.h>


extern "C" JNIEXPORT jlong JNICALL
Java_com_crankycoder_marisa_Trie_newTrie(JNIEnv * env,
                                         jclass)
{
    return (jlong) 555;
}
