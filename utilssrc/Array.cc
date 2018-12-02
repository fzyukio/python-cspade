#include <cstdio>
#include <cerrno>
#include "Array.h"

Array::Array (int sz, int npart){
   totSize = sz;
   theSize = 0;
   lastPos = 0;
   theFlg = 0;
   //theIncr = incr;
   theArray = NULL;
   offset = new long[npart];
   for (int i=0; i < npart; i++) offset[i]=0;
   if (sz > 0){
      theArray =  (int *) malloc (totSize*sizeof(int));
      //theArray = new int [totSize];
      if (theArray == NULL){
         perror("memory:: Array");
         exit(errno);
      }
   }
}

Array::~Array(){
   if (theArray) {
      free(theArray);
      //delete [] theArray;
   }
   delete [] offset;
   theArray = NULL;
}






