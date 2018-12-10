//
// Created by Yukio Fukuzawa on 8/12/18.
//

#include "FreqIt.h"

int FreqIt::compare(shared_ptr<Itemset> fit, unsigned int itpl) {
    int i;

    //first compare seqsz, one with larger seqsz is smaller
    if (seqsz > fit->size()) return -1;
    else if (seqsz < fit->size()) return 1;

    int *seq_data = seq->data();

    //compare items & template bits
    if (seq_data[0] < (*fit)[0]) return -1;
    else if (seq_data[0] > (*fit)[0]) return 1;

    int bpos = seqsz - 1;
    int b1, b2;
    for (i = 1; i < seqsz; i++) {
        b1 = GETBIT(templ, bpos - i);
        b2 = GETBIT(itpl, bpos - i);
        if (b1 < b2) return -1;
        else if (b1 > b2) return 1;

        if (seq_data[i] < (*fit)[i]) return -1;
        else if (seq_data[i] > (*fit)[i]) return 1;
    }
    return 0;
}


int FreqIt::compare(FreqIt_S fit, int recursive) {
    int i;
    int *seq_data = seq->data();
    int *fit_data = fit->seq->data();

    //first compare seqsz, one with larger seqsz is smaller
    if (seqsz > fit->seqsz) return -1;
    else if (seqsz < fit->seqsz) return 1;

    //compare items & template bits
    if (seq_data[seqsz - 1] < fit_data[fit->seqsz - 1]) return -1;
    else if (seq_data[seqsz - 1] > fit_data[fit->seqsz - 1]) return 1;

    int bpos = 0;
    int b1, b2;
    for (i = seqsz - 2; i >= 0; i--, bpos++) {
        b1 = GETBIT(templ, bpos);
        b2 = GETBIT(fit->templ, bpos);
        if (b1 < b2) return -1;
        else if (b1 > b2) return 1;

        if (seq_data[i] < fit_data[i]) return -1;
        else if (seq_data[i] > fit_data[i]) return 1;
    }
    return 0;
}

ostream &operator<<(ostream &outputStream, FreqIt_S freq) {
    int *freq_data = freq->seq->data();

    outputStream << "FREQ : ";
    for (int i = 0; i < freq->seqsz; i++)
        outputStream << " " << freq_data[i];
    outputStream << " --- " << freq->templ << endl;
    return outputStream;
}

int FreqIt::size() {
    return seqsz;
}

FreqIt::FreqIt(int sz, unsigned int tpl) {
    templ = tpl;
    seqsz = sz;
    seq = make_shared<vint>(sz);
}

FreqIt::FreqIt(shared_ptr<vint> ary, int sz, unsigned int tpl) {
    templ = tpl;
    seqsz = sz;
    seq = make_shared<vint>();
    seq->reserve(sz);
    auto ary_data = ary->data();
    for (int i = 0; i < sz; i++) {
        seq->push_back(ary_data[i]);
    }
}