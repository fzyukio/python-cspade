#include "ClassInfo.h"

void ClassInfo::init() {
    int i, maxval = 0;
    long numtrans;        // DD

    const string& classfn = args->classf;
    const double min_support_per_class = args->min_support_per_class;
    has_class = args->use_class;

    if (has_class) {
        fstream classf(classfn, ios::binary);
        if (!classf.is_open()) {
            throw runtime_error("Unable to read file " + classfn);
        }

        long fdlen = file_size(classf);
        int *clsaddr = read_file(classf, fdlen);
        if (! classf) {
            throw runtime_error("Error reading file " + classfn);
        }

        // first entry contains num classes
        num_class = clsaddr[0];
        //input is num_class followed by <cid, class> pairs
        numtrans = (fdlen / INT_SIZE - 1) / 2;
        maxval = clsaddr[numtrans * 2 - 1] + 1;
        classes.resize(maxval);
        for (i = 0; i < maxval; i++) classes[i] = NOCLASS;
        for (i = 1; i < (int) (fdlen / INT_SIZE); i += 2) {        // DD
            classes[clsaddr[i]] = clsaddr[i + 1];
        }
        delete [] clsaddr;
    }
    else {
        classes.resize(num_class);
    }

    class_count.resize(num_class, 0);
    tmpe.resize(num_class);
    tmpm.resize(num_class);
    tmpl.resize(num_class);
    min_supports.resize(num_class);

    if (has_class) {
        // class frequency
        for (i = 0; i < maxval; i++)
            if (classes[i] != NOCLASS)
                class_count[classes[i]]++;
    }
    else {
        class_count[0] = args->total_trans_count;
    }

    for (i = 0; i < num_class; i++) {
        min_supports[i] = (int) ceil(min_support_per_class * class_count[i]);
        if (min_supports[i] < 1) min_supports[i] = 1;
    }
}

int ClassInfo::get_num_class() const {
    return num_class;
}

int ClassInfo::get_min_support(int idx) const {
    return min_supports[idx];
}

const vint &ClassInfo::get_tmpe() const {
    return tmpe;
}

const vint &ClassInfo::get_tmpm() const {
    return tmpm;
}

const vint &ClassInfo::get_tmpl() const {
    return tmpl;
}

void ClassInfo::setArgs(const shared_ptr<SpadeArguments> &args) {
    ClassInfo::args = args;
}

ClassInfo::ClassInfo() {

}
