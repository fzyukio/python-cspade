#include "common.h"
#include "argv_parser.h"
#include "Env.h"

#define ITSZ sizeof(int)

const int lineSize = 8192;
const int wdSize = 256;

void convbin(ostream &fout, char *inBuf, int inSize) {
    char *p;
    int it;

    for (;;) {
        it = strtol(inBuf, &p, 10);
        if (p == inBuf)
            break;

        fout.write((char *) &it, ITSZ);
        inBuf = p;
    }
}

result_t makebinFunc(Env& env, const string& infilename, const string& outfilename) {
    char inBuf[lineSize];
    int inSize;
    ifstream fin(infilename);

    if (!fin) {
        string error_message = "can't open ascii file: " + infilename;
        throw runtime_error(error_message);
    }
    ofstream fout(outfilename, ios::binary);
    if (!fout) {
        string error_message = "can't open binary file: " + outfilename;
        throw runtime_error(error_message);
    }

    while (fin.getline(inBuf, lineSize)) {
        inSize = fin.gcount();
//        env.logger << "IN SIZE " << inSize << endl;
        convbin(fout, inBuf, inSize);
    }

    fin.close();
    fout.close();

    result_t result;
    result.logger = env.logger.str();
    return result;
}

result_t makebinWrapper(const string &s, shared_ptr<Env>& envptr) {
    args_t args = parse(s);
    Env env;
    string infilename(args.argv[1]);
    string outfilename(args.argv[2]);

    if (envptr == nullptr) {
        return makebinFunc(env, infilename, outfilename);
    }
    else {
        return makebinFunc(*envptr, infilename, outfilename);
    }
}