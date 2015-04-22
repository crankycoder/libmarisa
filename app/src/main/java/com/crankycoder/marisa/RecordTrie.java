/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

/*
 This class provides a subset of the API that the RecordTrie in Python provides.
 */
public class RecordTrie extends Trie {

    private final String struct_fmt;

    // Initialize the RecordTrie
    public RecordTrie(String fmt) {
        struct_fmt = fmt;
    }



}
