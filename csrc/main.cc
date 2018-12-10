#include "common.h"
#include "wrappers.h"


int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Usage: cpsde-full /path/to/file.txt" << endl;
        return 1;
    }
    string filename(argv[1]);
    spade_arg_t args;
    args.maxgap = 1;
    args.support = 0.01;
    args.maxsize = 10;
    args.maxlen = 10;

    result_t result = runSpade(filename, args);
    cout << result.seqstrm;
    return 0;
}

//int main1(int argc, char **argv) {
//    bool mb = false;
//    bool gc = false;
//    bool et = false;
//    bool spade = false;
//
//    if (argc == 1) {
//        mb = true;
//        gc = true;
//        et = true;
//        spade = true;
//    } else {
//        char *command = argv[1];
//        if (!strcmp(command, "makebin")) {
//            mb = true;
//            gc = false;
//            et = false;
//            spade = false;
//        }
//        if (!strcmp(command, "getconf")) {
//            mb = true;
//            gc = true;
//            et = false;
//            spade = false;
//        }
//        if (!strcmp(command, "exttpose")) {
//            mb = true;
//            gc = true;
//            et = true;
//            spade = false;
//        }
//        if (!strcmp(command, "spade")) {
//            mb = true;
//            gc = true;
//            et = true;
//            spade = true;
//        }
//    }
//    if (mb) {
//        makebinWrapper("makebin test/zaki.txt zaki.data");
//    }
//
//    if (gc) {
//        getconfWrapper("getconf -i zaki -o zaki");
//    }
//
//    if (et) {
//        exttposeWrapper("exttpose -i zaki -o zaki -p 1 -l -x -s 0.3");
//    }
//
//    if (spade) {
//        spadeWrapper("spade -i zaki -s 0.3 -Z 10 -z 10 -u 1 -r -e 1 -o");
//    }
//
//    result_t result = getResult();
//    cout << result.mined;
//    cout << result.nsequences;
//    return 0;
//}
