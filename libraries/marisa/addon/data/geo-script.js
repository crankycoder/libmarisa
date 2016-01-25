/*
 * This module defines a replacement navigator.geolocation.* which
 * invokes emscripten to do a lookup.
 */


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
       
        // remove script
        var s = document.getElementById('__lg_script');
        if (s) {
            s.remove();	// DEMO: in demo injectCode is run directly so there's no script
        }
    }

    if (document.documentElement.tagName.toLowerCase() == 'html') { // only for html
        var inject = "(function(){"
            + injectedCode +
            " injectedCode();" +
            "})()";

        var script = document.createElement('script');
        script.setAttribute('id', '__lg_script');
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
    }
}
