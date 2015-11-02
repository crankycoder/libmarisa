extern "C" void MZOF_test_http_recordtrie();
extern "C" void MZOF_lookup_rtrie(const char *fname, int count, ...);
extern "C" void MZOF_sync_idbfs();
extern "C" void MZOF_load_record_trie(const char *rtrie_url, const char* fname);

extern "C" void fsync_success();

extern "C" void runHttpTrie();

extern "C" void flush_trie(int length, int* int_trie_bytes);

