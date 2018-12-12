#include "common.h"
#include "exttpose.h"
#include "calcdb.h"
#include "Env.h"
#include "TransArray.h"


int cmp2it(const void *a, const void *b) {
    auto *ary = (int *) a;
    auto *bry = (int *) b;
    if (ary[0] < bry[0]) return -1;
    else if (ary[0] > bry[0]) return 1;
    else {
        if (ary[1] < bry[1]) return -1;
        else if (ary[1] > bry[1]) return 1;
        else return 0;
    }
}

void Exttpose::sort_get_l2(int &l2cnt, fstream &file, ofstream &ofd, vector<unsigned char> &cntary, bool use_seq) {
    //write 2-itemsets counts to file

    int i, j, k;
    int fcnt;
    int lit;
    int itbuf[3];
    int *sortary;

    unsigned long filesize = file_size(file);

    if (filesize > 0) {
        sortary = read_file(file, filesize);

        if (!file) {
            throw runtime_error("Error reading file.");
        }
        qsort(sortary, (filesize / sizeof(int)) / 2, 2 * INT_SIZE, cmp2it);
    }

    unsigned long numel = filesize / INT_SIZE;
    i = 0;
    fcnt = 0;
    for (j = 0; j < numfreq; j++) {
        if (args.use_seq) k = 0;
        else k = j + 1;
        for (; k < numfreq; k++) {
            fcnt = 0;
            if (filesize > 0 && i < numel) {
                while (i < numel &&
                       j == freqidx[sortary[i]] && k == freqidx[sortary[i + 1]]) {
                    fcnt += 256;
                    i += 2;
                }
            }
            if (args.use_seq) fcnt += (int) cntary[j * numfreq + k];
            else {
                lit = j;
                lit = (offsets[lit] - lit - 1);
                fcnt += (int) cntary[lit + k];
            }

            if (fcnt >= args.MINSUPPORT) {
                if (args.write_only_fcnt) {
                    ofd.write((char *) &fcnt, INT_SIZE);
                } else {
                    itbuf[0] = backidx[j];
                    itbuf[1] = backidx[k];
                    itbuf[2] = fcnt;
                    ofd.write((char *) itbuf, 3 * sizeof(int));
                }
                l2cnt++;
            }
        }
    }
    if (filesize > 0) delete[] sortary;
}


void Exttpose::process_cust(int fcnt, fstream &seqfd, fstream &isetfd) {
    int j, k;
    int ii1, ii2, lit;

    for (k = 0; k < fcnt; k++) {
        for (j = k; j < fcnt; j++) {
            if (args.use_seq && extary[fidx[j]].size() > 0) {
                lit = extary[fidx[j]].size() - 1;
                if (extary[fidx[k]][0] < extary[fidx[j]][lit]) {
                    if ((++seq2[fidx[k] * numfreq + fidx[j]]) == 0) {
                        seqfd.write((char *) &backidx[fidx[k]], INT_SIZE);
                        seqfd.write((char *) &backidx[fidx[j]], INT_SIZE);
                    }
                }
            }
            if (j > k) {
                if (fidx[k] < fidx[j]) {
                    ii1 = fidx[k];
                    ii2 = fidx[j];
                } else {
                    ii2 = fidx[k];
                    ii1 = fidx[j];
                }
                lit = offsets[ii1] - ii1 - 1;
                if (ocust[lit + ii2] == 1) {
                    if ((++itcnt2[lit + ii2]) == 0) {
                        isetfd.write((char *) &backidx[ii1], INT_SIZE);
                        isetfd.write((char *) &backidx[ii2], INT_SIZE);
                        //itcnt2[lit+ii2] = 0;
                    }
                    ocust[lit + ii2] = 0;
                }

                if (extary[fidx[k]].size() > 0) {
                    lit = extary[fidx[k]].size() - 1;
                    if (extary[fidx[j]][0] < extary[fidx[k]][lit]) {
                        if ((++seq2[fidx[j] * numfreq + fidx[k]]) == 0) {
                            seqfd.write((char *) &backidx[fidx[j]], INT_SIZE);
                            seqfd.write((char *) &backidx[fidx[k]], INT_SIZE);
                        }
                    }
                }
            }
        }
        extary[fidx[k]].reset();
    }
}

