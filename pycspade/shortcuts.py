import os
import uuid

from .cspade import cpp_cspade


def data_to_rows(data):
    rows = ['{} {} {} {}'.format(sid, eid, len(els), ' '.join(list(map(str, els)))) for sid, eid, els in data]
    return rows


class Item:
    def __init__(self, elements):
        self.elements = elements

    def __repr__(self):
        return '({})'.format(' '.join(list(map(str, self.elements))))


class Sequence:
    def __init__(self, name, noccurs):
        self.items = []
        self.name = name
        self.noccurs = noccurs
        self.accum_occurs = noccurs
        self.confidence = None
        self.lift = None
        self.up_to_prev = None
        self.last_child = None
        self.frm_second = None
        self.up_to_prev_str = None
        self.last_child_str = None
        self.frm_second_str = None

    def add_item(self, item):
        self.items.append(item)

    def accumulate_occurs(self, child_occurs):
            self.accum_occurs += child_occurs
            if self.frm_second:
                self.frm_second.accumulate_occurs(child_occurs)

    def __repr__(self):
        return '{} - [{}]'.format('->'.join(list(map(str, self.items))), self.noccurs)


def decode_results(result):
    lifts = {}
    confidences = {}
    nseqs = result['nsequences']

    mined = result['mined']
    lines = mined.strip().decode('latin-1').split('\n')
    lines.sort()
    sequences = {}
    for line in lines:
        if '0' <= line[0] <= '9':
            sequence_str, stats = line.split(' -- ')
            item_strs = sequence_str.split(' -> ')
            noccurs = int(stats[:stats.index(' ')])

            sequence = Sequence(sequence_str, noccurs)
            if len(item_strs) > 1:
                sequence.up_to_prev_str = ' -> '.join(item_strs[:-1])
                sequence.last_child_str = item_strs[-1]
                sequence.frm_second_str = ' -> '.join(item_strs[1:])

            for _item in item_strs:
                _elements = list(map(int, _item.split(' ')))
                item = Item(_elements)
                sequence.add_item(item)
            sequences[sequence_str] = sequence

    # Second pass
    for sequence in list(sequences.values()):
        sequence.up_to_prev = up_to_prev = sequences.get(sequence.up_to_prev_str, None)
        sequence.last_child = last_child = sequences.get(sequence.last_child_str, None)
        sequence.frm_second = sequences.get(sequence.frm_second_str, None)

        if up_to_prev is not None:
            sequence.confidence = sequence.noccurs / up_to_prev.noccurs
            confidences[sequence.name] = sequence.confidence

            if last_child is not None:
                sequence.lift = sequence.noccurs * nseqs / (up_to_prev.noccurs * last_child.noccurs)
                lifts[sequence.name] = sequence.lift

    # Third pass - to calculate accummulated occurrence counts
    for sequence in list(sequences.values()):
        if sequence.frm_second is not None:
            sequence.frm_second.accumulate_occurs(sequence.noccurs)

    result['mined_objects'] = list(sequences.values())


def cspade(filename=None, data=None, support=3, maxsize=None, maxlen=None, mingap=None, maxgap=None):
    """
    Shortcut to call cspade
    :param filename: path to the ascii file, must be given if data is None
    :param data: raw data as list of transactions, must be given if filename is None
    :param support: is interpreted as the threshold of mimimum normalised support if within [0, 1]:
                         if > 1: interpreted as the threshold of absolute support (e.g. 50 over 100 transactions)
    :param maxsize: an integer value specifying the maximum number of items of a sequence (default=100)
    :param maxlen: an integer value specifying the maximum number of elements of a sequence (default=100)
    :param mingap: an integer value specifying the minimum time difference between consecutive elements of a sequence
    :param maxgap: an integer value specifying the maximum time difference between consecutive elements of a sequence
    :param decode: if True, the return strings will be decoded and line-separated, otherwise raw C++ strings
                   (python bytes) are returned
    :return: (result, logger, summary). where:
             -result: the mined sequences
             -logger: general logging
             -summary: equivalent to the content of summary.out
    """
    if filename is None and data is None:
        raise Exception('You must provide either filename or data')
    if filename is not None and data is not None:
        raise Exception('You must provide either filename or data')

    if data:
        rows = data_to_rows(data)
        hex = uuid.uuid4().hex
        filename = '/tmp/{}.ascii.data'.format(hex)
        with open(filename, 'w', encoding='latin-1') as f:
            for row in rows:
                f.write(row)
                f.write('\n')

    try:
        retval = cpp_cspade(filename, support, maxsize, maxlen, mingap, maxgap, decode=False)
        decode_results(retval)
        return retval
    finally:
        if data:
            os.remove(filename)
