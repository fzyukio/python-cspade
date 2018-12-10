//
// Created by Yukio Fukuzawa on 3/12/18.
//

#ifndef SPADE_UTILITY_WRAPPERS_H
#define SPADE_UTILITY_WRAPPERS_H

#include "common.h"

struct spade_arg_t {
    double support = 0.1;
    int maxsize = -1;
    int maxlen = -1;
    int mingap = -1;
    int maxgap = -1;
    int memsize = -1;
    int numpart = -1;
    int maxwin = -1;
    bool bfstype = false;
    bool tid_lists = false;
};

/**
 * One function to call all 4 functions and return the result
 * @param filename name of the input file, e.g. /path/to/zaki.txt
 * @param args arguments to spade.
 * @param tmpdir temporary folder for spade to operate. Temp files will be cleaned afterwards. Must end with a slash
 * @return same as getResult
 */
result_t runSpade(const string& filename, spade_arg_t args, const string& tmpdir = "/tmp/");

#endif //SPADE_UTILITY_WRAPPERS_H
