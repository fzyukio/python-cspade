
#ifndef __ITEMSET_H
#define __ITEMSET_H
#include <iostream>
#include <cstdio>
#include <errno.h>
#include "ArraySpade.h"
#include "Lists.h"

using namespace std;

//#define SETBIT(a,b) ((a) |= (1 << (b)))
//#define UNSETBIT(a,b) ((a) &= ~(1 << (b)))
#define SETBIT(a,v,b)  (((v) != 0) ? ((a) | (01 << (b))): ((a) & ~(01 << (b))))
#define GETBIT(a,b) ((a) & (01 << (b)))

extern int NUMCLASS;

class Itemset{
protected:
   ArraySpade *theItemset;
   ArraySpade *theIval;
   int theSupport;
   int *clsSup;
   
public:
   Itemset(int it_sz, int ival_sz, int nclass = NUMCLASS);
   ~Itemset();

   friend ostream& operator << (ostream& outputStream, Itemset& itemset);
   void intersect_neighbors(Itemset *it1, Itemset *it2);
   int compare(Itemset& ar2, int len);
   int compare(Itemset& ar2);
   int compare(ArraySpade& ar2, int len);
   int compare(Itemset& ar2, int len, unsigned int);
   int subsequence(Itemset * ar);
   void print_seq(int itempl);
   void print_idlist();

   ArraySpade *&ival()
   {
      return theIval;
   }
   int ival(int pos)
   {
      return (*theIval)[pos];
   }
   int ivalsize()
   {
      return theIval->size();
   }
   void add_ival(int it)
   {
      theIval->add(it);
   }
   void reallocival()
   {
      theIval->realloc(ivalsize());
   }
   void add_ival(ArraySpade * ary)
   {
      for (int i=0; i < ary->size(); i++)
         theIval->add((*ary)[i]);
   }
   
   int operator [] (int pos){
      return (*theItemset)[pos];
   };
   
   int item(int pos){
      return (*theItemset)[pos];
   };
   
   void setitem(int pos, int val){
      theItemset->setitem(pos, val);
   };
   
   void set_itemset (ArraySpade *ary)
   {
      theItemset = ary;
   }
   
   ArraySpade * itemset(){
      return theItemset;
   };

   void add_item(int val){
      theItemset->add(val);
   };
   
   int size(){
      return theItemset->size();
   };

   int support(){ 
      return theSupport;
   };

   void set_support(int sup)
   {
      theSupport = sup;
   }
   
   void increment_support(){
      theSupport++;
   };

   int cls_support(int cls){
      return clsSup[cls];
   }
   void increment_cls_support(int cls){
      clsSup[cls]++;
   }
   void set_cls_support(int sup, int cls)
   {
      clsSup[cls] = sup;
   }

   static int intcmp (void *it1, void *it2)
   {
      int i1 = *(int *) it1;
      int i2 = *(int *) it2;
      //printf("cmp %d %d\n", i1->theSupport, 
      if (i1 > i2) return 1;
      else if (i1 < i2) return -1;
      else return 0;
   }
   
   static int supportcmp (void *it1, void *it2)
   {
      Itemset * i1 = (Itemset *)it1;
      Itemset * i2 = (Itemset *)it2;
      //printf("cmp %d %d\n", i1->theSupport, 
      if (i1->theSupport > i2->theSupport) return 1;
      else if (i1->theSupport < i2->theSupport) return -1;
      else return 0;
   }
   
   static int Itemcompare(void * iset1, void *iset2)
   {
      Itemset *it1 = (Itemset *) iset1;
      Itemset *it2 = (Itemset *) iset2;
      return it1->compare(*it2);
   }

   //assume ts1 and ts2 are of same length
   static int compare_seq(void *ts1, void *ts2, int len)
   {
      int *tseq1 = (int *)ts1;
      int *tseq2 = (int *)ts2;
      for (int i=0; i < len; i++){
         if (tseq1[i] > tseq2[i]) return 1;
         else if (tseq1[i] < tseq2[i]) return -1;
      }
      return 0;
   }
   //int find(int , int*);
   //int subsequence(Itemset &);
   //int compare(Itemset &);
};



#endif //__ITEMSET_H

