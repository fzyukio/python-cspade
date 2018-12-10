#include "makebin.h"

int main(int argc, char **argv) {
    try {
        Env env;
        string infilename(argv[1]);
        string outfilename(argv[2]);
        makebinFunc(env, infilename, outfilename);
        cout << env.logger.str();
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