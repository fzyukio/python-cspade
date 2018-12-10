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

typedef unsigned char CHAR;
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

    double tpose_time, offt_time, l2time;

    void parse_args(int argc, char **argv) {
        optind = 1;
        int c;

        if (argc < 2) {
            cerr << "usage: exttpose [OPTION]... -i<infile> -o<outfile>\n";
            exit(EXIT_FAILURE);
        } else {
            while ((c = getopt(argc, argv, "i:o:p:s:a:dvlfm:x")) != -1) {
                switch (c) {
                    case 'i': //input files
                        input = string(optarg) + ".data";
                        inconfn = string(optarg) + ".conf";
                        break;
                    case 'o': //output file names
                        output = string(optarg) + ".tpose";
                        idxfn = string(optarg) + ".idx";
                        it2fn = string(optarg) + ".2it";
                        seqfn = string(optarg) + ".2seq";
                        tmpfn = string(optarg) + ".tmp";
                        break;
                    case 'p': //number of partitions for inverted dbase
                        num_partitions = atoi(optarg);
                        break;
                    case 's': //support value for L2
                        MINSUP_PER = atof(optarg);
                        break;
                    case 'a': //turn off 2-SEQ calclation
                        use_seq = 0;
                        write_only_fcnt = atoi(optarg);
                        break;
                    case 'd': //output diff lists, i.e. U - tidlist, only with assoc
                        use_diff = 1;
                        break;
                    case 'l': //turn off L2 calculation
                        do_l2 = 0;
                        break;
                    case 'v': //turn of dbase inversion
                        do_invert = 0;
                        break;
                    case 'f':
                        use_newformat = 0;
                        break;
                    case 'm':
                        AMEM = atoi(optarg) * MEG;
                        break;
                    case 'x':
                        no_minus_off = 1;
                        break;
                    case '?':
                    default:
                        ostringstream message;
                        message << "Illegal option: " << char(optopt) << ". Full argv: \"";
                        for (int i = 0; i < argc - 1; i++) {
                            message << argv[i] << ' ';
                        }
                        message << argv[argc - 1] << '\"';
                        throw runtime_error(message.str());
                }
            }
        }

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
    vector<CHAR> seq2;
    vector<CHAR> itcnt2;
    vector<char> ocust;
    vector<int> offsets;
    vector<int> fidx;
public:
    void sort_get_l2(int &l2cnt, fstream& file, ofstream &ofd, vector<CHAR> &cntary, bool use_seq);

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
