#include "lib/marisa/trie.h"
#include "emscripten.h"

#include <iostream>
#include <memory.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include "js-marisa.h"

using namespace std;


typedef enum {UNLOADED, LOADED, LOAD_FAILURE} file_load_state;
file_load_state trie_state = UNLOADED;

void Record_Trie_load(marisa::Trie* _trie, const char* cFilePath)
{
    // Neither the Trie nor the filepath should be null
    assert(_trie != NULL);
    assert(cFilePath != NULL);

    _trie->load(cFilePath);
}

void header(const char* struct_name) {
    printf("%s tests!!!!!!!!!!!!!!\n", struct_name);
}

void footer() {
    printf("---------------\n");
}

void test_trie() {
    header("Trie");
    marisa::Trie* _trie = new marisa::Trie();
    _trie->mmap("tests/simple.trie");
    printf("Trie is loaded at %p\n", _trie);
    printf("Trie has %d keys\n", _trie->num_keys());
    footer();
}

void test_bytestrie() {
    header("BytesTrie");

    marisa::BytesTrie* _bytestrie = new marisa::BytesTrie();
    _bytestrie->mmap("tests/simple.bytestrie");
    printf("BytesTrie is loaded at %p\n", _bytestrie);
    printf("BytesTrie has %d keys\n", _bytestrie->num_keys());

    vector< vector<char> > results;
    printf("Searching foo\n");
    _bytestrie->get(&results, "foo", 0);

    printf("%d results\n", results.size());
    for(vector< vector<char> >::const_iterator i = results.begin(); i != results.end(); i++) {
        const char* byteData = i->data();
        cout << "{" << byteData << "}" << endl;
    }
    footer();

}

void test_recordtrie() {
    header("RecordTrie");

    marisa::RecordTrie* _rtrie = new marisa::RecordTrie(">iii");
    _rtrie->mmap("tests/demo.record_trie");
    printf("RecordTrie has %d keys\n", _rtrie->num_keys());

    vector<marisa::Record> results;

    const char* key = "foo";
    printf("Searching %s\n", key);
    _rtrie->getRecord(&results, key);

    for(vector<marisa::Record>::const_iterator i = results.begin(); i != results.end(); i++) {
        marisa::Record rec = *i;
        rec.printTuple();
    }
    results.clear();

    key = "bar";
    printf("Searching %s\n", key);
    _rtrie->getRecord(&results, key);
    for(vector<marisa::Record>::const_iterator i = results.begin(); i != results.end(); i++) {
        marisa::Record rec = *i;
        rec.printTuple();
    }
    footer();
}

extern "C" void MZOF_lookup_rtrie(const char *fname, int count, ...) {

    va_list ap;
    va_start(ap, count); /* Requires the last fixed parameter (to get the address) */

    marisa::RecordTrie* _rtrie = new marisa::RecordTrie(">iii");
    _rtrie->mmap(fname);
    printf("RecordTrie has %d keys\n", _rtrie->num_keys());

    vector<marisa::Record> results;

    char* key = NULL;
    for (int j = 0; j < count; j++) {
        key = va_arg(ap, char*); /* Increments ap to the next argument. */

        printf("Searching %s\n", key);
        _rtrie->getRecord(&results, key);

        for(vector<marisa::Record>::const_iterator i = results.begin(); i != results.end(); i++) {
            marisa::Record rec = *i;
            rec.printTuple();
        }
        printf("Finished searching %s\n", key);
        results.clear();
    }
    va_end(ap);



}

// TODO: make this public
extern "C" void MZOF_mount_idbfs() {
    EM_ASM(
        FS.mkdir('/IDBFS');
        FS.mount(IDBFS, {}, '/IDBFS');
    );
}

void EMSCRIPTEN_KEEPALIVE fsync_success()
{
    printf("fsync completed\n");
}

// TODO: make this public
extern "C" void MZOF_sync_idbfs() {
    EM_ASM(
            FS.syncfs(function (err) {
                assert(!err);
                ccall('fsync_success', 'v');
                });
          );
}

void load_success_callback(const char *fname) {
    // This assert ensures that we can timeout safely
    assert(trie_state == UNLOADED);

    printf("Success loading %s!\n", fname);
    trie_state = LOADED;
}


void load_failure_callback(const char *fname) {
    // This assert ensures that we can timeout safely
    assert(trie_state == UNLOADED);

    printf("Error loading %s!\n", fname);
    trie_state = LOAD_FAILURE;
}


extern "C" void MZOF_load_record_trie(const char *rtrie_url, const char* fname) {
    if( access( fname, F_OK ) != -1 ) {
        // file exists
        printf ("File already exists - trying to load directly.\n");
    } else {
        // file doesn't exist
        printf ("File not found.  Loading from HTTP!\n");
        emscripten_async_wget(rtrie_url, fname, 
                *load_success_callback, 
                *load_failure_callback);
    }

    for (int i = 0; i < 1000; i++) {
        emscripten_sleep_with_yield(20);
        if (trie_state != UNLOADED) {
            return;
        }
    }
    printf("Load failure. timeout.\n");
    trie_state = LOAD_FAILURE;
}

extern "C" void test_http_recordtrie() {
    header("HTTP RecordTrie");

    const char* rtrie_url = "http://127.0.0.1:8000/tests/demo.record_trie";
    const char* fname = "/IDBFS/net_demo.record_trie";

    MZOF_mount_idbfs();

    MZOF_load_record_trie(rtrie_url, fname);

    //MZOF_lookup_rtrie(fname, 4, "foo", "bar", "invalid_key", "foo");

    //MZOF_sync_idbfs();

    footer();
}

int EMSCRIPTEN_KEEPALIVE main() {
    printf("Main is started!");
    test_http_recordtrie();
    emscripten_exit_with_live_runtime();
    return 0;
}
