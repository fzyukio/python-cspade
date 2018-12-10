#ifndef _EQCLASS_H
#define _EQCLASS_H

#include "common.h"
#include "Lists.h"
#include "Itemset.h"

#define EQCTYP1 1
#define EQCTYP2 2
#define EQCTYP3 3

class Eqclass;
typedef shared_ptr<Eqclass> Eqclass_S;
typedef shared_ptr<vector<Eqclass_S>> Eqclass_SS;

class Eqclass {
private:
    shared_ptr<Lists<shared_ptr<Itemset>>> theList;
    int Iset_size;
    unsigned int seqTemplate;
    shared_ptr<Lists<shared_ptr<Itemset>>> theList2;
    unsigned int seqTemplate2;
    int Eqtype;
public:
    Eqclass(int iset_sz, int eqt);

    ~Eqclass();

    int templ_sz();

    int eqtype();

    shared_ptr<Lists<shared_ptr<Itemset>>>& list();

    shared_ptr<Lists<shared_ptr<Itemset>>>& list2();

    unsigned int templ();

    unsigned int templ2();

    void set_templ(unsigned int val);

    void set_templ2(unsigned int val);

    void append(Itemset_S it);

    void append2(Itemset_S it);

    void prepend(Itemset_S it);

    void prepend2(Itemset_S it);
};

#endif

