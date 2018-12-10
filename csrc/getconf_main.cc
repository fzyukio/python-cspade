#include "common.h"
#include "getconf.h"

int main(int argc, char **argv) {
    try {
        Env env;
        GetconfArgument args;
        args.parse_args(argc, argv);
        getconfFunc(env, args);

        cout << env.logger.str();
        cout << env.summary.str();
        return 0;
    }
    catch (exception &e) {
        cerr << "getconf: Caught exception: " << e.what() << endl;
    }
    catch (...) {
        cerr << "getconf: Caught unknown exception" << endl;
    }
    return 1;
}