import unittest

from pycspade import spade


class Test(unittest.TestCase):
    def setUp(self):
        result = spade(filename='tests/zaki.txt', support=2, maxsize=5, maxlen=5)
        self.nseqs = result['nsequences']
        self.occurs = {}
        self.supports = {}
        self.confids = {}
        self.lifts = {}
        self.accum_occurs = {}
        self.sequences = []

        for mined_object in result['mined_objects']:
            sequence = '->'.join(list(map(str, mined_object.items)))
            self.sequences.append(sequence)
            self.occurs[sequence] = mined_object.noccurs
            self.supports[sequence] = mined_object.noccurs / self.nseqs
            self.confids[sequence] = mined_object.confidence
            self.lifts[sequence] = mined_object.lift
            self.accum_occurs[sequence] = mined_object.accum_occurs

    def test_sequences(self):
        correct_sequences = ['(1)', '(1 2)', '(1 2 6)', '(1 6)', '(2)', '(2)->(1)', '(2 6)', '(2 6)->(1)', '(4)',
                             '(4)->(1)',
                             '(4)->(2)', '(4)->(2)->(1)', '(4)->(2 6)', '(4)->(2 6)->(1)', '(4)->(6)', '(4)->(6)->(1)',
                             '(6)', '(6)->(1)']
        correct_occurs = {'(1)': 4, '(1 2)': 3, '(1 2 6)': 3, '(1 6)': 3, '(2)': 4, '(2)->(1)': 2, '(2 6)': 4,
                          '(2 6)->(1)': 2, '(4)': 2, '(4)->(1)': 2, '(4)->(2)': 2, '(4)->(2)->(1)': 2, '(4)->(2 6)': 2,
                          '(4)->(2 6)->(1)': 2, '(4)->(6)': 2, '(4)->(6)->(1)': 2, '(6)': 4, '(6)->(1)': 2}
        correct_supports = {'(1)': 1.0, '(1 2)': 0.75, '(1 2 6)': 0.75, '(1 6)': 0.75, '(2)': 1.0, '(2)->(1)': 0.5,
                            '(2 6)': 1.0, '(2 6)->(1)': 0.5, '(4)': 0.5, '(4)->(1)': 0.5, '(4)->(2)': 0.5,
                            '(4)->(2)->(1)': 0.5, '(4)->(2 6)': 0.5, '(4)->(2 6)->(1)': 0.5, '(4)->(6)': 0.5,
                            '(4)->(6)->(1)': 0.5, '(6)': 1.0, '(6)->(1)': 0.5}
        correct_lifts = {'(1)': None, '(1 2)': None, '(1 2 6)': None, '(1 6)': None, '(2)': None, '(2)->(1)': 0.5,
                         '(2 6)': None, '(2 6)->(1)': 0.5, '(4)': None, '(4)->(1)': 1.0, '(4)->(2)': 1.0,
                         '(4)->(2)->(1)': 1.0, '(4)->(2 6)': 1.0, '(4)->(2 6)->(1)': 1.0, '(4)->(6)': 1.0,
                         '(4)->(6)->(1)': 1.0, '(6)': None, '(6)->(1)': 0.5}
        correct_confids = {'(1)': None, '(1 2)': None, '(1 2 6)': None, '(1 6)': None, '(2)': None, '(2)->(1)': 0.5,
                           '(2 6)': None, '(2 6)->(1)': 0.5, '(4)': None, '(4)->(1)': 1.0, '(4)->(2)': 1.0,
                           '(4)->(2)->(1)': 1.0, '(4)->(2 6)': 1.0, '(4)->(2 6)->(1)': 1.0, '(4)->(6)': 1.0,
                           '(4)->(6)->(1)': 1.0, '(6)': None, '(6)->(1)': 0.5}

        self.assertListEqual(self.sequences, correct_sequences)
        self.assertDictEqual(self.occurs, correct_occurs)
        self.assertDictEqual(self.supports, correct_supports)
        self.assertDictEqual(self.lifts, correct_lifts)
        self.assertDictEqual(self.confids, correct_confids)

        print(self.accum_occurs)
