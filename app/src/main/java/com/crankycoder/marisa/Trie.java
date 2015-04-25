package com.crankycoder.marisa;

import android.util.Log;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.nio.charset.Charset;

public class Trie {

    static {
        System.loadLibrary("stlport_shared");
        System.loadLibrary("marisa");
    }


    private native long newTrie();
    private native void deallocTrie(long handle);
    private native long mmapFile(long handle, String path);



    private native void load(long handle, String filePath);

    public long handle;

    public Trie() {
        handle = newTrie();
    }

    public void dealloc() {
        deallocTrie(handle);
    }

    /*
     This seems to be broken on Android
     */
    public void mmap(String path) {
        /*
         * Mmap trie to a file; this allows lookups without loading full
         * trie to memory.
         */
        handle = mmapFile(handle, path);
    }

    public void load(String path) {
        load(handle, path);
    }


}
