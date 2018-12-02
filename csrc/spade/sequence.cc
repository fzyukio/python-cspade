#include <errno.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fcntl.h>
#include <sys/stat.h>
//#include <sys/mode.h>
#include <unistd.h>
#include <sys/time.h>
#include <ctime>
#include <sys/types.h>
#include <cstring>
#include <sys/mman.h>
//#include <malloc.h>
#include <cstring>
#include "../utils.h"
#include "Eqclass.h"
#include "Itemset.h"
#include "Lists.h"
#include "extl2.h"
#include "partition.h"
#include "spade.h"
#include "maxgap.h"
#include "../funcs.h"

using namespace std;

int pruning_type = NOPRUNING;

long MEMUSED = 0;
long AVAILMEM = 128 * MBYTE;
char dataf[300];
char idxf[300];
char conf[300];
char it2f[300];
char seqf[300];
char classf[300];

double L2ISECTTIME = 0, EXTL1TIME = 0, EXTL2TIME = 0;

int *NumLargeItemset;
int maxitemsup;
Itemset *item1, *item2; // for use in reading external dbase
ArraySpade *interval, *interval2, *interval3;
int maxeqsize = 1;
EqGrNode **eqgraph;
int ext_l2_pass = 0;
int use_hash = 0;
int num_intersect = 0;
int recursive = 0;
int maxiter = 2;
int min_gap = 1;
int max_gap = 2147483647;
char use_maxgap = 0;
char use_window = 0;
char memtrace = 0;
char use_newformat = 1;
int use_ascending = -2;
bool count_multiple = false;
char use_isetonly = 0;
char use_class = 0;
char outputfreq = 0;
char print_tidlist = 0;

int L2pruning = 0;
int prepruning = 0;
int postpruning = 0;

int max_seq_len = 100;
int max_iset_len = 100;

int DBASE_NUM_TRANS;
int DBASE_MAXITEM;
float DBASE_AVG_TRANS_SZ;
float DBASE_AVG_CUST_SZ;
int DBASE_TOT_TRANS;

double MINSUP_PER;
int MINSUPPORT = -1;
float FOLLOWTHRESH = 1.0;
float ZEROTHRESH = 0.0;
int FreqArraySz = 100;
FreqIt **FreqArray;
int FreqArrayPos = 0;

void process_cluster1(Eqclass *cluster, Lists<Eqclass *> *LargeL, int iter);

void add_freq(Itemset *it, int templ) {
    FreqIt *freq = new FreqIt(it->itemset()->arraySpade(), it->size(), templ);
    if (FreqArrayPos + 1 >= FreqArraySz) {
        FreqArraySz = (int) (1.5 * FreqArraySz);
        FreqArray = (FreqIt **) realloc(FreqArray, FreqArraySz * sizeof(FreqIt *));
        if (FreqArray == NULL) {
            throw runtime_error("no mmeory fro FREqArray ");
        }
    }
    FreqArray[FreqArrayPos++] = freq;
}

void print_freqary() {
    int j = 0;
    cout << "FREQARRAY " << FreqArrayPos << ":" << endl;
    for (j = 0; j < FreqArrayPos; j++) {
        cout << *FreqArray[j];
    }
    cout << "!!!!!!!!!!!!!!!!!!!!" << endl;
}

void parse_args(int argc, char **argv) {
    extern char *optarg;
    char *options, *value;
    extern int optind;

    char c;
    optind = 1;

    if (argc < 2)
        cout << "usage: seq -i<infile> -o<outfile> -s<support>\n";
    else {
        while ((c = getopt(argc, argv, "a:bce:fhi:l:m:Mors:t:u:v:w:x:yz:Z:")) != -1) {
            switch (c) {
                case 'a':
                    //if val = -1 then do ascending generation
                    //else only generate the eqclass given by the value
                    use_ascending = atoi(optarg);
                    break;
                case 'b':
                    use_isetonly = 1;
                    break;
                case 'c': //for classification
                    use_class = 1;
                    break;
                case 'e': //calculate L2 from inverted dbase
                    num_partitions = atoi(optarg);
                    ext_l2_pass = 1;
                    break;
                case 'h': //use hashing to prune candidates
                    use_hash = 1;
                    break;
                case 'i': //input file
                    sprintf(dataf, "%s.tpose", optarg);
                    sprintf(idxf, "%s.idx", optarg);
                    sprintf(conf, "%s.conf", optarg);
                    sprintf(it2f, "%s.2it", optarg);
                    sprintf(seqf, "%s.2seq", optarg);
                    sprintf(classf, "%s.class", optarg);
                    break;
                case 'l': //min-gap between items
                    min_gap = atoi(optarg);
                    break;
                case 'm': //amount of mem available
                    AVAILMEM = (long) atof(optarg) * MBYTE;
                    break;
                case 'M': //count multiple ocurrences per seq
                    count_multiple = true;
                    break;
                case 'o':
                    outputfreq = 1;
                    break;
                case 'r': //use recursive algorithm (doesn't work with subseq pruning)
                    recursive = 1;
                    break;
                case 's': //min support
                    MINSUP_PER = atof(optarg);
                    break;
                case 't': //Kind of Pruning
                    pruning_type = atoi(optarg);
                    break;
                case 'u': //max-gap between items
                    max_gap = atoi(optarg);
                    use_maxgap = 1;
                    break;
                case 'v':
                    MINSUPPORT = atoi(optarg);
                    break;
                case 'w': //max sequence window
                    use_window = 1;
                    max_gap = atoi(optarg); //re-use maxgap for window size
                    break;
                case 'x':
                    memtrace = 1;
                    break;
                case 'y':
                    print_tidlist = 1;
                    break;
                    //case 'z': //length of itemset or sequence
                    //options = optarg;
                    //while (*options != '\0') {
                    //   switch(getsubopt(&options,lenopts,&value)) {
                    //   case SEQ :
                    //      max_seq_len = atoi(value);
                    //      break;
                    //   case ISET :
                    //      max_iset_len = atoi(value);
                    //      break;
                    //   }
                    //}
                    //break;
                case 'Z': //length of sequence
                    max_seq_len = atoi(optarg);
                    break;
                case 'z': // length of itemset
                    max_iset_len = atoi(optarg);
                    break;
            }
        }
    }

    if (use_maxgap) use_hash = 0;

    c = open(conf, O_RDONLY);
    if (c < 0) {
        throw runtime_error("ERROR: invalid conf file\n");
    }
    read(c, (char *) &DBASE_NUM_TRANS, ITSZ);
    if (MINSUPPORT == -1)
        MINSUPPORT = (int) (MINSUP_PER * DBASE_NUM_TRANS + 0.5);
    //ensure that support is at least 2
    if (MINSUPPORT < 1) MINSUPPORT = 1;
    logger << "MINSUPPORT " << MINSUPPORT << " out of " << DBASE_NUM_TRANS << " sequences" << endl;
    read(c, (char *) &DBASE_MAXITEM, ITSZ);
    read(c, (char *) &DBASE_AVG_CUST_SZ, sizeof(float));
    read(c, (char *) &DBASE_AVG_TRANS_SZ, sizeof(float));
    read(c, (char *) &DBASE_TOT_TRANS, ITSZ);
    //cout << "CONF " << DBASE_NUM_TRANS << " " << DBASE_MAXITEM << " "
    //     << DBASE_AVG_CUST_SZ << " " << DBASE_AVG_TRANS_SZ << " "
    //     << DBASE_TOT_TRANS << endl;
    close(c);
}


