from libcpp.string cimport string as c_string

cdef extern from "../csrc/utils.h":
    cdef struct result_t:
        int nsequences;
        c_string mined;
        c_string logger;
        c_string memlog;

cdef extern from "../csrc/wrappers.h":
    void spadeWrapper(const c_string& args)
    void exttposeWrapper(const c_string& s);
    void getconfWrapper(const c_string& s);
    void makebinWrapper(const c_string& s);
    result_t getResult();


def c_makebin(args):
    args = 'makebin {} \n'.format(args)
    args = bytes(args, encoding='latin-1')
    return makebinWrapper(args)


def c_getconf(args):
    args = 'getconf {} \n'.format(args)
    args = bytes(args, encoding='latin-1')
    return getconfWrapper(args)


def c_exttpose(args):
    args = 'exttpose {} \n'.format(args)
    args = bytes(args, encoding='latin-1')
    return exttposeWrapper(args)


def c_spade(args):
    args = 'spade {} \n'.format(args)
    args = bytes(args, encoding='latin-1')
    return spadeWrapper(args)


def c_get_result(decode=False):
    """
    :param decode: if True, the return strings will be decoded and line-separated, otherwise raw C++ strings
                   (python bytes) are returned
    """
    cdef result = getResult()

    if decode:
        result['mined'] = result['mined'].decode('latin-1').split('\n')
        result['logger'] = result['logger'].decode('latin-1').split('\n')
        result['memlog'] = result['memlog'].decode('latin-1').split('\n')

    return result
