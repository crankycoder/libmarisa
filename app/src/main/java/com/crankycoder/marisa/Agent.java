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
    private long queryHandle = 0;

    private native long newAgent();

    private native long getKeyHandle();

    private native long bSetQuery(long handle, byte[] b_prefix);
    private native void freeCharStar(long queryHandle);

    public Agent() {
        handle = newAgent();
    }

    public void b_set_query(byte[] b_prefix) {
        queryHandle = bSetQuery(handle, b_prefix);
    }

    public Key key() {
        return new Key(getKeyHandle());
    }

    // This must be called after the Agent is no longer used
    public void dealloc() {
        Log.i("libmarisa", "dealloc Agent");

        if (queryHandle != 0) {
            freeCharStar(queryHandle);
        }
    }

}