int choose(int n, int k) {
    int i;
    int val = 1;

    if (k >= 0 && k <= n) {
        for (i = n; i > n - k; i--)
            val *= i;
        for (i = 2; i <= k; i++)
            val /= i;
    }

    return val;
}


void get_2newf_intersect(Itemset *ljoin, Itemset *ejoin,
                         int *it1, int *it2, int sup1, int sup2) {
    int i, j, k, l;
    int nval1, nval2, diff;
    int lflge;

    num_intersect++;

    int icid, jcid;
    for (i = 0, j = 0; i < sup1 && j < sup2;) {
        icid = it1[i];
        jcid = it2[j];
        if (icid > jcid) {
            j += 2;
        } else if (icid < jcid) {
            i += 2;
        } else {
            nval1 = i;
            nval2 = j;
            while (it1[i] == it1[nval1] && nval1 < sup1) nval1 += 2;
            while (it2[j] == it2[nval2] && nval2 < sup2) nval2 += 2;
            if (ljoin && it1[i + 1] + min_gap <= it2[nval2 - 1]) {
                //add tid
                lflge = 0;
                for (k = i, l = j; k < nval1 && l < nval2;) {
                    diff = it2[l + 1] - it1[k + 1];
                    if (diff < min_gap) l += 2;
                    else if (diff > max_gap) k += 2;
                    else {
                        ljoin->ival()->optadd(icid);
                        ljoin->ival()->optadd(it1[k + 1]);
                        lflge = 1;
                        k += 2;
                    }
                }
                if (lflge) {
                    ljoin->increment_support();
                    ljoin->increment_cls_support(ClassInfo::getcls(icid));
                }
            }

            if (ejoin) {
                lflge = 0;
                for (k = i, l = j; k < nval1 && l < nval2;) {
                    if (it1[k + 1] < it2[l + 1]) k += 2;
                    else if (it1[k + 1] > it2[l + 1]) l += 2;
                    else {
                        ejoin->ival()->optadd(icid);
                        ejoin->ival()->optadd(it2[l + 1]);
                        lflge = 1;
                        k += 2;
                        l += 2;
                    }
                }
                if (lflge) {
                    ejoin->increment_support();
                    ejoin->increment_cls_support(ClassInfo::getcls(icid));
                }
            }
            i = nval1;
            j = nval2;
        }
    }
}

int interval_comp(int a, int b, int c, int d) {
    if (a < c) return -1;
    else if (a > c) return 1;
    else {
        if (b < d) return -1;
        else if (b > d) return 1;
        else return 0;
    }
}

void make_itemset(Itemset *it, ArraySpade *ary, int cnt, int *clscnt) {
    int i;
    for (i = 0; i < ary->size(); i++) {
        it->ival()->optadd((*ary)[i]);
    }
    it->set_support(cnt);
    for (i = 0; i < NUMCLASS; i++) it->set_cls_support(clscnt[i], i);
}

