exports.OfflineNotification = OfflineNotification;

var {Cc, Ci, Cu, Cr, Cm, components} = require("chrome");

var self = require('sdk/self');
var winutils = require('sdk/window/utils');
var tabs = require('sdk/tabs');
var url = require('sdk/url');
var mod_geoperm = require('./geoperm');

Cu.import('resource://gre/modules/Services.jsm');
Cu.import('resource://gre/modules/PopupNotifications.jsm');
Cu.import('resource://gre/modules/XPCOMUtils.jsm');

XPCOMUtils.defineLazyGetter(this, "gBrowserBundle", function() {
    return Services.strings.createBundle("chrome://browser/locale/browser.properties");
});

function makeURI(aURL, aOriginCharset, aBaseURI) {
    var ioService = Cc["@mozilla.org/network/io-service;1"]
        .getService(Ci.nsIIOService);
    return ioService.newURI(aURL, aOriginCharset, aBaseURI);
}

function OfflineNotification() {
}

OfflineNotification.prototype = {
    show: function(w, l) {
              var worker = w;
              var locator = l;

              var browserWindow = winutils.getMostRecentBrowserWindow();
              var gBrowser = browserWindow.gBrowser;

              var hostname = this.currentHostname();
              var thisURI = makeURI(tabs.activeTab.url);

              var sitePermission = new mod_geoperm.GeoSitePermission(thisURI);

              if (sitePermission.isUnknown()) {
                  var notify  = new PopupNotifications(gBrowser,
                          browserWindow.document.getElementById("notification-popup"),
                          browserWindow.document.getElementById("notification-popup-box"));

                  var message = "";
                  if (thisURI.schemeIs("file")) {
                      message = gBrowserBundle.GetStringFromName("geolocation.shareWithFile2");
                  } else {
                      message = gBrowserBundle.GetStringFromName("geolocation.shareWithSite2");
                  }

                  var aOptions = { displayURI: thisURI,
                                   learnMoreURL: Services.urlFormatter.formatURLPref("browser.geolocation.warning.infoURL")};

                  notify.show(gBrowser,
                          "geolocation",
                          message,
                          "geo-notification-icon", /* anchor ID */
                          {
                              label: "Share Location",
                              accessKey: "A",
                              callback: function() {
                                  console.log("locator.startWatch start: " + locator);
                                  sitePermission.setShareSession();
                                  locator.startWatch();
                              }
                          },
                          [
                            { label: "Always share location",
                              accessKey: "B",
                              callback: function() {
                                  sitePermission.setAlwaysShare();
                                  locator.startWatch();
                              }
                            },
                            { label: "Never share location",
                              accessKey: "C",
                              callback: function() {
                                  // emit a message to send a
                                  // PositionError
                                  sitePermission.setBlocked();
                                  worker.port.emit("offline_fix_unavailable", {});
                              }
                            }
                          ], aOptions);
              } else if (sitePermission.isAlwaysShare() || sitePermission.isSessionSharing()) {
                  locator.startWatch();
              } else if (sitePermission.isBlocked()) {
                  worker.port.emit("offline_fix_unavailable", {});
              }
    },
    currentHostname: function() {
        return (new url.URL(tabs.activeTab.url)).hostname;
    }
}

