all:
	./gradlew build

test:
	./gradlew test

LIB_MARISA=./src/main/jni

CPP_FILES=$(LIB_MARISA)/js-marisa.cc $(LIB_MARISA)/lib/marisa/agent.cc $(LIB_MARISA)/lib/marisa/keyset.cc $(LIB_MARISA)/lib/marisa/trie.cc $(LIB_MARISA)/lib/marisa/grimoire/io/mapper.cc $(LIB_MARISA)/lib/marisa/grimoire/io/reader.cc $(LIB_MARISA)/lib/marisa/grimoire/io/writer.cc $(LIB_MARISA)/lib/marisa/grimoire/trie/louds-trie.cc $(LIB_MARISA)/lib/marisa/grimoire/trie/tail.cc $(LIB_MARISA)/lib/marisa/grimoire/vector/bit-vector.cc

PRELOAD_FILES=--preload-file tests/simple.trie --preload-file tests/simple.bytestrie --preload-file tests/demo.record_trie

JS_OUTPUT_FILES=-o build/js/offlinegeo.js
HTML_OUTPUT_FILES=-o build/web/marisa.html

EXTRA_EMSCRIPTEN_ARGS=-O0 -s ASYNCIFY=1 --pre-js buildtools/prefix.js --post-js buildtools/postfix.js

EXPORT_FUNCS=-s EXPORTED_FUNCTIONS='["_MZOF_test_http_recordtrie","_MZOF_lookup_rtrie","_MZOF_mount_idbfs","_MZOF_sync_idbfs","_MZOF_load_record_trie", "_fsync_success"]'

# This target is the embeddable js
build/js/offlinegeo.js:
	mkdir -p build/js
	emcc $(CPP_FILES) $(EXTRA_EMSCRIPTEN_ARGS) $(EXPORT_FUNCS) $(JS_OUTPUT_FILES)
	cp build/js/offlinegeo.js web/lib/offlinegeo/main.js

build/web/marisa.html:
	mkdir -p build/web
	emcc $(CPP_FILES) $(PRELOAD_FILES) $(EXTRA_EMSCRIPTEN_ARGS) $(EXPORT_FUNCS) $(HTML_OUTPUT_FILES)

setup_fsync:
	mkdir -p build/web
	emcc $(LIB_MARISA)/test_idbfs_setup.c -o build/web/test_idbfs_sync.html

test_fsync:
	mkdir -p build/web
	emcc $(LIB_MARISA)/test_idbfs_sync.c -o build/web/test_idbfs_sync.html


upload:
	./gradlew bintrayUpload
