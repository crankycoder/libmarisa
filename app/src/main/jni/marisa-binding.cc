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
                                                jbyteArray jbyte_prefix,
                                                jobject resultArrayList)
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

    //__android_log_print(ANDROID_LOG_INFO, "clibmarisa", "ag.key().length() after set_query: %d", keyLength);

    jclass ArrayList_class = env->GetObjectClass(resultArrayList);
    jmethodID ArrayList_add_id = env->GetMethodID(ArrayList_class, "add", "(Ljava/lang/Object;)Z");
    //__android_log_print(ANDROID_LOG_INFO, "clibmarisa",
    //                    "arraylist add methodID: %d", (int) ArrayList_add_id);

    while (_trie->predictive_search(*ag)) {
        //__android_log_print(ANDROID_LOG_INFO, "clibmarisa",
        //        "Prefix len: %d", prefix_len);

        keyLength = ag->key().length();
        //__android_log_print(ANDROID_LOG_INFO, "clibmarisa",
        //        "ag.key().length() in predictive search loop: %d",
        //        keyLength);

        int buf_len = ag->key().length() - prefix_len;
        char* buf = new char[buf_len+1];
        memcpy(buf, ag->key().ptr()+prefix_len, buf_len);
        buf[buf_len] = '\0';

        //__android_log_print(ANDROID_LOG_INFO, "clibmarisa",
        //                "Buffer value [%s]", buf);

        jbyteArray jbuf = env->NewByteArray(buf_len);
        env->SetByteArrayRegion(jbuf, 0, buf_len, (jbyte*) buf);

        // safe to delete the buffer now
        delete buf;

        env->CallBooleanMethod(resultArrayList, ArrayList_add_id, jbuf);
    }

    delete b_prefix;
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
    //__android_log_print(ANDROID_LOG_INFO, "clibmarisa", "mmap filepath = [%s]", cFilePath);

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

    //__android_log_print(ANDROID_LOG_INFO, "clibmarisa", "load filepath = [%s]", cFilePath);

    _trie->load(cFilePath);


}