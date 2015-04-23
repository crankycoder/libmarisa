#include <jni.h>
#include <android/log.h>
#include <lib/marisa/trie.h>
#include <memory.h>
#include <cstring>

/*
 Trie APIs
 */

extern "C" JNIEXPORT jlong JNICALL
Java_com_crankycoder_marisa_Trie_newTrie(JNIEnv *env,
                                         jclass)
{
    marisa::Trie* _trie;
    _trie = new marisa::Trie();
    return (jlong) _trie;
}

extern "C" JNIEXPORT void JNICALL
Java_com_crankycoder_marisa_Trie_deallocTrie(JNIEnv *env,
                                             jclass,
                                             jlong handle)
{
    marisa::Trie* _trie;
    _trie = (marisa::Trie*) handle;
    delete _trie;
}


extern "C" JNIEXPORT jlong JNICALL
Java_com_crankycoder_marisa_Trie_mmapFile(JNIEnv *env,
                                          jclass,
                                          jlong handle,
                                          jstring filePath)
{
    // TODO: release the string later
    const char *cFilePath = env->GetStringUTFChars(filePath, 0);
     __android_log_print(ANDROID_LOG_INFO, "libmarisa", "mmap filepath = [%s]", cFilePath);

    marisa::Trie* _trie;
    _trie = (marisa::Trie*) handle;
    _trie->mmap(cFilePath);
    return (jlong) _trie;
}


extern "C" JNIEXPORT void JNICALL
Java_com_crankycoder_marisa_Trie_load(JNIEnv *env,
                                      jclass,
                                      jlong handle,
                                      jstring filePath)
{
    marisa::Trie* _trie;
    _trie = (marisa::Trie*) handle;

    // TODO: release the string later
    const char *cFilePath = env->GetStringUTFChars(filePath, 0);

     __android_log_print(ANDROID_LOG_INFO, "libmarisa", "load filepath = [%s]", cFilePath);

    _trie->load(cFilePath);


}


extern "C" JNIEXPORT jboolean JNICALL
Java_com_crankycoder_marisa_Trie_predictiveSearch(JNIEnv *env,
                                                  jclass,
                                                  jlong handle,
                                                  jlong agentHandle)
{
    marisa::Trie* _trie;
    marisa::Agent* _agent;
    _trie = (marisa::Trie*) handle;
    _agent = (marisa::Agent*) agentHandle;

    jboolean result = (jboolean) _trie->predictive_search(*_agent);

    return result;
}


/*
 Agent APIs
 */
extern "C" JNIEXPORT jlong JNICALL
Java_com_crankycoder_marisa_Agent_newAgent(JNIEnv *env,
                                           jclass)
{
    marisa::Agent* _agent;
    _agent = new marisa::Agent();
    return (jlong) _agent;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_crankycoder_marisa_Agent_bSetQuery(JNIEnv *env,
                                            jclass,
                                            jlong agentHandle,
                                            jbyteArray jbyte_prefix)
{
    marisa::Agent* _agent;
    _agent = (marisa::Agent*) agentHandle;

    int textLength = env->GetArrayLength(jbyte_prefix);

    // This is properly showing a length of 4
    __android_log_print(ANDROID_LOG_INFO, "libmarisa", "jbyte_prefix length: %d", textLength);


    jboolean isCopy;
    jbyte* a = env->GetByteArrayElements(jbyte_prefix, &isCopy);
    char* b_prefix = new char[textLength + 1];
    memcpy(b_prefix, a, textLength);
    b_prefix[textLength] = '\0';

     __android_log_print(ANDROID_LOG_INFO, "libmarisa", "b_prefix length: %d", strlen(b_prefix));
    // __android_log_print(ANDROID_LOG_INFO, "libmarisa", "b_prefix : [%s]", b_prefix);
    _agent->set_query(b_prefix);

    return (jlong) b_prefix;
}

extern "C" JNIEXPORT void JNICALL
Java_com_crankycoder_marisa_Agent_freeCharStar(JNIEnv *env,
                                                  jclass,
                                                  jlong queryHandle)
{
    char* b_prefix = (char *) queryHandle;
    delete b_prefix;
}



extern "C" JNIEXPORT jlong JNICALL
Java_com_crankycoder_marisa_Agent_getKeyHandle(JNIEnv *env,
                                            jclass,
                                            jlong agentHandle)
{
    marisa::Agent* _agent;
    _agent = (marisa::Agent*) agentHandle;

    return (jlong) &(_agent->key());
}

/*
 Key APIs
 */

 extern "C" JNIEXPORT jbyteArray JNICALL
 Java_com_crankycoder_marisa_Key_keyPtr(JNIEnv *env,
                                             jclass,
                                             jlong keyHandle)
 {

     /* Extract the char* data and return it as a byte array */
     marisa::Key* _key;
     _key = (marisa::Key*) keyHandle;

     const char* data =  _key->ptr();

     int n = 0;
     while (*data++) {
        n++;
     } if (n <= 0) return NULL;

     jbyteArray arr = env->NewByteArray(n);
     env->SetByteArrayRegion(arr, 0, n, (jbyte*)data);
     return arr;
 }

 extern "C" JNIEXPORT jint JNICALL
  Java_com_crankycoder_marisa_Key_keyLength(JNIEnv *env,
                                              jclass,
                                              jlong keyHandle)
{
    marisa::Key* _key;
    _key = (marisa::Key*) keyHandle;
    return (jint) _key->length();
}


