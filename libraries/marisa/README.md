This is the core libmarisa C++ library.

This is used in Java over a JNI bridge.

The codebase is also usable from Javascript using Emscripten as well.

To build the demo, issue:

```
make build/web/marisa.html
```

and open build/web/marisa.html.

This code is known to work in Firefox 39, Chrome 44 and Safari 8 on
OSX.

Note that Chrome will require that you open the marisa.html page over
HTTP or else the code will not run.
