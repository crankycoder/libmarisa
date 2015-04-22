#include <jni.h>
#include <android/log.h>
#include <lib/marisa/trie.h>


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
                                          jstring path)
{
   const char *nativeString = env->GetStringUTFChars(path, 0);

    marisa::Trie* _trie;
    _trie = (marisa::Trie*) handle;
    _trie->mmap(nativeString);
    return (jlong) _trie;
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

extern "C" JNIEXPORT void JNICALL
Java_com_crankycoder_marisa_Agent_setQuery(JNIEnv *env,
                                            jclass,
                                            jlong agentHandle,
                                            jstring prefix)
{
    const char *nativePrefix = env->GetStringUTFChars(prefix, 0);

    marisa::Agent* _agent;
    _agent = (marisa::Agent*) agentHandle;

    _agent->set_query(nativePrefix);

}