#include "common.h"
#include "InvertDatabase.h"

void InvertDatabase::incr(int sz) {
    int oldsz = numcust;
    numcust = sz;

    curits.resize(numcust);
    curcnts.resize(numcust);
    curcids.resize(numcust);
    curitszs.resize(numcust);

    int i;
    int ttval = (int) (args->avg_cust_size * args->avg_trans_count);
    for (i = oldsz; i < numcust; i++) {
        curitszs[i] = ttval;
        curits[i].resize(curitszs[i]);
        curcnts[i] = 0;
        curcids[i] = NOCLASS;
    }
}

void InvertDatabase::incr_curit(int midx) {
    curitszs[midx] *= 2;
    curits[midx].resize(curitszs[midx]);
}


void InvertDatabase::print_idlist(ostream& idlstrm, shared_ptr<vint> ival, int supsz) {
    int i, cid, cnt;
    int *ival_data = ival->data();

    if (supsz > 0) {
        cid = ival_data[0];
        cnt = 0;
        for (i = 0; i < supsz;) {
            if (cid == ival_data[i]) {
                cnt++;
                i += 2;
            } else {
                idlstrm << cid << " " << cnt << " ";
                cid = ival_data[i];
                cnt = 0;
            }
        }
        idlstrm << cid << " " << cnt;
    }
}

void InvertDatabase::get_l2file(const string& fname, char use_seq, int &l2cnt) {
    fstream file(fname.c_str(), ios::binary);
    if (!file.is_open()) {
        throw runtime_error("can't open l2 file");
    }
    unsigned long flen = file_size(file);
    if (flen > 0) {
        int *cntary = read_file(file, flen);

        if (!file) {
            throw runtime_error("Error reading file " + fname);
        }
        file.close();

        // build eqgraph -- large 2-itemset relations
        int lim = flen / INT_SIZE;
        char lflg = 0;
        int i, j;
        for (i = 0; i < lim; i += 3) {
            lflg = 0;
            for (j = 0; j < cls->get_num_class(); j++) {
                if (cntary[i + 2] >= cls->get_min_support(j)) {
                    lflg = 1;
                    break;
                }
            }
            if (lflg) {
                if (!extl2_pre_pruning(cntary[i + 2], cntary[i + 1], cntary[i], use_seq, vuint_null)) {
                    suffix_add_item_eqgraph(use_seq, cntary[i], cntary[i + 1]);
                    l2cnt++;
                    //assign sup to a single class, sice we don't know breakup
                    if (use_seq) eqgraph[cntary[i + 1]]->add_seqsup(cntary[i + 2], 0);
                    else eqgraph[cntary[i + 1]]->add_sup(cntary[i + 2], 0);
                    for (j = 1; j < cls->get_num_class(); j++)
                        if (use_seq) eqgraph[cntary[i + 1]]->add_seqsup(0, j);
                        else eqgraph[cntary[i + 1]]->add_sup(0, j);
                }
            }
        }

        delete [] cntary;
    }
}

void InvertDatabase::suffix_add_item_eqgraph(char use_seq, int it1, int it2) {
    if (eqgraph[it2] == nullptr) {
        eqgraph[it2].reset(new EqGrNode(2, cls->get_num_class()));
    }
    if (use_seq) eqgraph[it2]->seqadd_element(it1);
    else eqgraph[it2]->add_element(it1);
}

int InvertDatabase::make_l1_pass() {
    int i, j;
    int supsz;
    int bsz = 100;

    ostringstream &seqstrm = env.seqstrm;
    ostringstream &idlstrm = env.idlstrm;

    backidx.resize(bsz);
    fidx.resize(args->dbase_max_item);

    numfreq = 0;
    int ivalsz = 100;
    shared_ptr<vint> ival = make_shared<vint>(ivalsz);
//   int tt=0;
    for (i = 0; i < args->dbase_max_item; i++) {
        supsz = partition->partition_get_idxsup(i);
        if (ivalsz < supsz) {
            ivalsz = supsz;
            ival->resize(ivalsz);
        }
        int *ival_data = ival->data();
        partition->partition_read_item(ival, i);
        for (j = 0; j < cls->get_num_class(); j++) {
            cls->set_tmpe_item(j, 0);
        }

        int cid = -1;
        for (j = 0; j < supsz; j += 2) {
            if (cid != ival_data[j]) {
                cls->increase_tmpe(j);
            }
            cid = ival_data[j];
        }

        char lflg = 0;
        fidx[i] = -1;       // default init
        for (j = 0; j < cls->get_num_class(); j++) {
            if (cls->strong_esupport(j)) {
                lflg = 1;
                if (numfreq + 1 > bsz) {
                    bsz = 2 * bsz;
                    backidx.resize(bsz);
                }
                backidx[numfreq] = i;
                fidx[i] = numfreq;
                //   cls->TMPE[j] << endl;
                numfreq++;
                break;
            }
        }

        if (lflg) {
            for (j = 0; j < cls->get_num_class(); j++)
                add_sup(cls->get_tmpe_item(j), j);
            if (args->outputfreq) {
                seqstrm << i << " --";
                seqstrm << " " << get_sup(i);
                for (j = 0; j < cls->get_num_class(); j++)
                    seqstrm << " " << cls->get_tmpe_item(j);
                seqstrm << " ";
                if (args->print_tidlist) print_idlist(idlstrm, ival, supsz);
                seqstrm << endl;
            }
        }
    }

    backidx.resize(numfreq);
    ival = nullptr;

    return numfreq;
}

