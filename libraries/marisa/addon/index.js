var self = require("sdk/self");
var data = require("sdk/self").data;
var pageMod = require("sdk/page-mod");

var simple_prefs = require("sdk/simple-prefs");

var {Cc, Ci, Cu, Cr, Cm, components} = require("chrome");

// Import NetUtil
Cu.import("resource://gre/modules/NetUtil.jsm");

// Import FileUtils
Cu.import("resource://gre/modules/FileUtils.jsm");

// Raw writes to files
Cu.import("resource://gre/modules/osfile.jsm");


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

                             this.sha256 = require("./lib/sha256");

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
                                          this.ordered_city_data = this.PP.parse(bytes).data;

                                          for (let i=this.ordered_city_data.length-1; i >= 0; i--) {
                                              // clean up the
                                              // ordered city data to make
                                              // sure all rows are 2
                                              // elements long and cast
                                              // all elements into integer
                                              // This can be done
                                              // within the parse
                                              // function on
                                              // PP.parse(...)
                                              var row_data = this.ordered_city_data[i];
                                              if (row_data.length != 2) {
                                                  this.ordered_city_data = this.ordered_city_data.slice(0,i);
                                              } else {
                                                  row_data[0] = parseInt(row_data[0], 10);
                                                  row_data[1] = parseInt(row_data[1], 10);
                                              }
                                          }


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

                              this.macList = macList;
                              for (var i=0;i<macList.length;i++) {
                                  // convert each element in
                                  // this.macList into a sha256 hexdigest
                                  // and use the 12 character prefix of
                                  // the hash
                                  var bssid = this.macList[i];
                                  this.macList[i] = this.sha256.hash(bssid).slice(0,12);
                              }


                              // Note that we have to tack an extra
                              // delimiter at the end of line so that
                              // we can splice the string up easily in
                              // C++ land
                              let delimiter = "|";
                              let simpleMacList = this.macList.join(delimiter) + delimiter;

                              console.log("index.js captured macList: " + simpleMacList);
                              console.log("index.js captured rtrie_handle: " + this.rtrie_handle);
                              console.log("index.js captured offlinegeo_mod: " + this.offlinegeo_mod);

                              trie_lookup = this.offlinegeo_mod.cwrap(
                                      'trie_lookup', 'string', ['number', 'string']
                                      );

                              var result = trie_lookup(this.rtrie_handle, simpleMacList);

                              var tile_ids = result.split("|").map(function(k) { return parseInt(k, 10); });
                              var hit_map = {};
                              // Cast all the tile_id values to int
                              for (var idx = 0; idx < tile_ids.length; idx++) {
                                  var int_tile_id = tile_ids[idx];
                                  if (int_tile_id in hit_map) {
                                      hit_map[int_tile_id] += 1;
                                  } else {
                                      hit_map[int_tile_id] = 1;
                                  }
                              }

                              /********************************/
                              var inverted = {};
                              var tile_hits_arr = Object.keys(hit_map).map(function (tile_id) {
                                  var tile_hits = hit_map[tile_id];
                                  if (tile_hits in inverted) {
                                      inverted[tile_hits].push(tile_id);
                                  } else {
                                      inverted[tile_hits] = [tile_id];
                                  }
                                  return tile_hits;
                              });
                              var max = Math.max.apply(null, tile_hits_arr);
                              /********************************/

                              if (max > 1) {
                                  // Proper match found
                                  var matched_tile_ids = inverted[max];
                                  console.log("Matched tile IDs: " + matched_tile_ids);

                                  function tile2lon(x,z) {
                                      return (x/Math.pow(2,z)*360-180);
                                  }
                                  function tile2lat(y,z) {
                                      var n=Math.PI-2*Math.PI*y/Math.pow(2,z);
                                      return (180/Math.PI*Math.atan(0.5*(Math.exp(n)-Math.exp(-n))));
                                  }

                                  for (var idx = 0; idx < matched_tile_ids.length; idx++) {
                                      var tmp_tileid = matched_tile_ids[idx];
                                      var osm_tuple = this.ordered_city_data[tmp_tileid];

                                      var lat = tile2lat(osm_tuple[1], 18);
                                      var lon = tile2lon(osm_tuple[0], 18);

                                      console.log("Match tuple is: " + osm_tuple);
                                      console.log("Lat/lon: ["+lat+"] ["+lon+"]");
                                      worker.port.emit("offline_fix_found", {'lat': lat, 'lon': lon});
                                      console.log("Emitted the offline_fix data.");
                                  }
                                  // TODO: pass the lat/lon back over
                                  // the message passing interface
                                  return matched_tile_ids;
                              } else {
                                  // 0 or 1 hits isn't enough to
                                  // resolve a fix
                              }

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
