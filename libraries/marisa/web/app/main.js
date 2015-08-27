require.config({
        "packages": ["offlinegeo"]
});

require(["offlinegeo", "print"],
function (offlinegeo, print) {
    print("Offline geo library: " + offlinegeo);
    print("Print library: " + print);

    offlinegeo._MZOF_test_http_recordtrie();
    print("hello world");
});

