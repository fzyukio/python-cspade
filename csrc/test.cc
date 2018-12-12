#include "common.h"
#include "wrappers.h"


int main(int argc, char** argv) {
    string filename = "bb-tmi.txt";
    spade_arg_t args;
    args.maxsize = 10;
    args.maxlen = 10;
    args.maxgap = 1;
    args.support = 0.01;

    result_t result = runSpade(filename, args);

    ifstream f("test-cases/bb-tmi.Z10z10s0.01");
    string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    if (str != result.seqstrm){
        cerr << "TEST FAILED" << endl;
    }
    else {
        cout << "TEST PASSED" << endl;
    }
}