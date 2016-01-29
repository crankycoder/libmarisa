var self = require("sdk/self");
var data = require("sdk/self").data;
var pageMod = require("sdk/page-mod");

var simple_prefs = require("sdk/simple-prefs");

var {Cc, Ci, Cu, Cr, Cm, components} = require("chrome");
var wifi_service = Cc["@mozilla.org/wifi/monitor;1"].getService(Ci.nsIWifiMonitor);


var offlinegeo = require("./lib/offlinegeo");
var offlinegeo_mod = offlinegeo.offline_factory();
console.log("offlinegeo_mod: " + offlinegeo_mod);

var lib_trielookup = require("./lib/trielookup");

function onPrefChange(prefName) {
    console.log("The preference " + prefName + " value has changed!");
    console.log("New pref value: [" + simple_prefs.prefs[prefName] + "]");
    // TODO: update the trie and ordered city tile data herd
}


simple_prefs.on("offlineCity", onPrefChange);

// TODO: do a quick test to see if we've downloaded any trie and city
// tile data.  If no such content exists in persistent storage, force
// a download of the data.

var page = pageMod.PageMod({
    include: "*",
    contentScriptWhen: "start",
    contentScriptFile: [data.url("geo-script.js"), data.url("customWifi.js")],
    contentScriptOptions: {
        showOptions: true
    },
    onAttach: function(worker) {
                  worker.port.on("offline_bssid_scan", function(payload) {
                      console.log("Got payload: "+payload+"");

                      let bssid_list = payload["bssid_list"];

                  });

                  worker.port.on("check_chrome_bits", function(addonMessage) {
                      var listener = new lib_trielookup.test(offlinegeo_mod, worker);
                      listener.fetchTrie();
                      console.log("Addon received message: ["+addonMessage+"]");
                      if (addonMessage == "startOfflineScan") {
                          listener.startWatch();
                          console.log("wifi monitor is hooked and started!");
                      }
                  });
              }
});
