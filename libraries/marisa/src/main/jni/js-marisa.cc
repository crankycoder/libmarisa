#include "lib/marisa/trie.h"

#include <iostream>
#include <memory.h>
#include <assert.h>
#include <stdio.h>

using namespace std;

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
    printf("Searching foo\n");
    _rtrie->getRecord(&results, "foo");

    for(vector<marisa::Record>::const_iterator i = results.begin(); i != results.end(); i++) {
        marisa::Record rec = *i;
        rec.printTuple();

    }
    footer();

}

int main() {
    test_trie();
    test_bytestrie();
    test_recordtrie();
    return 0;
}
