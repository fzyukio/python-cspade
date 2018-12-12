from libcpp cimport bool
from libcpp.string cimport string as c_string

cdef extern from "../csrc/common.h":
    cdef struct result_t:
        int nsequences;
        c_string seqstrm;
        c_string logger;
        c_string summary;

    cdef struct spade_arg_t:
        double support;
        int maxsize;
        int maxlen;
        int mingap;
        int maxgap;
        int memsize;
        int numpart;
        int maxwin;
        bool bfstype;
        bool tid_lists;

cdef extern from "../csrc/wrappers.h":
    cdef result_t runSpade(const c_string& filename, spade_arg_t args) except +RuntimeError;


def c_runspade(filename, support=0.1, maxsize=None, maxlen=None, mingap=None, maxgap=None, memsize=None, numpart=None,
               maxwin=None, bfstype=None, tid_lists=None):
    cdef spade_arg_t args
    args.support = support
    args.maxsize = maxsize or -1
    args.maxlen = maxlen or -1
    args.mingap = mingap or -1
    args.maxgap = maxgap or -1
    args.memsize = memsize or -1
    args.numpart = numpart or -1
    args.maxwin = maxwin or -1
    args.bfstype = bfstype or False
    args.tid_lists = bfstype or False

    filename = bytes(filename, encoding='ascii')
    return runSpade(filename, args)