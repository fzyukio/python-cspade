#include "common.h"
#include "Partition.h"
#include "Sequence.h"


Partition::Partition() {}

void Partition::init() {
    DATAFD.resize(args->num_partitions);
    ITEMIDX.reserve(args->num_partitions);
}

void Partition::partition_alloc() {
    ostringstream datafnstream;
    ostringstream idxfnstream;

    for (int i = 0; i < args->num_partitions; i++) {
        datafnstream.str(string());
        idxfnstream.str(string());
        datafnstream << args->dataf;
        idxfnstream << args->idxf;

        if (args->num_partitions > 1) {
            datafnstream << ".P" << i;
            idxfnstream << ".P" << i;
        }
        fstream &datafstream = DATAFD[i];
        datafstream.open(datafnstream.str(), ios::binary | ios::in);
        if (!datafstream.is_open()) {
            throw runtime_error("can't open data file: " + datafnstream.str());
        }

        fstream idxfstream(idxfnstream.str().c_str(), ios::binary | ios::in);
        if (!idxfstream.is_open()) {
            throw runtime_error("can't open idx file: " + idxfnstream.str());
        }

        auto idxflen = file_size(idxfstream);
        vint chunk;
        chunk.reserve(idxflen);

        int *chunk_buf = read_file(idxfstream, idxflen);

        for (int j=0; j<idxflen; j++) {
            chunk.push_back(chunk_buf[j]);
        }

        ITEMIDX.push_back(chunk);
        delete [] chunk_buf;
    }
}


int Partition::partition_get_idxsup(int it) {
    int supsz = 0;
    for (int i = 0; i < args->num_partitions; i++) {
        int* data = ITEMIDX[i].data();
        supsz += data[it + 1] - data[it];
    }
    return supsz;
}

int Partition::partition_get_lidxsup(int idx, int it) {
    int* data = ITEMIDX[idx].data();
    return (data[it + 1] - data[it]);
}

void Partition::partition_read_item(shared_ptr<vint> ival, int it) {
    int ipos = 0;
    int supsz;
    for (int i = 0; i < args->num_partitions; i++) {
        supsz = ITEMIDX[i][it + 1] - ITEMIDX[i][it];
        if (supsz > 0) {
            fstream& f = DATAFD[i];
            f.seekg(ITEMIDX[i][it] * INT_SIZE, ios::beg);
            f.read((char *) &ival->at(ipos), supsz * INT_SIZE);
            if (!f) {
                throw runtime_error("Error reading item");
            }
            ipos += supsz;
        }
    }
}

void Partition::partition_lclread_item(shared_ptr<vint> ival, int pnum, int it) {
    int supsz;
    supsz = ITEMIDX[pnum][it + 1] - ITEMIDX[pnum][it];
    if (supsz > 0) {
        fstream& f = DATAFD[pnum];
        f.seekg(ITEMIDX[pnum][it] * INT_SIZE, ios::beg);
        f.read((char *) ival->data(), supsz * INT_SIZE);

        if (!f) {
            throw runtime_error("Error reading item");
        }
    }
}


void Partition::partition_get_minmaxcustid(vint& backidx, int numit, int pnum, int &minv, int &maxv) {
    int custid, it, i, supsz;
    minv = INT_MAX;
    maxv = 0;
    for (i = 0; i < numit; i++) {
        it = backidx[i];
        supsz = ITEMIDX[pnum][it + 1] - ITEMIDX[pnum][it];
        if (supsz > 0) {
            fstream& f = DATAFD[pnum];
            f.seekg(ITEMIDX[pnum][it] * INT_SIZE, ios::beg);
            f.read((char *) &custid, INT_SIZE);
            if (minv > custid) {
                minv = custid;
            }

            f.seekg((supsz - 3) * INT_SIZE, ios::cur);
            f.read((char *) &custid, INT_SIZE);

            if (maxv < custid) {
                maxv = custid;
            }
        }
    }
}

void Partition::set_args(const shared_ptr<SpadeArguments> &args) {
    Partition::args = args;
}