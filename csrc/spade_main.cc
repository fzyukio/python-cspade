#include "common.h"
#include "Sequence.h"

int main(int argc, char **argv) {
    try {
        Env env;
        shared_ptr<SpadeArguments> args = make_shared<SpadeArguments>();
        args->parse_args(argc, argv);
        result_t result = sequenceFunc(env, args);

        cout << result.logger << endl;
        cout << result.summary << endl;
        cout << result.seqstrm << endl;
        cout << result.nsequences << endl;
        return 0;
    }
    catch (exception &e) {
        cerr << "sequence: Caught '" << typeid(e).name() << "' exception: " << e.what() << endl;
    }
    catch (...) {
        cerr << "sequence: Caught unknown exception" << endl;
    }
    return 1;
}