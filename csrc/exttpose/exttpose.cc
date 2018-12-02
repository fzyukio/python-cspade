#include <cerrno>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <cstring>
#include <cmath>

#include "calcdb.h"
#include "Array.h"

#include "../funcs.h"
#include "../utils.h"

using namespace std;

extern char *optarg;
extern int optind;

namespace utility {
    namespace exttpose {

#define MEG (1024*1204)
        struct timeval tp;
#define seconds(tm) gettimeofday(&tp,(struct timezone *)0);\
tm=tp.tv_sec+tp.tv_usec/1000000.0

        char input[300];       //input file name
        char output[300];       //output file name
        char idxfn[300];
        char inconfn[300];
        char it2fn[300];
        char seqfn[300];
        double MINSUP_PER = 0.0;
        int MINSUPPORT = 0;
        int do_invert = 1;
        int do_l2 = 1;
        int use_seq = 1;
        int write_only_fcnt = 1;
        char use_newformat = 1;
        int num_partitions = 1;
        char no_minus_off = 0;

        char use_diff = 0;

        typedef unsigned char CHAR;
        long AMEM = 32 * MEG;

        int DBASE_NUM_TRANS; //tot trans for assoc, num cust for sequences
        int DBASE_MAXITEM;   //number of items
        float DBASE_AVG_TRANS_SZ; //avg trans size
        float DBASE_AVG_CUST_SZ = 0; //avg cust size for sequences
        int DBASE_TOT_TRANS; //tot trans for sequences

        double tpose_time, offt_time, l2time;

        void parse_args(int argc, char **argv) {
            char c;
            optind = 1;

            if (argc < 2)
                cout << "usage: assocFB -i<infile> -o<outfile>\n";
            else {
                while ((c = getopt(argc, argv, "i:o:p:s:a:dvlfm:x")) != -1) {
                    switch (c) {
                        case 'i': //input files
                            sprintf(input, "%s.data", optarg);
                            sprintf(inconfn, "%s.conf", optarg);
                            break;
                        case 'o': //output file names
                            sprintf(output, "%s.tpose", optarg);
                            sprintf(idxfn, "%s.idx", optarg);
                            sprintf(it2fn, "%s.2it", optarg);
                            sprintf(seqfn, "%s.2seq", optarg);
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
                    }
                }
            }

            c = open(inconfn, O_RDONLY);
            if (c < 0) {
                throw runtime_error("ERROR: invalid conf file\n");
            }
            if (use_seq) {
                read(c, (char *) &DBASE_NUM_TRANS, ITSZ);
                read(c, (char *) &DBASE_MAXITEM, ITSZ);
                read(c, (char *) &DBASE_AVG_CUST_SZ, sizeof(float));
                read(c, (char *) &DBASE_AVG_TRANS_SZ, sizeof(float));
                read(c, (char *) &DBASE_TOT_TRANS, ITSZ);
                write_only_fcnt = 0;
            } else {
                read(c, (char *) &DBASE_NUM_TRANS, ITSZ);
                read(c, (char *) &DBASE_MAXITEM, ITSZ);
                read(c, (char *) &DBASE_AVG_TRANS_SZ, sizeof(float));
            }
            logger << "CONF " << DBASE_NUM_TRANS << " " << DBASE_MAXITEM << " " <<
                 DBASE_AVG_TRANS_SZ << " " << DBASE_AVG_CUST_SZ << endl;

            close(c);

            if (use_diff) {
                use_seq = 0;
                num_partitions = 1;
                logger << "SEQ TURNED OFF and PARTITIONS = 1\n";
            }

        }

        int cmp2it(const void *a, const void *b) {
            int *ary = (int *) a;
            int *bry = (int *) b;
            if (ary[0] < bry[0]) return -1;
            else if (ary[0] > bry[0]) return 1;
            else {
                if (ary[1] < bry[1]) return -1;
                else if (ary[1] > bry[1]) return 1;
                else return 0;
            }
        }

