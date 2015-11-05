#include "lib/marisa/trie.h"
#include "emscripten.h"

#include "js-marisa.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>

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

bool check_trie() {
    const char* fname = "/IDBFS/net_demo.record_trie";
    int fd = open(fname, O_RDONLY);
    if (fd != -1) {
        close(fd);
        printf("File was ok.  Closed file descriptor\n");
        return 0;
    } else {
        // File descriptor could not be 
        printf("File open failed. Trie READ Open Errno: %d\n", errno);
        return -1;
    }
}

long EMSCRIPTEN_KEEPALIVE flush_trie(int length, int* int_trie_bytes) {

    EM_ASM(
        FS.mkdir('/IDBFS');
        FS.mount(IDBFS, {}, '/IDBFS');
    );

    FILE *fp;
    fp = fopen("/IDBFS/demo.record_trie", "w");
    char* bytes = (char *) int_trie_bytes;
    for (int i=0; i < length; i++) {
        printf("Character [0x%04x] = [0x%02x]\n", 0xffff & i, (0xff & bytes[i]));
    }
    size_t wrote_bytes = fwrite(bytes, 1, length, fp);
    fclose(fp);
    printf("Wrote out %d bytes in c/emscripten land.\n", wrote_bytes);


    marisa::RecordTrie* _rtrie = new marisa::RecordTrie();
    printf("Setting format\n");
    _rtrie->setFormat(">iii");
    printf("Tring to MMAP\n");
    _rtrie->mmap("/IDBFS/demo.record_trie");
    cout << "MMAP'd the demo.record_trie\n";

    cout << (long) _rtrie << " C++ pointer location\n";
    return (long) _rtrie;
}

void EMSCRIPTEN_KEEPALIVE test_trie(long rtrie_handle) {

    marisa::RecordTrie* _rtrie = (marisa::RecordTrie*) rtrie_handle;

    printf("Calling getRecord\n");
    vector<marisa::Record> results;
    _rtrie->getRecord(&results, "foo");
    cout << "lookup foo: ";
    for(vector<marisa::Record>::const_iterator i = results.begin(); i != results.end(); i++) {
        marisa::Record rec = *i;
        rec.printTuple();
    }
    cout << "\n";
    //cout << "Trie get('key1'): " << _trie.getRecord(record, "key1") << "\n";

}

void test_trie() {
    header("Trie");
    marisa::Trie* _trie = new marisa::Trie();
    _trie->mmap("tests/demo.record_trie");
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

extern "C" void EMSCRIPTEN_KEEPALIVE fsync_success()
{
    printf("fsync completed\n");
    check_trie();
}

// TODO: make this public
extern "C" void MZOF_sync_mem_to_idbfs() {
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
    printf ("MZOF_load_record_trie\n");

    if( access( fname, F_OK ) == 0) {
        printf ("F_OK == 0\n");
        // file exists
        printf ("File already exists - trying to load directly.\n");
    } else {
        printf ("F_OK == %d\n", access(fname, F_OK));
        // file doesn't exist
        printf ("File not found.  Loading from HTTP!\n");

        // Note that this function requres "-s ASYNCIFY=1" and will
        // trigger a non-fatal "uncaught exception: SimulateInfiniteLoop" error 
        // in the console.log
        //
        // This is almost certainly a bug in emterpreter.  Trying to
        // use emscripten_async_wget and a paired
        // emscripten_sleep_with_yield doesn't help. 
        // See commit: 5ca86a7c68 for an example using the async
        // version of this code.
        printf ("Fetching %s \n", rtrie_url);
        emscripten_wget(rtrie_url, fname);
        printf ("Trie fetched\n");
        check_trie();
    }
}

void EMSCRIPTEN_KEEPALIVE runHttpTrie() {
    const char* rtrie_url = "http://127.0.0.1:8000/tests/demo.record_trie";
    const char* fname = "/IDBFS/net_demo.record_trie";

    MZOF_load_record_trie(rtrie_url, fname);
    printf("RecordTrie has been loaded!\n");

    // TODO: this needs
    MZOF_lookup_rtrie(fname, 4, "foo", "bar", "invalid_key", "foo");
    check_trie();
    MZOF_sync_mem_to_idbfs();
}

extern "C" void EMSCRIPTEN_KEEPALIVE MZOF_test_http_recordtrie() {
    header("HTTP RecordTrie");
    // No file exists yet
    EM_ASM(
        FS.mkdir('/IDBFS');
        FS.mount(IDBFS, {}, '/IDBFS');

        // sync from persisted state into memory and then
        // run the 'test' function
        FS.syncfs(true, function (err) {
            assert(!err);
            ccall('runHttpTrie', 'v');
            });
    );


    footer();
}


