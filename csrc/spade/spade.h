#ifndef SPADE_H
#define SPADE_H

#include <limits.h>
#include "Itemset.h"
#include "Eqclass.h"

using namespace std;

#define min(a, b) ((a) < (b) ? (a) : (b))


#define NONMAXFLG -2
#define MBYTE (1024*1024)

#define CUSTID(a, b) ((a)[2*(b)])
#define TID(a, b) ((a)[2*(b)+1])

//join type
#define LJOIN 0
#define EJOIN 1
#define MJOIN 2

//pruning types
#define NOPRUNING 0            // no pruning
#define L2PRUNING 1
#define ZEROPRUNING 2     // when sup goes to zero in other classes, prune
#define FOLLOWPRUNING 4         //
extern int pruning_type;
extern char print_tidlist;
extern float FOLLOWTHRESH;

extern long AVAILMEM;
extern int MINSUPPORT;
extern int DBASE_NUM_TRANS;
extern int DBASE_MAXITEM;
extern float DBASE_AVG_TRANS_SZ;
extern float DBASE_AVG_CUST_SZ;
extern int DBASE_TOT_TRANS;

extern int num_partitions;
extern int maxiter;
extern int *backidx;            // in extl2.cc
extern int *NumLargeItemset;    // in sequence.cc
extern EqGrNode **eqgraph;      // -do-
extern int pruning_type;

extern int NUMCLASS;
extern int min_gap;
extern int max_gap;
extern int max_seq_len;
extern int max_iset_len;
extern char outputfreq;
extern int prepruning;
extern bool count_multiple;

extern void get_tmpnewf_intersect(Itemset *&ljoin, Itemset *&ejoin,
                                  Itemset *&mjoin, int &lcnt, int &ecnt, int &mcnt,
                                  Itemset *it1, Itemset *it2, int iter);
extern void fill_join(Itemset *join, Itemset *hdr1, Itemset *hdr2);
extern void pre_pruning(Itemset *&iset, unsigned int ptempl,
                        Itemset *it1, Itemset *it2, char use_seq);
extern void post_pruning(Itemset *&iset, unsigned int templ);
extern Itemset *prune_decision(Itemset *it1, Itemset *it2,
                               unsigned int ptempl, int jflg);

#endif
