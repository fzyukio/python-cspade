//
// Created by Yukio Fukuzawa on 10/12/18.
//

#ifndef UTILITIES_ENV_H
#define UTILITIES_ENV_H

#include "common.h"

class Env {
public:
    ostringstream seqstrm; // Print the sequences
    ostringstream logger;
    ostringstream summary;
    ostringstream idlstrm; // To print ID list
};


#endif //UTILITIES_ENV_H
