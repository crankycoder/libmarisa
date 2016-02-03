var self = require("sdk/self");
var pageMod = require("sdk/page-mod");
var simple_prefs = require("sdk/simple-prefs");
var libofflinegeo = require("./lib/offlinegeo");
var offlinegeo_mod = libofflinegeo.offline_factory();
var libtrielookup = require("./lib/trielookup");

var locator_container = {};
locator_container['locator'] = new libtrielookup.TrieLocator(offlinegeo_mod);

function getLocator() {
    return locator_container['locator'];
}

function update_city_urls() {
    var offline_base_url = simple_prefs.prefs["offlineCity"];
    var trie_url = offline_base_url + "/area.trie";
    var city_url = offline_base_url + "/ordered_city.csv";

    console.log("Updating with URLs: ["+trie_url+"]");
    console.log("Updating with URLs: ["+city_url+"]");
    getLocator().setDataURLs(trie_url, city_url);
    getLocator().fetchTrie();
}

// Register an update listener
function onPrefChange(prefName) {
    console.log("The preference " + prefName + " value has changed!");
    console.log("New pref value: [" + simple_prefs.prefs[prefName] + "]");
    update_city_urls();
}
update_city_urls();
simple_prefs.on("offlineCity", onPrefChange);

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
                      locator_worker = getLocator().set_worker(worker);

                      console.log("Addon received message: ["+addonMessage+"]");
                      if (addonMessage == "startOfflineScan") {
                          locator_worker.startWatch();
                      }
                  });
              }
});