void get_tmpnewf_intersect(Itemset *&ljoin, Itemset *&ejoin, Itemset *&mjoin,
                           int &lcnt, int &ecnt, int &mcnt,
                           Itemset *it1, Itemset *it2, int iter) {
    int i, j, k, l;
    int nval1, nval2, diff;
    int lflge;
    ArraySpade *lary, *eary, *mary;

    num_intersect++;

    lary = interval;
    eary = interval2;
    mary = interval3;
    lary->reset();
    eary->reset();
    mary->reset();

    lcnt = ecnt = mcnt = 0;
    for (i = 0; i < NUMCLASS; i++) {
        ClassInfo::TMPL[i] = 0;
        ClassInfo::TMPE[i] = 0;
        ClassInfo::TMPM[i] = 0;
    }

    int dc1 = it1->support() - MINSUPPORT;
    int dc2 = it2->support() - MINSUPPORT;
    int df1 = 0;
    int df2 = 0;
    int icid, jcid;
    for (i = 0, j = 0; i < it1->ivalsize() && j < it2->ivalsize();) {
        if (df1 > dc1 || df2 > dc2) break;
        icid = it1->ival(i);
        jcid = it2->ival(j);
        if (icid > jcid) {
            //df must be incremented only once per customer
            while (jcid == it2->ival(j) && j < it2->ivalsize()) j += 2;
            df2++;
        } else if (icid < jcid) {
            while (icid == it1->ival(i) && i < it1->ivalsize()) i += 2;
            df1++;
        } else {
            nval1 = i;
            nval2 = j;
            while (it1->ival(i) == it1->ival(nval1) && nval1 < it1->ivalsize())
                nval1 += 2;
            while (it2->ival(j) == it2->ival(nval2) && nval2 < it2->ivalsize())
                nval2 += 2;

            if (ljoin && it1->ival(i + 1) + min_gap <= it2->ival(nval2 - 1)) {
                lflge = 0;
                for (k = i, l = j; k < nval1 && l < nval2;) {
                    diff = it2->ival(l + 1) - it1->ival(k + 1);
                    if (diff < min_gap) l += 2;
                    else if (diff > max_gap) k += 2;
                    else {
                        lary->optadd(icid);
                        lary->optadd(it1->ival(k + 1));
                        lflge = 1;
                        k += 2;
                    }
                }
                if (lflge) {
                    lcnt++;
                    ClassInfo::TMPL[ClassInfo::getcls(icid)]++;
                }
            }

            if (ejoin) {
                lflge = 0;
                for (k = i, l = j; k < nval1 && l < nval2;) {
                    if (it1->ival(k + 1) < it2->ival(l + 1)) k += 2;
                    else if (it1->ival(k + 1) > it2->ival(l + 1)) l += 2;
                    else {
                        eary->optadd(icid);
                        eary->optadd(it2->ival(l + 1));
                        lflge = 1;
                        k += 2;
                        l += 2;
                    }
                }
                if (lflge) {
                    ecnt++;
                    ClassInfo::TMPE[ClassInfo::getcls(icid)]++;
                }
            }

            if (mjoin && it2->ival(j + 1) + min_gap <= it1->ival(nval1 - 1)) {
                lflge = 0;
                for (k = i, l = j; k < nval1 && l < nval2;) {
                    diff = it1->ival(k + 1) - it2->ival(l + 1);
                    if (diff < min_gap) k += 2;
                    else if (diff > max_gap) l += 2;
                    else {
                        mary->optadd(icid);
                        mary->optadd(it2->ival(l + 1));
                        lflge = 1;
                        l += 2;
                    }
                }
                if (lflge) {
                    mcnt++;
                    ClassInfo::TMPM[ClassInfo::getcls(icid)]++;
                }
            }
            i = nval1;
            j = nval2;
        }
    }
    if (ljoin) {
        ljoin = NULL;
        for (i = 0; i < NUMCLASS; i++) {
            if (ClassInfo::TMPL[i] >= ClassInfo::MINSUP[i]) {
                ljoin = new Itemset(iter, lary->size());
                make_itemset(ljoin, lary, lcnt, ClassInfo::TMPL);
                break;
            }
        }
    }
    if (ejoin) {
        ejoin = NULL;
        for (i = 0; i < NUMCLASS; i++) {
            if (ClassInfo::TMPE[i] >= ClassInfo::MINSUP[i]) {
                ejoin = new Itemset(iter, eary->size());
                make_itemset(ejoin, eary, ecnt, ClassInfo::TMPE);
                break;
            }
        }
    }
    if (mjoin) {
        mjoin = NULL;
        for (i = 0; i < NUMCLASS; i++) {
            if (ClassInfo::TMPM[i] >= ClassInfo::MINSUP[i]) {
                mjoin = new Itemset(iter, mary->size());
                make_itemset(mjoin, mary, mcnt, ClassInfo::TMPM);
                break;
            }
        }
    }
}

void pre_pruning(Itemset *&join, unsigned int ptempl,
                 Itemset *clas, Itemset *prefix, char use_seq) {
    float conf, conf2;
    int i, res, cit, pit;
    if (join == NULL) return;
    pit = (*prefix)[0];
    int bitval = 0;
    int nsz = clas->size() - 2;
    if (GETBIT(pruning_type, FOLLOWPRUNING - 1)) {

        //cout << "FOLLOW " << pit << (use_seq?" -1 ":" ");
        //clas->print_seq(SETBIT(ptempl,1,nsz+1));

        for (i = 0; i <= nsz + 1 && !bitval; i++) {
            cit = (*clas)[i];
            //cout << cit << " " << pit << " FOLLOW " << *clas;
            if (use_seq) {
                return; //TURN OFF FOR SEQUENCES

                res = eqgraph[cit]->seqfind(pit);
                if (res != -1) {
                    conf = (eqgraph[cit]->get_seqsup(res) * 1.0) / F1::get_sup(cit);
                    //cout << "RES " << conf << " " << pit << " -1 " << cit << " "
                    //     << res << " "<< eqgraph[cit]->get_seqsup(res) << " "
                    //     << F1::get_sup(cit) << endl;
                    if (conf >= FOLLOWTHRESH) {
                        //cout << "PRUNE_PRE " << ptempl << " : " << *join;
                        if (outputfreq) {
                            cout << "PRUNE_PRE " << pit << " -1 ";
                            clas->print_seq(SETBIT(ptempl, 1, nsz + 1));
                        }
                        prepruning++;
                        join = NULL;
                        break;
                    }
                }
            } else {
                res = eqgraph[cit]->find(pit);
                if (res != -1) {
                    conf = (eqgraph[cit]->get_sup(res) * 1.0) / F1::get_sup(cit);
                    conf2 = (eqgraph[cit]->get_sup(res) * 1.0) / F1::get_sup(pit);
                    //cout << "RES " << conf << " " << pit << " " << cit << " "
                    //     << res << " " << eqgraph[cit]->get_seqsup(res) << " "
                    //     << F1::get_sup(cit) << endl;
                    if (conf >= FOLLOWTHRESH || conf2 >= FOLLOWTHRESH) {
                        if (outputfreq) {
                            cout << "PRUNE_PRE " << pit << " ";
                            clas->print_seq(SETBIT(ptempl, 1, nsz + 1));
                        }
                        //cout << "PRUNE_PRE " << ptempl << " : " << *join;
                        //cout << "PRUNE " << pit << " " << cit << " " << conf << endl;
                        prepruning++;
                        join = NULL;
                        break;
                    }
                }
            }

            if (nsz - i >= 0) bitval = GETBIT(ptempl, nsz - i);
        }
    }
    //if (join == NULL){
    //   cout << "PRUNEXX_PRE " << conf << " " << *join;
    //}

}

