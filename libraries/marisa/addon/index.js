var self = require("sdk/self");
var data = require("sdk/self").data;
var pageMod = require("sdk/page-mod");

var {Cc, Ci, Cu, Cr, Cm, components} = require("chrome");

// Import NetUtil
Cu.import("resource://gre/modules/NetUtil.jsm");

// Import FileUtils
Cu.import("resource://gre/modules/FileUtils.jsm");

// Raw writes to files
Cu.import("resource://gre/modules/osfile.jsm");

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

                      function test() {
                          this.offlinegeo = require("./lib/offlinegeo");
                          this.offlinegeo_mod = this.offlinegeo.offline_factory();

                          this.default_trie_url = "http://127.0.0.1:8000/simple.trie";

                          this.fetchTrie();

                          console.log("offlinegeo_mod: " + this.offlinegeo_mod);
                      }


                      test.prototype = {
                          
                          fetchTrie: function(trie_url) {
                             // This method grabs the trie off the
                             // webserver.  We return an integer array
                             // of data which represents the bytes
                             // within the trie.
                             // On failure, we return null.
                             
                             if (!trie_url) {
                                 // Use the default trie URL if
                                 // nothing is passed in.
                                 trie_url = this.default_trie_url;
                             }
                             NetUtil.asyncFetch(
                                  trie_url,
                                  function(aInputStream, aResult) {
                                      // Check that we had success.
                                      if (!components.isSuccessCode(aResult))
                                      {
                                          console.log("An error occured while fetching the trie: " + aResult);
                                          return null;
                                      } else {
                                          var bstream = Cc["@mozilla.org/binaryinputstream;1"].
                                                createInstance(Ci.nsIBinaryInputStream);
                                          bstream.setInputStream(aInputStream);
                                          var bytes = bstream.readBytes(bstream.available());

                                          // Extract each of `bytes.charCodeAt(index)`
                                          // and stuff it into an integer
                                          // array and pass it into C.


                                          // Extract the bytes and stuff
                                          // them into a heap allocated
                                          // object that C++ can read from
                                          this.int_array = [];
                                          for (var i=0;i<bytes.length;i++) {
                                              this.int_array.push(bytes.charCodeAt(i));
                                          }
                                          this.data = Uint32Array.from(this.int_array);

                                          //////////////////////////////////
                                          // Don't pull this out into
                                          // a function
                                          // Dump a typed array of data into a file
                                          var profileDir = FileUtils.getFile("ProfD", []);
                                          OS.File.writeAtomic(profileDir.path + "/cached.rtrie", data).then(
                                                  function(aResult) { 
                                                      console.log("Write completed! Pushing into emscripten"); 
                                                      this.processData();
                                                  });
                                          //
                                          // Trust me.  Just don't.
                                          //////////////////////////////////


                                    }
                                }
                             )
                          },
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

                              console.log("Got raw mac addresses : ["+macList+"]");

                              var wifi_service = Cc["@mozilla.org/wifi/monitor;1"].getService(Ci.nsIWifiMonitor);
                              wifi_service.stopWatching(this);

                              this.offlinegeo_mod._MZOF_test_http_recordtrie();

                          },
                          processData: function() {
                              var nDataBytes = this.data.length * this.data.BYTES_PER_ELEMENT;
                              this.dataPtr = this.offlinegeo_mod._malloc(nDataBytes);

                              // Copy data to Emscripten heap
                              // (directly accessed from Module.HEAPU8)
                              this.dataHeap = new Uint8Array(this.offlinegeo_mod.HEAPU8.buffer,
                                                            dataPtr,
                                                            nDataBytes);
                              this.dataHeap.set(new Uint8Array(this.data.buffer));

                              // TODO: move this to after
                              // the point where we call
                              // into emscripten
                              this.offlinegeo_mod._free(this.dataHeap.byteOffset);
                              // TODO: set this.dataPtr to null
                              // TODO: set this.dataHeap to null
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

                      if (addonMessage == "startOfflineScan") {
                          wifi_service.startWatching(listener);
                          console.log("wifi monitor is hooked and started!");
                      }

                  });
              }
});



