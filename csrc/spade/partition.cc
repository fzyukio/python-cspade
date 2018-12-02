#include <cmath>
#include <errno.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
//#include <malloc.h>
#include <limits.h>

#include "partition.h"
struct timeval tp;

int num_partitions = 1;
int *DATAFD, *IDXFD, *IDXFLEN, **ITEMIDX;

void partition_alloc(char *dataf, char *idxf)
{
   DATAFD = new int[num_partitions];
   IDXFD = new int[num_partitions];
   IDXFLEN = new int[num_partitions];
   ITEMIDX = new int*[num_partitions];
   char tmpnam[300];
   for (int i=0; i < num_partitions; i++){
      if (num_partitions > 1) sprintf(tmpnam, "%s.P%d", dataf, i);
      else sprintf(tmpnam, "%s", dataf);
      DATAFD[i] = open(tmpnam, O_RDONLY);
      if (DATAFD[i] < 0){
         throw runtime_error("can't open data file");
      }
      
      if (num_partitions > 1) sprintf(tmpnam, "%s.P%d", idxf, i);
      else sprintf(tmpnam, "%s", idxf);
      IDXFD[i] = open(tmpnam, O_RDONLY);
      if (IDXFD[i] < 0){
         throw runtime_error("can't open idx file");
      }
      IDXFLEN[i] = lseek(IDXFD[i],0,SEEK_END);
      lseek(IDXFD[i],0,SEEK_SET);
#ifndef DEC
      ITEMIDX[i] = (int *) mmap((char *)NULL, IDXFLEN[i], PROT_READ,
                              MAP_PRIVATE,IDXFD[i], 0);   
#else
      ITEMIDX[i] = (int *) mmap((char *)NULL, IDXFLEN[i], PROT_READ,
                              (MAP_FILE|MAP_VARIABLE|MAP_PRIVATE),
                              IDXFD[i], 0);
#endif
      if (ITEMIDX[i] == (int *)-1){
         throw runtime_error("MMAP ERROR:item_idx");
      }
   }
}

void partition_dealloc()
{
   for (int i=0; i < num_partitions; i++){
      close(DATAFD[i]);
      close(IDXFD[i]);
      munmap((caddr_t)ITEMIDX[i], IDXFLEN[i]);
   }
   delete [] DATAFD;
   delete [] IDXFD;
   delete [] IDXFLEN;
   delete [] ITEMIDX;
}

int partition_get_blk_sz(int p)
{
   return lseek(DATAFD[p],0,SEEK_END);
}
int partition_get_max_blksz()
{
   int max = 0;
   int flen;
   for (int i=0; i < num_partitions; i++){
      flen = lseek(DATAFD[i],0,SEEK_END);
      if (max < flen) max = flen;
   }
   return max;
}
void partition_get_blk(int *MAINBUF, int p)
{
   int flen = lseek(DATAFD[p],0,SEEK_END);
   cout << "FILESZ " << flen << endl;
   lseek(DATAFD[p],0,SEEK_SET);
   if (read(DATAFD[p], (char *)MAINBUF, flen) < 0){
      throw runtime_error("read item1");
   }
}

int partition_get_idxsup(int it)
{
   int supsz = 0;
   for (int i=0; i < num_partitions; i++){
      supsz += ITEMIDX[i][it+1]-ITEMIDX[i][it];
   }
   return supsz;
}
int partition_get_lidxsup(int idx, int it)
{
   return (ITEMIDX[idx][it+1]-ITEMIDX[idx][it]);
}

int partition_get_idx(int idx, int it)
{
   return ITEMIDX[idx][it];
}

int *partition_idx(int idx)
{
   return ITEMIDX[idx];
}

void partition_read_item(int *ival, int it)
{
   int ipos=0;
   int supsz;
   for (int i=0; i < num_partitions; i++){
      supsz = ITEMIDX[i][it+1]-ITEMIDX[i][it];
      if (supsz > 0){
         lseek(DATAFD[i], ITEMIDX[i][it]*sizeof(int), SEEK_SET);
         if (read(DATAFD[i], (char *)&ival[ipos], supsz*sizeof(int)) < 0){
            throw runtime_error("read item1");
         }
         ipos+=supsz;
      }
   }
}