void post_pruning(Itemset *&iset, unsigned int templ) {
    int i;
    int remsup;
    float remdb;
    if (iset == NULL || NUMCLASS <= 1) return;

    if (GETBIT(pruning_type, ZEROPRUNING - 1)) {
        for (i = 0; i < NUMCLASS; i++) {
            remsup = iset->support() - iset->cls_support(i);
            remdb = ClassInfo::getcnt() - ClassInfo::getcnt(i);
            if (remsup / remdb <= ZEROTHRESH) {
                if (outputfreq) {
                    cout << "PRUNE_POST ";
                    iset->print_seq(templ);
                }
                postpruning++;
                //NumLargeItemset[iset->size()-1]++;
                delete iset;
                iset = NULL;
                break;
            }
        }
    }
}


void fill_seq_template(Eqclass *EQ, Eqclass *parent, int LR) {
    if (LR == 1) {
        EQ->set_templ(SETBIT(parent->templ(), 1, EQ->templ_sz() - 1));
        EQ->set_templ2(parent->templ());
    } else if (LR == 2) {
        EQ->set_templ(SETBIT(parent->templ2(), 1, EQ->templ_sz() - 1));
        EQ->set_templ2(parent->templ2());
    }
    //cout << "TEMPL " << parent->templ() << " " << parent->templ2()
    //     << " " << LR << " " << EQ->templ() << " " << EQ->templ2() << endl;
}

int get_valid_el(int it, char *ibvec, char *sbvec) {
    int i, j;
    int i1, i2;
    int rval = 0;

    if (pruning_type == NOPRUNING) {
        for (i = 0; i < eqgraph[it]->seqnum_elements(); i++) sbvec[i] = 1;
        for (i = 0; i < eqgraph[it]->num_elements(); i++) ibvec[i] = 1;
        rval = 1;
        return rval;
    }

    for (i = 0; i < eqgraph[it]->seqnum_elements(); i++) {
        sbvec[i] = 0;
    }
    for (i = 0; i < eqgraph[it]->num_elements(); i++) {
        ibvec[i] = 0;
    }

    for (i = 0; i < eqgraph[it]->seqnum_elements(); i++) {
        i1 = eqgraph[it]->seqget_element(i);
        for (j = i; j < eqgraph[it]->seqnum_elements(); j++) {
            i2 = eqgraph[it]->seqget_element(j);
            if (eqgraph[i2] && eqgraph[i2]->seqfind(i1) != -1) {
                sbvec[i] = 1;
                sbvec[j] = 1;
                rval = 1;
            }
            if (j > i) {
                if ((eqgraph[i2] && eqgraph[i2]->find(i1) != -1) ||
                    (eqgraph[i1] && eqgraph[i1]->seqfind(i2) != -1)) {
                    sbvec[i] = 1;
                    sbvec[j] = 1;
                    rval = 1;
                }
            }
        }
    }


    for (i = 0; i < eqgraph[it]->num_elements(); i++) {
        i1 = eqgraph[it]->get_element(i);
        for (j = i + 1; j < eqgraph[it]->num_elements(); j++) {
            i2 = eqgraph[it]->get_element(j);
            if (eqgraph[i2] && eqgraph[i2]->find(i1) != -1) {
                ibvec[i] = 1;
                ibvec[j] = 1;
                rval = 1;
            }
        }
        for (j = 0; j < eqgraph[it]->seqnum_elements(); j++) {
            i2 = eqgraph[it]->seqget_element(j);
            if (eqgraph[i1] && eqgraph[i1]->seqfind(i2) != -1) {
                ibvec[i] = 1;
                sbvec[j] = 1;
                rval = 1;
            }
        }
    }

    for (i = 0; i < eqgraph[it]->seqnum_elements(); i++)
        if (!sbvec[i]) {
            L2pruning++;
            if (outputfreq) {
                cout << "PRUNE_L2 " << it << " -1 " << eqgraph[it]->seqget_element(i)
                     << " " << eqgraph[it]->get_seqsup(i) << endl;
            }
        }

    for (i = 0; i < eqgraph[it]->num_elements(); i++)
        if (!ibvec[i]) {
            L2pruning++;
            if (outputfreq) {
                cout << "PRUNE_L2 " << it << " " << eqgraph[it]->get_element(i)
                     << " " << eqgraph[it]->get_sup(i) << endl;
            }
        }
    return rval;
}

