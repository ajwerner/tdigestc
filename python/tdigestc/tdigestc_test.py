import unittest
from tdigestc import TDigest

class TestBasic(unittest.TestCase):
    def test(self):
        f = TDigest(100)
        f.add(1)
        f.add(2)
        self.assertEqual(f.value_at(0), 1)
        self.assertEqual(f.value_at(.5), 1.5)
        self.assertEqual(f.value_at(1), 2)

if __name__ == '__main__':
    unittest.main()
