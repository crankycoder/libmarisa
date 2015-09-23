function startOfflineScan() {
    // This method is just a dumb proxy to initiate a wifi scan
    // within the chrome process.  You can find the actual
    // offline scan code in index.js and looking for a
    // worker.port.on("check_chrome_bits") block.
    self.port.emit("check_chrome_bits", "startOfflineScan");
}


 
exportFunction(startOfflineScan, unsafeWindow, {defineAs: "startOfflineScan"});
