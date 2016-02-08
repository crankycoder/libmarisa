function startOfflineScan() {
    // This method is just a dumb proxy to initiate a wifi scan
    // within the chrome process.  

    self.port.emit("initiate_geolocation", "startOfflineScan");
}


 
exportFunction(startOfflineScan, unsafeWindow, {defineAs: "startOfflineScan"});
