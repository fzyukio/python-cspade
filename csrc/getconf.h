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
    int use_seq = 1;

    void parse_args(int argc, char **argv) {
//        extern char *optarg;
        int c;
        optind = 1;

        if (argc < 2) {
            cerr << "usage: getconf [-a] -i<infile> -o<outfile>\n";
            exit(EXIT_FAILURE);
        } else {
            while ((c = getopt(argc, argv, "ai:o:")) != -1) {
                switch (c) {
                    case 'a': //work on assoc
                        use_seq = 0;
                        printf("USE SEQ = 0\n");
                        break;
                    case 'i':
                        input = string(optarg) + ".data";
                        break;
                    case 'o':
                        confn = string(optarg) + ".conf";
                        break;
                    case '?':
                    default:
                        ostringstream message;
                        message << "Illegal option: " << char(optopt) << ". Full argv: \"";
                        for (int i = 0; i < argc - 1; i++) {
                            message << argv[i] << ' ';
                        }
                        message << argv[argc - 1] << '\"';
                        throw runtime_error(message.str());
                }
            }
        }
    }
};

result_t getconfFunc(Env& env, const GetconfArgument& args);

/**
 * Call getconf given the argument list as string
 * @param args e.g. 'getconf -i zaki -o zaki'
 */
result_t getconfWrapper(const string &args, shared_ptr<Env>& envptr);

#endif //UTILITIES_GETCONF_H