void Exttpose::do_invert_db(int pblk, int mincustid, int maxcustid) {
    int numitem = 0, tid = 0, custid = 0; // DD: to avoid gcc warning uninitialized var
    int *buf = nullptr; // DD: to avoid gcc warning uninitialized var
    ostringstream tmpname;
    int fd;
    int i, j, k, idx;

    dcb.get_first_blk();
    dcb.get_next_trans(buf, numitem, tid, custid);
    int ocid;// = -1;
    for (int p = 0; p < args.num_partitions; p++) {
        tmpname << args.output;

        if (args.num_partitions > 1) {
            tmpname << ".P" << p;
        }
        
        string tmpnam = tmpname.str();
        tmpname.str(string());

        if ((fd = open(tmpnam.c_str(), (O_WRONLY | O_CREAT | O_TRUNC | O_BINARY), 0666)) < 0) {
            throw runtime_error("Can't open out file");
        }

        for (i = 0; i < numfreq; i++) {
            extary[i].reset();
        }
        //count 2-itemsets
        int plb = p * pblk + mincustid;
        int pub = plb + pblk;
        if (pub >= maxcustid) pub = maxcustid + 1;
        env.logger << "BOUNDS " << plb << " " << pub << endl;
        int fcnt;
        for (; !dcb.eof() && custid < pub;) {
            fcnt = 0;
            ocid = custid;
            //env.logger << "TID " << custid << " " << tid << " " << numitem << endl;
            while (!dcb.eof() && ocid == custid && custid < pub) {
                //for (k=0; k < numitem; k++){

                // }

                if (args.use_diff) {
                    //add this tid to all items not in the trans
                    k = 0;
                    for (j = 0; j < numitem; j++) {
                        if (freqidx[buf[j]] == -1) continue;

                        while (backidx[k] < buf[j]) {
                            //if ((idx = freqidx[backidx[k]]) != -1){
                            idx = k;
                            if (!args.use_newformat)
                                extary[idx].add(fd, tid, args.use_seq, p);
                            else extary[idx].add(fd, tid, args.use_seq, p, custid);
                            //}
                            k++;
                        }
                        k++; //skip over buf[j]
                    }
                    for (; k < numfreq; k++) {
                        //if ((idx = freqidx[backidx[k]]) != -1){
                        idx = k;
                        if (!args.use_newformat)
                            extary[idx].add(fd, tid, args.use_seq, p);
                        else extary[idx].add(fd, tid, args.use_seq, p, custid);
                        //}
                    }
                } else {
                    // add this tid to all items in the trans
                    for (j = 0; j < numitem; j++) {
                        idx = freqidx[buf[j]];
                        if (idx != -1) {
                            if (!args.use_newformat) {
                                if (args.use_seq && extary[idx].flg() == 0) {
                                    fidx[fcnt] = idx;
                                    fcnt++;
                                    extary[idx].setflg(1);
                                    extary[idx].add(fd, tid, args.use_seq, p, custid);
                                } else {
                                    extary[idx].add(fd, tid, args.use_seq, p);
                                }
                            } else {
                                extary[idx].add(fd, tid, args.use_seq, p, custid);
                            }
                        }
                    }
                }

                dcb.get_next_trans(buf, numitem, tid, custid);
            }
            if (!args.use_newformat && args.use_seq) {
                for (k = 0; k < fcnt; k++) {
                    extary[fidx[k]].setlastpos();
                    extary[fidx[k]].setflg(0);
                }
                fcnt = 0;
            }
        }

        for (i = 0; i < numfreq; i++) {
            //env.logger << "FLUSH " << i << " " << extary[i].lastPos << " " <<
            //   extary[i].theSize << endl;
            extary[i].flushbuf(fd, args.use_seq, p);
        }
        close(fd);
    }
    env.logger << "WROTE INVERT " << endl;
}

