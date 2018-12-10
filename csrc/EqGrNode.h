//
// Created by Yukio Fukuzawa on 8/12/18.
//

#ifndef CSPADE_EQGRNODE_H
#define CSPADE_EQGRNODE_H

#include "common.h"
#include "Array.h"
#include "FreqIt.h"

class EqGrNode {
private:
    Array_S theElements;
    Array_S stheElements;

    Array_SS _set_sup;          // support in different classes
    Array_SS _seq_sup;          // support in different classes

    FreqIt_SS freqArray; //frequent seq from this class
    int freqArraySz;

    int theFlg; //indicates if class is in memory

    int num_class;

public:
    static int bsearch(int min, int max, FreqIt_SS freqArray, FreqIt_S fit, int recursive);

    static int bsearch(int min, int max, shared_ptr<vint> itary, int it);

    EqGrNode(int sz, int num_class);

    ~EqGrNode();

    FreqIt_SS freqarray() {
        return freqArray;
    }

    int freqarraysz() {
        return freqArraySz;
    }

    void set_freqarray(FreqIt_SS fit, int sz) {
        freqArray = fit;
        freqArraySz = sz;
    }

    int find_freqarray(FreqIt_S fit, int recursive);

    int getflg() {
        return theFlg;
    }

    void setflg(int val) {
        theFlg = val;
    }

    void add_sup(int sup, int clas) {
        _set_sup->at(clas)->add(sup);
    }

    void add_seqsup(int sup, int clas) {
        _seq_sup->at(clas)->add(sup);
    }

    int get_sup(int idx, int clas = -1) {
        if (clas == -1) {
            int sum = 0;
            //return sup in all classes
            for (int i = 0; i < num_class; i++)
                sum += (*_set_sup->at(i))[idx];
            return sum;
        } else return (*_set_sup->at(clas))[idx]; //return sup in class only
    }

    int get_seqsup(int idx, int clas = -1) {
        if (clas == -1) {
            int sum = 0;
            //return sup in all classes
            for (int i = 0; i < num_class; i++)
                sum += (*_seq_sup->at(i))[idx];
            return sum;
        } else return (*_seq_sup->at(clas))[idx]; //return sup in class only
    }

    Array_S elements() {
        return theElements;
    }

    int num_elements() {
        if (theElements)
            return theElements->size();
        else return 0;
    }

    void add_element(int el) {
        //theElements[numElements] = el;
        //numElements++;
        theElements->add(el);
    }

    void add_element(int el, int pos) {
        theElements->setitem(pos, el);
    }

    int get_element(int pos) {
        return (*theElements)[pos];
    }

    void seqsetelements(Array_S ary) {
        stheElements = ary;
    }

    Array_S seqelements() {
        return stheElements;
    }

    int seqnum_elements() {
        if (stheElements)
            return stheElements->size();
        else return 0;
    }

    void seqadd_element(int el) {
        stheElements->add(el);
        //snumElements++;
    }

    void seqadd_element(int el, int pos) {
        stheElements->setitem(pos, el);
    }

    int seqget_element(int pos) {
        return (*stheElements)[pos];
    }


    int find(int it) {
        if (theElements) {
            //for (int i=0; i <theElements->size(); i++)
            //   if ((*theElements)[i] == it) return 1;
            return bsearch(0, theElements->size() - 1, theElements->array(), it);
        }
        return -1;
    }

    int seqfind(int it) {
        if (stheElements) {
            //for (int i=0; i <stheElements->size(); i++)
            //   if ((*stheElements)[i] == it) return 1;
            return bsearch(0, stheElements->size() - 1, stheElements->array(), it);
        }
        return -1;
    }

    friend ostream &operator<<(ostream &outputStream, EqGrNode &EQ);
};

typedef shared_ptr<EqGrNode> EqGrNode_S;
typedef shared_ptr<vector<EqGrNode_S>> EqGrNode_SS;

#endif //CSPADE_EQGRNODE_H
