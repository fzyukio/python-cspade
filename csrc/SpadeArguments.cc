//
// Created by Yukio Fukuzawa on 8/12/18.
//

#include "SpadeArguments.h"

void SpadeArguments::parse_args(int argc, char **argv) {
    string name;

    auto cmdl = argh::parser(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    cmdl("i") >> name;

    if (name.empty() || !cmdl("s")) {
        cerr << "usage: usage: spade [OPTION]... -i<infile> -s<support>\n";
        throw runtime_error("spade needs valid value of -i and -s");
    }
    cmdl("s") >> min_support_per_class;
    cmdl("a") >> use_ascending;
    cmdl("c") >> use_class;
    if (cmdl("e")) {
        ext_l2_pass = 1;
        cmdl("e") >> num_partitions;
    }
    if (cmdl["h"]) use_hash = 1;
    if (cmdl["o"]) outputfreq = 1;
    if (cmdl["r"]) recursive = 1;
    cmdl("l") >> min_gap;
    if (cmdl("u")) {
        use_maxgap = 1;
        use_hash = 0;
        cmdl("u") >> max_gap;
    }
    cmdl("t") >> pruning_type;
    cmdl("v") >> min_support;
    if (cmdl("w") || cmdl["w"]) {
        if (!cmdl("u")) {
            cerr << "-u is required when -w is enabled" << endl;
            throw runtime_error("-u is required when -w is enabled");
        }
        use_window = 1;
    }
    cmdl("y") >> print_tidlist;
    cmdl("z") >> max_seq_len;
    cmdl("Z") >> max_iset_len;


    dataf = name + ".tpose";
    idxf = name + ".idx";
    conf = name + ".conf";
    it2f = name + ".2it";
    seqf = name + ".2seq";
    classf = name + ".class";

    ifstream conff(conf, ios::binary);
    if (!conff.is_open()) {
        throw runtime_error("File " + string(conf) + " doesn\'t exist.");
    }

    conff.read((char *) &total_trans_count, INT_SIZE);
    if (min_support == -1)
        min_support = (int) ceil(min_support_per_class * total_trans_count);
    //ensure that support is at least 2
    if (min_support < 1) {
        min_support = 1;
    }

    conff.read((char *) &dbase_max_item, INT_SIZE);
    conff.read((char *) &avg_cust_size, FLOAT_SIZE);
    conff.read((char *) &avg_trans_count, FLOAT_SIZE);
    conff.read((char *) &dbase_total_trans, INT_SIZE);
    conff.close();
}