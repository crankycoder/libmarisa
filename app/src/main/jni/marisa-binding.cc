#include <jni.h>
#include <android/log.h>
#include <lib/marisa/trie.h>
#include <memory.h>
#include <cstring>

/*
 Trie APIs
 */

extern "C" JNIEXPORT void JNICALL
Java_com_crankycoder_marisa_BytesTrie_bGetValue(JNIEnv *env,
                                                jclass,
                                                jlong handle,
                                                jbyteArray jbyte_prefix)
{
    marisa::Trie* _trie;
    _trie = (marisa::Trie*) handle;

    int textLength = env->GetArrayLength(jbyte_prefix);
    jboolean isCopy;
    jbyte* a = env->GetByteArrayElements(jbyte_prefix, &isCopy);
    char* b_prefix = new char[textLength + 1];
    memcpy(b_prefix, a, textLength);
    b_prefix[textLength] = '\0';

    int prefix_len = strlen(b_prefix);

    marisa::Agent* ag = new marisa::Agent();
    ag->set_query(b_prefix);

    int keyLength = ag->key().length();

    __android_log_print(ANDROID_LOG_INFO, "clibmarisa", "ag.key().length() after set_query: %d", keyLength);

    while (_trie->predictive_search(*ag)) {
        __android_log_print(ANDROID_LOG_INFO, "clibmarisa",
                "Prefix len: %d", prefix_len);

        keyLength = ag->key().length();
        __android_log_print(ANDROID_LOG_INFO, "clibmarisa",
                "ag.key().length() in predictive search loop: %d",
                keyLength);

        int buf_len = ag->key().length() - prefix_len;
        char* buf = new char[buf_len+1];
        memcpy(buf, ag->key().ptr()+prefix_len, buf_len);
        buf[buf_len] = '\0';

        __android_log_print(ANDROID_LOG_INFO, "clibmarisa",
                        "Buffer value [%s]", buf);


    };


}

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
     __android_log_print(ANDROID_LOG_INFO, "clibmarisa", "mmap filepath = [%s]", cFilePath);

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

     __android_log_print(ANDROID_LOG_INFO, "clibmarisa", "load filepath = [%s]", cFilePath);

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
    __android_log_print(ANDROID_LOG_INFO, "clibmarisa", "jbyte_prefix length: %d", textLength);


    // We need to do some jiggery pokery to convert the java byte array into a NULL terminated
    // char* array.
    jboolean isCopy;
    jbyte* a = env->GetByteArrayElements(jbyte_prefix, &isCopy);
    // TODO: free this memory later
    char* b_prefix = new char[textLength + 1];
    memcpy(b_prefix, a, textLength);
    b_prefix[textLength] = '\0';

     __android_log_print(ANDROID_LOG_INFO, "clibmarisa", "b_prefix strlen: %d", strlen(b_prefix));
     __android_log_print(ANDROID_LOG_INFO, "clibmarisa", "b_prefix[0]  %02x", b_prefix[0]);
     __android_log_print(ANDROID_LOG_INFO, "clibmarisa", "b_prefix[1]  %02x", b_prefix[1]);
     __android_log_print(ANDROID_LOG_INFO, "clibmarisa", "b_prefix[2]  %02x", b_prefix[2]);
     __android_log_print(ANDROID_LOG_INFO, "clibmarisa", "b_prefix[3]  %02x", b_prefix[3]);

    // __android_log_print(ANDROID_LOG_INFO, "clibmarisa", "b_prefix : [%s]", b_prefix);
    _agent->set_query(b_prefix);

    return (jlong) b_prefix;
}



extern "C" JNIEXPORT jint JNICALL
Java_com_crankycoder_marisa_Agent_getKeyLength(JNIEnv *env,
                                            jclass,
                                            jlong agentHandle)
{
    marisa::Agent* _agent;
    _agent = (marisa::Agent*) agentHandle;

    // This is coming back as 4, but I expect to see 13
    int keyLen = _agent->key().length();
    __android_log_print(ANDROID_LOG_INFO, "clibmarisa", "c++ fetch agent->key().length() == %d", keyLen);

    int n = 0;
    const char* ptr = _agent->key().ptr();
    return (jint) keyLen;
}

/*
 Key APIs
 */



