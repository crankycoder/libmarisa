/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

/*
 This class provides a subset of the API that the RecordTrie in Python provides.
 */
public class RecordTrie extends BytesTrie {

    private final String struct_fmt;

    // Initialize the RecordTrie
    public RecordTrie(String fmt) {
        struct_fmt = fmt;
    }


    /*
     Return a list of payloads (as Record objects) for a given key.
     */
    public List<Record> getRecord(String key) {
        return b_get_record(key.getBytes(Charset.forName("UTF-8")));
    }

    public List<Record> b_get_record(byte[] byte_key) {
        List<byte[]> byteList = super.b_get_value(byte_key);
        List<Record> result = new ArrayList<Record>();
        for (byte[] buf: byteList) {
            result.add(_unpack(buf));
        }
        return result;
    }


    public Record _unpack(byte[] inBytes) {
        Record result = new Record();

        boolean hasOrdering = false;
        int offset = 0;

        final ByteBuffer buf = ByteBuffer.wrap(inBytes);

        if (struct_fmt.startsWith("<")) {
            buf.order(ByteOrder.LITTLE_ENDIAN);
            hasOrdering = true;
        } else if (struct_fmt.startsWith(">") || struct_fmt.startsWith("!")) {
            buf.order(ByteOrder.BIG_ENDIAN);
            hasOrdering = true;
        } else if (struct_fmt.startsWith("@") || struct_fmt.startsWith("=")) {
            // native encoding
            hasOrdering = true;
        }

        if (hasOrdering) {
            offset += 1;
        }

        for (; offset < struct_fmt.length(); offset++) {
            // I'm super lazy and haven't implemented anything beyond signed
            // byte and signed int.
            if (oneChar(struct_fmt, offset).equals("i")) {
                result.putInt(buf.getInt());
            } else if (oneChar(struct_fmt, offset).equals("b")) {
                result.putByte(buf.get());
            } else {
                throw new RuntimeException("Invalid or unsupported format");
            }
        }
        return result;
    }

    private String oneChar(String str, int offset) {
        return Character.toString(str.charAt(offset));
    }
}