        int intcmp(const void *a, const void *b) {
            int ary = *(int *) a;
            int bry = *(int *) b;
            if (ary < bry) return -1;
            else if (ary > bry) return 1;
            else return 0;
        }

// void write_middle_range(int &begi, int &begj, int endi, int endj, CHAR *cntary,
//                         ofstream &ofd, int *bidx, int numfreq, int *offsets,
//                         int &l2cnt, char seqflg)
// {
//    int i, j, ej;
//    int lit, idx;
//    if (MINSUPPORT >= 256) return;

//    for (i=begi; i <= endi; i++){
//       if (i == begi) j = begj;
//       else {
//          if (seqflg) j = 0;
//          else j = i+1;
//       }
//       if (i == endi) ej = endj;
//       else ej = numfreq-1;
//       if (!seqflg) idx = offsets[i]-i-1;
//       for (; j <= ej; j++){
//          if (seqflg){
//             lit = (int) cntary[i*numfreq+j];
//             cntary[i*numfreq+j] = 0;
//          }
//          else if (j>i){
//             lit = (int) cntary[idx+j];
//             cntary[idx+j] = 0;
//          }
//          else lit = 0;
//          if (lit >= MINSUPPORT){
//             ofd.write((char *)&bidx[i], ITSZ);
//             ofd.write((char *)&bidx[j], ITSZ);
//             ofd.write((char *)&lit, ITSZ);
//             l2cnt++;
//             //cout << bidx[i] << " " << bidx[j] << " " << lit << endl;
//          }
//       }
//    }
//    if (endj == numfreq-1){
//       begi = endi+1;
//       if (seqflg) begj=0;
//       else begj = begi+1;
//    }
//    else{
//       begi = endi;
//       begj = endj+1;
//    }
// }

// void sort_get_l2(int &l2cnt, int fd, ofstream &ofd, int *backidx, int *freqidx,
//                  int numfreq, int *offsets, CHAR *cntary, char seqflg)
// {
//    //write 2-itemsets counts to file

//    int i, fcnt, lit;
//    long sortflen;
//    int *sortary;
//    int itbuf[3];
//    int begi, begj;

//    sortflen = lseek(fd, 0, SEEK_CUR);
//    if (sortflen < 0){
//       throw runtime_error("SEEK SEQ");
//       exit(errno);
//    }
//    cout << "SORT " << sortflen << endl;
//    if (sortflen > 0){
// #ifdef SGI
//       sortary = (int *) mmap((char *)NULL, sortflen,
//                              (PROT_READ|PROT_WRITE),
//                              MAP_PRIVATE, fd, 0);
// #else
//       sortary = (int *) mmap((char *)NULL, sortflen,
//                              (PROT_READ|PROT_WRITE),
//                              (MAP_FILE|MAP_VARIABLE|MAP_PRIVATE), fd, 0);
// #endif
//       if (sortary == (int *)-1){
//          throw runtime_error("SEQFd MMAP ERROR");
//          exit(errno);
//       }

//       qsort(sortary, (sortflen/sizeof(int))/2, 2*sizeof(int), cmp2it);

//       itbuf[0] = itbuf[1] = -1;
//       begi = begj = 0;
//       fcnt = 0;
//       for (i=0; i < (sortflen/sizeof(int)); i+=2){
//          if (itbuf[0] != -1){
//             write_middle_range(begi, begj, freqidx[itbuf[0]], freqidx[itbuf[1]],
//                                cntary, ofd, backidx, numfreq, offsets,
//                                l2cnt, seqflg);
//          }         
//          if (itbuf[0] != sortary[i] || itbuf[1] != sortary[i+1]){
//             if (fcnt >= MINSUPPORT){
//                itbuf[2] = fcnt;
//                ofd.write((char *)itbuf, 3*ITSZ);
//                //cout << itbuf[0] << " " << itbuf[1] << " " << fcnt << endl;
//                l2cnt++;
//             }
//             itbuf[0] = sortary[i];
//             itbuf[1] = sortary[i+1];
//             if (seqflg){
//                fcnt = (int) cntary[freqidx[itbuf[0]]*numfreq+freqidx[itbuf[1]]];
//                cntary[freqidx[itbuf[0]]*numfreq+freqidx[itbuf[1]]] = 0;
//             }
//             else{
//                lit = freqidx[itbuf[0]];
//                lit = (offsets[lit]-lit-1);
//                fcnt = (int) cntary[lit+freqidx[itbuf[1]]];
//                cntary[lit+freqidx[itbuf[1]]] = 0;
//             }
//          }
//          fcnt += 256;
//       }
//       write_middle_range(begi, begj, freqidx[itbuf[0]], freqidx[itbuf[1]],
//                          cntary, ofd, backidx, numfreq, offsets, l2cnt, seqflg);
//       if (fcnt >= MINSUPPORT){
//          itbuf[2] = fcnt;
//          ofd.write((char *)itbuf, 3*ITSZ);
//          //cout << itbuf[0] << " " << itbuf[1] << " " << fcnt << endl;
//          l2cnt++;
//       }
//       write_middle_range(begi, begj, numfreq-1, numfreq-1, cntary, ofd, backidx,
//                          numfreq, offsets, l2cnt, seqflg);
//       munmap((caddr_t)sortary, sortflen);
//    }
//    else{
//       begi = begj = 0;
//       write_middle_range(begi, begj, numfreq-1, numfreq-1, cntary, ofd, backidx,
//                          numfreq, offsets, l2cnt, seqflg);
//    }
// }
        void sort_get_l2(int &l2cnt, int fd, ofstream &ofd, int *backidx, int *freqidx,
                         int numfreq, int *offsets, CHAR *cntary, char use_seq) {
            //write 2-itemsets counts to file

            int i, j, k, fcnt;
            int lit;
            long sortflen;
            int *sortary;
            int itbuf[3];

            sortflen = lseek(fd, 0, SEEK_CUR);
            if (sortflen < 0) {
                throw runtime_error("SEEK SEQ");
            }
            //cout << "SORT " << sortflen << endl;
            if (sortflen > 0) {
#ifdef DEC
                sortary = (int *) mmap((char *)NULL, sortflen,
                                       (PROT_READ|PROT_WRITE),
                                       (MAP_FILE|MAP_VARIABLE|MAP_PRIVATE), fd, 0);
#else
                sortary = (int *) mmap((char *) NULL, sortflen,
                                       (PROT_READ | PROT_WRITE),
                                       MAP_PRIVATE, fd, 0);
#endif
                if (sortary == (int *) -1) {
                    throw runtime_error("SEQFd MMAP ERROR");
                }

                qsort(sortary, (sortflen / sizeof(int)) / 2, 2 * sizeof(int), cmp2it);
            }

            int numel = sortflen / sizeof(int);
            i = 0;
            fcnt = 0;
            for (j = 0; j < numfreq; j++) {
                if (use_seq) k = 0;
                else k = j + 1;
                for (; k < numfreq; k++) {
                    fcnt = 0;
                    if (sortflen > 0 && i < numel) {
                        while (i < numel &&
                               j == freqidx[sortary[i]] && k == freqidx[sortary[i + 1]]) {
                            fcnt += 256;
                            i += 2;
                        }
                    }
                    if (use_seq) fcnt += (int) cntary[j * numfreq + k];
                    else {
                        lit = j;
                        lit = (offsets[lit] - lit - 1);
                        fcnt += (int) cntary[lit + k];
                    }

                    if (fcnt >= MINSUPPORT) {
                        if (write_only_fcnt) {
                            ofd.write((char *) &fcnt, ITSZ);
                        } else {
                            itbuf[0] = backidx[j];
                            itbuf[1] = backidx[k];
                            itbuf[2] = fcnt;
                            ofd.write((char *) itbuf, 3 * ITSZ);
                        }
                        //cout << backidx[j] << ((use_seq)?" -> ":" ")
                        //     << backidx[k] << " SUPP " << fcnt << endl;
                        l2cnt++;
                    }
                }
            }
            if (sortflen > 0) munmap((caddr_t) sortary, sortflen);
        }


