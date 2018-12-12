from pycspade.helpers import spade, print_result

if __name__ == '__main__':
    result = spade(filename='tests/zaki.txt', support=0.3)
    print('Sequences mined:')
    print((result['seqstrm']))
    print('Logger:')
    print((result['logger']))
    print('Summary:')
    print((result['summary']))

    print_result(result)

    # data = [
    #     [1, 10, [3, 4]],
    #     [1, 15, [1, 2, 3]],
    #     [1, 20, [1, 2, 6]],
    #     [1, 25, [1, 3, 4, 6]],
    #     [2, 15, [1, 2, 6]],
    #     [2, 20, [5]],
    #     [3, 10, [1, 2, 6]],
    #     [4, 10, [4, 7, 8]],
    #     [4, 20, [2, 6]],
    #     [4, 25, [1, 7, 8]]
    # ]
    #
    # result = spade(data=data, support=0.3)
    # print_result(result)
