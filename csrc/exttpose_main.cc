#include "common.h"
#include "exttpose.h"

int main(int argc, char **argv) {
    try {
        Env env;
        ExttposeArgument args;
        args.parse_args(argc, argv);
        exttposeFunc(env, args);
        cout << env.logger.str() << endl;
        cout << env.summary.str() << endl;
        return 0;
    }
    catch (exception &e) {
        cerr << "exttpose: Caught exception: " << e.what() << endl;
    }
    catch (...) {
        cerr << "exttpose: Caught unknown exception" << endl;
    }
    return 1;
}