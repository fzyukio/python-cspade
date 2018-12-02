from pycspade.helpers import spade


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


if __name__ == '__main__':
    result = spade(filename='tests/zaki.txt', support=0.3, maxgap=2)
    print_result(result)

    # data = [
    #     [1, 1, [1]],
    #     [1, 2, [2]],
    #     [1, 3, [3]],
    #
    #     [2, 1, [2]],
    #     [2, 2, [3]],
    #
    #     [3, 1, [1]],
    #     [3, 2, [3]],
    #     [3, 3, [2]],
    #     [3, 4, [3]],
    # ]
    #
    # result = spade(data=data, support=0.5)
    # print_result(result)
