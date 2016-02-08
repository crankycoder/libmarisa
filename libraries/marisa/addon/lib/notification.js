exports.OfflineNotification = OfflineNotification;

var self = require("sdk/self");
var {Cc, Ci, Cu, Cr, Cm, components} = require("chrome");
var winutils = require('sdk/window/utils');
var tabs = require('sdk/tabs');
var url = require('sdk/url');

Cu.import('resource://gre/modules/PopupNotifications.jsm');

function OfflineNotification() {
}

OfflineNotification.prototype = {
    show: function(w, l) {
              var worker = w;
              var locator = l;

              var browserWindow = winutils.getMostRecentBrowserWindow();
              var gBrowser = browserWindow.gBrowser;

              var hostname = this.currentHostname();

              if (locator.get_share_location(hostname) == undefined) {
                  var notify  = new PopupNotifications(gBrowser,
                          browserWindow.document.getElementById("notification-popup"),
                          browserWindow.document.getElementById("notification-popup-box"));


                  // TODO: there doesn't seem to be a way to style the
                  // message from the JS api to the PopupNotification
                  notify.show(gBrowser,
                          "geolocation",
                          'Would you like to share your location with the site: ' + hostname + '?',
                          "geo-notification-icon", /* anchor ID */
                          {
                              label: "Share Location",
                              accessKey: "A",
                              callback: function() {
                                  console.log("locator.startWatch start: " + locator);
                                  locator.startWatch();
                              }
                          },
                          [
                            { label: "Always share location",
                              accessKey: "B",
                              callback: function() {
                                  locator.set_share_location(hostname, true);
                                  locator.startWatch();
                              }
                            },
                            { label: "Never share location",
                              accessKey: "C",
                              callback: function() {
                                  // emit a message to send a
                                  // PositionError
                                  locator.set_share_location(hostname, false);
                                  worker.port.emit("offline_fix_unavailable", {});
                              }
                            }
                          ]
                          );
              } else if (locator.get_share_location(hostname) == true) {
                  locator.startWatch();
              } else if (locator.get_share_location(hostname) == false) {
                  worker.port.emit("offline_fix_unavailable", {});
              }
    },
    currentHostname: function() {
        return (new url.URL(tabs.activeTab.url)).hostname;
    }
}

