//
// Created by Yukio Fukuzawa on 7/12/18.
//

#ifndef UTILITIES_GETCONF_H
#define UTILITIES_GETCONF_H

#include "common.h"
#include "argv_parser.h"
#include "Env.h"

class GetconfArgument {
public:
    string input;       //input file name
    string confn;
    bool use_seq = true;

    void parse_args(int argc, char **argv) {
        auto cmdl = argh::parser(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
        cmdl("i") >> input;
        cmdl("o") >> confn;
        cmdl("a") >> use_seq;

        if (input.empty() || confn.empty()) {
            cerr << "usage: getconf [-a] -i<infile> -o<outfile>\n";
            throw runtime_error("getconf needs valid value of -i and -o");
        }

        input += ".data";
        confn += ".conf";
    }
};

result_t getconfFunc(Env& env, const GetconfArgument& args);

/**
 * Call getconf given the argument list as string
 * @param args e.g. 'getconf -i zaki -o zaki'
 */
result_t getconfWrapper(const string &args, shared_ptr<Env>& envptr);

#endif //UTILITIES_GETCONF_H
