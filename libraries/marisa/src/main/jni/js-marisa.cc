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

int main() {
    marisa::Trie* _trie = new marisa::Trie();
    _trie->mmap("tests/simple.trie");
    printf("Trie is loaded at %p\n", _trie);
    printf("Trie has %d keys\n", _trie->num_keys());
    return 0;
}
