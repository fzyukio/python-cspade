#include <cstdio>
#include <errno.h>
#include "ArraySpade.h"

ArraySpade::ArraySpade (int sz){
   totSize = sz;
   theSize = 0;
   //theIncr = incr;
   theArray = NULL;
   if (sz > 0){
      theArray =  (int *) malloc (totSize*sizeof(int));
      //theArray = new int [totSize];
      if (theArray == NULL){
         throw runtime_error("memory:: ArraySpade");
      }
      MEMUSED += totSize*sizeof(int);
   }
   MEMUSED += sizeof(ArraySpade);
}

ArraySpade::~ArraySpade(){
   if (theArray) {
      free(theArray);
      //delete [] theArray;
      MEMUSED -= totSize*sizeof(int);
      //cout << "CAME HERE " << MEMUSED <<endl;
   }
   theArray = NULL;
   MEMUSED -= sizeof(ArraySpade);
}

ostream& operator << (ostream& outputStream, ArraySpade& arr){
   for (int i=0; i < arr.theSize; i++)
      outputStream << arr[i] << " ";
   return outputStream;
}

int ArraySpade::subsequence(ArraySpade * ar)
{
   int i,j;
   int sz1, sz2;
   ArraySpade *ar1, *ar2;
   int retval;
   
   if (theSize <= ar->theSize){
      sz1 = theSize;
      sz2 = ar->theSize;
      ar1 = this;
      ar2 = ar;
      retval = 1;
   }
   else{
      sz1 = ar->theSize;
      sz2 = theSize;
      ar1 = ar;
      ar2 = this;
      retval = -1;
   }
   int start = 0;
   for(i=0; i < sz1; i++){
      for(j=start; j < sz2; j++){
         if ((ar1->theArray)[i] == (ar2->theArray)[j]){
            start = j+1;
            break;
         }
      }
      if (j >= ar2->theSize) return 0;
   }
   return retval;
}


int ArraySpade::compare(ArraySpade& ar2)
{
   int len;
   if (size() <= ar2.size()) len = size();
   else len = ar2.size();
   for(int i=0; i < len; i++){
      if ((theArray)[i] > (ar2.theArray)[i]) return 1;
      else if ((theArray)[i] < (ar2.theArray)[i]) return -1;
   }
   if (size() < ar2.size()) return -1;
   else if (size() > ar2.size()) return 1;
   else return 0;
}