        void process_cust(int *fidx, int fcnt, int numfreq, int *backidx,
                          Array **extary, CHAR *seq2, CHAR *itcnt2, char *ocust,
                          int *offsets, int seqfd, int isetfd) {
            int j, k, lit;
            int ii1, ii2;

//    qsort(fidx, fcnt, sizeof(int), intcmp);
//    cout << "FIDX ";
//    for (k=0; k < fcnt; k++)
//       cout << " " << backidx[fidx[k]];
//    cout << endl;
//    for (k=0; k < fcnt; k++){
//       cout << backidx[fidx[k]] << ": ";
//       for (j=k+1; j < fcnt; j++){
//          cout << " " << (int) ocust[offsets[fidx[k]]-fidx[k]-1+fidx[j]];
//       }
//       cout << endl;
//    }

            for (k = 0; k < fcnt; k++) {
                for (j = k; j < fcnt; j++) {
                    if (use_seq && extary[fidx[j]]->size() > 0) {
                        lit = extary[fidx[j]]->size() - 1;
                        if ((*extary[fidx[k]])[0] < (*extary[fidx[j]])[lit]) {
                            if ((++seq2[fidx[k] * numfreq + fidx[j]]) == 0) {
                                write(seqfd, (char *) &backidx[fidx[k]], ITSZ);
                                write(seqfd, (char *) &backidx[fidx[j]], ITSZ);
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
                                write(isetfd, (char *) &backidx[ii1], ITSZ);
                                write(isetfd, (char *) &backidx[ii2], ITSZ);
                                //itcnt2[lit+ii2] = 0;
                            }
                            ocust[lit + ii2] = 0;
                        }

                        if (extary[fidx[k]]->size() > 0) {
                            lit = extary[fidx[k]]->size() - 1;
                            if ((*extary[fidx[j]])[0] < (*extary[fidx[k]])[lit]) {
                                if ((++seq2[fidx[j] * numfreq + fidx[k]]) == 0) {
                                    write(seqfd, (char *) &backidx[fidx[j]], ITSZ);
                                    write(seqfd, (char *) &backidx[fidx[k]], ITSZ);
                                }
                            }
                        }
                    }
                }
                extary[fidx[k]]->reset();
            }
        }

