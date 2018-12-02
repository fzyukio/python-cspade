#ifndef __DATABASE_H
#define __DATABASE_H

#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

using namespace std;

#define ITSZ sizeof(int)
#define BUFSZ 8192
#define HDRSZ 0

class Database{
public:
   Database(char *infile, int buf_sz);
   ~Database();

   void get_next_trans_ext(int &numitem, int &tid, int &custid);
   inline void get_first_blk();
   inline void get_next_trans(int * &lbuf, int &numitem,
                              int & tid, int &custid);
   
   int fd;     
   int buf_size;
   int * buf;
   int cur_blk_size; 
   int cur_buf_pos;  
};

inline void Database::get_first_blk()
{
   lseek(fd, HDRSZ*ITSZ, SEEK_SET);
   cur_blk_size = (read(fd,(void *)buf, (buf_size*ITSZ)))/ITSZ;
   if (cur_blk_size < 0){
      throw runtime_error("get_first_blk");
   }
   cur_buf_pos = 3;
}

inline void Database::get_next_trans
(int * &lbuf, int &numitem,int & tid, int &custid)
{
   numitem = buf[cur_buf_pos-1];
   tid = buf[cur_buf_pos-2];
   custid = buf[cur_buf_pos-3];
   if ((cur_buf_pos + numitem + 3) > cur_blk_size)
   {
      // Need to get more items from file
      get_next_trans_ext(numitem, tid, custid);
   }
   lbuf = buf + cur_buf_pos;
   
   cur_buf_pos += numitem + 3;
}


#endif //__DATABASE_H





