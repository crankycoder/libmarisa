/*
 * This module defines a replacement navigator.geolocation.* which
 * invokes emscripten to do a lookup.
 */

self.port.on("offline_fix_found", function(message) {
    console.log("DEBUG: geo-script.js received a message: " + JSON.stringify(message));

    // This listener receives messages from index.js to get the
    // lat/lon JSON blob.  We then forward this message on to the page
    // script
    
    // Tag this JSON object so that we can just test the string in 
    // the page javascript before we try the callback
    message['__fx_elm_location'] = 1;
    var json_msg = JSON.stringify(message);
    window.postMessage(json_msg, "*");
});

self.port.on("offline_fix_unavailable", function(message) {
    console.log("DEBUG: geo-script.js received an unavailable location message: " + JSON.stringify(message));

    // This listener receives messages from index.js to get the
    // lat/lon JSON blob.  We then forward this message on to the page
    // script
    
    // Tag this JSON object so that we can just test the string in 
    // the page javascript before we try the callback
    message['__fx_elm_unavailable_location'] = 1;
    var json_msg = JSON.stringify(message);
    console.log("Sending page unavailable JSON: " + json_msg);
    window.postMessage(json_msg, "*");
});

if (self.options.showOptions) {
    function injectedCode() {

        /* None of this code will work within the regular javascript
         * debugger if you put a breakpoint inside of it.  ALl the
         * code here is clobbered into each HTML page and injected at
         * the top so the only way to set a break point is to go into
         * the individual page HTML and set a breakpoint there.
         *
         * You also can't even view the code within the HTML page as
         * injected code in a content script doesn't exist within
         * the regular Javascript runtime.
         *
         * So debugging this relies on console.log messages as you can
         * never actually see the code running within the page.
         *
         * Generally - debugging content scripts is awful.
         *
         * https://bugzilla.mozilla.org/show_bug.cgi?id=1016046
         *
         * I've managed to make things even *worse* by injecting code
         * dynamically into the actual content of the page with the
         * content script.
         *
         */

        navigator.geolocation.getCurrentPosition = function(callBack,
                                                            errBack,
                                                            options) {

            function MockGeoPositionObject(lat, lng, acc) {
              this.coords = new MockGeoCoordsObject(lat, lng, acc, 0, 0);
              this.address = null;
              this.timestamp = Date.now();
            }

            function MockGeoCoordsObject(lat, lon, acc, alt, altacc) {
              this.latitude = lat;
              this.longitude = lon;
              this.accuracy = acc;
              this.altitude = alt;
              this.altitudeAccuracy = altacc;
            }

            window.startOfflineScan();

            navigator.geolocation.captured_callBack = callBack;
            navigator.geolocation.captured_errBack = errBack;

            console.log("Callback function: " + navigator.geolocation.captured_callBack);
            console.log("^^ Bound callback to navigator.geolocation.captured_callBack");

            // TODO: this callback needs to be invoked after a lookup
            // in the marisa trie
            //callBack(new MockGeoPositionObject(44.033639,-79.490813, 50));
        };

        navigator.geolocation.watchPosition = function(cb1, cb2, options) {
            alert("watchPosition was called");
        };

        navigator.geolocation.clearWatch = function () {
            alert("clearWatch was called");
        };
        
        // Remove script
        // Ok - this is a bit wacky and probably unnecessary.
        // Once we're finished clobbering the navigator.geolocation
        // namespace - we actually *delete* the clobbering code from
        // the page content.  Just to be sneaky.
        
        /*
        var s = document.getElementById('__offline_clobber');
        if (s) {
            s.remove();	// DEMO: in demo injectCode is run directly so there's no script
        }
        */
    }

    if (document.documentElement.tagName.toLowerCase() == 'html') { // only for html
        // Step 1- Clobber the navigator.geolocation namespace
        var inject = "(function(){"
            + injectedCode +
            " injectedCode();" +
            "})()";

        var script = document.createElement('script');
        script.setAttribute('id', '__offline_clobber');
        script.appendChild(document.createTextNode(inject));

        // FF: there is already another variable in the scope named
        // 'parent', this causes a very hard to catch bug so we'll
        // just call this '_parent'
        var _parent = document.head || document.body || document.documentElement;
        var firstChild = (_parent.childNodes && (_parent.childNodes.length > 0)) ? _parent.childNodes[0] : null;
        if (firstChild) {
            _parent.insertBefore(script, firstChild);
        } else {
            _parent.appendChild(script);
        }

        // Step 2 - add a listener for offline fixes
        // Now inject a listener for when the content script passes 
        // a message with the offline fix.
        var script = document.createElement('script');
        var inject = "window.addEventListener('message', function(event) {  \
            console.log('Page got a message: ' + event.data);  \
            if (event.data.indexOf('__fx_elm_location')  > -1) {  \
                console.log('Available position received in page'); \
                var json_obj = JSON.parse(event.data);  \
                function MockGeoPositionObject(lat, lng, acc) {  \
                  this.coords = new MockGeoCoordsObject(lat, lng, acc, 0, 0);  \
                  this.address = null;  \
                  this.timestamp = Date.now();  \
                }  \
                function MockGeoCoordsObject(lat, lon, acc, alt, altacc) {  \
                  this.latitude = lat;  \
                  this.longitude = lon;  \
                  this.accuracy = acc;  \
                  this.altitude = alt;  \
                  this.altitudeAccuracy = altacc;  \
                }  \
                var mockLocation = new MockGeoPositionObject(json_obj['lat'], json_obj['lon'], 100); \
                navigator.geolocation.captured_callBack(mockLocation);  \
            }  \
            if (event.data.indexOf('__fx_elm_unavailable_location') > -1) {  \
                console.log('Unavailable position received in page'); \
                function MockPositionError() {  \
                  this.code = 2;  \
                  this.message = 'Offline position is unavailable'; \
                }  \
                var posError = new MockPositionError(); \
                console.log('ErrBack: ' + navigator.geolocation.captured_errBack);\
                console.log('PositionError: ' + posError); \
                var mockLocation = new MockGeoPositionObject(0, 0, 100); \
                navigator.geolocation.captured_errBack(posError); \
                navigator.geolocation.captured_callBack(mockLocation);  \
            }  \
         }, false);";


        script.setAttribute('id', '__offline_receiver');
        script.appendChild(document.createTextNode(inject));
        var _parent = document.head || document.body || document.documentElement;
        var firstChild = (_parent.childNodes && (_parent.childNodes.length > 0)) ? _parent.childNodes[0] : null;
        if (firstChild) {
            _parent.insertBefore(script, firstChild);
        } else {
            _parent.appendChild(script);
        }
    }
}
