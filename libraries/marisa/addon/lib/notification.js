exports.OfflineNotification = OfflineNotification;

var {Cc, Ci, Cu, Cr, Cm, components} = require("chrome");
var winutils = require('sdk/window/utils');
Cu.import('resource://gre/modules/PopupNotifications.jsm');

function OfflineNotification() {
}

OfflineNotification.prototype = {
    show: function() {
              var browserWindow = winutils.getMostRecentBrowserWindow();
              var gBrowser = browserWindow.gBrowser;
              var notify  = new PopupNotifications(gBrowser,
                      browserWindow.document.getElementById("notification-popup"),
                      browserWindow.document.getElementById("notification-popup-box"));
              notify.show(gBrowser, "geolocation",
                      "This is a sample popup notification.",
                      "geo-notification-icon", /* anchor ID */
                      {
                          label: "Do Something",
                  accessKey: "D",
                  callback: function() {alert("Doing something awesome!"); }
                      },
                      null  /* secondary action */
                      );
    }
}

