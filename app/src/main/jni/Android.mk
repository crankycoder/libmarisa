# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)
LOCAL_CPP_FEATURES := exceptions rtti

include $(CLEAR_VARS)
LOCAL_MODULE := libmarisa

LOCAL_SRC_FILES := test-libstl.cpp lib/marisa/agent.cc lib/marisa/keyset.cc lib/marisa/trie.cc lib/marisa/grimoire/io/mapper.cc lib/marisa/grimoire/io/reader.cc lib/marisa/grimoire/io/writer.cc lib/marisa/grimoire/trie/louds-trie.cc lib/marisa/grimoire/trie/tail.cc lib/marisa/grimoire/vector/bit-vector.cc

include $(BUILD_EXECUTABLE)