//construct the next set of eqclasses from external disk
Eqclass *get_ext_eqclass(int it) {
    double t1, t2;
    seconds(t1);
    //cout << "MEMEXT " << it << " " << MEMUSED << endl;
    int i, k, it2, supsz, supsz2;
    Itemset *ljoin = NULL;
    Itemset *ejoin = NULL;

    char *ibvec, *sbvec;
    if (!use_maxgap) {
        ibvec = sbvec = NULL;
        if (eqgraph[it]->num_elements() > 0)
            ibvec = new char[eqgraph[it]->num_elements()];
        if (eqgraph[it]->seqnum_elements() > 0)
            sbvec = new char[eqgraph[it]->seqnum_elements()];

        if (!get_valid_el(it, ibvec, sbvec)) return NULL;
    }

    Eqclass *L2 = new Eqclass(1, EQCTYP1);
    if (L2 == NULL) {
        throw runtime_error("memory exceeded : ext_class ");
    }
    //init seq pattern templates
    L2->set_templ(1);
    L2->set_templ2(0);

    interval->reset();
    interval2->reset();

    supsz = partition_get_idxsup(it);
    partition_read_item(interval->arraySpade(), it);

    int tmpit;
    for (i = 0, k = 0; i < eqgraph[it]->num_elements() ||
                       k < eqgraph[it]->seqnum_elements();) {
        ljoin = NULL;
        ejoin = NULL;

        it2 = DBASE_MAXITEM + 1;
        tmpit = DBASE_MAXITEM + 1;
        if (i < eqgraph[it]->num_elements() && (use_maxgap || ibvec[i]))
            it2 = eqgraph[it]->get_element(i);
        if (k < eqgraph[it]->seqnum_elements() && (use_maxgap || sbvec[k]))
            tmpit = eqgraph[it]->seqget_element(k);

        if (it2 == tmpit) {
            ejoin = (Itemset *) 1;
            ljoin = (Itemset *) 1;
            k++;
            i++;
            if (it2 == DBASE_MAXITEM + 1) continue;
        } else if (it2 < tmpit) {
            ejoin = (Itemset *) 1;
            i++;
        } else {
            ljoin = (Itemset *) 1;
            k++;
            it2 = tmpit;
        }
        //cout << "JOIN " << it << " " << it2 << " " << ejoin << " " << ljoin << endl << flush;
        supsz2 = partition_get_idxsup(it2);

        partition_read_item(interval2->arraySpade(), it2);

        if (ejoin) {
            ejoin = new Itemset(2, min(supsz, supsz2));
            if (ejoin == NULL) {
                throw runtime_error("memory exceeded");
            }
        } else ejoin = NULL;
        if (ljoin) {
            ljoin = new Itemset(2, supsz2);
            if (ljoin == NULL) {
                throw runtime_error("memory exceeded");
            }
        } else ljoin = NULL;
        //cout << "ljoin " << ljoin << " " << ejoin << " " <<
        //supsz << " " << supsz2 << " " << it << " " << it2 << endl;

        get_2newf_intersect(ljoin, ejoin, interval2->arraySpade(), interval->arraySpade(),
                            supsz2, supsz);

        if (ljoin) {
            ljoin->add_item(it2);
            ljoin->add_item(it);
        }
        if (pruning_type > 1) post_pruning(ljoin, L2->templ());
        if (ljoin) {
            //if (!use_isetonly){
            ljoin->reallocival();
            //ljoin->add_item(it2);
            //ljoin->add_item(it);
            L2->prepend(ljoin);
            //cout << "LARGE ";
            if (outputfreq) ljoin->print_seq(L2->templ());
            //NumLargeItemset[1]++;

            //}
            //else{
            //cout << "DELETED ";
            //ljoin->print_seq(L2->templ());
            //delete ljoin;
            //}
            //}
        }

        if (ejoin) {
            ejoin->add_item(it2);
            ejoin->add_item(it);
        }
        if (pruning_type > 1) post_pruning(ejoin, L2->templ2());
        if (ejoin) {
            ejoin->reallocival();
            //ejoin->add_item(it2);
            //ejoin->add_item(it);
            L2->prepend2(ejoin);
            //cout << "LARGE ";
            if (outputfreq) ejoin->print_seq(L2->templ2());
            //NumLargeItemset[1]++;
            //}
            //else{
            //   cout << "DELETED ";
            //   //ejoin->print_seq(L2->templ2());
            //   delete ejoin;
            //}
        }
    }

    //cout << "MEMEXTEND " << it << " " << MEMUSED << endl;
    seconds(t2);
    L2ISECTTIME += t2 - t1;
    return L2;
}

void delete_eq_list(Lists<Eqclass *> *eqlist) {
    ListNodes<Eqclass *> *eqhd = eqlist->head();

    for (; eqhd; eqhd = eqhd->next()) {
        delete eqhd->item()->list();
        eqhd->item()->set_list(NULL);
        delete eqhd->item();
    }
    delete eqlist;
}

void fill_join(Itemset *join, Itemset *hdr1, Itemset *hdr2) {
    int i;

    join->add_item((*hdr2)[0]);
    for (i = 0; i < hdr1->size(); i++) {
        join->add_item((*hdr1)[i]);
    }
}

Itemset *prune_decision(Itemset *it1, Itemset *it2,
                        unsigned int ptempl, int jflg) {
    FreqIt *res;
    int i, j, k;

    //prune if seq pat exceeds the max seq len or iset len
    int bit, seqlen = 0, isetlen = 0, maxisetlen = 0;
    for (i = 0; i < it2->size(); i++) {
        bit = GETBIT(ptempl, i);
        if (bit) {
            seqlen++;
            if (maxisetlen < isetlen) maxisetlen = isetlen;
            isetlen = 0;
        } else isetlen++;
    }
    if (maxisetlen < isetlen) maxisetlen = isetlen;
    seqlen++;
    maxisetlen++;

    if (seqlen > max_seq_len) return NULL;
    if (maxisetlen > max_iset_len) return NULL;


    //max_gap destroys the downward closure property, so we cannot prune
    if (use_maxgap) return (Itemset *) 1;

    int l1 = (*it1)[0];
    int l2 = (*it2)[0];
    int nsz;
    if (use_hash && (it2->size() > 2)) {
        if (recursive) return (Itemset *) 1;

        unsigned int ttpl;
        FreqIt fit(it2->size(), 0);

        //skip the last two subsets (or omit the first two elements)
        nsz = it2->size() - 2;

        for (i = nsz + 1; i >= 1; i--) {
            k = 0;
            ttpl = 0;
            //form new subset template
            if (i == nsz + 1) ttpl = (ptempl >> 1);
            else {
                for (j = 0; j < i; j++) {
                    bit = GETBIT(ptempl, nsz - j + 1);
                    ttpl = SETBIT(ttpl, bit, nsz - j);
                }
                bit = GETBIT(ptempl, nsz - j + 1);
                bit = bit || GETBIT(ptempl, nsz - j);
                ttpl = SETBIT(ttpl, bit, nsz - j);
                j += 2;
                for (; j < nsz + 2; j++) {
                    bit = GETBIT(ptempl, nsz - j + 1);
                    ttpl = SETBIT(ttpl, bit, nsz - j + 1);
                }
            }
            //form new subset by omitting the i-th item
            fit.seq[k++] = l1;
            fit.seq[k++] = l2;
            for (j = 1; j < nsz + 2; j++) {
                if (j != i) {
                    fit.seq[k++] = (*it2)[j];
                }
            }
            fit.templ = ttpl;

            //???? Does this work for suffix classes
            //cout << "SEARCH " << fit;
            if (fit.seq[fit.size() - 1] == (*it1)[it1->size() - 1] && !recursive) {
                //elements should be in current class
                if (FreqArrayPos > 0) {
                    if (!EqGrNode::bsearch(0, FreqArrayPos - 1, FreqArray,
                                           fit, recursive)) {
                        //print_freqary();
                        //cout << "NOT FOUND " << endl;
                        return NULL;
                    }
                } else return NULL;
            } else if (fit.seq[fit.size() - 1] > (*it1)[it1->size() - 1]) {
                // class must already have been processed, otherwise we can't prune
                if (!eqgraph[fit.seq[fit.size() - 1]]->find_freqarray(fit, recursive)) {
                    return NULL;
                }
            }
        }
    } else {// if (it1->size() == 2){
        bit = 0;
        nsz = it2->size() - 2;
        for (i = 0; i <= nsz + 1 && !bit; i++) {
            l2 = (*it2)[i];
            if (eqgraph[l2]) {
                if (jflg == LJOIN || jflg == MJOIN) {
                    if (eqgraph[l2]->seqfind(l1) == -1)
                        return NULL;
                } else {
                    if (eqgraph[l2]->find(l1) == -1)
                        return NULL;
                }
            } else return NULL;
            //cout << "FOUND " << endl;
            if (nsz - i >= 0) bit = GETBIT(ptempl, nsz - i);
        }
    }
    return (Itemset *) 1;
}


