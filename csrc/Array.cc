#include <stdio.h>
#include <errno.h>
#include "Array.h"

Array::Array(int sz) {
    totSize = sz;
    theSize = 0;
    if (sz > 0) {
        theArray = make_shared<vint>(sz);
    }
}

Array::~Array() = default;

ostream &operator<<(ostream &outputStream, Array &arr) {
    for (int i = 0; i < arr.theSize; i++)
        outputStream << arr[i] << " ";
    return outputStream;
}

int Array::subsequence(Array_S ar) {
    int i, j;
    int sz1, sz2;
    Array_S ar1, ar2;
    int retval;

    if (theSize <= ar->theSize) {
        sz1 = theSize;
        sz2 = ar->theSize;
        ar1 = shared_ptr<Array>(this);
        ar2 = ar;
        retval = 1;
    } else {
        sz1 = ar->theSize;
        sz2 = theSize;
        ar1 = ar;
        ar2 = shared_ptr<Array>(this);
        retval = -1;
    }
    int start = 0;
    for (i = 0; i < sz1; i++) {
        for (j = start; j < sz2; j++) {
            if (ar1->theArray->at(i) == ar2->theArray->at(j)) {
                start = j + 1;
                break;
            }
        }
        if (j >= ar2->theSize) return 0;
    }
    return retval;
}


int Array::compare(Array &ar2) {
    int len;
    if (size() <= ar2.size()) len = size();
    else len = ar2.size();
    for (int i = 0; i < len; i++) {
        if (theArray->at(i) > ar2.theArray->at(i)) return 1;
        else if (theArray->at(i) < ar2.theArray->at(i)) return -1;
    }
    if (size() < ar2.size()) return -1;
    else if (size() > ar2.size()) return 1;
    else return 0;
}





