//
// Created by Yukio Fukuzawa on 7/12/18.
//

#ifndef UTILITIES_MAKEBIN_H
#define UTILITIES_MAKEBIN_H

#include "common.h"
#include "argv_parser.h"
#include "Env.h"

result_t makebinFunc(Env& env, const string& infilename, const string& outfilename);


/**
 * Call makebin given the argument list as string
 * @param args e.g 'makebin test/zaki.txt zaki.data'
 */
result_t makebinWrapper(const string &arg, shared_ptr<Env>& envptr);

#endif //UTILITIES_MAKEBIN_H
