#ifndef ARGV_PARSER_H
#define ARGV_PARSER_H

#include "common.h"

/**
 * Same as strdup. Make a deep copy of a string and return char*
 * @param s
 * @return
 */
char *string2chars(const string &str);

/**
 * Tuple containing argc and argv (char**)
 */
struct args_t {
    int argc;
    char **argv;

    args_t(std::list<string> args) {
        argc = static_cast<int>(args.size());
        argv = new char *[argc];
        std::list<string>::const_iterator iterator;
        int idx = 0;
        for (iterator = args.begin(); iterator != args.end(); ++iterator) {
            argv[idx] = string2chars((*iterator));
            idx++;
        }
    }

    ~args_t() {
        for (int i = 0; i < argc; i++) {
            delete[] argv[i];
        }
        delete[] argv;
    }
};

/**
 * Parse a string of arguments into char** (such that can be used by main())
 * This function is string aware, e.g. "hello world" is one arg, not two
 * @param s
 * @return
 */
args_t parse(const string &s);

#endif //ARGV_PARSER_H
