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

#include "spade.h"
#include "maxgap.h"
#include "partition.h"

ItBufMgr *IBM;

void ItBufMgr::get_ext_item(int it)
{
   int supsz = partition_get_idxsup(it);
   //cout << "GETEXT " << it << " " << supsz << endl;
   int *newit = (int *) malloc (supsz*sizeof(int));
   partition_read_item(newit, it);
   _items[F1::fidx[it]]->set_support(supsz);
   _items[F1::fidx[it]]->ival()->set_size(supsz);
   _items[F1::fidx[it]]->ival()->set_array(newit);
}

void process_itemset(Itemset *iset, unsigned int templ, int iter)
{
   int i, it2;
   int lsup, esup;
   unsigned int ntpl;
   Itemset *iset2, *ejoin, *ljoin;
   int it = (*iset)[0];
   if (maxiter < iter) maxiter = iter;
   if (eqgraph[it]){
      for (i=0; i < eqgraph[it]->num_elements(); i++){
         it2 = eqgraph[it]->get_element(i);
         ntpl = templ;
         iset2 = IBM->get_item(it2);
         ljoin = NULL;
         ejoin = prune_decision(iset2, iset, ntpl, EJOIN);
         //cout << "MAX " << endl;
         if (pruning_type > 1) pre_pruning(ejoin, ntpl, iset, iset2,0);
         if (ejoin) get_tmpnewf_intersect(ljoin, ejoin, ljoin, lsup, esup, lsup,
                                          iset2, iset, iter);
         if (ejoin) fill_join(ejoin, iset, iset2);
         if (pruning_type > 1) post_pruning(ejoin, ntpl);
         if (ejoin){
            NumLargeItemset[iter-1]++;
            //fill_join(ejoin, iset, iset2);
            //if (iter > 3) ejoin->print_seq(ntpl);
            if (outputfreq) ejoin->print_seq(ntpl);
            process_itemset(ejoin, ntpl, iter+1);
            delete ejoin;
         }
      }

      for (i=0; i < eqgraph[it]->seqnum_elements(); i++){
         it2 = eqgraph[it]->seqget_element(i);
         ntpl = SETBIT(templ,1,iter-2);
         iset2 = IBM->get_item(it2);
         ejoin = NULL;
         //cout << "MAX " << endl;
         ljoin = prune_decision(iset2, iset, ntpl, LJOIN);
         if (pruning_type > 1) pre_pruning(ljoin, ntpl, iset, iset2,1);
         if (ljoin) get_tmpnewf_intersect(ljoin, ejoin, ejoin, lsup, esup, esup,
                                          iset2, iset, iter);
         if (ljoin) fill_join(ljoin, iset, iset2);
         if (pruning_type > 1) post_pruning(ljoin, ntpl);
         if (ljoin){
            NumLargeItemset[iter-1]++;
            //fill_join(ljoin, iset, iset2);
            //if (iter > 3) ljoin->print_seq(ntpl);
            if (outputfreq) ljoin->print_seq(ntpl);
            process_itemset(ljoin, ntpl, iter+1);
            delete ljoin;
         }
      }      
   }
}

void process_maxgap(Eqclass *L2)
{
   ListNodes<Itemset *> *hdr = L2->list()->head();
   for (; hdr; hdr = hdr->next()){
      process_itemset(hdr->item(), L2->templ(), 3);
   }
   
   hdr = L2->list2()->head();
   for (; hdr; hdr = hdr->next()){
      process_itemset(hdr->item(), L2->templ2(), 3);
   }
}
