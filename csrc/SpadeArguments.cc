//
// Created by Yukio Fukuzawa on 8/12/18.
//

#include "SpadeArguments.h"

void SpadeArguments::parse_args(int argc, char **argv) {
    optind = 1;
    int c;
    string name;

    if (argc < 2) {
        cout << "usage: spade [OPTION]... -i<infile> -o<outfile> -s<support>\n";
        exit(EXIT_FAILURE);
    } else {
        while ((c = getopt(argc, argv, "a:bce:fhi:l:m:Mors:t:u:v:w:x:yz:Z:")) != -1) {
            switch (c) {
                case 'a':
                    //if val = -1 then do ascending generation
                    //else only generate the eqclass given by the value
                    use_ascending = atoi(optarg);
                    break;
                case 'b': // Not implemented
                    break;
                case 'c': //for classification
                    use_class = true;
                    break;
                case 'e': //calculate L2 from inverted dbase
                    num_partitions = atoi(optarg);
                    ext_l2_pass = 1;
                    break;
                case 'h': //use hashing to prune candidates
                    use_hash = 1;
                    break;
                case 'i': //input file
                    name = string(optarg);
                    dataf = name + ".tpose";
                    idxf = name + ".idx";
                    conf = name + ".conf";
                    it2f = name + ".2it";
                    seqf = name + ".2seq";
                    classf = name + ".class";
                    break;
                case 'l': //min-gap between items
                    min_gap = atoi(optarg);
                    break;
                case 'm': //amount of mem available -- not used
                    break;
                case 'M': //count multiple ocurrences per seq - not implemented
                    break;
                case 'o':
                    outputfreq = 1;
                    break;
                case 'r': //use recursive algorithm (doesn't work with subseq pruning)
                    recursive = 1;
                    break;
                case 's': //min support
                    min_support_per_class = atof(optarg);
                    break;
                case 't': //Kind of Pruning
                    pruning_type = atoi(optarg);
                    break;
                case 'u': //max-gap between items
                    max_gap = atoi(optarg);
                    use_maxgap = 1;
                    break;
                case 'v':
                    min_support = atoi(optarg);
                    break;
                case 'w': //max sequence window
                    use_window = 1;
                    max_gap = atoi(optarg); //re-use maxgap for window size
                    break;
                case 'y':
                    print_tidlist = 1;
                    break;
                case 'z': //length of sequence
                    max_seq_len = atoi(optarg);
                    break;
                case 'Z': // length of itemset
                    max_iset_len = atoi(optarg);
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

    if (use_maxgap) use_hash = 0;

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