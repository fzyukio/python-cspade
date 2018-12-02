#include "calcdb.h"
#include <cstring>

namespace utility {
    namespace exttpose {
        ExtDatabase::ExtDatabase(char *infile, int buf_sz) {
            fd = open(infile, O_RDONLY);
            if (fd < 0) {
                printf("ERROR: InvalidFile -- ExtDatabase()\n");
            }
            buf_size = buf_sz;
            buf = new int[buf_sz];
            cur_buf_pos = 0;
            cur_blk_size = 0;
            readall = 0;
            endpos = lseek(fd, 0, SEEK_END);
        }

        ExtDatabase::~ExtDatabase() {
            delete[] buf;
            close(fd);
        }

        void ExtDatabase::get_next_trans_ext() {
            // Need to get more items from file
            int res = cur_blk_size - cur_buf_pos;
            if (res > 0) {
                // First copy partial transaction to beginning of buffer
                memcpy((void *) buf,
                       (void *) (buf + cur_buf_pos),
                       res * ITSZ);
                cur_blk_size = res;
            } else {
                // No partial transaction in buffer
                cur_blk_size = 0;
            }

            res = read(fd, (void *) (buf + cur_blk_size),
                       ((buf_size - cur_blk_size) * ITSZ));

            if (res < 0) {
                throw runtime_error("reading in database");
            }
            cur_blk_size += res / ITSZ;
            //if (cur_blk_size > 0)
            //{
            //   custid = buf[0];
            //   tid = buf[1];
            //   numitem = buf[2];
            //   cur_buf_pos = 3;
            //}
            cur_buf_pos = 0;
        }
    }
}