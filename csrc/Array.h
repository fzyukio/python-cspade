#ifndef __ARRAY_H
#define __ARRAY_H

#include "common.h"

class Array;
typedef shared_ptr<Array> Array_S;
typedef shared_ptr<vector<Array_S>> Array_SS;

class Array {
private:
    shared_ptr<vint> theArray = nullptr;
    int theSize;        // DD
    int totSize;        // DD
    //unsigned int theIncr;
public:

    //Array (int sz, int incr);
    explicit Array(int sz);

    ~Array();

    int subsequence(Array_S ar);

    //void add (int, unsigned int);
    void add_ext(int val, int off, int *ary) {
        ary[off + theSize] = val;
        theSize++;
    }

    int operator[](unsigned int index) {
        return theArray->at(index);
    };

    void setitem(int pos, int val) {
        theArray->at(pos) = val;
    };

    int totsize() {
        return totSize;
    }

    void set_totsize(int sz) {
        totSize = sz;
    }

    void set_size(int sz) {
        theSize = sz;
    }

    void reset() {
        theSize = 0;
    }

    shared_ptr<vint> array() {
        return theArray;
    }

    void set_array(shared_ptr<vint> ary) {
        theArray = ary;
    }

    //int subsequence(Array&);
    //int compare(Array&);
    friend ostream &operator<<(ostream &outputStream, Array &arr);

    int compare(Array &ar2);

    int item(unsigned int index) {
        return theArray->at(index);
    }

    int size()        // DD
    {
        return theSize;
    }

    void resize(int newsz) {
        totSize = newsz;
        theArray->resize(totSize);
    }

    void compact() {
        theArray->resize(theSize);
    }

    void optadd(int item) {
        add(item);
    }

    void add(int item) {
        if (theSize + 1 > totSize) {
            resize((int) (totSize * 1.5));
        }
        theArray->at(theSize) = (item);
        theSize++;
    }
};

#endif //__ARRAY_H


