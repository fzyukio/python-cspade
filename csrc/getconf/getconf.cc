#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <cstring>
#include <cmath>
#include "calcdb.h"
#include "../funcs.h"
#include "../utils.h"

using namespace std;

extern char *optarg;
extern int optind;

namespace utility {
    namespace getconf {

#define ITSZ sizeof(int)

        char input[300];       //input file name
        char confn[300];
        int use_seq = 1;

        void parse_args(int argc, char **argv) {
            char c;
            optind = 1;

            if (argc < 2)
                cout << "usage: assocFB -i<infile> -o<outfile>\n";
            else {
                while ((c = getopt(argc, argv, "ai:o:")) != -1) {
                    switch (c) {
                        case 'a': //work on assoc
                            use_seq = 0;
                            printf("USE SEQ = 0\n");
                            break;
                        case 'i':
                            sprintf(input, "%s.data", optarg);
                            break;
                        case 'o':
                            sprintf(confn, "%s.conf", optarg);
                            break;
                    }
                }
            }
        }


        void getconfFunc(int argc, char **argv) {
            parse_args(argc, argv);

            int DBASE_NUM_TRANS = 0;
            int DBASE_MAXITEM = 0;
            int DBASE_NUM_CUST = 0;
            int DBASE_MINTRANS = 0;
            int DBASE_MAXTRANS = 0;
            float DBASE_AVG_TRANS_SZ = 0;
            float DBASE_AVG_CUST_SZ = 0;

            int i;

            int custid, tid, nitem;
            int *buf;
            int oldcustid = -1;
            int oldtcnt = 0;
            int tsizesum = 0;
            int tcustsum = 0;
            int tsizesq = 0;
            int maxnitem = 0;

            GcDatabase *DCB = new GcDatabase(input);
            DCB->get_first_blk();
            DCB->get_next_trans(buf, nitem, tid, custid);
            DBASE_MINTRANS = custid;
            while (!DCB->eof()) {
                //printf ("%d %d %d\n", custid, tid, nitem);
                DBASE_MAXTRANS = custid;
                if (use_seq) {
                    if (oldcustid != custid) {
                        tcustsum += DBASE_NUM_TRANS - oldtcnt;
                        oldtcnt = DBASE_NUM_TRANS;
                        DBASE_NUM_CUST++;
                        oldcustid = custid;
                    }
                }
                DBASE_NUM_TRANS++;
                tsizesum += nitem;
                if (nitem > maxnitem) maxnitem = nitem;

                tsizesq += (nitem * nitem);
                for (i = 0; i < nitem; i++)
                    if (buf[i] > DBASE_MAXITEM) DBASE_MAXITEM = buf[i];
                DCB->get_next_trans(buf, nitem, tid, custid);
            }
            tcustsum += DBASE_NUM_TRANS - oldtcnt;
            DBASE_MAXITEM++;

            if (use_seq) DBASE_AVG_CUST_SZ = (1.0 * tcustsum) / DBASE_NUM_CUST;
            DBASE_AVG_TRANS_SZ = (1.0 * tsizesum) / DBASE_NUM_TRANS;
            double trans_sq_avg = (1.0 * tsizesq) / DBASE_NUM_TRANS;
            double stddev = sqrt(trans_sq_avg -
                                 (DBASE_AVG_TRANS_SZ * DBASE_AVG_TRANS_SZ));


            //write config info to new file
            int conffd;
            if ((conffd = open(confn, (O_WRONLY | O_CREAT), 0666)) < 0) {
                throw runtime_error("Can't open out file");
            }
            if (use_seq) {
                write(conffd, (char *) &DBASE_NUM_CUST, ITSZ);
                write(conffd, (char *) &DBASE_MAXITEM, ITSZ);
                write(conffd, (char *) &DBASE_AVG_CUST_SZ, sizeof(float));
                write(conffd, (char *) &DBASE_AVG_TRANS_SZ, sizeof(float));
                write(conffd, (char *) &DBASE_NUM_TRANS, ITSZ);
                write(conffd, (char *) &DBASE_MINTRANS, ITSZ);
                write(conffd, (char *) &DBASE_MAXTRANS, ITSZ);
            } else {
                write(conffd, (char *) &DBASE_NUM_TRANS, ITSZ);
                write(conffd, (char *) &DBASE_MAXITEM, ITSZ);
                write(conffd, (char *) &DBASE_AVG_TRANS_SZ, sizeof(float));
                write(conffd, (char *) &DBASE_MINTRANS, ITSZ);
                write(conffd, (char *) &DBASE_MAXTRANS, ITSZ);
            }

            close(conffd);
            char out[255];
            sprintf(out, "CONF %d %d %f %f %d %d %d %f %d\n", DBASE_NUM_CUST, DBASE_MAXITEM,
                   DBASE_AVG_CUST_SZ, DBASE_AVG_TRANS_SZ, DBASE_NUM_TRANS,
                   DBASE_MINTRANS, DBASE_MAXTRANS, stddev, maxnitem);
            logger << out;
            delete DCB;
        }
    }
}


void getconfFunc(int argc, char **argv) {
    utility::getconf::getconfFunc(argc, argv);
}
