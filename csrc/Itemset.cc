#include "Itemset.h"


Itemset::Itemset(int it_sz, int ival_sz, int nclass, bool print) {
    num_class = nclass;
    do_print = print;
    theItemset.reset(new Array(it_sz));
    theIval.reset(new Array(ival_sz));
    theSupport = 0;
    clsSup.resize(num_class, 0);
}

Itemset::~Itemset() = default;

int Itemset::compare(Itemset &ar2) {
    int len;
    if (size() <= ar2.size()) len = size();
    else len = ar2.size();
    for (int i = 0; i < len; i++) {
        if ((*theItemset)[i] > (*ar2.theItemset)[i]) return 1;
        else if ((*theItemset)[i] < (*ar2.theItemset)[i]) return -1;
    }
    if (size() < ar2.size()) return -1;
    else if (size() > ar2.size()) return 1;
    else return 0;
}

int Itemset::subsequence(Itemset_S ar) {
    int i, j;
    if (size() > ar->size()) return 0;
    int start = 0;
    for (i = 0; i < size(); i++) {
        for (j = start; j <ar->size(); j++) {
            if ((*theItemset)[i] == (*ar->theItemset)[j]) {
                start = j + 1;
                break;
            }
        }
        if (j >= ar->size()) return 0;
    }
    return 1;
}

ostream &operator<<(ostream &outputStream, Itemset &itemset) {
    outputStream << "ITEM: ";
    outputStream << *itemset.theItemset;
    outputStream << "(" << itemset.theSupport << ")";
    outputStream << "\n";
    return outputStream;
}

void Itemset::print_seq(ostream& seqstrm, int itempl) {
    int i;
    int sz = size();
    seqstrm << (*theItemset)[0] << " ";
    for (i = 1; i < sz - 1; i++) {
        if (GETBIT(itempl, sz - 1 - i))
            seqstrm << "-> ";
        seqstrm << (*theItemset)[i] << " ";
    }
    if (GETBIT(itempl, sz - 1 - i))
        seqstrm << "-> ";
    seqstrm << (*theItemset)[sz - 1] << " ";
    seqstrm << "-- " << theSupport;
    for (i = 0; i < num_class; i++)
        seqstrm << " " << clsSup[i];
    seqstrm << " ";
//    if (do_print) print_idlist();
    seqstrm << endl;
}

void Itemset::print_idlist(ostream& idlstrm) {
    int i, cid, cnt;

    if (theIval && theIval->size() > 0) {
        cid = (*theIval)[0];
        cnt = 0;
        for (i = 0; i <theIval->size();) {
            if (cid == (*theIval)[i]) {
                cnt++;
                i += 2;
            } else {
                idlstrm << cid << " " << cnt << " ";
                cid = (*theIval)[i];
                cnt = 0;
            }
        }
        idlstrm << cid << " " << cnt;
    }
}

Itemset_S placeholder = make_shared<Itemset>(true);