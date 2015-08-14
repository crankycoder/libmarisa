extern "C" void test_http_record_trie();
extern "C" void MZOF_lookup_rtrie(const char *fname, int count, ...);
extern "C" void MZOF_mount_idbfs();
extern "C" void MZOF_sync_idbfs();
extern "C" void MZOF_load_record_trie(const char *rtrie_url, const char* fname);
