#ifndef __DATABASE_H
#define __DATABASE_H

#include "common.h"

#define DCBBUFSZ 2048
#define TRANSOFF 3

#ifndef O_BINARY
#define O_BINARY 0
#endif

class DbaseCtrlBlk {
public:
    DbaseCtrlBlk() = default;

    void init(const string& infilename, int buf_sz = DCBBUFSZ);

    ~DbaseCtrlBlk();

    void get_next_trans_ext();

    void get_first_blk();

    void get_next_trans(int *&lbuf, int &nitems, int &tid, int &cid);

    int eof() {
        return (readall == 1);
    }

    int fd;
    int buf_size;
    int *buf;
    unsigned long cur_blk_size;
    int cur_buf_pos;
    unsigned long endpos;
    char readall;
};

#endif //__DATABASE_H





