from marisa_trie import RecordTrie

def test_load():
    keys = [u'foo', u'bar', u'foobar', u'foo']
    expected_values = [(1, 2), (2, 1), (3, 3), (2, 1)]
    fmt = "<HH"
    rtrie = RecordTrie(fmt)
    rtrie.load('tests/foobar.rtrie')
    for k in keys:
        print rtrie.get(k)

if __name__ == '__main__':
    test_load()
