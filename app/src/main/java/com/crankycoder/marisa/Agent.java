/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

import android.util.Log;

/*
 This is a minimal port of the Agent class
 */
public class Agent {

    static {
        System.loadLibrary("stlport_shared");
        System.loadLibrary("marisa");
    }

    final long handle;
    private native long newAgent();
    private native int getKeyLength();
    private native long bSetQuery(long handle, byte[] b_prefix);

    public Agent() {
        handle = newAgent();
    }

    public void b_set_query(byte[] b_prefix) {
        bSetQuery(handle, b_prefix);
    }

    public int keyLength() {
        return getKeyLength();
    }


}
