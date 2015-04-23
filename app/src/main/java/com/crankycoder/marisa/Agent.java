/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

/*
 This is a minimal port of the Agent class
 */
public class Agent {

    static {
        System.loadLibrary("stlport_shared");
        System.loadLibrary("marisa");
    }

    private native long newAgent();
    private native long getKeyHandle();
    private native void bSetQuery(long handle, String prefix);

    final long handle;

    public Agent() {
        handle = newAgent();
    }

    public void b_set_query(String prefix) {
        bSetQuery(handle, prefix);
    }

    public Key key() {
        return new Key(getKeyHandle());
    }
}
