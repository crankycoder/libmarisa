package com.crankycoder.marisa;

public class Trie {

    static {
        System.loadLibrary("stlport_shared");
        System.loadLibrary("marisa");
    }


    public static byte _VALUE_SEPARATOR = (byte) 0xff;

    native long newTrie();
    native void deallocTrie(long handle);
    native long mmapFile(long handle, String path);

    public long handle;

    public Trie() {
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



}