void insert_freqarray(Lists<Eqclass *> *LargeL) {
    //insert frequent itemsets into hash table
    ListNodes<Eqclass *> *chd;
    ListNodes<Itemset *> *hdr1, *hdr2;
    Eqclass *cluster;

    chd = LargeL->head();
    for (; chd; chd = chd->next()) {
        cluster = chd->item();
        hdr1 = cluster->list()->head();
        for (; hdr1; hdr1 = hdr1->next()) {
            add_freq(hdr1->item(), cluster->templ());
            //hdr1->item()->print_seq(cluster->templ());
        }
        hdr2 = cluster->list2()->head();
        for (; hdr2; hdr2 = hdr2->next()) {
            add_freq(hdr2->item(), cluster->templ2());
            //hdr2->item()->print_seq(cluster->templ2());
        }
    }
}

void process_cluster_list1(ListNodes<Itemset *> *hdr1,
                           Lists<Itemset *> *cluster1,
                           Lists<Itemset *> *cluster2, Lists<Eqclass *> *LargeL,
                           int iter, int eqtype, Eqclass *parent) {
    ListNodes<Itemset *> *hdr2;
    Eqclass *EQ = new Eqclass(iter - 1, eqtype);
    if (EQ == NULL) {
        throw runtime_error("memory exceeded");
    }
    fill_seq_template(EQ, parent, 2);
    //int first;
    Itemset *ljoin, *ejoin, *mjoin;
    int lsup, esup, msup;
    //cout << "BEG CLUSERT 1 : " << MEMUSED << endl;

    //cout << "PROCESS " << *hdr1->item() << endl;
    //first = 1;
    hdr2 = cluster2->head();
    for (; hdr2; hdr2 = hdr2->next()) {
        //ljoin = (Itemset *)1;
        ljoin = prune_decision(hdr2->item(), hdr1->item(), EQ->templ(), LJOIN);
        ejoin = NULL;
        mjoin = NULL;
        lsup = esup = msup = 0;
        //cout << "process 1 0 0" << endl;
        if (pruning_type > 1)
            pre_pruning(ljoin, EQ->templ(), hdr1->item(), hdr2->item(), 1);
        if (ljoin)
            get_tmpnewf_intersect(ljoin, ejoin, mjoin, lsup, esup, msup,
                                  hdr2->item(), hdr1->item(), iter);
        if (ljoin) fill_join(ljoin, hdr1->item(), hdr2->item());
        if (pruning_type > 1) post_pruning(ljoin, EQ->templ());
        if (ljoin) {
            NumLargeItemset[iter - 1]++;
            //fill_join(ljoin, hdr1->item(), hdr2->item());
            //cout << "XXLARGE ";
            if (outputfreq) ljoin->print_seq(EQ->templ());
            EQ->append(ljoin);
        }
    }

    //hdr2 = cluster1->head();
    //for (; hdr2 != hdr1; hdr2=hdr2->next()){
    hdr2 = hdr1->next();
    for (; hdr2 != NULL; hdr2 = hdr2->next()) {
        //ejoin = (Itemset *)1;
        ejoin = prune_decision(hdr2->item(), hdr1->item(), EQ->templ2(), EJOIN);
        ljoin = NULL;
        mjoin = NULL;
        lsup = esup = msup = 0;
        //cout << "process 0 1 0" << endl;
        if (pruning_type > 1)
            pre_pruning(ejoin, EQ->templ2(), hdr1->item(), hdr2->item(), 0);
        if (ejoin)
            get_tmpnewf_intersect(ljoin, ejoin, mjoin, lsup, esup, msup,
                                  hdr2->item(), hdr1->item(), iter);
        //cout << "AFT JOIN " << MEMUSED << endl;
        if (ejoin) fill_join(ejoin, hdr1->item(), hdr2->item());
        if (pruning_type > 1) post_pruning(ejoin, EQ->templ2());
        if (ejoin) {
            NumLargeItemset[iter - 1]++;
            //fill_join(ejoin, hdr1->item(), hdr2->item());
            //cout << "XXXLARGE ";
            if (outputfreq) ejoin->print_seq(EQ->templ2());
            EQ->append2(ejoin);
        }
    }

    if (EQ) {
        if ((EQ->list()->size() > 0) || (EQ->list2()->size() > 0)) {
            if (recursive) {
                //if (use_hash) insert_freqarray(EQ);
                process_cluster1(EQ, NULL, iter + 1);
                delete EQ;
            } else LargeL->append(EQ);
        } else {
            //   if (use_hash && EQ->list2()->size() == 1)
            //      add_freq(EQ->list2()->head()->item(), EQ->templ2());
            delete EQ;
            EQ = NULL;
        }
    }
    //cout << "END CLUSTER1 : " << MEMUSED << endl;
}

