require.config({
        "packages": ["offlinegeo"]
});

require(["offlinegeo", "print"],
function (offlinegeo, print) {
    print("Offline geo library: " + offlinegeo);
    print("Print library: " + print);

    // TODO: setup a closure with the wifi scan
    // data here, convert the pointer into a long
    // and pass the pointer in
    offlinegeo._MZOF_test_http_recordtrie();
    print("hello world");
});