void InvertDatabase::add_sup(int sup, int clsidx) {
    itsup[clsidx].add(sup);
}

int InvertDatabase::get_sup(int it, int clsidx) {
    if (clsidx == -1) {
        int sum = 0;
        auto num_class = cls->get_num_class();
        for (int i = 0; i < num_class; i++)
            sum += itsup[i][fidx[it]];
        return sum;
    } else return itsup[clsidx][fidx[it]];
}

void InvertDatabase::process_cust_invert(int custidx) {
    int cid = curcids[custidx];
    int curcnt = curcnts[custidx];
    vint curit = curits[custidx];

    int i, j, k, l;
    int nv1, nv2, diff;
    int it1, it2;

    for (i = 0; i < curcnt; i = nv1) {
        nv1 = i;
        it1 = curit[i];
        while (nv1 < curcnt && it1 == curit[nv1]) nv1 += 2;
        for (j = i; j < curcnt; j = nv2) {
            nv2 = j;
            it2 = curit[j];
            while (nv2 < curcnt && it2 == curit[nv2]) nv2 += 2;
            if (!seq_sup[it1].empty() && curit[i + 1] + args->min_gap <= curit[nv2 - 1]) {
                for (k = i, l = j; k < nv1 && l < nv2;) {
                    diff = curit[l + 1] - curit[k + 1];
                    if (diff < args->min_gap) l += 2;
                    else if (diff > args->max_gap) k += 2;
                    else {
                        seq_sup[it1][it2][cls->getcls(cid)]++;
                        break;
                    }
                }
            }

            if (j > i) {
                if (!seq_sup[it2].empty() && curit[j + 1] + args->min_gap <= curit[nv1 - 1]) {
                    for (k = j, l = i; k < nv2 && l < nv1;) {
                        diff = curit[l + 1] - curit[k + 1];
                        if (diff < args->min_gap) l += 2;
                        else if (diff > args->max_gap) k += 2;
                        else {
                            seq_sup[it2][it1][cls->getcls(cid)]++;
                            break;
                        }
                    }
                }

                if (!set_sup[it1].empty()) {
                    for (k = i, l = j; k < nv1 && l < nv2;) {
                        if (curit[k + 1] > curit[l + 1]) l += 2;
                        else if (curit[k + 1] < curit[l + 1]) k += 2;
                        else {
                            set_sup[it1][it2 - it1 - 1][cls->getcls(cid)]++;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void InvertDatabase::process_invert(int pnum) {
    int i, k;
    int minv, maxv;
    partition->partition_get_minmaxcustid(backidx, numfreq, pnum, minv, maxv);
    if (numcust < maxv - minv + 1)
        incr(maxv - minv + 1);

    int supsz;
    int ivalsz = 0;
    shared_ptr<vint> ival = make_shared<vint>();
    for (i = 0; i < numfreq; i++) {
        supsz = partition->partition_get_lidxsup(pnum, backidx[i]);
        if (ivalsz < supsz) {
            ivalsz = supsz;
            ival->resize(ivalsz);
        }
        partition->partition_lclread_item(ival, pnum, backidx[i]);

        int cid;
        int midx;
        int *ival_data = ival->data();
        for (int pos = 0; pos < supsz; pos += 2) {
            cid = ival_data[pos];
            midx = cid - minv;
            if (curcnts[midx] + 2 > curitszs[midx]) {
                incr_curit(midx);
            }
            curcids[midx] = cid;
            curits[midx][curcnts[midx]++] = i;
            curits[midx][curcnts[midx]++] = ival_data[pos + 1];

        }
    }
    for (k = 0; k < maxv - minv + 1; k++) {
        if (curcnts[k] > 0) {
            process_cust_invert(k);
        }
        curcnts[k] = 0;
        curcids[k] = NOCLASS;
    }
}

bool InvertDatabase::extl2_pre_pruning(int totsup, int it, int pit, char use_seq, vuint& clsup) {
    ostringstream &logger = env.logger;

    float conf, conf2;
    int itsup;
    if (args->pruning_type == NOPRUNING) return false;
    if (use_seq) return false;
    if (GETBIT(args->pruning_type, FOLLOWPRUNING - 1)) {
        itsup = get_sup(it);
        conf = (1.0f * totsup) / itsup;
        conf2 = (1.0f * totsup) / get_sup(pit);
        if (conf >= args->follow_thresh || conf2 >= args->follow_thresh) {
            if (args->outputfreq && !clsup.empty()) {
                logger << "PRUNE_EXT " << pit << (use_seq ? " -2 " : " ")
                     << it << " -1 " << totsup;
                for (int i = 0; i < cls->get_num_class(); i++)
                    logger << " " << clsup[i];
                logger << endl;
            }
            args->prepruning++;
            return true;
        }
    }
    return false;
}

void InvertDatabase::get_F2(int &l2cnt) {
    int i, j, k;
    int fcnt;
    char lflg;
    char use_seq;

    for (j = 0; j < numfreq; j++) {
        if (set_sup[j].empty()) {
            use_seq = 0;
            for (k = j + 1; k < numfreq; k++) {
                lflg = 0;
                for (i = 0; i < cls->get_num_class(); i++) {
                    fcnt = set_sup[j][k - j - 1][i];
                    if (fcnt >= cls->get_min_support(i)) {
                        lflg = 1;
                        break;
                    }
                }
                if (lflg) {
                    fcnt = 0;
                    for (i = 0; i < cls->get_num_class(); i++) {
                        fcnt += set_sup[j][k - j - 1][i];
                    }
                    if (!extl2_pre_pruning(fcnt, backidx[k], backidx[j], use_seq, set_sup[j][k - j - 1])) {
                        suffix_add_item_eqgraph(use_seq, backidx[j], backidx[k]);
                        for (i = 0; i < cls->get_num_class(); i++) {
                            int ffcnt = set_sup[j][k - j - 1][i];
                            eqgraph[backidx[k]]->add_sup(ffcnt, i);
                        }
                        l2cnt++;
                    }
                }
            }
        }
        if (!seq_sup[j].empty()) {
            use_seq = 1;
            for (k = 0; k < numfreq; k++) {
                lflg = 0;
                for (i = 0; i < cls->get_num_class(); i++) {
                    fcnt = seq_sup[j][k][i];
                    if (fcnt >= cls->get_min_support(i)) {
                        lflg = 1;
                        break;
                    }
                }
                if (lflg) {
                    fcnt = 0;
                    for (i = 0; i < cls->get_num_class(); i++) {
                        fcnt += seq_sup[j][k][i];
                    }
                    if (!extl2_pre_pruning(fcnt, backidx[k], backidx[j], use_seq, seq_sup[j][k])) {
                        suffix_add_item_eqgraph(use_seq, backidx[j], backidx[k]);
                        l2cnt++;
                        for (i = 0; i < cls->get_num_class(); i++) {
                            int ffcnt = seq_sup[j][k][i];
                            eqgraph[backidx[k]]->add_seqsup(ffcnt, i);
                        }
                    }
                }
            }
        }
    }
}

int InvertDatabase::make_l2_pass() {
    int i, j;

    int l2cnt = 0;
    int num_class = cls->get_num_class();

    // support for 2-itemsets
    set_sup.resize(numfreq);
    seq_sup.resize(numfreq);

    int low, high;

    for (low = 0; low < numfreq; low = high) {
        for (high = low; high < numfreq; high++) {
            if (args->max_iset_len > 1 && numfreq - high - 1 > 0) {
                set_sup[high].resize(numfreq - high - 1);
                for (i = 0; i < numfreq - high - 1; i++) {
                    set_sup[high][i].resize(num_class, 0);
                }
            }
            if (args->max_seq_len > 1) {
                seq_sup[high].resize(numfreq);
                for (i = 0; i < numfreq; i++) {
                    seq_sup[high][i].resize(num_class, 0);
                }
            }
        }
        for (int p = 0; p < args->num_partitions; p++) {
            process_invert(p);
        }
        get_F2(l2cnt);
    }

    return l2cnt;
}

void InvertDatabase::init(int sz) {
    int i;
    numcust = 0;
    incr(sz);
    eqgraph.resize(args->dbase_max_item);

    auto num_class = cls->get_num_class();
    itsup.reserve(num_class);
    for (i = 0; i <num_class; i++) {
        itsup.emplace_back(2);
    }
}

InvertDatabase::InvertDatabase(Env& env_) : env(env_) {}

void InvertDatabase::set_cls(const shared_ptr<ClassInfo> &cls) {
    InvertDatabase::cls = cls;
}

void InvertDatabase::setPartition(const shared_ptr<Partition> &partition) {
    InvertDatabase::partition = partition;
}

void InvertDatabase::setArgs(const shared_ptr<SpadeArguments> &args) {
    InvertDatabase::args = args;
}