        void do_invert_db(ExtDatabase *DCB, int pblk, Array **extary, int numfreq,
                          int *freqidx, int *backidx, int *fidx,
                          int mincustid, int maxcustid) {
            double t1, t2;
            seconds(t1);
            int numitem, tid, custid;
            int *buf;
            char tmpnam[300];
            int i, j, k;
            int fd;
            int idx;

            DCB->get_first_blk();
            DCB->get_next_trans(buf, numitem, tid, custid);
            int ocid;// = -1;
            for (int p = 0; p < num_partitions; p++) {
                if (num_partitions > 1) sprintf(tmpnam, "%s.P%d", output, p);
                else sprintf(tmpnam, "%s", output);
                if ((fd = open(tmpnam, (O_WRONLY | O_CREAT | O_TRUNC), 0666)) < 0) {
                    throw runtime_error("Can't open out file");
                }

                for (i = 0; i < numfreq; i++) {
                    extary[i]->reset();
                }
                //count 2-itemsets
                int plb = p * pblk + mincustid;
                int pub = plb + pblk;
                if (pub >= maxcustid) pub = maxcustid + 1;
                logger << "BOUNDS " << plb << " " << pub << endl;
                int fcnt;
                for (; !DCB->eof() && custid < pub;) {
                    fcnt = 0;
                    ocid = custid;
                    //cout << "TID " << custid << " " << tid << " " << numitem << endl;
                    while (!DCB->eof() && ocid == custid && custid < pub) {
                        //for (k=0; k < numitem; k++){

                        // }

                        if (use_diff) {
                            //add this tid to all items not in the trans
                            k = 0;
                            for (j = 0; j < numitem; j++) {
                                if (freqidx[buf[j]] == -1) continue;

                                while (backidx[k] < buf[j]) {
                                    //if ((idx = freqidx[backidx[k]]) != -1){
                                    idx = k;
                                    if (!use_newformat)
                                        extary[idx]->add(fd, tid, use_seq, p);
                                    else extary[idx]->add(fd, tid, use_seq, p, custid);
                                    //}
                                    k++;
                                }
                                k++; //skip over buf[j]
                            }
                            for (; k < numfreq; k++) {
                                //if ((idx = freqidx[backidx[k]]) != -1){
                                idx = k;
                                if (!use_newformat)
                                    extary[idx]->add(fd, tid, use_seq, p);
                                else extary[idx]->add(fd, tid, use_seq, p, custid);
                                //}
                            }
                        } else {
                            // add this tid to all items in the trans
                            for (j = 0; j < numitem; j++) {
                                idx = freqidx[buf[j]];
                                if (idx != -1) {
                                    if (!use_newformat) {
                                        if (use_seq && extary[idx]->flg() == 0) {
                                            fidx[fcnt] = idx;
                                            fcnt++;
                                            extary[idx]->setflg(1);
                                            extary[idx]->add(fd, tid, use_seq, p, custid);
                                        } else {
                                            extary[idx]->add(fd, tid, use_seq, p);
                                        }
                                    } else {
                                        extary[idx]->add(fd, tid, use_seq, p, custid);
                                    }
                                }
                            }
                        }

                        DCB->get_next_trans(buf, numitem, tid, custid);
                    }
                    if (!use_newformat && use_seq) {
                        for (k = 0; k < fcnt; k++) {
                            extary[fidx[k]]->setlastpos();
                            extary[fidx[k]]->setflg(0);
                        }
                        fcnt = 0;
                    }
                }

                for (i = 0; i < numfreq; i++) {
                    //cout << "FLUSH " << i << " " << extary[i]->lastPos << " " <<
                    //   extary[i]->theSize << endl;
                    extary[i]->flushbuf(fd, use_seq, p);
                }
                close(fd);
            }
            seconds(t2);
            logger << "WROTE INVERT " << t2 - t1 << endl;
            tpose_time = t2 - t1;
        }

