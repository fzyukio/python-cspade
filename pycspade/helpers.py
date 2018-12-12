import os
import uuid

from pycspade.cspade import c_runspade


def data_to_rows(data):
    rows = ['{} {} {} {}'.format(sid, eid, len(els), ' '.join(list(map(str, els)))) for sid, eid, els in data]
    return rows


def file_len(fname):
    with open(fname) as f:
        i = 0
        for l in f:
            if len(l):
                i += 1
    return i


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


def parse_results(result):
    lifts = {}
    confidences = {}
    nseqs = result['nsequences']
    lines = result['seqstrm'].split('\n')
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
    for sequence in sequences.values():
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
    for sequence in sequences.values():
        if sequence.frm_second is not None:
            sequence.frm_second.accumulate_occurs(sequence.noccurs)

    result['mined_objects'] = sequences.values()


def spade(filename=None, data=None, support=0.1, maxsize=None, maxlen=None, mingap=None, maxgap=None, memsize=None,
          numpart=None, maxwin=None, bfstype=None, tid_lists=None, parse=True):
    '''
    Call C++'s cspade()
    :param filename: full path to the input file (ascii)
    :param support: is interpreted as the threshold of mimimum normalised support if within [0, 1]:
                         if > 1: interpreted as the threshold of absolute support (e.g. 50 over 100 transactions)
    :param maxsize: an integer value specifying the maximum number of items of an element of a sequence (default=100)
    :param maxlen: an integer value specifying the maximum number of elements of a sequence (default=100)
    :param mingap: an integer value specifying the minimum time difference between consecutive elements of a sequence
    :param maxgap: an integer value specifying the minimum time difference between consecutive elements of a sequence

    :return: (result, logger, summary). where:
             -result: the mined sequences
             -logger: general logging
             -summary: same content as summary.out created by the original C code
    '''
    if filename is None and data is None:
        raise Exception('You must provide either filename or data')
    if filename is not None and data is not None:
        raise Exception('You must provide either filename or data')

    if filename and not os.path.isfile(filename):
        raise Exception('File {} does not exist'.format(filename))

    if memsize:
        if not isinstance(memsize, int):
            raise Exception('memsize must be integer')
    if numpart:
        if not isinstance(numpart, int):
            raise Exception('numpart must be integer')

    assert (0 < support <= 1), 'support must be a floating point in range (0-1]'
    
    if mingap is not None:
        assert mingap > 0, 'mingap cannot be 0 - that would mean two transactions happen at the same time'
    if maxgap is not None:
        assert maxgap > 0, 'maxgap cannot be 0'
        if mingap and maxgap < mingap:
            mingap = maxgap

    if data:
        rows = data_to_rows(data)
        hex = uuid.uuid4().hex
        filename = '/tmp/cspade-{}.txt'.format(hex)
        with open(filename, 'w', encoding='latin-1') as f:
            for row in rows:
                f.write(row)
                f.write('\n')

    try:
        result = c_runspade(filename, support, maxsize, maxlen, mingap, maxgap, memsize, numpart, maxwin, bfstype,
                            tid_lists)
        decode_result(result)
        if parse:
            parse_results(result)
        return result

    finally:
        if data:
            os.remove(filename)


def print_result(result):
    nseqs = result['nsequences']
    print('{0:>9s} {1:>9s} {2:>9s} {3:>9s} {4:>9s} {5:>80s}'.format('Occurs', 'Accum', 'Support', 'Confid', 'Lift',
                                                                    'Sequence'))
    for mined_object in result['mined_objects']:
        conf = 'N/A'
        lift = 'N/A'
        if mined_object.confidence:
            conf = '{:0.7f}'.format(mined_object.confidence)
        if mined_object.lift:
            lift = '{:0.7f}'.format(mined_object.lift)

        print('{0:>9d} {1:>9d} {2:>0.7f} {3:>9s} {4:>9s} {5:>80s} '.format(
            mined_object.noccurs,
            mined_object.accum_occurs,
            mined_object.noccurs / nseqs,
            conf,
            lift,
            '->'.join(list(map(str, mined_object.items)))))


def decode_result(result):
    result['seqstrm'] = result['seqstrm'].strip().decode('latin-1')
    result['logger'] = result['logger'].strip().decode('latin-1')
    result['summary'] = result['summary'].strip().decode('latin-1')