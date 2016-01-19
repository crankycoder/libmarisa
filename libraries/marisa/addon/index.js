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

                          // These URLs are preconfigured in my test
                          // server.
                          this.default_trie_url = "http://ec2-52-1-93-147.compute-1.amazonaws.com/offline_geo/newmarket/area.trie";
                          this.ordered_city_url = "http://ec2-52-1-93-147.compute-1.amazonaws.com/offline_geo/newmarket/ordered_city.csv";

                          console.log("offlinegeo_mod: " + this.offlinegeo_mod);
                      }


                      test.prototype = {

                          fetchTrie: function(trie_url) {
                             // This method grabs the trie off the
                             // webserver.  We return an integer array
                             // of data which represents the bytes
                             // within the trie.
                             // On failure, we return null.

                             if (trie_url) {
                                 // Use the default trie URL if
                                 // nothing is passed in.
                                 this.default_trie_url = trie_url;
                             }

                             var chainAsyncFetchTrie = (trie_url) => {
                                 console.log("successfully chained off the async fetch of the marisa trie!");
                                 NetUtil.asyncFetch(
                                      this.default_trie_url,
                                      (aInputStream, aResult) => {
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

                                              // Dump a typed array of data into a file
                                              var profileDir = FileUtils.getFile("ProfD", []);
                                              var atomicCallback = (aResult) => {
                                                  var byteData = Uint8Array.from(this.int_array, (n) => n);
                                                  console.log("Write completed! Pushing into emscripten"); 
                                                  var nDataBytes = byteData.length * byteData.BYTES_PER_ELEMENT;

                                                  var dataPtr = this.offlinegeo_mod._malloc(nDataBytes);
                                                  console.log("malloc'd "+nDataBytes+" bytes");

                                                  // Copy data to Emscripten heap
                                                  // (directly accessed from Module.HEAPU8)
                                                  var dataHeap = new Uint8Array(this.offlinegeo_mod.HEAPU8.buffer,
                                                                                dataPtr,
                                                                                nDataBytes);
                                                  dataHeap.set(new Uint8Array(byteData.buffer));

                                                  // This pushes the trie
                                                  // into C++ emscripten
                                                  // space
                                                  push_trie = this.offlinegeo_mod.cwrap(
                                                            'push_trie', 'number', ['number', 'number']
                                                          );

                                                  this.rtrie_handle = push_trie(nDataBytes, dataPtr);

                                                  ////////////////simple_test_trie = this.offlinegeo_mod.cwrap(
                                                  //          'simple_test_trie', null, ['number']
                                                  //        );
                                                  //simple_test_trie(this.rtrie_handle);

                                                  // You must free the
                                                  // memory after playing
                                                  // in emscripten land
                                                  this.offlinegeo_mod._free(dataHeap.byteOffset);
                                                  console.log("free'd bytes for dataHeap.byteOffset");
                                              }

                                              // I have no idea why I have
                                              // to do this double
                                              // Uint8Array copy here, and
                                              // in the atomicCallback
                                              OS.File.writeAtomic(profileDir.path + "/cached.rtrie",
                                                      Uint8Array.from(this.int_array, (n) => n)).then(
                                                      atomicCallback);
                                        }}
                                 )
                             }

                             console.log("Loading papaparse!");
                             this.PP = require("./lib/papaparse");
                             console.log("Successfully loaded papaparse: " + this.PP);

                             // Fetch the city tiles
                             NetUtil.asyncFetch(
                                  this.ordered_city_url,
                                  (aInputStream, aResult) => {
                                      // Check that we had success.
                                      if (!components.isSuccessCode(aResult))
                                      {
                                          console.log("An error occured while fetching the ordered tile data: " + aResult);
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
                                          this.ordered_city_data = this.PP.parse(bytes);

                                          Array.prototype.remove = function(from, to) {
                                              var rest = this.slice((to || from) + 1 || this.length);
                                              this.length = from < 0 ? this.length + from : from;
                                              return this.push.apply(this, rest);
                                          };

                                          for (let i=this.ordered_city_data.data.length-1; i >= 0; i--) {
                                              var row_data = this.ordered_city_data.data[i];
                                              if (row_data.length != 2) {
                                                  this.ordered_city_data.data = this.ordered_city_data.data.slice(0,i);
                                              } else {
                                                  row_data[0] = parseInt(row_data[0], 10);
                                                  row_data[1] = parseInt(row_data[1], 10);
                                              }
                                          }

                                          // TODO: clean up the
                                          // ordered city data to make
                                          // sure all rows are 2
                                          // elements long and cast
                                          // all elements into integer

                                          // Dump a typed array of data into a file
                                          var profileDir = FileUtils.getFile("ProfD", []);
                                          var atomicCallback = (aResult) => {
                                              console.log("Status of writing city tile data: " + aResult);
                                              chainAsyncFetchTrie();
                                          }

                                          // I have no idea why I have
                                          // to do this double
                                          // Uint8Array copy here, and
                                          // in the atomicCallback
                                          OS.File.writeAtomic(profileDir.path + "/ordered_city.csv",
                                                  Uint8Array.from(this.int_array, (n) => n)).then(
                                                  atomicCallback);
                                    }}
                             )

                             // Fetch the marisa trie
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

                              // Bind the maclist to the 'test'
                              // instance
                              this.macList = macList;

                              // Note that we have to tack an extra
                              // delimiter at the end of line so that
                              // we can splice the string up easily in
                              // C++ land
                              let delimiter = "|";
                              let simpleMacList = this.macList.join(delimiter) + delimiter;

                              console.log("index.js captured macList: " + simpleMacList);
                              console.log("index.js captured rtrie_handle: " + this.rtrie_handle);
                              console.log("index.js captured offlinegeo_mod: " + this.offlinegeo_mod);

                              // TODO: invoke a function passing in
                              // the rtrie_handle, the simpleMacList
                              // and then getting a fix.
                              // Expect a return value of null or an
                              // integer encoded as a string which
                              // maps to a particular tile id.

                              trie_lookup = this.offlinegeo_mod.cwrap(
                                      'trie_lookup', 'string', ['number', 'string']
                                      );

                              var result = trie_lookup(this.rtrie_handle, simpleMacList);

                              console.log("------------ Result buffer --------------");
                              console.log(result);
                              console.log("-------------------------------------");

                              var wifi_service = Cc["@mozilla.org/wifi/monitor;1"].getService(Ci.nsIWifiMonitor);
                              wifi_service.stopWatching(this);

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

                      listener.fetchTrie();

                      console.log("Addon received message: ["+addonMessage+"]");

                      if (addonMessage == "startOfflineScan") {
                          wifi_service.startWatching(listener);
                          console.log("wifi monitor is hooked and started!");
                      }

                  });
              }
});



