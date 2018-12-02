#ifndef _MAXGAP_H
#define _MAXGAP_H

#include "Itemset.h"
#include "Eqclass.h"

using namespace std;

//ItemBufferManager
class ItBufMgr{
private:
   Itemset ** _items;
   int _size;                   // size of _items
   void get_ext_item(int it);
public:
   ItBufMgr(int sz){
      int i;
      _size = sz;
      _items = new Itemset *[sz];
      Itemset *tmp;
      for (i = 0; i < sz; i++){
         tmp = new Itemset(1,0);
         tmp->setitem(0,F1::backidx[i]);
         _items[i] = tmp;
      }
   }
   ~ItBufMgr(){
      for (int i=0; i < _size; i++)
         delete _items[i];
      delete [] _items;
   }
   
   int in_mem(int it){
      if (_items[F1::fidx[it]]->ival()->arraySpade() == NULL) return 0;
      else return 1;
   }

   Itemset *get_item(int it){
      if (!in_mem(it)) get_ext_item(it);
      return _items[F1::fidx[it]];
   }
};

extern ItBufMgr *IBM;
extern void process_maxgap(Eqclass *L2);
#endif
