#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#include  "Itemset.h"
class HashTable {
private:
   unsigned int theSize;
   int num_find;
   int num_probe;
   Itemset **theCells;
   unsigned int hashval (Itemset *);
   unsigned int hashval (ArraySpade *, int);
   unsigned int hash(int, int);
   unsigned int hashval (Itemset *, unsigned int);   
public:
   HashTable (int);
   ~HashTable();
   
   int find(Itemset *);
   int find(ArraySpade *, int);
   int find(Itemset *, unsigned int, int);
   int add (Itemset *);
   void clear(); //simply clear cells without deleting contents
   void clear_cells(); //deletes cell contents
   inline Itemset *get_cell(int pos)
   {
      return theCells[pos];
   }
   inline double hit_ratio()
   {
      double hr=0;
      if (num_find > 0) hr = ((double)num_probe)/num_find;
      return hr;
   }
   inline int size()
   {
      return theSize;
   }
   
   friend ostream& operator << (ostream&, HashTable&);
};


#endif //__HASHTABLE_H
