var self = require("sdk/self");
var data = require("sdk/self").data;
var pageMod = require("sdk/page-mod");

var {Cc, Ci, Cu, Cr, Cm, components} = require("chrome");

var page = pageMod.PageMod({
    include: "*",
    contentScriptWhen: "start",
    contentScriptFile: [data.url("geo-script.js"), data.url("customWifi.js")],
    contentScriptOptions: {
        showOptions: true
    },
    onAttach: function(worker) {

                  worker.port.on("check_chrome_bits", function(addonMessage) {
                      function test() {
                      }

                      test.prototype =
                      {
                          onChange: function (accessPoints)
                          {

                              // Destructuring assignment to get
                              // utility functions
                              let { add } = require('sdk/util/array');

                              let macList = [];

                              for (var i=0; i < accessPoints.length; i++) {
                                  var a = accessPoints[i];

                                  // Clean up the BSSID
                                  let bssid = a.mac;

                                  if (bssid) {
                                      bssid = bssid.toLowerCase();
                                      bssid = bssid.replace(/\W+/g, "")
                                          add(macList, bssid);
                                  }

                              }

                              // TODO: check the length of macList
                              // to ensure that it's > 0
                              console.log("Got mac addresses : ["+macList+"]");
                              self.port.emit("check_chrome_bits", "stopScan");
                          },

                          onError: function (value) {
                                       alert("error: " +value);
                                   },

                          QueryInterface: function(iid) {
                                              if (iid.equals(Ci.nsIWifiListener) ||
                                                      iid.equals(Ci.nsISupports)) {
                                                  return this;
                                              }
                                              throw components.results.NS_ERROR_NO_INTERFACE;
                                          },
                      }


                      var wifi_service = Cc["@mozilla.org/wifi/monitor;1"].getService(Ci.nsIWifiMonitor);
                      var listener = new test();

                      console.log("Addon received message: ["+addonMessage+"]");

                      if (addonMessage == "startScan") {
                          wifi_service.startWatching(listener);
                          console.log("wifi monitor is hooked and started!");
                      }

                  });
              }
});



