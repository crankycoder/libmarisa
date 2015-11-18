extern "C" void MZOF_lookup_rtrie(const char *fname, int count, ...);

// This is how you push an array of bytes into emscripten C++ code
// We use this to push our RecordTrie into C++ from a JS function.
extern "C" long flush_trie(int length, int* int_trie_bytes);

// This is the main entry point where we pass in a pointer to the
// record trie we have created with flush_trie.
extern "C" void test_trie(long rtrie_handle);
