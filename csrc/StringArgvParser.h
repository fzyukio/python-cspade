#ifndef SPADE_UTILITY_STRINGARGVPARSER_H
#define SPADE_UTILITY_STRINGARGVPARSER_H

#include <iostream>
#include <string>
#include <list>

using namespace std;

const int ARGV_MAX = 255;
const int ARGV_TOKEN_MAX = 255;

char *copyChars(const char *s);

class StringArgvParser {
    int argc;
    list<string> argv;
    list<char> token;

    /* add a character to the current token */
    void addCharToToken(char c);

    /* finish the current token: copy it into argv and setup next token */
    void finishToken();

    void parse(string s);

public:
    int getArgc() const;
    list<string>& getArgv();

    StringArgvParser(string s);
};


struct args_t {
    int argc;
    char **argv;

    ~args_t() {
        for (int i=0; i<argc; i++) {
            delete argv[i];
        }
        delete argv;
    }
};

args_t* parse(string s);

#endif //SPADE_UTILITY_STRINGARGVPARSER_H
