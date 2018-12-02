#ifndef __ARRAYSPADE_H
#define __ARRAYSPADE_H
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
//#include <malloc.h>

using namespace std;

extern long MEMUSED;

class ArraySpade {
protected:   
   int *theArray;
   unsigned int theSize;
   unsigned int totSize;
   //unsigned int theIncr;
public:
   
   //ArraySpade (int sz, int incr);
   ArraySpade(int sz);
   ~ArraySpade();
   
   int subsequence(ArraySpade * ar);
   //void add (int, unsigned int);
   void add_ext(int val, int off, int *ary)
   {
      ary[off+theSize] = val;
      theSize++;
   }
   
   int operator [] (unsigned int index)
   {
      return theArray[index];
   };
   
   void setitem(int pos, int val){
      theArray[pos] = val;
   };
   
   int totsize()
   {
      return totSize;
   }
   void set_totsize(int sz){
      totSize = sz;
   }
   void set_size(int sz){
      theSize = sz;
   }
   void reset()
   {
      theSize = 0;
   }

   int *arraySpade()
   {
      return theArray;
   }
   void set_array(int *ary){
      theArray = ary;
   }
   //int subsequence(ArraySpade&);
   //int compare(ArraySpade&);
   friend ostream& operator << (ostream& outputStream, ArraySpade& arr);
   static int Arraycompare(void * iset1, void *iset2)
   {
      ArraySpade *it1 = (ArraySpade *) iset1;
      ArraySpade *it2 = (ArraySpade *) iset2;
      return it1->compare(*it2);
   }
   int compare(ArraySpade& ar2);

   int item (unsigned int index) 
   {
      return theArray[index];
   }
   
   unsigned int size() 
   {
      return theSize; 
   }
   
   void realloc(int newsz)
   {
      MEMUSED -= totSize*sizeof(int);
      totSize = newsz;
      theArray = (int *)::realloc(theArray, totSize*sizeof(int));
      if (theArray == NULL){
         cout << "MEMORY EXCEEDED\n";
      }
      MEMUSED += totSize*sizeof(int);
   }

   void compact(){
      realloc(theSize);
   }

   void optadd(int item)
   {
      theArray[theSize++] = item;
   }

   void add (int item)
   {
      if (theSize+1 > totSize){
         //totSize = (int) (totSize*1.5);
         //cout << " " << MEMUSED;
         realloc((int)(totSize*1.5));
         //theArray = (int *)realloc(theArray, totSize*sizeof(int));
         //if (theArray == NULL){
         //   cout << "MEMORY EXCEEDED\n";
         //   exit(-1);
         //}
         //MEMUSED += totSize*sizeof(int);
         //cout << " " << MEMUSED << endl;
      }
      theArray[theSize] = item;
      theSize++;
   }
};
#endif //__ARRAYSPADE_H


