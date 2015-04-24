/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

import android.util.Log;

import java.nio.charset.Charset;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

public class BytesTrie extends Trie {


    private native void bGetValue(long handle, byte[] b_prefix);


    /*
     Return a list of payloads (as byte objects) for a given key.
     */
    public List<byte[]> get(byte[] b_key) {
        return b_get_value(b_key);
    }


    /*
     Return a list of payloads (as byte objects) for a given key.
     */
    public List<byte[]> get(String key) {
        return b_get_value(key.getBytes(Charset.forName("UTF-8")));
    }


    List<byte[]> b_get_value(byte[] byte_key) {

        Log.i("libmarisa", "Raw key byte_key for b_get_value is : ["+new String(byte_key)+"]");

        byte _VALUE_SEPARATOR = (byte)0xff;

        LinkedList<byte[]> result = new LinkedList<byte[]>();

        byte[] b_prefix = Arrays.copyOf(byte_key, byte_key.length+1);
        b_prefix[b_prefix.length-1] = _VALUE_SEPARATOR;


        Log.i("libmarisa", "Setting b_prefix to len: " + b_prefix.length);

        Log.i("libmarisa", "Setting b_prefix[0] to: " + Integer.toHexString(b_prefix[0]));
        Log.i("libmarisa", "Setting b_prefix[1] to: " + Integer.toHexString(b_prefix[1]));
        Log.i("libmarisa", "Setting b_prefix[2] to: " + Integer.toHexString(b_prefix[2]));
        Log.i("libmarisa", "Setting b_prefix[3] to: " + Integer.toHexString(b_prefix[3]));
        
        bGetValue(handle, b_prefix);

        return result;
    }

}
