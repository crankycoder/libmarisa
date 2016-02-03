exports.TrieLocator = TrieLocator;

var {Cc, Ci, Cu, Cr, Cm, components} = require("chrome");

// Import NetUtil
Cu.import("resource://gre/modules/NetUtil.jsm");

// Import FileUtils
Cu.import("resource://gre/modules/FileUtils.jsm");

// Raw writes to files
Cu.import("resource://gre/modules/osfile.jsm");

console.log("Loading papaparse!");
PP = require("./papaparse");
console.log("Successfully loaded papaparse: " + PP);

var wifi_service = Cc["@mozilla.org/wifi/monitor;1"].getService(Ci.nsIWifiMonitor);

var sha256 = require("./sha256");

function TrieLocator(m) {
    // These URLs are preconfigured in my test
    // server.
    this.offlinegeo_mod = m;
    this.trie_area_url = null;
    this.ordered_city_url = null;

    this.has_data_loaded = false;

    // This pushes the trie
    // into C++ emscripten
    // space
    this.push_trie = this.offlinegeo_mod.cwrap('push_trie', 'number', ['number', 'number']);
}


TrieLocator.prototype = {
    setDataURLs: function(trie_url, city_url) {
        this.trie_area_url = trie_url;
        this.ordered_city_url = city_url;
    },
    fetchTrie: function() {
                   // This method grabs the trie off the
                   // webserver.  We return an integer array
                   // of data which represents the bytes
                   // within the trie.
                   // On failure, we return null.

                   var chainAsyncFetchTrie = () => {
                       console.log("Fetching: " + this.trie_area_url);
                       NetUtil.asyncFetch(
                               this.trie_area_url,
                               (aInputStream, aResult) => {
                                   // Check that we had success.
                                   if (!components.isSuccessCode(aResult))
                       {
                           this.has_data_loaded = false;
                           console.log("An error occured while fetching the trie: " + aResult);
                           return null;
                       } else {
                           var bstream = Cc["@mozilla.org/binaryinputstream;1"].
                           createInstance(Ci.nsIBinaryInputStream);
                       bstream.setInputStream(aInputStream);
                       var bytes = bstream.readBytes(bstream.available());
                       if (bytes.length < 10000) {
                           // There's no way this is a proper CSV
                           // file.  Just abort.
                           console.log("Aborting with short trie file error.");
                           this.has_data_loaded = false;
                           return;
                       }

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

                           console.log("Pushing trie data into emscripten heap");
                           this.rtrie_handle = this.push_trie(nDataBytes, dataPtr);
                           console.log("rtrie handle returned: " + this.rtrie_handle);

                           // You must free the
                           // memory after playing
                           // in emscripten land
                           this.offlinegeo_mod._free(dataHeap.byteOffset);
                           console.log("free'd bytes for dataHeap.byteOffset");
                           this.has_data_loaded = true;
                           console.log("marked data loaded to true");
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


                   // Fetch the city tiles
                   console.log("Fetching: " + this.ordered_city_url);
                   NetUtil.asyncFetch(
                           this.ordered_city_url,
                           (aInputStream, aResult) => {
                               // Check that we had success.
                              console.log("Fetching the City URL gave a ["+aResult+"] status code");
                               if (!components.isSuccessCode(aResult))
                   {
                       this.has_data_loaded = false;
                       console.log("An error occured while fetching the ordered tile data: " + aResult);
                       return null;
                   } else {
                       var bstream = Cc["@mozilla.org/binaryinputstream;1"].
                       createInstance(Ci.nsIBinaryInputStream);
                       bstream.setInputStream(aInputStream);
                       var bytes = bstream.readBytes(bstream.available());
                       if (bytes.length < 10000) {
                           // There's no way this is a proper CSV
                           // file.  Just abort.
                           console.log("Aborting with short CSV file error.");
                           this.has_data_loaded = false;
                           return;
                       }
                       console.log("bytes length for city tile CSV: " + bytes.length);

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
                       this.ordered_city_data = PP.parse(bytes).data;

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
                   }})
                   // Fetch the marisa trie
               },
    set_worker: function(w) {
        // TODO: change this to return a closure so that 
        // the worker is bound to just this instance.
        this.worker = w;
    },
    startWatch: function()
    {
        if (this.has_data_loaded) {
            wifi_service.startWatching(this);
            console.log("wifi monitor is hooked and started!");
        } else {
            // No data is loaded - immediately return a failure
            // message
        }
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
            this.macList[i] = sha256.hash(bssid).slice(0,12);
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
                var result_obj = {'lat': lat, 'lon': lon};
                this.worker.port.emit("offline_fix_found", result_obj);
                console.log("Emitted the offline_fix data: " + JSON.stringify(result_obj));
            }

        } else {
            // 0 or 1 hits isn't enough to
            // resolve a fix
            //
            // Send back an empty match
            this.worker.port.emit("offline_fix_found", {});
        }

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
