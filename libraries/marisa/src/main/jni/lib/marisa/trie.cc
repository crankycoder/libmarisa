#include "stdio.h"
#include "iostream.h"
#include "trie.h"
#include "grimoire/trie.h"
#include <arpa/inet.h>

namespace marisa {

    Trie::Trie() : trie_() {}

    Trie::~Trie() {}

    void Trie::build(Keyset &keyset, int config_flags) {
        scoped_ptr<grimoire::LoudsTrie> temp(new (std::nothrow) grimoire::LoudsTrie);
        MARISA_THROW_IF(temp.get() == NULL, MARISA_MEMORY_ERROR);

        temp->build(keyset, config_flags);
        trie_.swap(temp);
    }

    void Trie::mmap(const char *filename) {
        MARISA_THROW_IF(filename == NULL, MARISA_NULL_ERROR);

        scoped_ptr<grimoire::LoudsTrie> temp(new (std::nothrow) grimoire::LoudsTrie);
        MARISA_THROW_IF(temp.get() == NULL, MARISA_MEMORY_ERROR);

        grimoire::Mapper mapper;
        mapper.open(filename);
        temp->map(mapper);
        trie_.swap(temp);
    }

    void Trie::map(const void *ptr, std::size_t size) {
        MARISA_THROW_IF((ptr == NULL) && (size != 0), MARISA_NULL_ERROR);

        scoped_ptr<grimoire::LoudsTrie> temp(new (std::nothrow) grimoire::LoudsTrie);
        MARISA_THROW_IF(temp.get() == NULL, MARISA_MEMORY_ERROR);

        grimoire::Mapper mapper;
        mapper.open(ptr, size);
        temp->map(mapper);
        trie_.swap(temp);
    }

    void Trie::load(const char *filename) {
        MARISA_THROW_IF(filename == NULL, MARISA_NULL_ERROR);

        scoped_ptr<grimoire::LoudsTrie> temp(new (std::nothrow) grimoire::LoudsTrie);
        MARISA_THROW_IF(temp.get() == NULL, MARISA_MEMORY_ERROR);

        grimoire::Reader reader;
        reader.open(filename);
        temp->read(reader);
        trie_.swap(temp);
    }

    void Trie::read(int fd) {
        MARISA_THROW_IF(fd == -1, MARISA_CODE_ERROR);

        scoped_ptr<grimoire::LoudsTrie> temp(new (std::nothrow) grimoire::LoudsTrie);
        MARISA_THROW_IF(temp.get() == NULL, MARISA_MEMORY_ERROR);

        grimoire::Reader reader;
        reader.open(fd);
        temp->read(reader);
        trie_.swap(temp);
    }

    void Trie::save(const char *filename) const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        MARISA_THROW_IF(filename == NULL, MARISA_NULL_ERROR);

