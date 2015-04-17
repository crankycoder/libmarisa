#include <jni.h>
#include <android/log.h>
#include <lib/marisa/trie.h>


extern "C" JNIEXPORT jlong JNICALL
Java_com_crankycoder_marisa_Trie_newTrie(JNIEnv * env,
                                         jclass)
{
    marisa::Trie* _trie;
    _trie = new marisa::Trie();
    return (jlong) _trie;
}

extern "C" JNIEXPORT void JNICALL
Java_com_crankycoder_marisa_Trie_deallocTrie(JNIEnv * env,
                                             jclass,
                                             jlong handle)
{
    marisa::Trie* _trie;
    _trie = (marisa::Trie*) handle;
    delete _trie;
}
