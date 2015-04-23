/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package com.crankycoder.marisa;

public class Key {
    static {
        System.loadLibrary("stlport_shared");
        System.loadLibrary("marisa");
    }

    private native byte[] keyPtr(long keyHandle);

    // TODO: bind this
    private native int keyLength(long keyHandle);

    long handle;

    public Key(long keyHandle) {
        handle = keyHandle;
    }

    public byte[] ptr() {
        return keyPtr(handle);
    }

    public int length() {
        return keyLength(handle);
    }

}
