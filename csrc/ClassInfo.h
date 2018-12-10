#ifndef CSPADE_CLASSINFO_H
#define CSPADE_CLASSINFO_H


#include "common.h"
#include "SpadeArguments.h"

#define NOCLASS -1

class ClassInfo {
private:
    bool has_class;
    vint clsaddr;
    vint classes;
    vint class_count;
    vint min_supports;
    vint tmpe;            // temporary variables to keep support
    vint tmpm;            // counts during intersections
    vint tmpl;
    int num_class = 1;
    shared_ptr<SpadeArguments> args;
public:
    ClassInfo();

    void setArgs(const shared_ptr<SpadeArguments> &args);

    void reset_temps() {
        std::fill(tmpe.begin(), tmpe.end(), 0);
        std::fill(tmpm.begin(), tmpm.end(), 0);
        std::fill(tmpl.begin(), tmpl.end(), 0);
    }

    void increase_tmpl(int i) {
        tmpl[getcls(i)]++;
    }

    void init();

    void increase_tmpm(int i) {
        tmpm[getcls(i)]++;
    }

    void increase_tmpe(int i) {
        tmpe[getcls(i)]++;
    }

    void set_tmpe_item(int i, int val) {
        tmpe[i] = val;
    }

    int get_tmpe_item(int i) {
        return tmpe[i];
    }

    bool strong_lsupport(int i) {
        return tmpl[i] >= min_supports[i];
    }

    bool strong_esupport(int i) {
        return tmpe[i] >= min_supports[i];
    }

    bool strong_msupport(int i) {
        return tmpm[i] >= min_supports[i];
    }

    const vint &get_tmpe() const;

    const vint &get_tmpm() const;

    const vint &get_tmpl() const;

    int get_num_class() const;

    int get_min_support(int idx) const;

    int getcnt(int cls = -1) {
        if (cls == -1) {
            int sum = 0;
            for (int i = 0; i < num_class; i++)
                sum += class_count[i];
            return sum;
        } else return class_count[cls];
    }

    int getcls(int idx) {
        if (has_class)
            return classes[idx];
        else
            return 0;
    }
};

#endif //CSPADE_CLASSINFO_H
