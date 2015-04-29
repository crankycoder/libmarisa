/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

import java.util.ArrayList;

public class Record {

    /*
     This class provides support to extract python structs.

     Note that Java only supports signed types.

     This also only supports sig
     */

    private ArrayList<Object> values = new ArrayList<Object>();

    public Integer getInt(int offset) {
        return (Integer) values.get(offset);
    }

    /*
      This maps to python's struct format of 'i'
     */
    public void putInt(int v) {
        values.add(new Integer(v));
    }

    /*
      This maps to python's struct format of 'b'
     */
    public void putByte(byte b) {
        values.add(new Byte(b));
    }

    public Byte getByte(int offset) {
        return (Byte) values.get(offset);
    }

    public int size() {
        return values.size();
    }

}
