#ifndef __PARTITION_H_
#define __PARTITION_H_

//#include "spade.h"
//#include "sequence.h"

#include "common.h"
#include "SpadeArguments.h"
#include "Array.h"

class Partition {
private:
    shared_ptr<SpadeArguments> args;
    vector<fstream> DATAFD;
    vvint ITEMIDX;
public:
    Partition();

    void init();

    void set_args(const shared_ptr<SpadeArguments> &args);

    void partition_alloc();

    int partition_get_idxsup(int it);

    int partition_get_lidxsup(int idx, int it);

    void partition_read_item(shared_ptr<vint> ival, int it);

    void partition_lclread_item(shared_ptr<vint> ival, int pnum, int it);

    void partition_get_minmaxcustid(vint& backidx, int numit, int pnum, int &minv, int &maxv);
};

#endif// __PARTITION_H_
