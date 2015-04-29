[![Build
Status](https://travis-ci.org/crankycoder/libmarisa.svg?branch=dev)](https://travis-ci.org/crankycoder/libmarisa)


This is a port of the Python package 'marisa-trie'.

Original Python code: https://github.com/kmike/marisa-trie

And it's ancestor: https://code.google.com/p/marisa-trie/


libmarisa
=========

Static memory-efficient Trie-like structures for Android.

This library is binary compatible with the Python marisa-trie library and utilizes the exact
same C++ code.

Installation
============

Use gradle!

Add the repository:

```
    maven { url "http://dl.bintray.com/crankycoder/libmarisa" }
```

Add the dependency:

```
    compile 'org.mozilla.mozstumbler.marisa:marisa:0.3'
```

And you're done!

See the example in `app/build.gradle` for a fully working example.



Usage
=====

Not all methods or classes from the Python library have been exposed.

In particular, the Agent class is not exposed at all.  This seemed to cause some
idiosyncratic bugs with lookups.


* `marisa.RecordTrie` - read-only trie-based data structure that maps unicode
   keys to lists of `Record`.  All tuples must be of the same format (the data is
   packed using format strings that are compatible with the  python ``struct``
   module).

* ``marisa_trie.BytesTrie`` - read-only Trie that maps unicode
  keys to lists of byte[] arrays.


Writing to a trie is not supported yet in this library.
