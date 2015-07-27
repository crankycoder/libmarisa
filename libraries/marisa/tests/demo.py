from marisa_trie import RecordTrie, BytesTrie


def test_load():
    keys = [u'foo', u'bar']
    expected_values = [(1, 2, 3), (4, 5, 6)]
    fmt = ">iii"
    rtrie = RecordTrie(fmt)
    rtrie.load('tests/demo.record_trie')
    for i, k in enumerate(keys):
        assert [expected_values[i]] == rtrie.get(k)
        print "Got: %s" % (rtrie.get(k))


def test_bytestrie():
    btrie = BytesTrie()
    btrie.mmap('tests/simple.bytestrie')
    print 'start bytes trie-------'
    print btrie.get('foo')
    print 'end bytes trie-------'

if __name__ == '__main__':
    test_load()
    test_bytestrie()