Exttpose::Exttpose(Env &env_, ExttposeArgument &args_) : env(env_), args(args_) {
    dcb.init(args.input);
    itcnt.resize(args.DBASE_MAXITEM);
    ocnt.resize(args.DBASE_MAXITEM, -1);
    itlen.resize(args.DBASE_MAXITEM);
    freqidx.resize(args.DBASE_MAXITEM);
}

void Exttpose::tpose() {
    int i, j, l;
    int idx;
    int custid, tid, numitem, fcnt;
    ofstream ofd;
    int sumsup = 0, sumdiff = 0;

    //count 1 items
    int *buf;
    dcb.get_first_blk();
    dcb.get_next_trans(buf, numitem, tid, custid);
    int mincustid = custid;
    while (!dcb.eof()) {
        //env.logger << custid << " " << tid << " " << numitem;
        for (j = 0; j < numitem; j++) {
            //env.logger << " " << buf[j] << flush;
            itlen[buf[j]]++;
            if (args.use_seq && ocnt[buf[j]] != custid) {
                itcnt[buf[j]]++;
                ocnt[buf[j]] = custid;
            }
            //if (buf[j] == 17) env.logger << " " << tid;
        }
        //env.logger << endl;
        dcb.get_next_trans(buf, numitem, tid, custid);
    }
    //env.logger << endl;
    int maxcustid = static_cast<int>(custid);
    env.logger << "MINMAX " << mincustid << " " << maxcustid << endl;

    for (i = 0; i < args.DBASE_MAXITEM; i++) {
        if (args.use_seq) {
            if (itcnt[i] >= args.MINSUPPORT) {
                env.logger << i << " SUPP " << itcnt[i] << endl;
                freqidx[i] = numfreq;
                numfreq++;
            } else freqidx[i] = static_cast<int>(-1);
        } else {
            if (itlen[i] >= args.MINSUPPORT) {
                freqidx[i] = numfreq;
                numfreq++;
                sumsup += itlen[i];
                sumdiff += (args.DBASE_NUM_TRANS - itlen[i]);
            } else freqidx[i] = static_cast<int>(-1);
        }
        //if (i == 17) env.logger << " 17 SUP " << itlen[17] << endl;
    }

    backidx.resize(numfreq);

    numfreq = 0;
    for (i = 0; i < args.DBASE_MAXITEM; i++) {
        if (args.use_seq) {
            if (itcnt[i] >= args.MINSUPPORT)
                backidx[numfreq++] = i;
        } else {
            if (itlen[i] >= args.MINSUPPORT)
                backidx[numfreq++] = i;
        }
    }

    env.logger << "numfreq " << numfreq << " :  " << " SUMSUP SUMDIFF = " << sumsup << " " << sumdiff << endl;

    env.summary << " F1stats = [ " << numfreq << " " << sumsup << " " << sumdiff << " ]";

    if (numfreq == 0) return;

    long extarysz = args.AMEM / numfreq;
    extarysz /= INT_SIZE;
    env.logger << "EXTRARYSZ " << extarysz << endl;
    if (extarysz < 2) extarysz = 2;

    extary.reserve(numfreq);

    for (i = 0; i < numfreq; i++) {
        extary.emplace_back(extarysz, args.num_partitions);
    }

    ostringstream tmpnamestrm;
    int plb, pub, pblk;
    pblk = static_cast<int>(ceil(((double) (maxcustid - mincustid + 1)) / args.num_partitions));
    if (args.do_invert) {
        if (args.num_partitions > 1) {
            dcb.get_first_blk();
            dcb.get_next_trans(buf, numitem, tid, custid);
        }
        for (j = 0; j < args.num_partitions; j++) {
            //construct offsets for 1-itemsets
            tmpnamestrm << args.idxfn;
            if (args.num_partitions > 1) {
                tmpnamestrm << ".P" << j;
                plb = j * pblk + mincustid;
                pub = plb + pblk;
                if (pub > maxcustid) pub = maxcustid + 1;
                std::fill(itcnt.begin(), itcnt.end(), 0);
                std::fill(ocnt.begin(), ocnt.end(), -1);
                std::fill(itlen.begin(), itlen.end(), 0);

                for (; !dcb.eof() && custid < pub;) {
                    for (i = 0; i < numitem; i++) {
                        itlen[buf[i]]++;
                        if (args.use_seq && ocnt[buf[i]] != custid) {
                            itcnt[buf[i]]++;
                            ocnt[buf[i]] = custid;
                        }
                    }
                    dcb.get_next_trans(buf, numitem, tid, custid);
                }
            }
            string tmpnam = tmpnamestrm.str();
            env.logger << "OPENED " << tmpnam << endl;
            ofd.open(tmpnam, ios::binary);
            if (!ofd) {
                throw runtime_error("Can't open file " + tmpnam);
            }

            int file_offset = 0;
            int null = -1;
            for (i = 0; i < args.DBASE_MAXITEM; i++) {
                //if (i == 17) env.logger << "LIDX " << i << " " << itlen[i] << endl;
                if (freqidx[i] != -1) {
                    ofd.write((char *) &file_offset, INT_SIZE);
                    extary[freqidx[i]].set_offset(file_offset, j);
                    if (args.use_seq) {
                        if (args.use_newformat) file_offset += (2 * itlen[i]);
                        else file_offset += (2 * itcnt[i] + itlen[i]);
                    } else {
                        if (args.use_diff) file_offset += (args.DBASE_NUM_TRANS - itlen[i]);
                        else file_offset += itlen[i];
                    }
                } else if (args.no_minus_off) {
                    ofd.write((char *) &file_offset, INT_SIZE);
                } else ofd.write((char *) &null, INT_SIZE);
                //env.logger << "OFF " << i <<" " << file_offset << endl;
            }
            env.logger << "OFF " << i << " " << file_offset << endl;
            ofd.write((char *) &file_offset, INT_SIZE);
            ofd.close();
        }
    }

    env.logger << "Wrote Offt " << endl;

    fidx.resize(numfreq);

    int ocid = -1;
    if (args.do_l2) {
        fstream isetfd;
        fstream seqfd;
        string tmpseq, tmpiset;

        if (args.use_seq) {
            tmpseq = args.tmpfn;
            seqfd.open(tmpseq, ios::binary | ios::trunc);
            if (!seqfd.is_open()) {
                throw runtime_error("Can't open out file");
            }
        }
        tmpiset = args.tmpfn + "iset";
        isetfd.open(tmpiset, ios::binary | ios::trunc);

        if (!isetfd.is_open()) {
            throw runtime_error("Can't open out file");
        }

        if (args.use_seq) {
            seq2.reserve(numfreq * numfreq);
        }

        itcnt2.resize(numfreq * (numfreq - 1) / 2);
        ocust.resize(numfreq * (numfreq - 1) / 2);
        offsets.resize(numfreq);

        int offt = 0;
        int start = static_cast<int>(numfreq);
        for (i = start - 1; i >= 0; i--) {
            offsets[numfreq - i - 1] = offt;
            offt += i;
        }

        ocid = -1;
        int lit;
        //count 2-itemsets
        dcb.get_first_blk();
        dcb.get_next_trans(buf, numitem, tid, custid);
        while (!dcb.eof()) {
            fcnt = 0;
            ocid = custid;
            while (!dcb.eof() && ocid == custid) {
                for (j = 0; j < numitem; j++) {
                    idx = freqidx[buf[j]];
                    if (idx != -1) {
                        if (args.use_seq) {
                            if (extary[idx].size() == 0) {
                                fidx[fcnt] = idx;
                                fcnt++;
                                //extary[idx].add(isetfd,tid,args.use_seq,0);
                                //extary[idx].add(isetfd,tid,args.use_seq,0);
                                extary[idx].setitem(0, tid);
                                extary[idx].setitem(1, tid);
                                extary[idx].setsize(2);
                            } else {
                                extary[idx].setitem(1, tid);
                            }

                            lit = offsets[idx] - idx - 1;
                            for (l = j + 1; l < numitem; l++) {
                                if (freqidx[buf[l]] != -1) {
                                    ocust[lit + freqidx[buf[l]]] = 1;
                                }
                            }
                        } else {
                            lit = offsets[idx] - idx - 1;
                            for (l = j + 1; l < numitem; l++) {
                                if (freqidx[buf[l]] != -1) {
                                    if ((++itcnt2[lit + freqidx[buf[l]]]) == 0) {
                                        isetfd.write((char *) &buf[j], INT_SIZE);
                                        isetfd.write((char *) &buf[l], INT_SIZE);
                                    }
                                }
                            }
                        }
                    }
                }
                dcb.get_next_trans(buf, numitem, tid, custid);
            }

            if (args.use_seq) {
                process_cust(fcnt, seqfd, isetfd);
            }
        }
        ocust.clear();
        env.logger << "2-IT " << " " << endl;

        //write 2-itemsets counts to file
        int l2cnt = 0;
        if (args.use_seq) {
            ofd.open(args.seqfn, ios::binary);
            if (ofd.fail()) {
                throw runtime_error("Can't open seq file");
            }
            sort_get_l2(l2cnt, seqfd, ofd, seq2, true);

            ofd.close();
            env.logger << "SEQ2 cnt " << l2cnt << endl;
            env.summary << " " << l2cnt;
        }
        int seqs = l2cnt;

        ofd.open(args.it2fn, ios::binary);
        //if ((fd = open(args.it2fn, (O_WRONLY|O_CREAT|O_TRUNC|O_BINARY), 0666)) < 0){
        if (ofd.fail()) {
            throw runtime_error("Can't open it2 file");
        }
        sort_get_l2(l2cnt, isetfd, ofd, itcnt2, false);
        ofd.close();
        env.logger << "SORT " << l2cnt << "  " << endl;

        env.summary << " F2stats = [" << l2cnt << " " << seqs << " ]";
        offsets.clear();
        itcnt2.clear();
        seq2.clear();
    }

    if (args.do_invert) {
        do_invert_db(pblk, mincustid, maxcustid);
    }

    freqidx.clear();
    backidx.clear();
}


