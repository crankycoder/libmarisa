/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

public class BytesTrie extends _Trie {

    public List<byte[]> b_get_value(byte[] byte_key) {

        byte _VALUE_SEPARATOR = (byte)0xff;

        LinkedList<byte[]> result = new LinkedList<byte[]>();

        byte[] b_prefix = Arrays.copyOf(byte_key, byte_key.length+1);
        b_prefix[b_prefix.length-1] = _VALUE_SEPARATOR;

        Agent ag = new Agent();

        ag.b_set_query(b_prefix);

        int prefix_len = b_prefix.length;

        while (predictive_search(ag)) {
            byte[] slice = ag.key().ptr();
            byte[] subArray = Arrays.copyOfRange(slice, prefix_len, ag.key().length());
            result.add(subArray);
        }
        return result;
    }

}
