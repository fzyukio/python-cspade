#ifndef __EXT_H_
#define __EXT_H_

#include "partition.h"
#include "Eqclass.h"

#define ITSZ sizeof(int)

class invdb{
public:
   int numcust;
   int **curit;
   int *curcnt;
   int *curcid;
   int *curitsz;

   invdb(int sz);
   ~invdb();
   void incr(int sz);
   void incr_curit(int midx);
};



extern int make_l1_pass();
extern int make_l2_pass();
extern int get_file_l2(char *it2f, char *seqf);

#endif //__EXT_H_
