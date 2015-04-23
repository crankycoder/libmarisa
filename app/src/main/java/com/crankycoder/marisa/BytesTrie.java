/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

public class BytesTrie extends _Trie {

    public List<byte[]> b_get_value(String key) {

        LinkedList<byte[]> result = new LinkedList<byte[]>();

        Agent ag = new Agent();

        // TODO: b_set_query should really take in a byte array
        // so that we can properly compute `prefix_len`
        ag.b_set_query(key);

        int prefix_len = key.length()+1;

        while (predictive_search(ag)) {
            byte[] slice = ag.key().ptr();
            byte[] subArray = Arrays.copyOfRange(slice, prefix_len, ag.key().length());
            result.add(subArray);
        }
        return result;
    }

}
