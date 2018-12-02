#include <iostream>
#include "Lists.h"
#include "ArraySpade.h"
#include "Itemset.h"
#include "Eqclass.h"


template <class Items>
ListNodes<Items>::ListNodes(Items item, ListNodes<Items> *next)
{
   theItem = item;
   theNext = next;
   MEMUSED += sizeof(ListNodes<Items>);
}

template <class Items>
ListNodes<Items>::~ListNodes(){
   theNext = NULL;
   theItem = NULL;
   MEMUSED -= sizeof(ListNodes<Items>);
}

template <class Items>
Lists<Items>::Lists(){
   theHead = 0;
   theLast = 0;
   theSize = 0;
   MEMUSED += sizeof(Lists<Items>);
}

//only listnodes are deleted, if node->item() is a pointer to some object
//that object is *not* deleted
template <class Items>
Lists<Items>::~Lists(){
   ListNodes<Items> *node = theHead;
   ListNodes<Items> *tmp;
   while (node){
      tmp = node;
      node = node->next();
      delete tmp;
   }
   theHead = NULL;
   theLast = NULL;
   theSize = 0;
   MEMUSED -= sizeof(Lists<Items>);
}

//listnodes are deleted, if node->item() is a pointer to some object
//that object is *also*  deleted
template <class Items>
void Lists<Items>::clear(){
   ListNodes<Items> *node = theHead;
   ListNodes<Items> *tmp;
   while (node){
      tmp = node;
      node = node->next();
      if (tmp->item()) delete tmp->item();
      delete tmp;
   }
   theHead = NULL;
   theLast = NULL;
   theSize = 0;
}
   

   
template <class Items>
void Lists<Items>::append (Items item){
   ListNodes<Items> *node;
   
   theSize++;
   node = new ListNodes<Items> (item, 0);
   if (node == NULL){
      cout << "MEMORY EXCEEDED\n";
   }
   
   if (theHead == 0){
      theHead = node;
      theLast = node;
   }
   else{
      theLast->set_next(node);
      theLast = node;
   }
}


template <class Items>
void Lists<Items>::prepend (Items item){
   ListNodes<Items> *node;
   
   theSize++;
   node = new ListNodes<Items> (item, 0);
   if (node == NULL){
      cout << "MEMORY EXCEEDED\n";
   }
   
   if (theHead == 0){
      theHead = node;
      theLast = node;
   }
   else{
      node->set_next(theHead);
      theHead = node;
   }
}

template <class Items>
void Lists<Items>::remove(ListNodes<Items> * prev, ListNodes<Items> * val)
{
   if (prev == NULL) theHead = val->next();
   else prev->set_next(val->next());
   if (theLast == val) theLast = prev;
   theSize--;
}
   
template <class Items>
void Lists<Items>::sortedDescend(Items item, CMP_FUNC cmpare){
   ListNodes<Items> *node;
   ListNodes<Items> *temp = theHead;
   
   //printf("theSize %d\b", theSize);
   theSize++;
   node = new ListNodes<Items>(item, 0);
   if (node == NULL){
      cout << "MEMORY EXCEEDED\n";
   }
   
   if (theHead == 0){
      theHead = node;
      theLast = node;
   }
   else if (cmpare((void *)item,(void *)theHead->item()) > 0){
      node->set_next(theHead);
      theHead = node;
   }
   else{
      while (temp->next()){
         if (cmpare((void *)item,(void *)temp->next()->item()) > 0){
            node->set_next(temp->next());
            temp->set_next(node);
            return;
         }
         temp = temp->next();
      }
      theLast->set_next(node);
      theLast = node;
   } 
}
   
   
template <class Items>
void Lists<Items>::sortedAscend (Items item, CMP_FUNC cmpare) 
{
   ListNodes<Items> *node;
   ListNodes<Items> *temp = theHead;
   
   theSize++;
   node = new ListNodes<Items>(item,0);
   if (node == NULL){
      cout << "MEMORY EXCEEDED\n";
   }
   
   if (theHead == 0){
      theHead = node;
      theLast = node;
   }
   else if (cmpare((void *)item,(void *)theHead->item()) < 0){
      node->set_next(theHead);
      theHead = node;
   }
   else{
      while (temp->next()){
         if (cmpare((void *)item,(void *)temp->next()->item()) < 0){
            node->set_next(temp->next());
            temp->set_next(node);
            return;
         }
         temp = temp->next();
      }
      theLast->set_next(node);
      theLast = node;
   }
}

template <class Items>
Items Lists<Items>::find(Items item, CMP_FUNC cmpare)
{
   ListNodes<Items> *temp = theHead;
   for (;temp; temp = temp->next())
      if (cmpare((void *)item,(void *)temp->item()) == 0)
         return temp->item();
   return NULL;
}

template <class Items>
int Lists<Items>::find_ascend(ListNodes<Items> *&prev,
                       Items item, CMP_FUNC cmpare)
{
   ListNodes<Items> *temp = theHead;
   if (theHead == 0) return 0;
   else{
      int res = cmpare((void *)item,(void *)theHead->item());
      if (res == 0) return 1;
      else if (res < 0) return 0;
      else{
         while (temp->next()){
            res = cmpare((void *)item,(void *)temp->next()->item());
            if (res < 0){
               prev = temp;
               return 0;
            }
            else if (res == 0){
               prev = temp;
               return 1;
            }
            temp = temp->next();
         }
         prev = theLast;
         return 0;
      }
   }      
}

template <class Items>
void Lists<Items>::insert(ListNodes<Items> *&prev, Items item)
{
   theSize++;
   ListNodes<Items> *node = new ListNodes<Items>(item,0);
   if (node == NULL){
      cout << "MEMORY EXCEEDED\n";
   }
   
   if (prev == NULL){
      theHead = node;
      theLast = node;
   }
   else{
      node->set_next(prev->next());
      prev->set_next(node);
      if (prev == theLast) theLast = node;
   }  
}


//for G++ template class instiations
template class Lists<int *>;
template class Lists<ArraySpade *>;
template class Lists<Itemset *>;
template class Lists<Eqclass *>;

