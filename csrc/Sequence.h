//
// Created by Yukio Fukuzawa on 7/12/18.
//

#ifndef CSPADE_SEQUENCE_H
#define CSPADE_SEQUENCE_H

#include "common.h"
#include "SpadeArguments.h"
#include "argv_parser.h"
#include "Partition.h"
#include "InvertDatabase.h"
#include "FreqIt.h"

result_t sequenceFunc(Env& env, const shared_ptr<SpadeArguments>& args);

/**
 * Call spade given the argument list as string
 * @param args e.g 'spade -i zaki -s 0.3 -Z 10 -z 10 -u 1 -r -e 1 -o'
 */
result_t sequenceWrapper(const string &s, shared_ptr<Env>& envptr);

class Sequence {
private:
    Env& env;
    shared_ptr<SpadeArguments> args;
    shared_ptr<Partition> partition;
    shared_ptr<ClassInfo> cls;
    shared_ptr<InvertDatabase> invdb;

    Array_S l_array, e_array, m_array;
    FreqIt_SS FreqArray;
    unsigned long FreqArraySz = 100;
    int FreqArrayPos = 0;
    shared_ptr<vint> numLargeItemset;
public:

    explicit Sequence(Env& env_);

    void set_args(const shared_ptr<SpadeArguments> &args);

    void set_partition(const shared_ptr<Partition> &partition);

    void set_cls(const shared_ptr<ClassInfo> &cls);

    void set_num_large_dataset(const shared_ptr<vint> &numLargeItemset);

    void set_invdb(const shared_ptr<InvertDatabase> &invdb);

    void read_files();

    int get_file_l2() {
        int l2cnt = 0;

        if (args->max_iset_len > 1) {
            invdb->get_l2file(args->it2f, 0, l2cnt);
        }
        if (args->max_seq_len > 1) {
            invdb->get_l2file(args->seqf, 1, l2cnt);
        }

        cerr << "L2 : " << l2cnt << endl;
        return l2cnt;
    }

    void get_tmpnewf_intersect(Itemset_S &ljoin, Itemset_S &ejoin, Itemset_S &mjoin,
                               int &lcnt, int &ecnt, int &mcnt,
                               Itemset_S& it1, Itemset_S& it2, int iter);

    void make_itemset(Itemset_S& it, Array_S& ary, int cnt, const vint &clscnt);

    void pre_pruning(Itemset_S &join, unsigned int ptempl, Itemset_S& clas, Itemset_S& prefix, char use_seq);

    void post_pruning(Itemset_S &iset, unsigned int templ);

    void newSeq();

    void process_class(int it);

    Eqclass_S get_ext_eqclass(int it);

    void get_2newf_intersect(Itemset_S& ljoin, Itemset_S& ejoin, shared_ptr<vint> vit1, shared_ptr<vint> vit2, int sup1,
                             int sup2);

    void add_freq(Itemset_S &it, int templ);

    Itemset_S prune_decision(Itemset_S& it1, Itemset_S& it2, unsigned int ptempl, int jflg);

    void find_large(Eqclass_S cluster, int it);

    void insert_freqarray(shared_ptr<Lists<Eqclass_S>>& LargeL);

    int get_valid_el(int it, vector<char> &ibvec, vector<char> &sbvec);

    void process_itemset(Itemset_S iset, unsigned int templ, int iter);

    void process_maxgap(Eqclass_S L2);

    void process_cluster1(Eqclass_S cluster, shared_ptr<Lists<Eqclass_S>> LargeL, int iter);

    void process_cluster_list1(shared_ptr<ListNodes<shared_ptr<Itemset>>>& hdr1,
                               shared_ptr<Lists<shared_ptr<Itemset>>>& cluster1,
                               shared_ptr<Lists<shared_ptr<Itemset>>>& cluster2,
                               shared_ptr<Lists<Eqclass_S>>& LargeL,
                               int iter, int eqtype, Eqclass_S& parent);

    void process_cluster_list2(shared_ptr<ListNodes<shared_ptr<Itemset>>>& hdr1, int i, Eqclass_SS& EQ,
                               shared_ptr<Lists<shared_ptr<Itemset>>>& cluster,
                               shared_ptr<Lists<Eqclass_S>>& LargeL,
                               int iter, int eqtype, Eqclass_S& parent);

    void fill_join(Itemset_S& join, Itemset_S& hdr1, Itemset_S& hdr2);
};

#endif //CSPADE_SEQUENCE_H
