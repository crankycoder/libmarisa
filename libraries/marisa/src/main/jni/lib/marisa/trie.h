#ifndef MARISA_TRIE_H_
#define MARISA_TRIE_H_

#include <assert.h>
#include <string>
#include <vector>
#include "keyset.h"
#include "agent.h"

namespace marisa {
    namespace grimoire {
        namespace trie {

            class LoudsTrie;

        }  // namespace trie
    }  // namespace grimoire

    class Trie {
        friend class TrieIO;

        public:
        Trie();
        ~Trie();

        void build(Keyset &keyset, int config_flags = 0);

        void mmap(const char *filename);
        void map(const void *ptr, std::size_t size);

        void load(const char *filename);
        void read(int fd);

        void save(const char *filename) const;
        void write(int fd) const;

        bool lookup(Agent &agent) const;
        void reverse_lookup(Agent &agent) const;
        bool common_prefix_search(Agent &agent) const;
        bool predictive_search(Agent &agent) const;

        std::size_t num_tries() const;
        std::size_t num_keys() const;
        std::size_t num_nodes() const;

        TailMode tail_mode() const;
        NodeOrder node_order() const;

        bool empty() const;
        std::size_t size() const;
        std::size_t total_size() const;
        std::size_t io_size() const;

        void clear();
        void swap(Trie &rhs);

        private:
        scoped_ptr<grimoire::trie::LoudsTrie> trie_;

        // Disallows copy and assignment.
        Trie(const Trie &);
        Trie &operator=(const Trie &);
    };

    class BytesTrie: public Trie {
        public:
            BytesTrie();
            void get(std::vector< std::vector<char> > *result, const char* b_prefix, int bytes_to_read);
            void printbuf(const char* buffer, int len);
    };

    class Record {
        public:
            Record();
            char* printTuple();
            int* getIntTuple();

            std::vector<int> int_vector;
    };

    class RecordTrie: public BytesTrie {
        public:
            RecordTrie();
            RecordTrie(const char* fmt);
            void getRecord(std::vector<Record> *result, const char* b_prefix);
            void setFormat(const char *fmt);
        private:
            std::string _fmt;
            int _fmtByteLength;

            void _initFmtLength();
    };


}  // namespace marisa

#endif  // MARISA_TRIE_H_
