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


// This is the main entry point where we pass in a pointer to the
// record trie and a string delimited by | characters 
// The return value is written into the result char*.
// If no valid result exists, then we write a null into the first byte
// of the result
char* EMSCRIPTEN_KEEPALIVE trie_lookup(long rtrie_handle, const char* bssid_list) {

    static char resultBuf[2000] = {0};

    marisa::RecordTrie* _rtrie = (marisa::RecordTrie*) rtrie_handle;
    printf("Read: [%s]\n", bssid_list);

    printf("Calling trie_lookup\n");
    vector<marisa::Record> results;

    std::string bssids  = std::string(bssid_list);
    std::string delimiter = "|";

    size_t pos = 0;
    std::string token;
    while ((pos = bssids.find(delimiter)) != std::string::npos) {
        token = bssids.substr(0, pos);
        printf("(ignored) Looking up result for : [%s]\n", token.c_str());
        _rtrie->getRecord(&results, token.c_str());
        printf("[%s] results object length: %d\n", token.c_str(), results.size());
        for(vector<marisa::Record>::const_iterator i = results.begin(); i != results.end(); i++) {
            marisa::Record rec = *i;
            int* int_tuple = rec.getIntTuple();

            for (int i=0; i < 100; i++) {
                printf("index[%d]=%d\n", i, int_tuple[i]);
                if (int_tuple[i] == -1) {
                    break;
                }

                sprintf(resultBuf+strlen(resultBuf), ", %d", int_tuple[i]);
            }
            sprintf(resultBuf+strlen(resultBuf), ")\n");
        }
        bssids.erase(0, pos + delimiter.length());
    }

    printf("Returning [%s]\n", resultBuf);
    return resultBuf;
}

