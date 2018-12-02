#include <cmath>
#include <cstdlib>
#include <stddef.h>
#include <iostream>
#include <sstream>
#include "HashTable.h"


HashTable::HashTable (int size)
{
   int i, root;
   if (size < 3)
   {
      size = 3;
   }
   // Smallest power of 2 >= x
   for (i = 4; i < size; i *= 2);
   theSize = i+1;
   root = (int) sqrt(theSize);
   i = 3;
   while (i <= root)
   {
      if ((theSize % i) == 0)     // n is composite:
      {
         i = 1;                // restart loop and
         theSize += 2;            // check next odd n
         root = (int) sqrt(theSize);
      }
      i += 2;
   }
   //cout << "SIZE : "<< theSize << " " << size << endl;
   theCells = new Itemset *[theSize];
   if (theCells == NULL){
      cout << "MEMORY EXCEEDED\n";
   }
   for (i=0; i < theSize; i++){
      theCells[i] = NULL;
   }
   num_find=0;
   num_probe=0;
}


HashTable::~HashTable()
{
   delete [] theCells;
}



int HashTable::add (Itemset * item)
{
   num_find++;
   unsigned int hval = hashval(item);
   int pos;
   int i;
   for (i=0; i < theSize; i++){
      pos = hash(hval,i);
      num_probe++;
      if (theCells[pos] == NULL){
         theCells[pos] = item;
         return pos;
      }
   }

   ostringstream error;

   error << "ERROR: hash table full: ";
   error << *item;
   error << "HVAL : " << hval;
   error << "PROBE SEQ: ";
   for (i = 0; i < theSize; i++) {
      pos = hash(hval, i);
      error << " " << pos;
   }
   error << ".";
   error << *this;

   throw runtime_error(error.str());
}

int HashTable::find(Itemset * item)
{
   unsigned int hval = hashval(item);
   int pos;
   int i;
   for (i=0; i < theSize; i++){
      pos = hash(hval,i);
      if (theCells[pos] == NULL) break;
      if (item->compare(*theCells[pos]) == 0){
         return pos;
      }
   }
   return -1;
}

int HashTable::find(ArraySpade * item, int len)
{
   unsigned int hval = hashval(item, len);
   int pos;
   int i;
   for (i=0; i < theSize; i++){
      pos = hash(hval,i);
      if (theCells[pos] == NULL) break;
      if (theCells[pos]->compare(*item, len) == 0){
         return pos;
      }
   }
   return -1;
}

int HashTable::find(Itemset * item, unsigned int bvec, int len)
{
   unsigned int hval = hashval(item, bvec);
   //cout << "LOOKUP: " << *item;
   //cout << "BVEC : " << bvec << " " << hval << endl;
   int pos;
   int i;
   for (i=0; i < theSize; i++){
      pos = hash(hval,i);
      if (theCells[pos] == NULL) break;
      //cout << "CELL : " << pos << "=" << *theCells[pos];
      if (item->compare(*theCells[pos], len, bvec) == 0){
         return pos;
      }
   }
   return -1;
}

void HashTable::clear()
{
   for (int i=0; i < theSize; i++){
      theCells[i] = NULL;
   }
}
void HashTable::clear_cells()
{
   for (int i=0; i < theSize; i++){
      delete theCells[i];
      theCells[i] = NULL;
   }
}

unsigned int HashTable::hash (int hval, int num) 
{
   int h1 = hval%theSize;
   int h2 = 1+(hval%(theSize-1));
   return ((h1 + num*h2)%theSize);
}

unsigned int HashTable::hashval (Itemset * item) 
{
   unsigned int value=0;
   
   for (int i=0; i < item->size()-1; i++){
      value += (*item)[i];
      //value += (*item)[i];
      //value = 65599*value+(*item)[i];
   }
   value *= (*item)[item->size()-1];
   return value;
}
unsigned int HashTable::hashval (ArraySpade * item, int len)
{
   unsigned int value=0;
   
   for (int i=0; i < len-1; i++){
      value += (*item)[i];
      //value += (*item)[i];
      //value = 65599*value+(*item)[i];
   }
   value *= (*item)[len-1];
   return value;
}
unsigned int HashTable::hashval (Itemset * item, unsigned int bvec) 
{
   unsigned int value=0;
   int pos = 0;
   int last;
   for (;pos < item->size();pos++){
      if (GETBIT(bvec, pos)){
         //value = 65599*value+(*item)[pos];
         value += (*item)[pos];
         last = pos;
      }
   }
   value -= (*item)[last];
   value *= (*item)[last];
   return value;
}

ostream& operator << (ostream& outputStream, HashTable& hasht){
   outputStream << "HASH TABLE: Size = " <<hasht.theSize << "\n" ;
   outputStream.flush();
   for (int i=0; i < hasht.theSize; i++){
      if (hasht.theCells[i] == NULL)
         outputStream << "[" << i << "] = 0\n";
      else outputStream << "[" << i << "] = 1\n";
   }
   return  outputStream;
}



