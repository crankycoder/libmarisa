var salutation = "hello, ";

function startOfflineScan() {
    self.port.emit("check_chrome_bits", "startScan");
}


 
exportFunction(startOfflineScan, unsafeWindow, {defineAs: "startOfflineScan"});
