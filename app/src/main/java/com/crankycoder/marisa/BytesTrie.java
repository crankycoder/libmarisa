/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

import java.util.LinkedList;
import java.util.List;

public class BytesTrie extends _Trie {

    public List<byte[]> b_get_value(String key) {

        LinkedList<byte[]> result = new LinkedList<byte[]>();

        Agent ag = new Agent();
        ag.b_set_query(key);
        return result;
    }

}
