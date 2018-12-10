#include "common.h"
#include "TransArray.h"

TransArray::TransArray(long sz, int npart) {
    totSize = sz;
    theSize = 0;
    lastPos = 0;
    theFlg = 0;
    offset.reserve(npart);
    for (int i = 0; i < npart; i++) offset[i] = 0;
    if (sz > 0) {
        theArray.reserve(totSize);
    }
}

TransArray::~TransArray() {
    theArray.clear();
    offset.clear();
}






