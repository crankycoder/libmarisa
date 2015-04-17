package com.crankycoder.marisa;

public class Trie {

    static {
        System.loadLibrary("stlport_shared");
        System.loadLibrary("marisa");
    }

    native long newTrie();
    native void deallocTrie(long handle);

    long handle;

    public String doTheThing() {
        handle = newTrie();
        return "Should get 5555: " + handle;
    }

    public String dealloc() {
        deallocTrie(handle);
        return "Deallocated trie!";
    }

}
