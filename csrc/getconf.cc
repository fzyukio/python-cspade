#include "common.h"
#include "argv_parser.h"
#include "calcdb.h"
#include "Env.h"
#include "getconf.h"


result_t getconfFunc(Env& env, const GetconfArgument& args) {
    int DBASE_NUM_TRANS = 0;
    int DBASE_MAXITEM = 0;
    int DBASE_NUM_CUST = 0;
    int DBASE_MINTRANS = 0;
    int DBASE_MAXTRANS = 0;
    float DBASE_AVG_TRANS_SZ = 0;
    float DBASE_AVG_CUST_SZ = 0;

    int i;

    int custid = 0, tid = 0, nitem = 0;
    int *buf = nullptr;
    int oldcustid = -1;
    int oldtcnt = 0;
    int tsizesum = 0;
    int tcustsum = 0;
    int tsizesq = 0;
    int maxnitem = 0;

    DbaseCtrlBlk dcb;
    dcb.init(args.input);

    dcb.get_first_blk();
    dcb.get_next_trans(buf, nitem, tid, custid);
    DBASE_MINTRANS = custid;
    while (!dcb.eof()) {
        //printf ("%d %d %d\n", custid, tid, nitem);
        DBASE_MAXTRANS = custid;
        if (args.use_seq) {
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
        dcb.get_next_trans(buf, nitem, tid, custid);
    }
    tcustsum += DBASE_NUM_TRANS - oldtcnt;
    DBASE_MAXITEM++;

    if (args.use_seq) {
        DBASE_AVG_CUST_SZ = (1.0 * tcustsum) / DBASE_NUM_CUST;
    }
    DBASE_AVG_TRANS_SZ = (1.0 * tsizesum) / DBASE_NUM_TRANS;
    double trans_sq_avg = (1.0 * tsizesq) / DBASE_NUM_TRANS;
    double stddev = sqrt(trans_sq_avg - (DBASE_AVG_TRANS_SZ * DBASE_AVG_TRANS_SZ));


    //write config info to new file
    ofstream conffd(args.confn.c_str(), ios::binary | ios::out);

    if (!conffd) {
        string error_message = "can't open file: " + args.confn;
        throw runtime_error(error_message);
    }

    if (args.use_seq) {
        conffd.write((char *) &DBASE_NUM_CUST, INT_SIZE);
        conffd.write((char *) &DBASE_MAXITEM, INT_SIZE);
        conffd.write((char *) &DBASE_AVG_CUST_SZ, sizeof(float));
        conffd.write((char *) &DBASE_AVG_TRANS_SZ, sizeof(float));
        conffd.write((char *) &DBASE_NUM_TRANS, INT_SIZE);
        conffd.write((char *) &DBASE_MINTRANS, INT_SIZE);
        conffd.write((char *) &DBASE_MAXTRANS, INT_SIZE);
    } else {
        conffd.write((char *) &DBASE_NUM_TRANS, INT_SIZE);
        conffd.write((char *) &DBASE_MAXITEM, INT_SIZE);
        conffd.write((char *) &DBASE_AVG_TRANS_SZ, sizeof(float));
        conffd.write((char *) &DBASE_MINTRANS, INT_SIZE);
        conffd.write((char *) &DBASE_MAXTRANS, INT_SIZE);
    }

    conffd.close();
    env.summary << "CONF " << DBASE_NUM_CUST << " " << DBASE_MAXITEM << " " << DBASE_AVG_CUST_SZ
            << " " << DBASE_AVG_TRANS_SZ << " " << DBASE_NUM_TRANS << " " << DBASE_MINTRANS
            << " " << DBASE_MAXTRANS << " " << stddev << " " << maxnitem << endl;

    result_t result;
    result.logger = env.logger.str();
    result.summary = env.summary.str();
    return result;
}

result_t getconfWrapper(const string &s, shared_ptr<Env>& envptr) {
    args_t args_ = parse(s);
    GetconfArgument args;
    args.parse_args(args_.argc, args_.argv);

    if (envptr == nullptr) {
        Env env;
        return getconfFunc(env, args);
    }
    else {
        return getconfFunc(*envptr, args);
    }
}

// remark: the implementation assumes that a customer's transactions
//         appear as a contiguous block in the binary input data, and
//         therefore, in the user-supplied database.
//
// ceeboo 2007
