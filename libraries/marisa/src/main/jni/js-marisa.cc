#include "lib/marisa/trie.h"
#include <memory.h>
#include <assert.h>
#include <stdio.h>

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

    std::vector<std::string> results;
    printf("Searching foo\n");
    _bytestrie->get(&results, "foo");

    printf("%d results\n", results.size());
    for(std::vector<std::string>::const_iterator i = results.begin(); i != results.end(); ++i) {
        printf("[%s]\n", i->c_str());
    }
    footer();

}

int main() {
    test_trie();
    test_bytestrie();
    return 0;
}
