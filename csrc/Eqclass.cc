#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "Eqclass.h"

Eqclass::Eqclass(int iset_sz, int eqt) {
    Iset_size = iset_sz;
    Eqtype = eqt;
    theList.reset(new Lists<shared_ptr<Itemset>>());
    seqTemplate = seqTemplate2 = 0;

    if (Eqtype == EQCTYP1) {
        theList2.reset(new Lists<shared_ptr<Itemset>>());
    }
}

Eqclass::~Eqclass() = default;


int Eqclass::templ_sz() {
    return Iset_size;
}

int Eqclass::eqtype() {
    return Eqtype;
}

shared_ptr<Lists<shared_ptr<Itemset>>> &Eqclass::list() {
    return theList;
}

shared_ptr<Lists<shared_ptr<Itemset>>> &Eqclass::list2() {
    return theList2;
}

unsigned int Eqclass::templ() {
    return seqTemplate;
}

unsigned int Eqclass::templ2() {
    return seqTemplate2;
}

void Eqclass::set_templ(unsigned int val) {
    seqTemplate = val;
}

void Eqclass::set_templ2(unsigned int val) {
    seqTemplate2 = val;
}

void Eqclass::append(Itemset_S it) {
    theList->append(std::move(it));
}

void Eqclass::append2(Itemset_S it) {
    theList2->append(std::move(it));
}

void Eqclass::prepend(Itemset_S it) {
    theList->prepend(std::move(it));
}

void Eqclass::prepend2(Itemset_S it) {
    theList2->prepend(std::move(it));
}
