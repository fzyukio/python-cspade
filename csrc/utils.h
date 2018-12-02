#ifndef UTILS_H
#define UTILS_H

#include <sstream>
using namespace std;

#ifndef bzero
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#endif

// Logger and mem logger
extern ostringstream logger;
extern ostringstream mined;
extern ostringstream memlog;

struct result_t {
    int nsequences;
    string mined;
    string logger;
    string memlog;
};

#endif