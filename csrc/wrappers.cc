#include "StringArgvParser.h"
#include "wrappers.h"
#include "funcs.h"
#include "spade/spade.h"

void spadeWrapper(const string& s) {
    args_t* args = parse(s);
    sequenceFunc(args->argc, args->argv);
    delete args;
}

void exttposeWrapper(const string& s) {
    args_t* args = parse(s);
    exttposeFunc(args->argc, args->argv);
    delete args;
}

void getconfWrapper(const string& s) {
    args_t* args = parse(s);
    getconfFunc(args->argc, args->argv);
    delete args;
}

void makebinWrapper(const string& s) {
    args_t* args = parse(s);
    makebinFunc(args->argc, args->argv);
    delete args;
}

result_t getResult() {
    result_t result;

    result.mined = mined.str();
    result.logger = logger.str();
    result.memlog = memlog.str();
    result.nsequences = DBASE_NUM_TRANS;

    // Clear the stringstream otherwise the next run will get duplicated result
    mined.str(std::string());
    mined.clear();
    logger.str(std::string());
    logger.clear();
    memlog.str(std::string());
    memlog.clear();
    return result;
}