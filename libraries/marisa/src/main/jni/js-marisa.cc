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


long EMSCRIPTEN_KEEPALIVE push_trie(int length, int* int_trie_bytes) {

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

void EMSCRIPTEN_KEEPALIVE simple_test_trie(long rtrie_handle) {

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
