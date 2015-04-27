/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class BytesTrie extends Trie {
    private native void bGetValue(long handle, byte[] b_prefix, ArrayList<byte[]> resultArrayList);


    /*
     Return a list of payloads (as byte objects) for a given key.
     */
    public List<byte[]> get(String key) {
        return b_get_value(key.getBytes(Charset.forName("UTF-8")));
    }


    List<byte[]> b_get_value(byte[] byte_key) {
        byte _VALUE_SEPARATOR = (byte)0xff;
        ArrayList<byte[]> result = new ArrayList<byte[]>();
        byte[] b_prefix = Arrays.copyOf(byte_key, byte_key.length+1);
        b_prefix[b_prefix.length-1] = _VALUE_SEPARATOR;

        // Note that bGetValue will mutate the contents of result
        bGetValue(handle, b_prefix, result);
        return result;
    }

}
