import os
import uuid

import math

from logging import warning

from pycspade.cspade import c_exttpose, c_makebin, c_getconf, c_spade, c_get_result


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
          numpart=None, maxwin=None, bfstype=None, tid_lists=None):
    '''
    Call C++'s cspade()
    :param filename: full path to the input file (ascii)
    :param support: is interpreted as the threshold of mimimum normalised support if within [0, 1]:
                         if > 1: interpreted as the threshold of absolute support (e.g. 50 over 100 transactions)
    :param maxsize: an integer value specifying the maximum number of items of an element of a sequence (default=100)
    :param maxlen: an integer value specifying the maximum number of elements of a sequence (default=100)
    :param mingap: an integer value specifying the minimum time difference between consecutive elements of a sequence
    :param maxgap: an integer value specifying the minimum time difference between consecutive elements of a sequence

    :return: (result, logger, memlog). where:
             -result: the mined sequences
             -logger: general logging
             -memlog: logging of memory usage
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

    assert (0 < support < 1), 'support must be a floating point in range (0-1]'
    
    if mingap is not None:
        assert mingap > 0, 'mingap cannot be 0 - that would mean two transactions happen at the same time'
    if maxgap is not None:
        assert maxgap > 0, 'maxgap cannot be 0'
        if mingap and maxgap < mingap:
            mingap = maxgap
    
    hex = uuid.uuid4().hex

    if data:
        rows = data_to_rows(data)
        nrows = len(rows)
        filename = '/tmp/cspade-{}.txt'.format(hex)
        with open(filename, 'w', encoding='latin-1') as f:
            for row in rows:
                f.write(row)
                f.write('\n')
    else:
        nrows = file_len(filename)

    opt = ''
    nop = math.ceil((nrows + 2 * nrows) * 8 / math.pow(4, 10) / 5)
    if memsize:
        opt += '-m {}'.format(memsize)
        nop = math.ceil(nop * 32 / memsize)

    if numpart:
        if numpart < nop:
            warning('numpart less than recommended')
        nop = numpart

    datafile = '/tmp/cspade-{}.data'.format(hex)
    otherfile = '/tmp/cspade-{}'.format(hex)
    makebin_args = '{} {}'.format(filename, datafile)
    getconf_args = '-i {} -o {}'.format(otherfile, otherfile)
    exttpose_args = '-i {} -o {} -p 1 {} -l -x -s {}'.format(otherfile, otherfile, opt, support)

    if maxsize:
        opt += ' -Z {}'.format(maxsize)
    if maxlen:
        opt += ' -z {}'.format(maxlen)
    if mingap:
        opt += ' -l {}'.format(mingap)
    if maxgap:
        opt += ' -u {}'.format(maxgap)
    if maxwin:
        opt += ' -w {}'.format(maxwin)
    if bfstype is None:
        opt += ' -r'
    if tid_lists:
        opt += ' -y'

    spade_args = '-i {} -s {} {} -e {} -o'.format(otherfile, support, opt, nop)

    try:
        c_makebin(makebin_args)
        c_getconf(getconf_args)
        c_exttpose(exttpose_args)
        c_spade(spade_args)
        result = c_get_result(decode=False)
        decode_results(result)

        return result

    except Exception as e:
        raise RuntimeError(str(e))

    finally:
        for file in os.listdir('/tmp'):
            if file.startswith('cspade-{}'.format(hex)):
                os.remove('/tmp/{}'.format(file))