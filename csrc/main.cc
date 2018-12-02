#include <iostream>

#include "wrappers.h"

using namespace std;

int main(int argc, char **argv) {
    bool mb = false;
    bool gc = false;
    bool et = false;
    bool spade = false;

    if (argc == 1) {
        mb = true;
        gc = true;
        et = true;
        spade = true;
    } else {
        char *command = argv[1];
        if (!strcmp(command, "makebin")) {
            mb = true;
            gc = false;
            et = false;
            spade = false;
        }
        if (!strcmp(command, "getconf")) {
            mb = true;
            gc = true;
            et = false;
            spade = false;
        }
        if (!strcmp(command, "exttpose")) {
            mb = true;
            gc = true;
            et = true;
            spade = false;
        }
        if (!strcmp(command, "spade")) {
            mb = true;
            gc = true;
            et = true;
            spade = true;
        }
    }
    if (mb) {
        makebinWrapper("makebin test/zaki.txt zaki.data\n");
    }

    if (gc) {
        getconfWrapper("getconf -i zaki -o zaki\n");
    }

    if (et) {
        exttposeWrapper("exttpose -i zaki -o zaki -p 1 -l -x -s 0.3\n");
    }

    if (spade) {
        spadeWrapper("spade -i zaki -s 0.3 -Z 10 -z 10 -u 1 -r -e 1 -o\n");
    }

    result_t result = getResult();
    cout << result.mined;
    cout << result.nsequences;
    return 0;
}