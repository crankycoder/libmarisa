var self = require("sdk/self");
var pageMod = require("sdk/page-mod");
var simple_prefs = require("sdk/simple-prefs");

var libofflinegeo = require("./lib/offlinegeo");
var offlinegeo_mod = libofflinegeo.offline_factory();

var libtrielookup = require("./lib/trielookup");

function onPrefChange(prefName) {
    console.log("The preference " + prefName + " value has changed!");
    console.log("New pref value: [" + simple_prefs.prefs[prefName] + "]");
    // TODO: update the trie and ordered city tile data herd
}

simple_prefs.on("offlineCity", onPrefChange);

// TODO: do a quick test to see if we've downloaded any trie and city
// tile data.  If no such content exists in persistent storage, force
// a download of the data.
//
//
var locator = new libtrielookup.TrieLocator(offlinegeo_mod);
locator.fetchTrie();

var page = pageMod.PageMod({
    include: "*",
    contentScriptWhen: "start",
    contentScriptFile: [self.data.url("geo-script.js"), self.data.url("customWifi.js")],
    contentScriptOptions: {
        showOptions: true
    },
    onAttach: function(worker) {
                  worker.port.on("check_chrome_bits", function(addonMessage) {
                      // TODO: split these operations:
                      // 1. construction with offlinegeo_mod
                      // 2. Passing in the worker
                      // 3. Downloading the trie
                      // Note: the trielookup needs to return an
                      // object for *each* worker and the lookup must
                      // operate scoped to just that worker.
                      locator.set_worker(worker);

                      console.log("Addon received message: ["+addonMessage+"]");
                      if (addonMessage == "startOfflineScan") {
                          locator.startWatch();
                      }
                  });
              }
});
