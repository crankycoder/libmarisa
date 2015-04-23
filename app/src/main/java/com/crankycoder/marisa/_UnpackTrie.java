/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

import java.util.LinkedList;
import java.util.List;

public class _UnpackTrie extends BytesTrie {

    @Override
    public List<byte[]> b_get_value(String key) {
        List<byte[]> result = new LinkedList<byte[]>();
        for (byte[] v: super.b_get_value(key)) {
            result.add(_unpack(v));
        }
        return result;
    }

    public byte[] _unpack(byte[] x) {
        return x;
    }
}
