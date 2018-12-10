#ifndef __TRANS_ARRAY_H
#define __TRANS_ARRAY_H

#include "common.h"

class TransArray {
private:
    vector<int> theArray;
    char theFlg;
    int lastPos;
    // DD: remove unsigned to avoid gcc warning signed vs non-signed cmp
    int theSize;
    // DD: remove unsigned to avoid gcc warning signed vs non-signed cmp
    long totSize;
    vector<long> offset;
public:

    explicit TransArray(long sz, int npart = 1);

    ~TransArray();

    int operator[](unsigned int index) {
        return theArray[index];
    };

    char flg() {
        return theFlg;
    }

    void setflg(char flg) {
        theFlg = flg;
    }

    int lastpos() {
        return lastPos;
    }

    //to be used ony for use_seq
    void setlastpos() {
        theArray[lastPos + 1] = theSize - lastPos - 2;
        lastPos = theSize;
    }

    long get_offset(int pos = 0) {
        return offset[pos];
    }

    void set_offset(long off, int pos = 0) {
        offset[pos] = off;
    }

    int totsize() {
        return totSize;
    }

    void reset() {
        theSize = 0;
        lastPos = 0;
        theFlg = 0;
    }

    vector<int>& array() {
        return theArray;
    }

    int size() {
        return theSize;
    }

    void setsize(int size) {
        theSize = size;
    }

    void setitem(int pos, int item) {
        theArray[pos] = item;
    }

    void additem(int item) {
        theArray[theSize] = item;
        theSize++;
    }

    void flushbuf(int fd, int use_seq, int pos = 0) {
        lseek(fd, offset[pos] * sizeof(int), SEEK_SET);
        int wblk = theSize;
        if (wblk > 0) {
            ssize_t res = ::write(fd, (char *) theArray.data(), wblk * sizeof(int));
            if (res < wblk * sizeof(int)) {
                throw runtime_error("Error writing");
            }
            offset[pos] += wblk;
        }
        theSize = 0;
    }

    void add(int fd, int item, int use_seq, int pos, int custid = -1) {
        if (use_seq) {
            if (theSize + 2 > totSize) {
                flushbuf(fd, use_seq, pos);
            }
            theArray[theSize++] = custid;
        } else {
            if (theSize + 1 > totSize) {
                flushbuf(fd, use_seq, pos);
            }
        }
        theArray[theSize++] = item;
    }
};

#endif //__TRANS_ARRAY_H


