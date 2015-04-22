package com.crankycoder.marisa;

public class _Trie {

    static {
        System.loadLibrary("stlport_shared");
        System.loadLibrary("marisa");
    }


    native long newTrie();
    native void deallocTrie(long handle);
    native long mmapFile(long handle, String path);
    native boolean predictiveSearch(long handle,
                                    long agentHandle);

    public long handle;

    public _Trie() {
        handle = newTrie();
    }

    public void dealloc() {
        deallocTrie(handle);
    }

    public void mmap(String path) {
        /*
         * Mmap trie to a file; this allows lookups without loading full
         * trie to memory.
         */
        handle = mmapFile(handle, path);
    }

    public boolean predictive_search(Agent ag) {
        return predictiveSearch(handle, ag.handle);
    }

}