void process_cluster_list2(ListNodes<Itemset *> *hdr1, int i, Eqclass **EQ,
                           Lists<Itemset *> *cluster, Lists<Eqclass *> *LargeL,
                           int iter, int eqtype, Eqclass *parent) {
    int j;

    ListNodes<Itemset *> *hdr2;
    Itemset *ljoin, *ejoin, *mjoin;
    int lsup, esup, msup;

    //join with sequences
    hdr2 = hdr1;
    for (j = i; hdr2; j++, hdr2 = hdr2->next()) {
        ljoin = prune_decision(hdr1->item(), hdr2->item(), EQ[j]->templ(), LJOIN);
        if (hdr2 == hdr1) {
            ejoin = mjoin = NULL;
        } else {
            ejoin = prune_decision(hdr2->item(), hdr1->item(), EQ[i]->templ2(), EJOIN);
            mjoin = prune_decision(hdr2->item(), hdr1->item(), EQ[i]->templ(), MJOIN);
            //ejoin = mjoin = (Itemset *)1;
        }
        //cout << "process 1 1 1" << endl;
        lsup = esup = msup = 0;
        if (pruning_type > 1) {
            pre_pruning(ejoin, EQ[i]->templ2(), hdr1->item(), hdr2->item(), 0);
            pre_pruning(ljoin, EQ[j]->templ(), hdr2->item(), hdr1->item(), 1);
            pre_pruning(mjoin, EQ[i]->templ(), hdr1->item(), hdr2->item(), 1);
        }

        if (ljoin || ejoin || mjoin)
            get_tmpnewf_intersect(ljoin, ejoin, mjoin, lsup, esup, msup,
                                  hdr1->item(), hdr2->item(), iter);
        //cout << "SUPPP " << lsup << " " << esup << " " << msup << endl;
        if (ljoin) fill_join(ljoin, hdr2->item(), hdr1->item());
        if (pruning_type > 1) post_pruning(ljoin, EQ[j]->templ());
        if (ljoin) {
            NumLargeItemset[iter - 1]++;
            //fill_join(ljoin, hdr2->item(), hdr1->item());
            //cout << "LARGE ";
            if (outputfreq) ljoin->print_seq(EQ[j]->templ());
            EQ[j]->append(ljoin);
        }

        if (ejoin) fill_join(ejoin, hdr1->item(), hdr2->item());
        if (pruning_type > 1) post_pruning(ejoin, EQ[i]->templ2());
        if (ejoin) {
            NumLargeItemset[iter - 1]++;
            //fill_join(ejoin, hdr1->item(), hdr2->item());
            //cout << "LARGE ";
            if (outputfreq) ejoin->print_seq(EQ[i]->templ2());
            EQ[i]->append2(ejoin);
        }

        if (mjoin) fill_join(mjoin, hdr1->item(), hdr2->item());
        if (pruning_type > 1) post_pruning(mjoin, EQ[i]->templ());
        if (mjoin) {
            NumLargeItemset[iter - 1]++;
            //fill_join(mjoin, hdr1->item(), hdr2->item());
            //cout << "LARGE ";
            if (outputfreq) mjoin->print_seq(EQ[i]->templ());
            EQ[i]->append(mjoin);
        }
    }
    if ((EQ[i]->list()->size() > 0) || (EQ[i]->list2()->size() > 0)) {
        if (recursive) {
            //if (use_hash) insert_freqarray(EQ[i]);
            process_cluster1(EQ[i], NULL, iter + 1);
            delete EQ[i];
            EQ[i] = NULL;
        } else LargeL->append(EQ[i]);
    } else {
        //if (use_hash && EQ[i]->list2()->size() == 1)
        //   add_freq(EQ[i]->list2()->head()->item(), EQ[i]->templ2());
        delete EQ[i];
        EQ[i] = NULL;
    }

    //cout << "END cluster 2 : " << MEMUSED << endl;
}


void process_cluster1(Eqclass *cluster, Lists<Eqclass *> *LargeL, int iter) {
    Eqclass **EQ = NULL;
    ListNodes<Itemset *> *hdr1, *hdr2;
    int i;

    if (cluster->list()->head()) {
        EQ = new Eqclass *[cluster->list()->size()];
        if (EQ == NULL) {
            throw runtime_error("memory exceeded");
        }
        for (i = 0; i < cluster->list()->size(); i++) {
            EQ[i] = new Eqclass(iter - 1, EQCTYP1);
            if (EQ[i] == NULL) {
                throw runtime_error("memory exceeded");
            }
            fill_seq_template(EQ[i], cluster, 1);
        }
    }

    hdr1 = cluster->list()->head();
    for (i = 0; hdr1; hdr1 = hdr1->next(), i++) {
        //if (use_hash && iter > 3) add_freq(hdr1->item(), cluster->templ());
        process_cluster_list2(hdr1, i, EQ, cluster->list(), LargeL, iter,
                              EQCTYP1, cluster);
    }
    if (EQ) delete[] EQ;


    hdr2 = cluster->list2()->head();
    for (; hdr2; hdr2 = hdr2->next()) {
        //if (use_hash && iter > 3) add_freq(hdr2->item(), cluster->templ2());
        process_cluster_list1(hdr2, cluster->list2(), cluster->list(),
                              LargeL, iter, EQCTYP1, cluster);
    }

    //if (recursive) delete cluster;
    if (maxiter < iter) maxiter = iter;

}


void find_large(Eqclass *cluster, int it) {
    Lists<Eqclass *> *LargeL, *Candidate;
    ListNodes<Eqclass *> *chd;
    int iter;
    int LargelistSum = 0;
    int more;

    more = 1;
    Candidate = new Lists<Eqclass *>;
    Candidate->append(cluster);
    //cout << "MEMFIND " << it << " " << MEMUSED << endl;
    for (iter = 3; more; iter++) {
        //cout << "ITER " << iter << endl;
        LargeL = new Lists<Eqclass *>;
        chd = Candidate->head();
        for (; chd; chd = chd->next()) {
            //cout << "EQCLASS ";
            //chd->item()->print_template();
            //chd->item()->print_list(chd->item()->list());
            //cout << "***\n";
            //chd->item()->print_list(chd->item()->list2());
            //cout << "------------------" << endl;
            process_cluster1(chd->item(), LargeL, iter);
            //cout << "BEF MEMFIND " << it << " " << MEMUSED << endl;
            //reclaim memory for this class immediately
            delete chd->item();
            //cout << "AFT MEMFIND " << it << " " << MEMUSED << endl;
            chd->set_item(NULL);
        }
        Candidate->clear();
        delete Candidate;
        //if (maxiter < iter) maxiter = iter;

        if (use_hash) insert_freqarray(LargeL);
        chd = LargeL->head();
        LargelistSum = 0;
        for (; chd; chd = chd->next()) {
            LargelistSum += chd->item()->list()->size();
            if (chd->item()->list2())
                LargelistSum += chd->item()->list2()->size();
        }
        //print_freqary();
        more = (LargelistSum > 0);

        Candidate = LargeL;
        if (memtrace) memlog <<  it << " " << MEMUSED << endl;

        if (!more) {
            LargeL->clear();
            delete LargeL;
        }
        //cout << "AFT DEL " << it << " " << MEMUSED << " " << iter << endl;
    }
    //cout << "MEMLAST " << it << " " << MEMUSED << endl;
}


