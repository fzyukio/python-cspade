//
// Created by Yukio Fukuzawa on 8/12/18.
//

#include "EqGrNode.h"
#include "Array.h"
#include "FreqIt.h"


EqGrNode::EqGrNode(int sz, int num_class_) {
    num_class = num_class_;
    if (sz > 0) {
        theElements.reset(new Array(sz));
        stheElements.reset(new Array(sz));

        _set_sup = make_shared<vector<Array_S>>();
        _seq_sup = make_shared<vector<Array_S>>();

        _set_sup->reserve(num_class);
        _seq_sup->reserve(num_class);

        for (int i = 0; i < num_class; i++) {
            _set_sup->push_back(make_shared<Array>(sz));
            _seq_sup->push_back(make_shared<Array>(sz));
        }
    } else {
        theElements = nullptr;
        stheElements = nullptr;
        _set_sup->resize(0);
        _seq_sup->resize(0);
    }

    freqArray = nullptr;
    freqArraySz = 0;
    theFlg = 0;
}

EqGrNode::~EqGrNode() = default;


//assume that elements are sorted in descending order
int EqGrNode::bsearch(int min, int max, FreqIt_SS freqArray, FreqIt_S fit, int recursive) {
    int mid = (max + min) / 2;
    if (max < min) return -1;

    int res = freqArray->at(mid)->compare(fit, recursive);
    if (res == 0) return mid;
    else if (res < 0) return bsearch(min, mid - 1, freqArray, fit, recursive);
    else return bsearch(mid + 1, max, freqArray, fit, recursive);
}


int EqGrNode::bsearch(int min, int max, shared_ptr<vint> itary, int it) {
    int mid = (max + min) / 2;
    if (max < min) return -1;

    if (it == itary->at(mid)) return mid;
    else if (it < itary->at(mid)) return bsearch(min, mid - 1, itary, it);
    else return bsearch(mid + 1, max, itary, it);
}


int EqGrNode::find_freqarray(FreqIt_S fit, int recursive) {
    if (freqArraySz > 0)
        return bsearch(0, freqArraySz - 1, freqArray, fit, recursive);
    else return 0;
}


ostream &operator<<(ostream &outputStream, EqGrNode &EQ) {
    int i;
    if (EQ.theElements) {
        outputStream << "SET " << *EQ.theElements << endl;
        for (i = 0; i < EQ.num_class; i++)
            outputStream << "Sup" << i << " : " << *EQ._set_sup->at(i) << endl;
        outputStream << "Tot";
        for (i = 0; i < EQ.theElements->size(); i++)
            outputStream << " " << EQ.get_sup(i);
        outputStream << endl;
    }
    if (EQ.stheElements) {
        outputStream << "SEQ " << *EQ.stheElements << endl;
        for (i = 0; i < EQ.num_class; i++)
            outputStream << "SSup" << i << " : " << *EQ._seq_sup->at(i) << endl;
        outputStream << "Tot";
        for (i = 0; i < EQ.stheElements->size(); i++)
            outputStream << " " << EQ.get_seqsup(i);
        outputStream << endl;
    }

    return outputStream;
}
