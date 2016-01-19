extern "C" void MZOF_lookup_rtrie(const char *fname, int count, ...);

// This is how you push an array of bytes into emscripten C++ code
// We use this to push our RecordTrie into C++ from a JS function.
extern "C" long push_trie(int length, int* int_trie_bytes);

// This is the main entry point where we pass in a pointer to the
// record trie and a string delimited by | characters 
// The return value is written into the result char*.
// If no valid result exists, then we write a null into the first byte
// of the result
extern "C" char* trie_lookup(long rtrie_handle, const char* bssid_list);
