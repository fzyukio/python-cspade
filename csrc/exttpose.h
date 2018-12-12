//
// Created by Yukio Fukuzawa on 7/12/18.
//

#ifndef UTILITIES_EXTTPOSE_H
#define UTILITIES_EXTTPOSE_H

#include "common.h"
#include "Env.h"
#include "argv_parser.h"
#include "TransArray.h"
#include "calcdb.h"

#define MEG (1024*1204)


class ExttposeArgument {
public:
    string input;       //input file name
    string output;       //output file name
    string idxfn;
    string inconfn;
    string it2fn;
    string seqfn;
    string tmpfn;    // template for temporary files
    double MINSUP_PER = 0.0;
    long MINSUPPORT;
    int do_invert = 1;
    int do_l2 = 1;
    int use_seq = 1;
    int write_only_fcnt = 1;
    char use_newformat = 1;
    int num_partitions = 1;
    char no_minus_off = 0;

    char use_diff = 0;
    int DBASE_NUM_TRANS; //tot trans for assoc, num cust for sequences
    int DBASE_MAXITEM;   //number of items
    float DBASE_AVG_TRANS_SZ; //avg trans size
    float DBASE_AVG_CUST_SZ = 0; //avg cust size for sequences
    int DBASE_TOT_TRANS; //tot trans for sequences

    long AMEM = 32 * MEG;
    
    string name;
    void parse_args(int argc, char **argv) {
        auto cmdl = argh::parser(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
        if (!cmdl({"s", "i", "o"})) {
            cerr << "usage: exttpose [OPTION]... -i<infile> -o<outfile> -s\n";
            throw runtime_error("exttpose needs valid value of -i -o and -s");
        }
        cmdl("i") >> name;
        input = name + ".data";
        inconfn = name + ".conf";
        cmdl("o") >> name;
        output = name + ".tpose";
        idxfn = name + ".idx";
        it2fn = name + ".2it";
        seqfn = name + ".2seq";
        tmpfn = name + ".tmp";
        cmdl("p") >> num_partitions;
        cmdl("s") >> MINSUP_PER;
        if (cmdl("a")) {
            use_seq = 0;
            cmdl("a") >> write_only_fcnt;
        }
        if (cmdl["d"]) use_diff = 1;
        if (cmdl["l"]) do_l2 = 0;
        if (cmdl["v"]) do_invert = 0;
        if (cmdl["f"]) use_newformat = 0;
        if (cmdl("m")) {
            cmdl("m") >> AMEM;
            AMEM *= MEG;
        }
        if (cmdl["x"]) no_minus_off = 1;

        ifstream inconff(inconfn, ios::binary);
        if (!inconff) {
            throw runtime_error("ERROR: Can\'t read conf file: " + inconfn);
        }

        if (use_seq) {
            inconff.read((char *) &DBASE_NUM_TRANS, INT_SIZE);
            inconff.read((char *) &DBASE_MAXITEM, INT_SIZE);
            inconff.read((char *) &DBASE_AVG_CUST_SZ, sizeof(float));
            inconff.read((char *) &DBASE_AVG_TRANS_SZ, sizeof(float));
            inconff.read((char *) &DBASE_TOT_TRANS, INT_SIZE);
        } else {
            inconff.read((char *) &DBASE_NUM_TRANS, INT_SIZE);
            inconff.read((char *) &DBASE_MAXITEM, INT_SIZE);
            inconff.read((char *) &DBASE_AVG_TRANS_SZ, sizeof(float));
        }

        if (use_diff) {
            use_seq = 0;
            num_partitions = 1;
        }
        if (use_seq) {
            write_only_fcnt = 0;
        }
    }
};

class Exttpose {
    Env& env;
    ExttposeArgument& args;
    DbaseCtrlBlk dcb;
    unsigned long numfreq = 0;

    vector<int> itcnt;
    vector<int> ocnt;
    vector<int> itlen;
    vector<int> freqidx;
    vector<int> backidx;
    vector<TransArray> extary;
    vector<unsigned char> seq2;
    vector<unsigned char> itcnt2;
    vector<char> ocust;
    vector<int> offsets;
    vector<int> fidx;
public:
    void sort_get_l2(int &l2cnt, fstream& file, ofstream &ofd, vector<unsigned char> &cntary, bool use_seq);

    void process_cust(int fcnt, fstream &seqfd, fstream &isetfd);

    void do_invert_db(int pblk, int mincustid, int maxcustid);
    void tpose();

    Exttpose(Env& env_, ExttposeArgument& args_);
};

result_t exttposeFunc(Env &env, ExttposeArgument &args);

/**
 * Call exttpose given the argument list as string
 * @param args e.g. 'exttpose -i zaki -o zaki -p 1 -l -x -s 0.3'
 */
result_t exttposeWrapper(const string& args, shared_ptr<Env>& envptr);

#endif //UTILITIES_MAKEBIN_H
