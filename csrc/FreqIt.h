//
// Created by Yukio Fukuzawa on 8/12/18.
//

#ifndef CSPADE_FREQIT_H
#define CSPADE_FREQIT_H

#include "common.h"
#include "Itemset.h"

class FreqIt;
typedef shared_ptr<FreqIt> FreqIt_S;
typedef shared_ptr<vector<FreqIt_S>> FreqIt_SS;

class FreqIt {
public:
    shared_ptr<vint> seq;
    int seqsz;
    unsigned int templ;

    FreqIt(int sz, unsigned int tpl);

    FreqIt(shared_ptr<vint> ary, int sz, unsigned int tpl);

    ~FreqIt() = default;

    int size();

    int compare(Itemset_S iset, unsigned int itpl);

    int compare(FreqIt_S fit, int recursive);

    friend ostream &operator<<(ostream &outputStream, FreqIt_S freq);
};



#endif //CSPADE_FREQIT_H
