/*
 * This module defines a replacement navigator.geolocation.* which
 * invokes emscripten to do a lookup.
 */


if (self.options.showOptions) {
    function injectedCode() {
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

            // TODO: this callback needs to be invoked after a lookup
            // in the marisa trie
            callBack(new MockGeoPositionObject(44.033639,-79.490813, 50));
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

        // FF: there is another variables in the scope named parent, this causes a very hard to catch bug
        var _parent = document.head || document.body || document.documentElement;
        var firstChild = (_parent.childNodes && (_parent.childNodes.length > 0)) ? _parent.childNodes[0] : null;
        if (firstChild) {
            _parent.insertBefore(script, firstChild);
        } else {
            _parent.appendChild(script);
        }
    }
}
