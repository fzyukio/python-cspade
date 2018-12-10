
#ifndef __ITEMSET_H
#define __ITEMSET_H

#include "common.h"
#include "Array.h"
#include "Lists.h"
#include "SpadeArguments.h"
#include "Partition.h"

#define SETBIT(a, v, b)  (((v) != 0) ? ((a) | (01 << (b))): ((a) & ~(01 << (b))))
#define GETBIT(a, b) ((a) & (01 << (b)))

class Itemset;
typedef shared_ptr<Itemset> Itemset_S;

class Itemset {
protected:
    Array_S theItemset;
    Array_S theIval;
    int theSupport;
    vint clsSup;
    int num_class;
    bool do_print;
//    shared_ptr<ClassInfo> cls;
//    shared_ptr<SpadeArguments> args;
public:
    explicit Itemset(bool x){}

    Itemset(int it_sz, int ival_sz, int nclass, bool print);

    ~Itemset();

    friend ostream &operator<<(ostream &outputStream, Itemset &itemset);

    int compare(Itemset &ar2);

    int subsequence(Itemset_S ar);

    void print_seq(ostream& seqstrm, int itempl);

    void print_idlist(ostream& idlstrm);

    Array_S &ival() {
        return theIval;
    }

    int ival(int pos) {
        return (*theIval)[pos];
    }

    int ivalsize() {
        return theIval->size();
    }

    void reallocival() {
        theIval->resize(ivalsize());
    }

    int operator[](int pos) {
        return (*theItemset)[pos];
    };

    void setitem(int pos, int val) {
        theItemset->setitem(pos, val);
    };

    Array_S itemset() {
        return theItemset;
    };

    void add_item(int val) {
        theItemset->add(val);
    };

    int size() {
        return theItemset->size();
    };

    int support() {
        return theSupport;
    };

    void set_support(int sup) {
        theSupport = sup;
    }

    void increment_support() {
        theSupport++;
    };

    int cls_support(int cls) {
        return clsSup[cls];
    }

    void increment_cls_support(int cls) {
        clsSup[cls]++;
    }

    void set_cls_support(int sup, int cls) {
        clsSup[cls] = sup;
    }

    static int Itemcompare(void *iset1, void *iset2) {
        Itemset* it1 = (Itemset* ) iset1;
        Itemset* it2 = (Itemset* ) iset2;
        return it1->compare(*it2);
    }
};

extern Itemset_S placeholder;

#endif //__ITEMSET_H