        grimoire::Writer writer;
        writer.open(filename);
        trie_->write(writer);
    }

    void Trie::write(int fd) const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        MARISA_THROW_IF(fd == -1, MARISA_CODE_ERROR);

        grimoire::Writer writer;
        writer.open(fd);
        trie_->write(writer);
    }

    bool Trie::lookup(Agent &agent) const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        if (!agent.has_state()) {
            agent.init_state();
        }
        return trie_->lookup(agent);
    }

    void Trie::reverse_lookup(Agent &agent) const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        if (!agent.has_state()) {
            agent.init_state();
        }
        trie_->reverse_lookup(agent);
    }

    bool Trie::common_prefix_search(Agent &agent) const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        if (!agent.has_state()) {
            agent.init_state();
        }
        return trie_->common_prefix_search(agent);
    }

    bool Trie::predictive_search(Agent &agent) const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        if (!agent.has_state()) {
            agent.init_state();
        }
        return trie_->predictive_search(agent);
    }

    std::size_t Trie::num_tries() const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        return trie_->num_tries();
    }

    std::size_t Trie::num_keys() const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        return trie_->num_keys();
    }

    std::size_t Trie::num_nodes() const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        return trie_->num_nodes();
    }

    TailMode Trie::tail_mode() const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        return trie_->tail_mode();
    }

    NodeOrder Trie::node_order() const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        return trie_->node_order();
    }

    bool Trie::empty() const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        return trie_->empty();
    }

    std::size_t Trie::size() const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        return trie_->size();
    }

    std::size_t Trie::total_size() const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        return trie_->total_size();
    }

    std::size_t Trie::io_size() const {
        MARISA_THROW_IF(trie_.get() == NULL, MARISA_STATE_ERROR);
        return trie_->io_size();
    }

    void Trie::clear() {
        Trie().swap(*this);
    }

    void Trie::swap(Trie &rhs) {
        trie_.swap(rhs.trie_);
    }

    BytesTrie::BytesTrie() {
        // fill in this constructor
    }


    /*
     * Return a list of payloads (as byte objects) for a given key.
     */
    void BytesTrie::get(std::vector< std::vector<char> > *results, 
                        const char *prefix,
                        int bytes_to_read) {
        // allocate a new buffer
        int b_prefix_len = strlen(prefix)+2;
        char b_prefix[b_prefix_len];
        strcpy(b_prefix, prefix);
        b_prefix[b_prefix_len-2] = 0xff; // Default value separator
        b_prefix[b_prefix_len-1] = 0x00; // Default value separator

        size_t prefix_len = strlen(b_prefix);

        marisa::Agent ag;
        ag.set_query(b_prefix);

        while (this->predictive_search(ag)) {
            // we need to copy the char* into a vector<char> to store
            // a byte array
            const char *tmp = ag.key().ptr()+b_prefix_len-1;
            if (bytes_to_read == 0) {
                // This is almost certainly wrong for 'real' binary
                // data.
                bytes_to_read = strlen(tmp);
            }
            std::vector<char> byte_array(tmp, tmp+bytes_to_read);
            printf("Pushing %d bytes into byte array\n", bytes_to_read);
            for (int i = 0; i < bytes_to_read; i++) {
                printf("[0x%02d], ", int(byte_array.at(i)));
            }
            printf("\n");
            results->push_back(byte_array);
        }
    }

    void printbuf(const char* buffer, int len) {
        for (int i = 0; i < len; ++i)
            printf("%c", buffer[i]);
    }

    RecordTrie::RecordTrie(const char *fmt) {
        _fmt.assign(fmt, strlen(fmt));
    }

    void RecordTrie::getRecord(std::vector<marisa::Record> *result, const char* b_prefix) {

        bool hasOrdering = false;
        int offset = 0;
        char encodingChar;
        std::vector< std::vector<char> > tmpResult;

        // Grab the byte dump for this kye
        
        // TODO: use the _fmt byte length here
        this->get(&tmpResult, b_prefix, 12);

        if (_fmt.substr(0, 1) == "<") {
            encodingChar = '<';
            hasOrdering = true;
        } else if ((_fmt.substr(0, 1) == ">") || (_fmt.substr(0, 1) == "!")) {
            encodingChar = '>';
            hasOrdering = true;
        } else if ((_fmt.substr(0, 1) == "@") || (_fmt.substr(0, 1) == "=")) {
            encodingChar = '@';
            hasOrdering = true;
        }

        if (hasOrdering) {
            offset += 1;
        }
        std::cout << "Offset: " << offset << std::endl;
        if (offset == 1) {
            std::cout << "Encoding Char: " << encodingChar << std::endl;
        }


        std::cout << "tmpResult length: " << tmpResult.size() << std::endl;

        for(int idx = 0; idx < tmpResult.size(); idx++) {
            std::cout << "Processing result " << idx << std::endl;
            const std::vector<char> byteArray = tmpResult.at(idx);

            const char* bytes = byteArray.data();
            printf("Reading %d bytes [%s]\n", byteArray.size(), bytes);

            int byte_offset = 0;
            // Ok, each of the vector<char> buffers must be decoded.
            for (; offset < _fmt.length(); offset++) {
                printf("Character: %c\n", _fmt.at(offset));
                if (_fmt.at(offset) == 'i') {
                    printf("Decoding int\n");
                } else if (_fmt.at(offset) == 'b') {
                    printf("Decoding byte\n");
                } else {
                    throw std::runtime_error(std::string("Invalid or unsupported format"));
                }
                byte_offset++;
            }
        }

    }

    Record::Record() {}



}  // namespace marisa

#include <iostream>

namespace marisa {

class TrieIO {
 public:
  static void fread(std::FILE *file, Trie *trie) {
    MARISA_THROW_IF(trie == NULL, MARISA_NULL_ERROR);

    scoped_ptr<grimoire::LoudsTrie> temp(
        new (std::nothrow) grimoire::LoudsTrie);
    MARISA_THROW_IF(temp.get() == NULL, MARISA_MEMORY_ERROR);

    grimoire::Reader reader;
    reader.open(file);
    temp->read(reader);
    trie->trie_.swap(temp);
  }
  static void fwrite(std::FILE *file, const Trie &trie) {
    MARISA_THROW_IF(file == NULL, MARISA_NULL_ERROR);
    MARISA_THROW_IF(trie.trie_.get() == NULL, MARISA_STATE_ERROR);
    grimoire::Writer writer;
    writer.open(file);
    trie.trie_->write(writer);
  }

  static std::istream &read(std::istream &stream, Trie *trie) {
    MARISA_THROW_IF(trie == NULL, MARISA_NULL_ERROR);

    scoped_ptr<grimoire::LoudsTrie> temp(
        new (std::nothrow) grimoire::LoudsTrie);
    MARISA_THROW_IF(temp.get() == NULL, MARISA_MEMORY_ERROR);

    grimoire::Reader reader;
    reader.open(stream);
    temp->read(reader);
    trie->trie_.swap(temp);
    return stream;
  }
  static std::ostream &write(std::ostream &stream, const Trie &trie) {
    MARISA_THROW_IF(trie.trie_.get() == NULL, MARISA_STATE_ERROR);
    grimoire::Writer writer;
    writer.open(stream);
    trie.trie_->write(writer);
    return stream;
  }
};

void fread(std::FILE *file, Trie *trie) {
  MARISA_THROW_IF(file == NULL, MARISA_NULL_ERROR);
  MARISA_THROW_IF(trie == NULL, MARISA_NULL_ERROR);
  TrieIO::fread(file, trie);
}

void fwrite(std::FILE *file, const Trie &trie) {
  MARISA_THROW_IF(file == NULL, MARISA_NULL_ERROR);
  TrieIO::fwrite(file, trie);
}

std::istream &read(std::istream &stream, Trie *trie) {
  MARISA_THROW_IF(trie == NULL, MARISA_NULL_ERROR);
  return TrieIO::read(stream, trie);
}

std::ostream &write(std::ostream &stream, const Trie &trie) {
  return TrieIO::write(stream, trie);
}

std::istream &operator>>(std::istream &stream, Trie &trie) {
  return read(stream, &trie);
}

std::ostream &operator<<(std::ostream &stream, const Trie &trie) {
  return write(stream, trie);
}

}  // namespace marisa