void partition_lclread_item(int *ival, int pnum, int it)
{
   int supsz;
   supsz = ITEMIDX[pnum][it+1]-ITEMIDX[pnum][it];
   if (supsz > 0){
      lseek(DATAFD[pnum], ITEMIDX[pnum][it]*sizeof(int), SEEK_SET);
      if (read(DATAFD[pnum], (char *)ival, supsz*sizeof(int)) < 0){
         throw runtime_error("read item1");
      }
   }
}


void partition_get_minmaxcustid(int *backidx, int numit, int pnum,
                               int &minv, int &maxv)
{
   int custid, it, i, supsz;
   minv = INT_MAX;
   maxv = 0;
   for (i=0; i < numit; i++){
      it = backidx[i];
      supsz = ITEMIDX[pnum][it+1]-ITEMIDX[pnum][it];
      if (supsz > 0){
         lseek(DATAFD[pnum], ITEMIDX[pnum][it]*sizeof(int), SEEK_SET);
         read(DATAFD[pnum], (char *)&custid, sizeof(int));
         if (minv > custid) minv = custid;
         lseek(DATAFD[pnum], (supsz-3)*sizeof(int), SEEK_CUR);
         read(DATAFD[pnum], (char *)&custid, sizeof(int));
         if (maxv < custid) maxv = custid;
      }
   }
}


//public
int *ClassInfo::CLASSCNT = NULL;
int *ClassInfo::MINSUP = NULL;
int *ClassInfo::TMPE = NULL;
int *ClassInfo::TMPM = NULL;
int *ClassInfo::TMPL = NULL;

//private                                
int ClassInfo::fd = -1;
int *ClassInfo::classes = NULL;
int *ClassInfo::clsaddr = NULL;

//extern
int NUMCLASS = 1;
extern double MINSUP_PER;
extern int DBASE_NUM_TRANS;

ClassInfo::ClassInfo(char use_class, char *classf)
{
   int i, numtrans, maxval;
   if (use_class){
      //cout << "FILE " << classf << endl << flush;
      fd = open (classf, O_RDONLY);
      if (fd < 0){
         printf("ERROR: InvalidClassFile\n");
      }   
      
      long fdlen = lseek(fd,0,SEEK_END);
      clsaddr = (int *) mmap((char *)NULL, fdlen, PROT_READ, MAP_PRIVATE, fd, 0);
      if (clsaddr == (int *)-1){
         throw runtime_error("MMAP ERROR:classfile_idx");
      }
      // first entry contains num classes
      NUMCLASS = clsaddr[0];
      //input is numclass followed by <cid, class> pairs
      numtrans = (fdlen/sizeof(int) - 1)/2;
      maxval = clsaddr[numtrans*2-1]+1;
      classes = new int [maxval];
      for (i=0; i < maxval; i++) classes[i] = NOCLASS;
      for (i=1; i < fdlen/sizeof(int); i+=2){
         classes[clsaddr[i]] = clsaddr[i+1];
      }
   }

   //cout << "NUMCLAS " << NUMCLASS << endl << flush;
   CLASSCNT = new int[NUMCLASS];
   TMPE = new int[NUMCLASS];
   TMPM = new int[NUMCLASS];
   TMPL = new int[NUMCLASS];
   MINSUP = new int[NUMCLASS];
   
   for (i = 0; i < NUMCLASS; i++)
      CLASSCNT[i] = 0;
   
   if (use_class){
      // class frequency
      for (i = 0; i < maxval; i++)
         if (classes[i] != NOCLASS)
            CLASSCNT[classes[i]]++;   
   }
   else CLASSCNT[0] = DBASE_NUM_TRANS;
   
   for (i = 0; i < NUMCLASS; i++){
      MINSUP[i] = (int) (MINSUP_PER*CLASSCNT[i]+0.5);
      if (MINSUP[i] < 1) MINSUP[i] = 1;
      //cout << "CLASS " << i << " " << CLASSCNT[i] << " " << MINSUP[i] << endl;
   }
}

ClassInfo::~ClassInfo()
{
   delete [] CLASSCNT;
   delete [] MINSUP;
   delete [] TMPE;
   delete [] TMPM;
   delete [] TMPL;
   if (fd != -1){
      long fdlen = lseek(fd,0,SEEK_END);
      munmap((caddr_t)clsaddr, fdlen);
      close(fd);
   }
}

