#include <jni.h>
#include <android/log.h>
#include <lib/marisa/trie.h>
#include <memory.h>
#include <cstring>
#include <assert.h>

/*
 Trie APIs
 */

extern "C" JNIEXPORT void JNICALL
Java_com_crankycoder_marisa_BytesTrie_bGetValue(JNIEnv *env,
                                                jclass,
                                                jlong handle,
                                                jbyteArray jbyte_prefix,
                                                jobject resultArrayList)
{
    jboolean isCopy;

    marisa::Trie* _trie = (marisa::Trie*) handle;
    assert(_trie != NULL);

    int prefix_len = env->GetArrayLength(jbyte_prefix);

    jbyte* a = env->GetByteArrayElements(jbyte_prefix, &isCopy);
    assert(a != NULL);

    char* b_prefix = new char[prefix_len + 1];
    memcpy(b_prefix, a, prefix_len);
    b_prefix[prefix_len] = '\0';

    marisa::Agent ag;
    ag.set_query(b_prefix);

    jclass ArrayList_class = env->GetObjectClass(resultArrayList);
    jmethodID ArrayList_add_id = env->GetMethodID(ArrayList_class, "add", "(Ljava/lang/Object;)Z");
    assert(ArrayList_add_id != 0);

    while (_trie->predictive_search(ag)) {
        // I have no idea why, but you seem to need to copy out the bytes
        // into a char* buffer instead of just passing key().ptr() into SetByteArrayRegion
        int buf_len = ag.key().length() - prefix_len;
        assert (buf_len > -1);

        char* buf = new char[buf_len+1];
        memcpy(buf, ag.key().ptr()+prefix_len, buf_len);
        buf[buf_len] = '\0';

        jbyteArray jbuf = env->NewByteArray(buf_len);
        env->SetByteArrayRegion(jbuf, 0, buf_len, (jbyte*) buf);

        // safe to delete the char* buffer now
        delete buf;

        env->CallBooleanMethod(resultArrayList, ArrayList_add_id, jbuf);
    }

    delete b_prefix;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_crankycoder_marisa_Trie_newTrie(JNIEnv *env,
                                         jclass)
{
    marisa::Trie* _trie = new marisa::Trie();
    assert(_trie != NULL);
    return (jlong) _trie;
}

extern "C" JNIEXPORT void JNICALL
Java_com_crankycoder_marisa_Trie_deallocTrie(JNIEnv *env,
                                             jclass,
                                             jlong handle)
{
    marisa::Trie* _trie = (marisa::Trie*) handle;
    assert(_trie != NULL);
    delete _trie;
}


extern "C" JNIEXPORT jlong JNICALL
Java_com_crankycoder_marisa_Trie_mmapFile(JNIEnv *env,
                                          jclass,
                                          jlong handle,
                                          jstring filePath)
{
    const char *cFilePath = env->GetStringUTFChars(filePath, 0);
    marisa::Trie* _trie = (marisa::Trie*) handle;

    // Neither the Trie nor the filepath should be null
    assert(_trie != NULL);
    assert(cFilePath != NULL);

    _trie->mmap(cFilePath);
    env->ReleaseStringUTFChars(filePath, cFilePath);
    return (jlong) _trie;
}


extern "C" JNIEXPORT void JNICALL
Java_com_crankycoder_marisa_Trie_load(JNIEnv *env,
                                      jclass,
                                      jlong handle,
                                      jstring filePath)
{

    marisa::Trie* _trie = (marisa::Trie*) handle;
    const char *cFilePath = env->GetStringUTFChars(filePath, 0);

    // Neither the Trie nor the filepath should be null
    assert(_trie != NULL);
    assert(cFilePath != NULL);

    _trie->load(cFilePath);
    env->ReleaseStringUTFChars(filePath, cFilePath);
}