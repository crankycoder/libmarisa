/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

/*
 This subclass of RecordTrie assumes that all parts of the struct are just basic integers
 */
public class IntRecordTrie {
    final RecordTrie delegate;
    private final String struct_fmt;
    final int numParts;


    public IntRecordTrie(String fmt) {
        delegate = new RecordTrie(fmt);
        struct_fmt = fmt;
        numParts = parseStructDef();
    }

    public int parseStructDef() {
        boolean hasOrdering = false;
        int offset = 0;

        if (struct_fmt.startsWith("<")) {
            hasOrdering = true;
        } else if (struct_fmt.startsWith(">") || struct_fmt.startsWith("!")) {
            hasOrdering = true;
        } else if (struct_fmt.startsWith("@") || struct_fmt.startsWith("=")) {
            // native encoding
            hasOrdering = true;
        }

        if (hasOrdering) {
            offset += 1;
        }

        int numCount = 0;
        for (; offset < struct_fmt.length(); offset++) {
            if (oneChar(struct_fmt, offset).equals("i")) {
                numCount += 1;
            } else {
                throw new RuntimeException("Invalid or unsupported format");
            }
        }
        return numCount;
    }

    public Set<Integer> getResultSet(String bssid) {
        Set<Integer> resultSet = new HashSet<Integer>();;
        List<Record> curBatch = delegate.getRecord(bssid);
        for (Record record: curBatch) {
            for (int i = 0; i < record.size(); i++) {
                Integer v = record.getInt(i);
                resultSet.add(v);
            }
        }
        return resultSet;
    }


    private String oneChar(String str, int offset) {
        return Character.toString(str.charAt(offset));
    }

    public void mmap(String absolutePath) {
        delegate.mmap(absolutePath);
    }
}
