//
// Created by Yukio Fukuzawa on 8/12/18.
//

#ifndef CSPADE_SPADE_ARGS_T_H
#define CSPADE_SPADE_ARGS_T_H

#include "common.h"
#include "argv_parser.h"

//join type
#define LJOIN 0
#define EJOIN 1
#define MJOIN 2

//pruning types
#define NOPRUNING 0            // no pruning
#define L2PRUNING 1
#define ZEROPRUNING 2     // when sup goes to zero in other classes, prune
#define FOLLOWPRUNING 4         //

class SpadeArguments {
public:
    int pruning_type = NOPRUNING;
    string dataf;
    string idxf;
    string conf;
    string it2f;
    string seqf;
    string classf;
    int ext_l2_pass = 0;
    int use_hash = 0;
    int num_intersect = 0;
    int recursive = 0;
    int maxiter = 2;
    int min_gap = 1;
    int max_gap = INT_MAX;
    char use_maxgap = 0;
    char use_window = 0;
    int use_ascending = -2;
    bool use_class = false;
    char outputfreq = 0;
    char print_tidlist = 0;

    int L2pruning = 0;
    int prepruning = 0;
    int postpruning = 0;

    int max_seq_len = 100;
    int max_iset_len = 100;

    int total_trans_count;
    int dbase_max_item;
    float avg_trans_count;
    float avg_cust_size;
    int dbase_total_trans;

    double min_support_per_class;
    float follow_thresh = 1.0;
    float zero_thresh = 0.0;
    int min_support = -1;

    int num_partitions;

    void parse_args(int argc, char **argv);
};


#endif //CSPADE_SPADE_ARGS_T_H
