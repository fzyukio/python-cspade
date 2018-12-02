#include <cstring>
#include "Database.h"

Database::Database(char *infile, int buf_sz)
{
   fd = open (infile, O_RDONLY);
   if (fd < 0){
      throw runtime_error("ERROR: InvalidFile -- Dbase_Ctrl_Blk()");
   }

   buf_size = buf_sz;
   buf = new int [buf_sz];
   cur_buf_pos = 0;
   cur_blk_size = 0;
}
   
Database::~Database()
{
   delete [] buf;
   close(fd);
}

void Database::get_next_trans_ext(int &numitem, int &tid, int &custid)
{
   // Need to get more items from file
   int res = cur_blk_size - cur_buf_pos+3;
   if (res > 0)
   {
      // First copy partial transaction to beginning of buffer
      memcpy((void *)buf,
             (void *)(buf + cur_buf_pos - 3),
             res * ITSZ);
      cur_blk_size = res;
   }
   else
   {
      // No partial transaction in buffer
      cur_blk_size = 0;
   }

   res = read(fd, (void *)(buf + cur_blk_size), ((buf_size - cur_blk_size)*ITSZ));
   
   if (res < 0){
      throw runtime_error("reading in database");
   }
   cur_blk_size += res/ITSZ;
   if (cur_blk_size > 0)
   {
      custid = buf[0];
      tid = buf[1];
      numitem = buf[2];
      cur_buf_pos = 3;   
   }
}