        void tpose() {
            int i, j, l;
            int idx;
            int custid, tid, numitem, fcnt;
            ofstream ofd;
            double t1, t2;
            int sumsup = 0, sumdiff = 0;


            //read original Dbase config info
            ExtDatabase *DCB = new ExtDatabase(input);

            int *itcnt = new int[DBASE_MAXITEM];
            int *ocnt = new int[DBASE_MAXITEM];
            int *itlen = new int[DBASE_MAXITEM];
            bzero((char *) itcnt, ((DBASE_MAXITEM) * ITSZ));
            //bzero((char *)ocnt, ((DBASE_MAXITEM)*ITSZ));
            for (i = 0; i < DBASE_MAXITEM; i++) ocnt[i] = -1;
            bzero((char *) itlen, ((DBASE_MAXITEM) * ITSZ));

            seconds(t1);
            //count 1 items
            int *buf;
            DCB->get_first_blk();
            DCB->get_next_trans(buf, numitem, tid, custid);
            int mincustid = custid;
            while (!DCB->eof()) {
                //cout << custid << " " << tid << " " << numitem;
                for (j = 0; j < numitem; j++) {
                    //cout << " " << buf[j] << flush;
                    itlen[buf[j]]++;
                    if (use_seq && ocnt[buf[j]] != custid) {
                        itcnt[buf[j]]++;
                        ocnt[buf[j]] = custid;
                    }
                    //if (buf[j] == 17) cout << " " << tid;
                }
                //cout << endl;
                DCB->get_next_trans(buf, numitem, tid, custid);
            }
            //cout << endl;
            int maxcustid = custid;
            logger << "MINMAX " << mincustid << " " << maxcustid << endl;

            int *freqidx = new int[DBASE_MAXITEM];
            int numfreq = 0;
            for (i = 0; i < DBASE_MAXITEM; i++) {
                if (use_seq) {
                    if (itcnt[i] >= MINSUPPORT) {
                        logger << i << " SUPP " << itcnt[i] << endl;
                        freqidx[i] = numfreq;
                        numfreq++;
                    } else freqidx[i] = -1;
                } else {
                    if (itlen[i] >= MINSUPPORT) {
                        freqidx[i] = numfreq;
                        numfreq++;
                        sumsup += itlen[i];
                        sumdiff += (DBASE_NUM_TRANS - itlen[i]);
                    } else freqidx[i] = -1;
                }
                //if (i == 17) cout << " 17 SUP " << itlen[17] << endl;
            }
            int *backidx = new int[numfreq];
            numfreq = 0;
            for (i = 0; i < DBASE_MAXITEM; i++) {
                if (use_seq) {
                    if (itcnt[i] >= MINSUPPORT)
                        backidx[numfreq++] = i;
                } else {
                    if (itlen[i] >= MINSUPPORT)
                        backidx[numfreq++] = i;
                }
            }

            seconds(t2);
            logger << "numfreq " << numfreq << " :  " << t2 - t1
                 << " SUMSUP SUMDIFF = " << sumsup << " " << sumdiff << endl;

            if (numfreq == 0) return;

            int extarysz = AMEM / numfreq;
            extarysz /= sizeof(int);
            logger << "EXTRARYSZ " << extarysz << endl;
            if (extarysz < 2) extarysz = 2;
            Array **extary = new Array *[numfreq];
            for (i = 0; i < numfreq; i++) {
                extary[i] = new Array(extarysz, num_partitions);
            }

            seconds(t1);

            char tmpnam[300];
            int plb, pub, pblk;
            pblk = (int) ceil(((double) (maxcustid - mincustid + 1)) / num_partitions);
            if (do_invert) {
                if (num_partitions > 1) {
                    DCB->get_first_blk();
                    DCB->get_next_trans(buf, numitem, tid, custid);
                }
                for (j = 0; j < num_partitions; j++) {
                    //construct offsets for 1-itemsets
                    if (num_partitions > 1) {
                        sprintf(tmpnam, "%s.P%d", idxfn, j);
                        plb = j * pblk + mincustid;
                        pub = plb + pblk;
                        if (pub > maxcustid) pub = maxcustid + 1;
                        bzero((char *) itcnt, ((DBASE_MAXITEM) * ITSZ));
                        //bzero((char *)ocnt, ((DBASE_MAXITEM)*ITSZ));
                        for (i = 0; i < DBASE_MAXITEM; i++) ocnt[i] = -1;
                        bzero((char *) itlen, ((DBASE_MAXITEM) * ITSZ));
                        for (; !DCB->eof() && custid < pub;) {
                            for (i = 0; i < numitem; i++) {
                                itlen[buf[i]]++;
                                if (use_seq && ocnt[buf[i]] != custid) {
                                    itcnt[buf[i]]++;
                                    ocnt[buf[i]] = custid;
                                }
                            }
                            DCB->get_next_trans(buf, numitem, tid, custid);
                        }
                    } else sprintf(tmpnam, "%s", idxfn);
                    //cout << "100 VAL " << itcnt[100] << endl;
                    logger << "OPENED " << tmpnam << endl;
                    ofd.open(tmpnam);
                    if (!ofd) {
                        throw runtime_error("Can't open out file");
                    }

                    int file_offset = 0;
                    int null = -1;
                    for (i = 0; i < DBASE_MAXITEM; i++) {
                        //if (i == 17) cout << "LIDX " << i << " " << itlen[i] << endl;
                        if (freqidx[i] != -1) {
                            ofd.write((char *) &file_offset, ITSZ);
                            extary[freqidx[i]]->set_offset(file_offset, j);
                            if (use_seq) {
                                if (use_newformat) file_offset += (2 * itlen[i]);
                                else file_offset += (2 * itcnt[i] + itlen[i]);
                            } else {
                                if (use_diff) file_offset += (DBASE_NUM_TRANS - itlen[i]);
                                else file_offset += itlen[i];
                            }
                        } else if (no_minus_off) {
                            ofd.write((char *) &file_offset, ITSZ);
                        } else ofd.write((char *) &null, ITSZ);
                        //cout << "OFF " << i <<" " << file_offset << endl;
                    }
                    logger << "OFF " << i << " " << file_offset << endl;
                    ofd.write((char *) &file_offset, ITSZ);
                    ofd.close();
                }
            }

            delete[] ocnt;
            delete[] itlen;
            delete[] itcnt;

            seconds(t2);
            logger << "Wrote Offt " << t2 - t1 << endl;
            offt_time = t2 - t1;

            int *fidx = new int[numfreq];
            if (fidx == NULL) {
                throw runtime_error("Can't alloc fidx");
            }

            int ocid = -1;
            if (do_l2) {
                seconds(t1);
                int seqfd, isetfd;
                if (use_seq) {
                    if ((seqfd = open("tmpseq", (O_RDWR | O_CREAT | O_TRUNC), 0666)) < 0) {
                        throw runtime_error("Can't open out file");
                    }
                }

                if ((isetfd = open("tmpiset", (O_RDWR | O_CREAT | O_TRUNC), 0666)) < 0) {
                    throw runtime_error("Can't open out file");
                }

                CHAR *seq2;
                if (use_seq) {
                    seq2 = new CHAR[numfreq * numfreq];
                    if (seq2 == NULL) {
                        throw runtime_error("SEQ MMAP ERROR");
                    }
                    //for (i=0; i < numfreq*numfreq; i++) seq2[i] = 0;
                    bzero((char *) seq2, numfreq * numfreq * sizeof(CHAR));
                }

                CHAR *itcnt2 = new CHAR[(numfreq * (numfreq - 1) / 2)];
                if (itcnt2 == NULL) {
                    throw runtime_error("ITCNT MMAP ERROR");
                }
                bzero((char *) itcnt2, (numfreq * (numfreq - 1) / 2) * sizeof(CHAR));
                //for (i=0; i < numfreq*(numfreq-1)/2; i++) itcnt2[i] = 0;
                char *ocust = new char[(numfreq * (numfreq - 1) / 2)];
                if (ocust == NULL) {
                    throw runtime_error("OCUSt MMAP ERROR");
                }
                bzero((char *) ocust, (numfreq * (numfreq - 1) / 2) * sizeof(char));
                int *offsets = new int[numfreq];
                int offt = 0;
                for (i = numfreq - 1; i >= 0; i--) {
                    offsets[numfreq - i - 1] = offt;
                    offt += i;
                }

                ocid = -1;
                int lit;
                //count 2-itemsets
                DCB->get_first_blk();
                DCB->get_next_trans(buf, numitem, tid, custid);
                while (!DCB->eof()) {
                    fcnt = 0;
                    ocid = custid;
                    while (!DCB->eof() && ocid == custid) {
                        for (j = 0; j < numitem; j++) {
                            idx = freqidx[buf[j]];
                            if (idx != -1) {
                                if (use_seq) {
                                    if (extary[idx]->size() == 0) {
                                        fidx[fcnt] = idx;
                                        fcnt++;
                                        //extary[idx]->add(isetfd,tid,use_seq,0);
                                        //extary[idx]->add(isetfd,tid,use_seq,0);
                                        extary[idx]->setitem(0, tid);
                                        extary[idx]->setitem(1, tid);
                                        extary[idx]->setsize(2);
                                    } else {
                                        extary[idx]->setitem(1, tid);
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
                                                write(isetfd, (char *) &buf[j], ITSZ);
                                                write(isetfd, (char *) &buf[l], ITSZ);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        DCB->get_next_trans(buf, numitem, tid, custid);
                    }

                    if (use_seq) {
                        process_cust(fidx, fcnt, numfreq, backidx, extary, seq2, itcnt2,
                                     ocust, offsets, seqfd, isetfd);
                    }
                }
                delete[] ocust;
                seconds(t2);
                cout << "2-IT " << t2 - t1 << " " << endl;
                l2time = t2 - t1;

                seconds(t1);
                //write 2-itemsets counts to file
                int l2cnt = 0;
                if (use_seq) {
                    ofd.open(seqfn);
                    if (ofd.fail()) {
                        throw runtime_error("Can't open seq file");
                    }
                    sort_get_l2(l2cnt, seqfd, ofd, backidx, freqidx,
                                numfreq, offsets, seq2, 1);

                    ofd.close();
                    cout << "SEQ2 cnt " << l2cnt << endl;
                }
                int seqs = l2cnt;

                ofd.open(it2fn);
                //if ((fd = open(it2fn, (O_WRONLY|O_CREAT|O_TRUNC), 0666)) < 0){
                if (ofd.fail()) {
                    throw runtime_error("Can't open it2 file");
                }
                sort_get_l2(l2cnt, isetfd, ofd, backidx, freqidx,
                            numfreq, offsets, itcnt2, 0);
                ofd.close();
                seconds(t2);
                cout << "SORT " << l2cnt << "  " << t2 - t1 << endl;

                l2time += t2 - t1;

                if (use_seq) unlink("tmpseq");
                unlink("tmpiset");
                delete[] offsets;
                delete[] itcnt2;
                if (use_seq) delete[] seq2;
            }

            if (do_invert) {
                do_invert_db(DCB, pblk, extary, numfreq, freqidx, backidx, fidx, mincustid, maxcustid);
            }

            delete[] freqidx;
            delete[] backidx;

            //for (i=0; i < numfreq; i++)
            //   cout << "LAST " << backidx[i] << " "
            //        << extary[i]->get_offset() << endl;
            delete DCB;
        }


        void exttposeFunc(int argc, char **argv) {
            double ts, te;
            seconds(ts);
            parse_args(argc, argv);

            MINSUPPORT = (int) (MINSUP_PER * DBASE_NUM_TRANS + 0.5);
            //ensure that support is at least 2
            if (!write_only_fcnt && MINSUPPORT < 1) MINSUPPORT = 1;
            logger << "MINSUPPORT " << MINSUPPORT << " " << DBASE_NUM_TRANS << endl;

            tpose();

            seconds(te);
            logger << "Total elapsed time " << te - ts << endl;
        }
    }
}

void exttposeFunc(int argc, char **argv) {
    utility::exttpose::exttposeFunc(argc, argv);
}