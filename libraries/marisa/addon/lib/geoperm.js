/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

'use strict';

/* global Services: false, SitePermissions: false */


const {Ci, Cu} = require('chrome');

Cu.import("resource://gre/modules/Services.jsm");
Cu.import("resource:///modules/SitePermissions.jsm");

// Construct this site permission object with the nsIURI object
// https://developer.mozilla.org/en-US/docs/Mozilla/Tech/XPCOM/Reference/Interface/nsIURI
//
function GeoSitePermission(nsIURI) {
    this.uri = nsIURI;
}

GeoSitePermission.prototype = {

    UNKNOWN: Services.perms.UNKNOWN_ACTION,
    ALLOW: Services.perms.ALLOW_ACTION,
    BLOCK: Services.perms.DENY_ACTION,
    SESSION: Ci.nsICookiePermission.ACCESS_SESSION,

    isUnknown: function() {
        // A shortcut as we need to check for unknown state all the
        // time.
        return this._getState() == this.UNKNOWN;
    },
    isAlwaysShare: function() {
        return this._getState() == this.ALLOW;
    },
    isSessionSharing: function() {
        return this._getState() == this.SESSION;
    },
    isBlocked: function() {
        return this._getState() == this.BLOCK;
    },
    _getState: function() {
        return SitePermissions.get(this.uri, "geo");
    },
    setAlwaysAsk: function() {
        // Always share the location for this domain
        SitePermissions.set(this.uri, "geo", this.UNKNOWN);
    },
    setAlwaysShare: function() {
        // Always share the location for this domain
        SitePermissions.set(this.uri, "geo", this.ALLOW);
    },
    setNeverShare: function() {
        // Never share the location for this domain
        SitePermissions.set(this.uri, "geo", this.BLOCK);
    },
    setShareSession: function() {
        // Always ask the user for permission each time
        SitePermissions.set(this.uri, "geo", this.SESSION);
    }
};

exports.GeoSitePermission = GeoSitePermission;
