all:
	./gradlew build

test:
	./gradlew test

LIB_MARISA=./src/main/jni

build/web/marisa.html:
	mkdir -p build/web
	emcc $(LIB_MARISA)/js-marisa.cc $(LIB_MARISA)/lib/marisa/agent.cc $(LIB_MARISA)/lib/marisa/keyset.cc $(LIB_MARISA)/lib/marisa/trie.cc $(LIB_MARISA)/lib/marisa/grimoire/io/mapper.cc $(LIB_MARISA)/lib/marisa/grimoire/io/reader.cc $(LIB_MARISA)/lib/marisa/grimoire/io/writer.cc $(LIB_MARISA)/lib/marisa/grimoire/trie/louds-trie.cc $(LIB_MARISA)/lib/marisa/grimoire/trie/tail.cc $(LIB_MARISA)/lib/marisa/grimoire/vector/bit-vector.cc --preload-file tests/simple.trie --preload-file tests/simple.bytestrie -o build/web/marisa.html


upload:
	./gradlew bintrayUpload