result_t exttposeFunc(Env &env, ExttposeArgument &args) {
    env.logger << "CONF " << args.DBASE_NUM_TRANS << " " << args.DBASE_MAXITEM << " "
               << args.DBASE_AVG_TRANS_SZ << " " << args.DBASE_AVG_CUST_SZ << endl;

    if (args.use_diff) {
        env.logger << "SEQ TURNED OFF and PARTITIONS = 1\n";
    }

    args.MINSUPPORT = lround(args.MINSUP_PER * args.DBASE_NUM_TRANS + 0.5);

    //ensure that support is at least 2
    if (!args.write_only_fcnt && args.MINSUPPORT < 1) args.MINSUPPORT = 1;
    env.logger << "args.MINSUPPORT " << args.MINSUPPORT << " " << args.DBASE_NUM_TRANS << endl;

    env.summary << "TPOSE";
    if (args.use_diff) env.summary << " DIFF";
    if (args.use_seq) env.summary << " SEQ";
    if (!args.do_invert) env.summary << " NOINVERT";
    if (!args.do_l2) env.summary << " NOF2";
    env.summary << " " << args.input << " " << args.MINSUP_PER << " " << args.DBASE_NUM_TRANS << " "
                << args.MINSUPPORT << " " << args.num_partitions;

    Exttpose exttpose(env, args);
    exttpose.tpose();

    result_t result;
    result.logger = env.logger.str();
    result.summary = env.summary.str();
    return result;
}

result_t exttposeWrapper(const string &s, shared_ptr<Env>& envptr) {
    args_t args_ = parse(s);
    ExttposeArgument args;
    args.parse_args(args_.argc, args_.argv);
    if (envptr == nullptr) {
        Env env;
        return exttposeFunc(env, args);
    }
    else {
        return exttposeFunc(*envptr, args);
    }
}