void process_class(int it) {

    //from 2-itemsets from ext disk
    Eqclass *large2it = get_ext_eqclass(it);
    if (large2it == NULL) return;

    //cout << "*********************" << endl;
    //large2it->print_list(large2it->list());
    //cout << "-----------" << endl;
    //large2it->print_list(large2it->list2());
    //cout << "*********************" << endl;
    if (memtrace) memlog <<  it << " " << MEMUSED << endl;
    if (use_maxgap) {
        process_maxgap(large2it);
    } else {
        if (recursive) {
            process_cluster1(large2it, NULL, 3);
            delete large2it;
        } else find_large(large2it, it);
    }
}

void newSeq() {
    int i, j;

    if (use_hash)
        FreqArray = (FreqIt **) malloc(FreqArraySz * sizeof(FreqIt *));
    //form large itemsets for each eqclass
    if (use_ascending != -2) {
        if (use_ascending == -1) {
            for (i = 0; i < DBASE_MAXITEM; i++)
                if (eqgraph[i]) {
                    if (memtrace) memlog <<  i << " " << MEMUSED << endl;
                    process_class(i);
                    if (memtrace) memlog <<  i << " " << MEMUSED << endl;
                }
        } else if (eqgraph[use_ascending])
            process_class(use_ascending);
    } else {
        for (i = DBASE_MAXITEM - 1; i >= 0; i--) {
            if (eqgraph[i]) {
                if (memtrace) memlog <<  i << " " << MEMUSED << endl;
                //cout << "PROCESSS ITEM " << i << endl << flush;
                if (use_hash) FreqArrayPos = 0;
                process_class(i);
                if (use_hash) {
                    if (FreqArrayPos > 0) {
                        //cout << "FREQUENT ARRAY3" << endl;
                        FreqIt **fit = new FreqIt *[FreqArrayPos];
                        for (j = 0; j < FreqArrayPos; j++) {
                            fit[j] = FreqArray[j];
                            //cout << *fit[j];
                        }
                        eqgraph[i]->set_freqarray(fit, FreqArrayPos);
                    }
                }
                //cout << " -------- " << endl;
                if (memtrace) memlog <<  i << " " << MEMUSED << endl;
            }
        }
    }
}


void read_files() {
    int i;

    NumLargeItemset = new int[(int) DBASE_AVG_TRANS_SZ * 30];
    bzero((char *) NumLargeItemset, sizeof(int) * ((int) DBASE_AVG_TRANS_SZ * 30));

    eqgraph = new EqGrNode *[DBASE_MAXITEM];
    bzero((char *) eqgraph, DBASE_MAXITEM * sizeof(EqGrNode *));

    double t1, t2;
    seconds(t1);
    NumLargeItemset[0] = make_l1_pass();
    seconds(t2);
    EXTL1TIME = t2 - t1;

    if (ext_l2_pass) {
        NumLargeItemset[1] = make_l2_pass();
        //cout << "L2 " <<  NumLargeItemset[1] <<endl;
        seconds(t1);
        EXTL2TIME = t1 - t2;
    } else {
        NumLargeItemset[1] = get_file_l2(it2f, seqf);
        seconds(t1);
        EXTL2TIME = t1 - t2;
    }

    for (int i = 0; i < DBASE_MAXITEM; i++) {
        if (eqgraph[i]) {
            if (eqgraph[i]->num_elements() > 0)
                eqgraph[i]->elements()->compact();
            if (eqgraph[i]->seqnum_elements() > 0)
                eqgraph[i]->seqelements()->compact();
        }
    }

    //cout << "BIDX " << NumLargeItemset[0] << ": ";
    //for (int i=0; i < NumLargeItemset[0]; i++)
    //  cout << " " << F1::backidx[i];
    //cout << endl;

    //cout << NumLargeItemset[0] << "LARGE 1 ITEMS\n";
    maxitemsup = 0;
    int sup;
    for (i = 0; i < DBASE_MAXITEM; i++) {
        sup = partition_get_idxsup(i);
        if (maxitemsup < sup) maxitemsup = sup;
    }
    //cout << "MAXITEMSUP " << maxitemsup << endl;
    interval = new ArraySpade(maxitemsup);
    interval2 = new ArraySpade(maxitemsup);
    interval3 = new ArraySpade(maxitemsup);
    //cout << "MAXEQSZIE " << maxeqsize << " " << t2-ts << endl;
}

void sequenceFunc(int argc, char **argv) {
    int i;

    double ts, te;
    double t1, t2;

    seconds(ts);
    //cout << "BEGIN MEM " << MEMUSED << endl;
    parse_args(argc, argv);

    partition_alloc(dataf, idxf);
    ClassInfo cls(use_class, classf);
    read_files();

    //cout << "AFTER READFILE " << MEMUSED << endl;

    if (use_maxgap) IBM = new ItBufMgr(NumLargeItemset[0]);
    seconds(t1);
    newSeq();
    seconds(t2);
    double FKtime = t2 - t1;
    if (use_maxgap) delete IBM;
    //print_freqary();
    //cout << "AFTER SEQ " << MEMUSED << endl;
    seconds(te);

    partition_dealloc();

    delete interval;
    delete interval2;
    delete interval3;
    for (i = 0; i < DBASE_MAXITEM; i++) {
        if (eqgraph[i]) delete eqgraph[i];
    }
    delete[] eqgraph;

    if (memtrace) {
        memlog <<  MEMUSED << endl;
    }
}
