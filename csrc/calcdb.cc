#include "calcdb.h"
#include <string.h>

void DbaseCtrlBlk::init(const string &infilename, int buf_sz) {
    fd = open(infilename.c_str(), O_RDONLY | O_BINARY);
    if (fd < 0) {
        throw runtime_error("ERROR: InvalidFile -- Dbase_Ctrl_Blk()");
    }

    buf_size = buf_sz;
    buf = new int[buf_sz];
    cur_buf_pos = 0;
    cur_blk_size = 0;
    readall = 0;
    endpos = lseek(fd, 0, SEEK_END);
}

DbaseCtrlBlk::~DbaseCtrlBlk() {
    delete[] buf;
    close(fd);
}

void DbaseCtrlBlk::get_next_trans_ext() {
    // Need to get more items from file
    ssize_t res = cur_blk_size - cur_buf_pos;
    if (res > 0) {
        // First copy partial transaction to beginning of buffer
        memcpy((void *) buf,
               (void *) (buf + cur_buf_pos),
               res * INT_SIZE);
        cur_blk_size = res;
    } else {
        // No partial transaction in buffer
        cur_blk_size = 0;
    }

    res = read(fd, (void *) (buf + cur_blk_size), ((buf_size - cur_blk_size) * INT_SIZE));

    if (res < 0) {
        throw runtime_error("reading in database");
    }
    cur_blk_size += res / INT_SIZE;
    cur_buf_pos = 0;
}

void DbaseCtrlBlk::get_first_blk() {
    readall = 0;
    lseek(fd, 0, SEEK_SET);
    cur_blk_size = (read(fd, (void *) buf, (buf_size * INT_SIZE))) / INT_SIZE;
    if (cur_blk_size < 0) {
        throw runtime_error("get_first_blk");
    }
    cur_buf_pos = 0;
}

void DbaseCtrlBlk::get_next_trans(int *&lbuf, int &nitems, int &tid, int &cid) {
    if (cur_buf_pos + TRANSOFF >= cur_blk_size ||
        cur_buf_pos + buf[cur_buf_pos + TRANSOFF - 1] + TRANSOFF > cur_blk_size) {
        if (lseek(fd, 0, SEEK_CUR) == endpos) readall = 1;
        if (!readall) {
            // Need to get more items from file
            get_next_trans_ext();
        }
    }

    if (!readall) {
        cid = buf[cur_buf_pos];
        tid = buf[cur_buf_pos + TRANSOFF - 2];
        nitems = buf[cur_buf_pos + TRANSOFF - 1];
        lbuf = buf + cur_buf_pos + TRANSOFF;
        cur_buf_pos += nitems + TRANSOFF;
    }
}


