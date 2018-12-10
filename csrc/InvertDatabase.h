#ifndef __EXT_H_
#define __EXT_H_

#include "common.h"
#include "Partition.h"
#include "Eqclass.h"
#include "EqGrNode.h"
#include "ClassInfo.h"
#include "Env.h"

class InvertDatabase {
    int numcust;
    vvint curits;
    vint curcnts;
    vint curcids;
    vint curitszs;
    Env& env;
    shared_ptr<ClassInfo> cls;
    shared_ptr<Partition> partition;
    shared_ptr<SpadeArguments> args;
    vector<EqGrNode_S> eqgraph;
    vint backidx;
    vint fidx;
    int numfreq;
    vector<Array> itsup;
    vector<shared_ptr<Itemset>> _items;
    vector<vector<vector<unsigned int>>> set_sup, seq_sup;
public:
    InvertDatabase(Env& env);

    const EqGrNode_S &get_eqgraph_item(int i) {
        return eqgraph[i];
    }

    void set_cls(const shared_ptr<ClassInfo> &cls);

    void setPartition(const shared_ptr<Partition> &partition);

    void setArgs(const shared_ptr<SpadeArguments> &args);

    void init(int sz);

    void incr(int sz);

    void incr_curit(int midx);

    void process_invert(int pnum);

    void process_cust_invert(int custidx);

    int make_l1_pass();

    int make_l2_pass();

    void get_l2file(const string &fname, char use_seq, int &l2cnt);

    bool extl2_pre_pruning(int totsup, int it, int pit, char use_seq, vuint& clsup);

    void suffix_add_item_eqgraph(char use_seq, int it1, int it2);

    void get_F2(int &l2cnt);

    void print_idlist(ostream& idlstrm, shared_ptr<vint> ival, int supsz);

    void add_sup(int sup, int cls);

    int get_sup(int it, int cls = -1);

    int in_mem(int it) {
        if (_items[fidx[it]]->ival()->array() == nullptr) return 0;
        else return 1;
    }

    Itemset_S get_item(int it) {
        if (!in_mem(it)) get_ext_item(it);
        return _items[fidx[it]];
    }

    void get_ext_item(int it) {
        int supsz = partition->partition_get_idxsup(it);
        shared_ptr<vint> newit = make_shared<vint>(supsz);
        partition->partition_read_item(newit, it);
        _items[fidx[it]]->set_support(supsz);
        _items[fidx[it]]->ival()->set_size(supsz);
        _items[fidx[it]]->ival()->set_array(newit);
    }

    void init_buffer(int num_class, int size) {
        _items.reserve(size);
        bool print_idlist = args->print_tidlist;

        for (int i = 0; i < size; i++) {
            Itemset_S tmp = make_shared<Itemset>(1, 0, num_class, print_idlist);
            tmp->setitem(0, backidx[i]);
            _items.push_back(tmp);
        }
    }

};

#endif //__EXT_H_
