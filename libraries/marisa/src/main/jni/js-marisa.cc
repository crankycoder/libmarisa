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




