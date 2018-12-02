#ifndef __LISTS_H
#define __LISTS_H
#include <iostream>

using namespace std;

typedef int (*CMP_FUNC) (void *, void *);
extern long MEMUSED;

template <class Items>
class ListNodes {
private:
   ListNodes<Items> *theNext;
   Items     theItem;

public:
   ListNodes(Items item, ListNodes<Items> *next);
   ~ListNodes();

   ListNodes<Items> *next (){
      return theNext;
   }
   
   void set_next (ListNodes<Items> *next){
      theNext = next;
   }

   Items item(){
      return theItem;
   }
   
   void set_item(Items item){
      theItem = item;
   }
};

template <class Items>
class Lists {
private:
   ListNodes<Items> *theHead;
   ListNodes<Items> *theLast;
   int theSize;
   
public:

   Lists();
   ~Lists();
   void clear();

   ListNodes<Items> *head (){
      return theHead;
   };
   ListNodes<Items> *last (){
      return theLast;
   };
   int size (){
      return theSize;
   };
   void set_head( ListNodes<Items> *hd)
   {
      theHead = hd;
   }
   void set_last( ListNodes<Items> *lst)
   {
      theLast = lst;
   }
   
   void append (Items item);   
   void prepend(Items item);
   void remove(ListNodes<Items> * prev, ListNodes<Items> * val);
   void sortedDescend(Items item, CMP_FUNC cmpare);
   void sortedAscend (Items item, CMP_FUNC cmpare) ;
   Items find(Items item, CMP_FUNC cmpare);
   int find_ascend(ListNodes<Items> *&prev,Items item, CMP_FUNC cmpare);
   void insert(ListNodes<Items> *&prev, Items item);
};

#endif// __LISTS_H


