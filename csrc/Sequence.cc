#include "common.h"
#include "Eqclass.h"
#include "Itemset.h"
#include "Lists.h"
#include "InvertDatabase.h"
#include "Partition.h"
#include "Sequence.h"


void Sequence::add_freq(Itemset_S &it, int templ) {
    FreqIt_S freq = make_shared<FreqIt>(it->itemset()->array(), it->size(), templ);
    if (FreqArrayPos + 1 >= FreqArraySz) {
        FreqArraySz = (unsigned long) (1.5 * FreqArraySz);
        FreqArray->reserve(FreqArraySz);
    }
    FreqArray->push_back(freq);
}

void Sequence::get_2newf_intersect
        (Itemset_S& ljoin, Itemset_S& ejoin, shared_ptr<vint> vit1, shared_ptr<vint> vit2, int sup1, int sup2) {
    int i, j, k, l;
    int nval1, nval2, diff;
    int lflge;

    int *it2 = vit2->data();
    int *it1 = vit1->data();

    args->num_intersect++;

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
            while (nval1 < sup1 && it1[i] == it1[nval1]) nval1 += 2;
            while (nval2 < sup2 && it2[j] == it2[nval2]) nval2 += 2;
            if (ljoin && it1[i + 1] + args->min_gap <= it2[nval2 - 1]) {
                //add tid
                lflge = 0;
                for (k = i, l = j; k < nval1 && l < nval2;) {
                    diff = it2[l + 1] - it1[k + 1];
                    if (diff < args->min_gap) l += 2;
                    else if (diff > args->max_gap) k += 2;
                    else {
                        ljoin->ival()->optadd(icid);
                        ljoin->ival()->optadd(it1[k + 1]);
                        lflge = 1;
                        k += 2;
                    }
                }
                if (lflge) {
                    ljoin->increment_support();
                    ljoin->increment_cls_support(cls->getcls(icid));
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
                    ejoin->increment_cls_support(cls->getcls(icid));
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

void Sequence::make_itemset(Itemset_S& it, Array_S& ary, int cnt, const vint &clscnt) {
    int i;
    for (i = 0; i < ary->size(); i++) {
        it->ival()->optadd((*ary)[i]);
    }
    it->set_support(cnt);
    for (i = 0; i < cls->get_num_class(); i++) it->set_cls_support(clscnt[i], i);
}

void Sequence::get_tmpnewf_intersect(Itemset_S& ljoin, Itemset_S& ejoin, Itemset_S& mjoin,
                                     int &lcnt, int &ecnt, int &mcnt,
                                     Itemset_S& it1, Itemset_S& it2, int iter) {
    int i, j, k, l;
    int nval1, nval2, diff;
    int lflge;

    args->num_intersect++;

    l_array->reset();
    e_array->reset();
    m_array->reset();

    int num_class = cls->get_num_class();
    bool print_idlist = args->print_tidlist;

    lcnt = ecnt = mcnt = 0;
    cls->reset_temps();


    int dc1 = it1->support() - args->min_support;
    int dc2 = it2->support() - args->min_support;
    int df1 = 0;
    int df2 = 0;
    int icid, jcid;
    for (i = 0, j = 0; i < it1->ivalsize() && j < it2->ivalsize();) {
        if (df1 > dc1 || df2 > dc2) break;
        icid = it1->ival(i);
        jcid = it2->ival(j);
        if (icid > jcid) {
            //df must be incremented only once per customer
            while (j < it2->ivalsize() && jcid == it2->ival(j)) j += 2;
            df2++;
        } else if (icid < jcid) {
            while (i < it1->ivalsize() && icid == it1->ival(i)) i += 2;
            df1++;
        } else {
            nval1 = i;
            nval2 = j;
            while (nval1 < it1->ivalsize() && it1->ival(i) == it1->ival(nval1))
                nval1 += 2;
            while (nval2 < it2->ivalsize() && it2->ival(j) == it2->ival(nval2))
                nval2 += 2;

            if (ljoin && it1->ival(i + 1) + args->min_gap <= it2->ival(nval2 - 1)) {
                lflge = 0;
                for (k = i, l = j; k < nval1 && l < nval2;) {
                    diff = it2->ival(l + 1) - it1->ival(k + 1);
                    if (diff < args->min_gap) l += 2;
                    else if (diff > args->max_gap) k += 2;
                    else {
                        l_array->optadd(icid);
                        l_array->optadd(it1->ival(k + 1));
                        lflge = 1;
                        k += 2;
                    }
                }
                if (lflge) {
                    lcnt++;
                    cls->increase_tmpl(icid);
                }
            }

            if (ejoin) {
                lflge = 0;
                for (k = i, l = j; k < nval1 && l < nval2;) {
                    if (it1->ival(k + 1) < it2->ival(l + 1)) k += 2;
                    else if (it1->ival(k + 1) > it2->ival(l + 1)) l += 2;
                    else {
                        e_array->optadd(icid);
                        e_array->optadd(it2->ival(l + 1));
                        lflge = 1;
                        k += 2;
                        l += 2;
                    }
                }
                if (lflge) {
                    ecnt++;
                    cls->increase_tmpe(icid);
                }
            }

            if (mjoin && it2->ival(j + 1) + args->min_gap <= it1->ival(nval1 - 1)) {
                lflge = 0;
                for (k = i, l = j; k < nval1 && l < nval2;) {
                    diff = it1->ival(k + 1) - it2->ival(l + 1);
                    if (diff < args->min_gap) k += 2;
                    else if (diff > args->max_gap) l += 2;
                    else {
                        m_array->optadd(icid);
                        m_array->optadd(it2->ival(l + 1));
                        lflge = 1;
                        l += 2;
                    }
                }
                if (lflge) {
                    mcnt++;
                    cls->increase_tmpm(icid);
                }
            }
            i = nval1;
            j = nval2;
        }
    }
    if (ljoin) {
        ljoin = nullptr;
        for (i = 0; i < num_class; i++) {
            if (cls->strong_lsupport(i)) {
                ljoin.reset(new Itemset(iter, l_array->size(), num_class, print_idlist));
                make_itemset(ljoin, l_array, lcnt, cls->get_tmpl());
                break;
            }
        }
    }
    if (ejoin) {
        ejoin = nullptr;
        for (i = 0; i < num_class; i++) {
            if (cls->strong_esupport(i)) {
                ejoin.reset(new Itemset(iter, e_array->size(), num_class, print_idlist));
                make_itemset(ejoin, e_array, ecnt, cls->get_tmpe());
                break;
            }
        }
    }
    if (mjoin) {
        mjoin = nullptr;
        for (i = 0; i < num_class; i++) {
            if (cls->strong_msupport(i)) {
                mjoin.reset(new Itemset(iter, m_array->size(), num_class, print_idlist));
                make_itemset(mjoin, m_array, mcnt, cls->get_tmpm());
                break;
            }
        }
    }
}

void Sequence::pre_pruning(Itemset_S &join, unsigned int ptempl, Itemset_S& clas, Itemset_S& prefix, char use_seq) {
    ostringstream &logger = env.logger;
    ostringstream &seqstrm = env.seqstrm;

    float conf, conf2;
    int i, res, cit, pit;
    if (join == nullptr) return;
    pit = (*prefix)[0];
    int bitval = 0;
    int nsz = clas->size() - 2;
    if (GETBIT(args->pruning_type, FOLLOWPRUNING - 1)) {

        //clas->print_seq(seqstrm, SETBIT(ptempl,1,nsz+1));

        for (i = 0; i <= nsz + 1 && !bitval; i++) {
            cit = (*clas)[i];
            if (use_seq) {
                return; //TURN OFF FOR SEQUENCES

                res = invdb->get_eqgraph_item(cit)->seqfind(pit);
                if (res != -1) {
                    conf = (invdb->get_eqgraph_item(cit)->get_seqsup(res) * 1.0) / invdb->get_sup(cit);
                    //     << res << " "<< invdb->get_eqgraph_item(cit)->get_seqsup(res) << " "
                    //     << invdb->get_sup(cit) << endl;
                    if (conf >= args->follow_thresh) {
                        if (args->outputfreq) {
                            logger << "PRUNE_PRE " << pit << " -1 ";
                            clas->print_seq(seqstrm, SETBIT(ptempl, 1, nsz + 1));
                        }
                        args->prepruning++;
                        join = nullptr;
                        break;
                    }
                }
            } else {
                res = invdb->get_eqgraph_item(cit)->find(pit);
                if (res != -1) {
                    conf = (invdb->get_eqgraph_item(cit)->get_sup(res) * 1.0) / invdb->get_sup(cit);
                    conf2 = (invdb->get_eqgraph_item(cit)->get_sup(res) * 1.0) / invdb->get_sup(pit);
                    if (conf >= args->follow_thresh || conf2 >= args->follow_thresh) {
                        if (args->outputfreq) {
                            logger << "PRUNE_PRE " << pit << " ";
                            clas->print_seq(seqstrm, SETBIT(ptempl, 1, nsz + 1));
                        }
                        args->prepruning++;
                        join = nullptr;
                        break;
                    }
                }
            }

            if (nsz - i >= 0) bitval = GETBIT(ptempl, nsz - i);
        }
    }

}

void Sequence::post_pruning(Itemset_S &iset, unsigned int templ) {
    int i;
    int remsup;
    float remdb;
    if (iset == nullptr || cls->get_num_class() <= 1) return;
    ostringstream &logger = env.logger;
    ostringstream &seqstrm = env.seqstrm;

    if (GETBIT(args->pruning_type, ZEROPRUNING - 1)) {
        for (i = 0; i < cls->get_num_class(); i++) {
            remsup = iset->support() - iset->cls_support(i);
            remdb = cls->getcnt() - cls->getcnt(i);
            if (remsup / remdb <= args->zero_thresh) {
                if (args->outputfreq) {
                    logger << "PRUNE_POST ";
                    iset->print_seq(seqstrm, templ);
                }
                args->postpruning++;
                //numLargeItemset->at(iset->size()-1)++;
                iset = nullptr;
                break;
            }
        }
    }
}


void fill_seq_template(Eqclass_S& EQ, Eqclass_S& parent, int LR) {
    if (LR == 1) {
        EQ->set_templ(SETBIT(parent->templ(), 1, EQ->templ_sz() - 1));
        EQ->set_templ2(parent->templ());
    } else if (LR == 2) {
        EQ->set_templ(SETBIT(parent->templ2(), 1, EQ->templ_sz() - 1));
        EQ->set_templ2(parent->templ2());
    }
    //     << " " << LR << " " << EQ->templ() << " " << EQ->templ2() << endl;
}

int Sequence::get_valid_el(int it, vector<char>& ibvec, vector<char>& sbvec) {
    int i, j;
    int i1, i2;
    int rval = 0;
    ostringstream &logger = env.logger;
    ostringstream &seqstrm = env.seqstrm;

    if (args->pruning_type == NOPRUNING) {
        for (i = 0; i < invdb->get_eqgraph_item(it)->seqnum_elements(); i++) sbvec[i] = 1;
        for (i = 0; i < invdb->get_eqgraph_item(it)->num_elements(); i++) ibvec[i] = 1;
        rval = 1;
        return rval;
    }

    for (i = 0; i < invdb->get_eqgraph_item(it)->seqnum_elements(); i++) {
        sbvec[i] = 0;
    }
    for (i = 0; i < invdb->get_eqgraph_item(it)->num_elements(); i++) {
        ibvec[i] = 0;
    }

    for (i = 0; i < invdb->get_eqgraph_item(it)->seqnum_elements(); i++) {
        i1 = invdb->get_eqgraph_item(it)->seqget_element(i);
        for (j = i; j < invdb->get_eqgraph_item(it)->seqnum_elements(); j++) {
            i2 = invdb->get_eqgraph_item(it)->seqget_element(j);
            if (invdb->get_eqgraph_item(i2) && invdb->get_eqgraph_item(i2)->seqfind(i1) != -1) {
                sbvec[i] = 1;
                sbvec[j] = 1;
                rval = 1;
            }
            if (j > i) {
                if ((invdb->get_eqgraph_item(i2) && invdb->get_eqgraph_item(i2)->find(i1) != -1) ||
                    (invdb->get_eqgraph_item(i1) && invdb->get_eqgraph_item(i1)->seqfind(i2) != -1)) {
                    sbvec[i] = 1;
                    sbvec[j] = 1;
                    rval = 1;
                }
            }
        }
    }


    for (i = 0; i < invdb->get_eqgraph_item(it)->num_elements(); i++) {
        i1 = invdb->get_eqgraph_item(it)->get_element(i);
        for (j = i + 1; j < invdb->get_eqgraph_item(it)->num_elements(); j++) {
            i2 = invdb->get_eqgraph_item(it)->get_element(j);
            if (invdb->get_eqgraph_item(i2) && invdb->get_eqgraph_item(i2)->find(i1) != -1) {
                ibvec[i] = 1;
                ibvec[j] = 1;
                rval = 1;
            }
        }
        for (j = 0; j < invdb->get_eqgraph_item(it)->seqnum_elements(); j++) {
            i2 = invdb->get_eqgraph_item(it)->seqget_element(j);
            if (invdb->get_eqgraph_item(i1) && invdb->get_eqgraph_item(i1)->seqfind(i2) != -1) {
                ibvec[i] = 1;
                sbvec[j] = 1;
                rval = 1;
            }
        }
    }

    for (i = 0; i < invdb->get_eqgraph_item(it)->seqnum_elements(); i++)
        if (!sbvec[i]) {
            args->L2pruning++;
            if (args->outputfreq) {
                logger << "PRUNE_L2 " << it << " -1 " << invdb->get_eqgraph_item(it)->seqget_element(i)
                     << " " << invdb->get_eqgraph_item(it)->get_seqsup(i) << endl;
            }
        }

    for (i = 0; i < invdb->get_eqgraph_item(it)->num_elements(); i++)
        if (!ibvec[i]) {
            args->L2pruning++;
            if (args->outputfreq) {
                logger << "PRUNE_L2 " << it << " " << invdb->get_eqgraph_item(it)->get_element(i)
                     << " " << invdb->get_eqgraph_item(it)->get_sup(i) << endl;
            }
        }
    return rval;
}

//construct the next set of eqclasses from external disk
Eqclass_S Sequence::get_ext_eqclass(int it) {
    ostringstream &logger = env.logger;
    ostringstream &seqstrm = env.seqstrm;

    int i, k, it2, supsz, supsz2;
    Itemset_S ljoin = nullptr;
    Itemset_S ejoin = nullptr;

    int num_class = cls->get_num_class();
    bool print_idlist = args->print_tidlist;

    vector<char> ibvec, sbvec;        // DD
    if (!args->use_maxgap) {
        if (invdb->get_eqgraph_item(it)->num_elements() > 0)
            ibvec.resize(invdb->get_eqgraph_item(it)->num_elements());
        if (invdb->get_eqgraph_item(it)->seqnum_elements() > 0)
            sbvec.resize(invdb->get_eqgraph_item(it)->seqnum_elements());

        if (!get_valid_el(it, ibvec, sbvec)) return nullptr;
    }

    Eqclass_S L2 = make_shared<Eqclass>(1, EQCTYP1);

    //init seq pattern templates
    L2->set_templ(1);
    L2->set_templ2(0);

    l_array->reset();
    e_array->reset();

    supsz = partition->partition_get_idxsup(it);
    partition->partition_read_item(l_array->array(), it);

    int tmpit;
    for (i = 0, k = 0; i < invdb->get_eqgraph_item(it)->num_elements() ||
                       k < invdb->get_eqgraph_item(it)->seqnum_elements();) {
        ljoin = nullptr;
        ejoin = nullptr;

        it2 = args->dbase_max_item + 1;
        tmpit = args->dbase_max_item + 1;
        if (i < invdb->get_eqgraph_item(it)->num_elements() && (args->use_maxgap || ibvec[i]))
            it2 = invdb->get_eqgraph_item(it)->get_element(i);
        if (k < invdb->get_eqgraph_item(it)->seqnum_elements() && (args->use_maxgap || sbvec[k]))
            tmpit = invdb->get_eqgraph_item(it)->seqget_element(k);

        if (it2 == tmpit) {
            ejoin = placeholder;
            ljoin = placeholder;
            k++;
            i++;
            if (it2 == args->dbase_max_item + 1) continue;
        } else if (it2 < tmpit) {
            ejoin = placeholder;
            i++;
        } else {
            ljoin = placeholder;
            k++;
            it2 = tmpit;
        }
        supsz2 = partition->partition_get_idxsup(it2);

        partition->partition_read_item(e_array->array(), it2);

        if (ejoin) {
            ejoin.reset(new Itemset(2, min(supsz, supsz2), num_class, print_idlist));
        } else ejoin = nullptr;

        if (ljoin) {
            ljoin.reset(new Itemset(2, supsz2, num_class, print_idlist));
        } else ljoin = nullptr;

        get_2newf_intersect(ljoin, ejoin, e_array->array(), l_array->array(), supsz2, supsz);

        if (ljoin) {
            ljoin->add_item(it2);
            ljoin->add_item(it);
        }
        if (args->pruning_type > 1) post_pruning(ljoin, L2->templ());
        if (ljoin) {
            ljoin->reallocival();
            L2->prepend(ljoin);
            if (args->outputfreq) {
                ljoin->print_seq(seqstrm, L2->templ());
            }
        }

        if (ejoin) {
            ejoin->add_item(it2);
            ejoin->add_item(it);
        }
        if (args->pruning_type > 1) post_pruning(ejoin, L2->templ2());
        if (ejoin) {
            ejoin->reallocival();
            L2->prepend2(ejoin);
            if (args->outputfreq) {
                ejoin->print_seq(seqstrm, L2->templ2());
            }
        }
    }

    return L2;
}

void Sequence::fill_join(Itemset_S& join, Itemset_S& hdr1, Itemset_S& hdr2) {
    int i;

    join->add_item((*hdr2)[0]);
    for (i = 0; i < hdr1->size(); i++) {
        join->add_item((*hdr1)[i]);
    }
}

Itemset_S Sequence::prune_decision(Itemset_S& it1, Itemset_S& it2, unsigned int ptempl, int jflg) {
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

    if (seqlen > args->max_seq_len) return nullptr;
    if (maxisetlen > args->max_iset_len) return nullptr;


    //args->max_gap destroys the downward closure property, so we cannot prune
    if (args->use_maxgap) return placeholder;

    int l1 = (*it1)[0];
    int l2 = (*it2)[0];
    int nsz;
    if (args->use_hash && (it2->size() > 2)) {
        if (args->recursive) return placeholder;

        unsigned int ttpl;
        FreqIt_S fit = make_shared<FreqIt>(it2->size(), 0);
        int *fit_data = fit->seq->data();

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
            fit_data[k++] = l1;
            fit_data[k++] = l2;
            for (j = 1; j < nsz + 2; j++) {
                if (j != i) {
                    fit_data[k++] = (*it2)[j];
                }
            }
            fit->templ = ttpl;

            //???? Does this work for suffix classes
            if (fit_data[fit->size() - 1] == (*it1)[it1->size() - 1] && !args->recursive) {
                //elements should be in current class
                if (FreqArrayPos > 0) {
                    if (!EqGrNode::bsearch(0, FreqArrayPos - 1, FreqArray, fit, args->recursive)) {
                        return nullptr;
                    }
                } else return nullptr;
            } else if (fit_data[fit->size() - 1] > (*it1)[it1->size() - 1]) {
                // class must already have been processed, otherwise we can't prune
                if (!invdb->get_eqgraph_item(fit_data[fit->size() - 1])->find_freqarray(fit, args->recursive)) {
                    return nullptr;
                }
            }
        }
    } else {// if (it1->size() == 2){
        bit = 0;
        nsz = it2->size() - 2;
        for (i = 0; i <= nsz + 1 && !bit; i++) {
            l2 = (*it2)[i];
            if (invdb->get_eqgraph_item(l2)) {
                if (jflg == LJOIN || jflg == MJOIN) {
                    if (invdb->get_eqgraph_item(l2)->seqfind(l1) == -1)
                        return nullptr;
                } else {
                    if (invdb->get_eqgraph_item(l2)->find(l1) == -1)
                        return nullptr;
                }
            } else return nullptr;
            if (nsz - i >= 0) bit = GETBIT(ptempl, nsz - i);
        }
    }
    return placeholder;
}


void Sequence::insert_freqarray(shared_ptr<Lists<Eqclass_S>>& LargeL) {
    //insert frequent itemsets into hash table
    shared_ptr<ListNodes<Eqclass_S>> chd;
    shared_ptr<ListNodes<shared_ptr<Itemset>>> hdr1, hdr2;
    Eqclass_S cluster;

    chd = LargeL->head();
    for (; chd; chd = chd->next()) {
        cluster = chd->item();
        hdr1 = cluster->list()->head();
        for (; hdr1; hdr1 = hdr1->next()) {
            add_freq(hdr1->item(), cluster->templ());
            //hdr1->item()->print_seq(seqstrm, cluster->templ());
        }
        hdr2 = cluster->list2()->head();
        for (; hdr2; hdr2 = hdr2->next()) {
            add_freq(hdr2->item(), cluster->templ2());
            //hdr2->item()->print_seq(seqstrm, cluster->templ2());
        }
    }
}

void Sequence::process_cluster_list1(shared_ptr<ListNodes<shared_ptr<Itemset>>>& hdr1,
                                     shared_ptr<Lists<shared_ptr<Itemset>>>& cluster1,
                                     shared_ptr<Lists<shared_ptr<Itemset>>>& cluster2,
                                     shared_ptr<Lists<Eqclass_S>>& LargeL,
                                     int iter, int eqtype, Eqclass_S& parent) {

    ostringstream &logger = env.logger;
    ostringstream &seqstrm = env.seqstrm;

    shared_ptr<ListNodes<shared_ptr<Itemset>>> hdr2;
    Eqclass_S EQ = make_shared<Eqclass>(iter - 1, eqtype);
    fill_seq_template(EQ, parent, 2);

    Itemset_S ljoin, ejoin, mjoin;
    int lsup, esup, msup;
    hdr2 = cluster2->head();
    for (; hdr2; hdr2 = hdr2->next()) {
        //ljoin = (Itemset_S )1;
        ljoin = prune_decision(hdr2->item(), hdr1->item(), EQ->templ(), LJOIN);
        ejoin = nullptr;
        mjoin = nullptr;
        lsup = esup = msup = 0;
        if (args->pruning_type > 1)
            pre_pruning(ljoin, EQ->templ(), hdr1->item(), hdr2->item(), 1);
        if (ljoin) get_tmpnewf_intersect(ljoin, ejoin, mjoin, lsup, esup, msup, hdr2->item(), hdr1->item(), iter);
        if (ljoin) fill_join(ljoin, hdr1->item(), hdr2->item());
        if (args->pruning_type > 1) {
            post_pruning(ljoin, EQ->templ());
        }
        if (ljoin) {
            numLargeItemset->at(iter - 1)++;
            //fill_join(ljoin, hdr1->item(), hdr2->item());
            if (args->outputfreq) ljoin->print_seq(seqstrm, EQ->templ());
            EQ->append(ljoin);
        }
    }

    //hdr2 = cluster1->head();
    //for (; hdr2 != hdr1; hdr2=hdr2->next()){
    hdr2 = hdr1->next();
    for (; hdr2 != nullptr; hdr2 = hdr2->next()) {
        //ejoin = (Itemset_S )1;
        ejoin = prune_decision(hdr2->item(), hdr1->item(), EQ->templ2(), EJOIN);
        ljoin = nullptr;
        mjoin = nullptr;
        lsup = esup = msup = 0;
        if (args->pruning_type > 1)
            pre_pruning(ejoin, EQ->templ2(), hdr1->item(), hdr2->item(), 0);
        if (ejoin) get_tmpnewf_intersect(ljoin, ejoin, mjoin, lsup, esup, msup, hdr2->item(), hdr1->item(), iter);
        if (ejoin) fill_join(ejoin, hdr1->item(), hdr2->item());
        if (args->pruning_type > 1) post_pruning(ejoin, EQ->templ2());
        if (ejoin) {
            numLargeItemset->at(iter - 1)++;
            //fill_join(ejoin, hdr1->item(), hdr2->item());
            if (args->outputfreq) ejoin->print_seq(seqstrm, EQ->templ2());
            EQ->append2(ejoin);
        }
    }

    if (EQ) {
        if ((EQ->list()->size() > 0) || (EQ->list2()->size() > 0)) {
            if (args->recursive) {
                //if (use_hash) insert_freqarray(EQ);
                process_cluster1(EQ, nullptr, iter + 1);
                EQ = nullptr;
            } else LargeL->append(EQ);
        } else {
            EQ = nullptr;
        }
    }
}

void Sequence::process_cluster_list2(shared_ptr<ListNodes<shared_ptr<Itemset>>>& hdr1, int i, Eqclass_SS& EQ,
                                     shared_ptr<Lists<shared_ptr<Itemset>>>& cluster,
                                     shared_ptr<Lists<Eqclass_S>>& LargeL,
                                     int iter, int eqtype, Eqclass_S& parent) {
    ostringstream &logger = env.logger;
    ostringstream &seqstrm = env.seqstrm;

    int j;
    shared_ptr<ListNodes<shared_ptr<Itemset>>> hdr2;
    Itemset_S ljoin, ejoin, mjoin;
    int lsup, esup, msup;

    //join with sequences
    hdr2 = hdr1;
    for (j = i; hdr2; j++, hdr2 = hdr2->next()) {
        ljoin = prune_decision(hdr1->item(), hdr2->item(), EQ->at(j)->templ(), LJOIN);
        if (hdr2 == hdr1) {
            ejoin = mjoin = nullptr;
        } else {
            ejoin = prune_decision(hdr2->item(), hdr1->item(), EQ->at(i)->templ2(), EJOIN);
            mjoin = prune_decision(hdr2->item(), hdr1->item(), EQ->at(i)->templ(), MJOIN);
            //ejoin = mjoin = (Itemset_S )1;
        }
        lsup = esup = msup = 0;
        if (args->pruning_type > 1) {
            pre_pruning(ejoin, EQ->at(i)->templ2(), hdr1->item(), hdr2->item(), 0);
            pre_pruning(ljoin, EQ->at(j)->templ(), hdr2->item(), hdr1->item(), 1);
            pre_pruning(mjoin, EQ->at(i)->templ(), hdr1->item(), hdr2->item(), 1);
        }

        if (ljoin || ejoin || mjoin) get_tmpnewf_intersect(ljoin, ejoin, mjoin, lsup, esup, msup, hdr1->item(), hdr2->item(), iter);
        if (ljoin) fill_join(ljoin, hdr2->item(), hdr1->item());
        if (args->pruning_type > 1) post_pruning(ljoin, EQ->at(j)->templ());
        if (ljoin) {
            numLargeItemset->at(iter - 1)++;
            //fill_join(ljoin, hdr2->item(), hdr1->item());
            if (args->outputfreq) ljoin->print_seq(seqstrm, EQ->at(j)->templ());
            EQ->at(j)->append(ljoin);
        }

        if (ejoin) fill_join(ejoin, hdr1->item(), hdr2->item());
        if (args->pruning_type > 1) post_pruning(ejoin, EQ->at(i)->templ2());
        if (ejoin) {
            numLargeItemset->at(iter - 1)++;
            //fill_join(ejoin, hdr1->item(), hdr2->item());
            if (args->outputfreq) ejoin->print_seq(seqstrm, EQ->at(i)->templ2());
            EQ->at(i)->append2(ejoin);
        }

        if (mjoin) fill_join(mjoin, hdr1->item(), hdr2->item());
        if (args->pruning_type > 1) post_pruning(mjoin, EQ->at(i)->templ());
        if (mjoin) {
            numLargeItemset->at(iter - 1)++;
            //fill_join(mjoin, hdr1->item(), hdr2->item());
            if (args->outputfreq) mjoin->print_seq(seqstrm, EQ->at(i)->templ());
            EQ->at(i)->append(mjoin);
        }
    }
    if ((EQ->at(i)->list()->size() > 0) || (EQ->at(i)->list2()->size() > 0)) {
        if (args->recursive) {
            //if (use_hash) insert_freqarray(EQ->at(i));
            process_cluster1(EQ->at(i), nullptr, iter + 1);
            EQ->at(i) = nullptr;
        } else LargeL->append(EQ->at(i));
    } else {
        EQ->at(i) = nullptr;
    }

}


void Sequence::process_cluster1(Eqclass_S cluster, shared_ptr<Lists<Eqclass_S>> LargeL, int iter) {
    Eqclass_SS EQ = nullptr;
    shared_ptr<ListNodes<shared_ptr<Itemset>>> hdr1, hdr2;
    int i;

    if (cluster->list()->head()) {
        EQ = make_shared<vector<Eqclass_S>>(cluster->list()->size());
        for (i = 0; i < cluster->list()->size(); i++) {
            EQ->at(i).reset(new Eqclass(iter - 1, EQCTYP1));
            fill_seq_template(EQ->at(i), cluster, 1);
        }
    }

    hdr1 = cluster->list()->head();
    for (i = 0; hdr1; hdr1 = hdr1->next(), i++) {
        //if (use_hash && iter > 3) add_freq(hdr1->item(), cluster->templ());
        process_cluster_list2(hdr1, i, EQ, cluster->list(), LargeL, iter, EQCTYP1, cluster);
    }
    if (EQ) EQ = nullptr;


    hdr2 = cluster->list2()->head();
    for (; hdr2; hdr2 = hdr2->next()) {
        //if (use_hash && iter > 3) add_freq(hdr2->item(), cluster->templ2());
        process_cluster_list1(hdr2, cluster->list2(), cluster->list(), LargeL, iter, EQCTYP1, cluster);
    }

    //if (recursive) delete cluster;
    if (args->maxiter < iter) args->maxiter = iter;

}


void Sequence::find_large(Eqclass_S cluster, int it) {
    shared_ptr<Lists<Eqclass_S>> LargeL, Candidate;
    shared_ptr<ListNodes<Eqclass_S>> chd;
    int iter;
    int LargelistSum = 0;
    int more;

    more = 1;
    Candidate.reset(new Lists<Eqclass_S>);
    Candidate->append(cluster);
    for (iter = 3; more; iter++) {
        LargeL.reset(new Lists<Eqclass_S>);
        chd = Candidate->head();
        for (; chd; chd = chd->next()) {
            process_cluster1(chd->item(), LargeL, iter);
            chd->set_item(nullptr);
        }
        Candidate->clear();
        Candidate = nullptr;

        if (args->use_hash) insert_freqarray(LargeL);
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

        if (!more) {
            LargeL->clear();
            LargeL = nullptr;
        }
    }
}


void Sequence::process_class(int it) {

    //from 2-itemsets from ext disk
    Eqclass_S large2it = get_ext_eqclass(it);
    if (large2it == nullptr) return;
    if (args->use_maxgap) {
        process_maxgap(large2it);
    } else {
        if (args->recursive) {
            process_cluster1(large2it, nullptr, 3);
            large2it = nullptr;
        } else find_large(large2it, it);
    }
}

void Sequence::newSeq() {
    int i, j;

    if (args->use_hash) {
        FreqArray->reserve(FreqArraySz);
    }
    //form large itemsets for each eqclass
    if (args->use_ascending != -2) {
        if (args->use_ascending == -1) {
            for (i = 0; i < args->dbase_max_item; i++)
                if (invdb->get_eqgraph_item(i)) {
                    process_class(i);
                }
        } else if (invdb->get_eqgraph_item(args->use_ascending))
            process_class(args->use_ascending);
    } else {
        for (i = args->dbase_max_item - 1; i >= 0; i--) {
            auto x = invdb->get_eqgraph_item(i);
            if (x != nullptr) {
                if (args->use_hash) FreqArrayPos = 0;
                process_class(i);
                if (args->use_hash) {
                    if (FreqArrayPos > 0) {
                        FreqIt_SS fit = make_shared<vector<FreqIt_S>>(FreqArrayPos);
                        for (j = 0; j < FreqArrayPos; j++) {
                            fit->at(j) = FreqArray->at(j);
                        }
                        invdb->get_eqgraph_item(i)->set_freqarray(fit, FreqArrayPos);
                    }
                }
            }
        }
    }
}


void Sequence::read_files() {
    int i;
    auto capacity = static_cast<unsigned long>(args->avg_trans_count * 30);

    numLargeItemset->clear();
    numLargeItemset->resize(capacity);

    numLargeItemset->at(0) = invdb->make_l1_pass();

    if (args->ext_l2_pass) {
        numLargeItemset->at(1) = invdb->make_l2_pass();
    } else {
        numLargeItemset->at(1) = get_file_l2();
    }

    for (i = 0; i < args->dbase_max_item; i++) {
        if (invdb->get_eqgraph_item(i)) {
            if (invdb->get_eqgraph_item(i)->num_elements() > 0)
                invdb->get_eqgraph_item(i)->elements()->compact();
            if (invdb->get_eqgraph_item(i)->seqnum_elements() > 0)
                invdb->get_eqgraph_item(i)->seqelements()->compact();
        }
    }

    int maxitemsup = 0;
    int sup;
    for (i = 0; i < args->dbase_max_item; i++) {
        sup = partition->partition_get_idxsup(i);
        if (maxitemsup < sup) maxitemsup = sup;
    }
    l_array = make_shared<Array>(maxitemsup);
    e_array = make_shared<Array>(maxitemsup);
    m_array = make_shared<Array>(maxitemsup);
}

void Sequence::set_args(const shared_ptr<SpadeArguments> &args) {
    Sequence::args = args;
}

void Sequence::set_partition(const shared_ptr<Partition> &partition) {
    Sequence::partition = partition;
}

void Sequence::set_cls(const shared_ptr<ClassInfo> &cls) {
    Sequence::cls = cls;
}

void Sequence::set_invdb(const shared_ptr<InvertDatabase> &invdb) {
    Sequence::invdb = invdb;
}

void Sequence::process_itemset(Itemset_S iset, unsigned int templ, int iter) {
    if (iter > numLargeItemset->size()) return;

    ostringstream &logger = env.logger;
    ostringstream &seqstrm = env.seqstrm;

    int i, it2;
    int lsup, esup;
    unsigned int ntpl;
    Itemset_S iset2, ejoin, ljoin;
    int it = (*iset)[0];
    if (args->maxiter < iter) args->maxiter = iter;
    if (invdb->get_eqgraph_item(it)) {
        for (i = 0; i < invdb->get_eqgraph_item(it)->num_elements(); i++) {
            it2 = invdb->get_eqgraph_item(it)->get_element(i);
            ntpl = templ;
            iset2 = invdb->get_item(it2);
            ljoin = nullptr;
            ejoin = prune_decision(iset2, iset, ntpl, EJOIN);
            if (args->pruning_type > 1) pre_pruning(ejoin, ntpl, iset, iset2, 0);
            if (ejoin) get_tmpnewf_intersect(ljoin, ejoin, ljoin, lsup, esup, lsup, iset2, iset, iter);
            if (ejoin) fill_join(ejoin, iset, iset2);
            if (args->pruning_type > 1) post_pruning(ejoin, ntpl);
            if (ejoin) {
                numLargeItemset->at(iter - 1)++;
                //fill_join(ejoin, iset, iset2);
                //if (iter > 3) ejoin->print_seq(seqstrm, ntpl);
                if (args->outputfreq) ejoin->print_seq(seqstrm, ntpl);
                process_itemset(ejoin, ntpl, iter + 1);
                ejoin = nullptr;
            }
        }

        for (i = 0; i < invdb->get_eqgraph_item(it)->seqnum_elements(); i++) {
            it2 = invdb->get_eqgraph_item(it)->seqget_element(i);
            ntpl = SETBIT(templ, 1, iter - 2);
            iset2 = invdb->get_item(it2);
            ejoin = nullptr;
            ljoin = prune_decision(iset2, iset, ntpl, LJOIN);
            if (args->pruning_type > 1) pre_pruning(ljoin, ntpl, iset, iset2, 1);
            if (ljoin) get_tmpnewf_intersect(ljoin, ejoin, ejoin, lsup, esup, esup, iset2, iset, iter);
            if (ljoin) fill_join(ljoin, iset, iset2);
            if (args->pruning_type > 1) post_pruning(ljoin, ntpl);
            if (ljoin) {
                numLargeItemset->at(iter - 1)++;
                //fill_join(ljoin, iset, iset2);
                //if (iter > 3) ljoin->print_seq(seqstrm, ntpl);
                if (args->outputfreq) ljoin->print_seq(seqstrm, ntpl);
                process_itemset(ljoin, ntpl, iter + 1);
                ljoin = nullptr;
            }
        }
    }
}

void Sequence::process_maxgap(Eqclass_S L2) {
    shared_ptr<ListNodes<shared_ptr<Itemset>>> hdr = L2->list()->head();
    for (; hdr; hdr = hdr->next()) {
        auto item = hdr->item();
        auto clsnum = (*item)[0];
        process_itemset(hdr->item(), L2->templ(), 3);
    }

    hdr = L2->list2()->head();
    for (; hdr; hdr = hdr->next()) {
        auto item = hdr->item();
        auto clsnum = (*item)[0];
        process_itemset(hdr->item(), L2->templ2(), 3);
    }
}

//void Sequence::set_ibm(const shared_ptr<ItemBufferManager> &ibm) {
//    Sequence::ibm = ibm;
//}

void Sequence::set_num_large_dataset(const shared_ptr<vint> &numLargeItemset) {
    Sequence::numLargeItemset = numLargeItemset;
}

Sequence::Sequence(Env &env_) : env(env_) {}

result_t sequenceFunc(Env& env, const shared_ptr<SpadeArguments>& args) {
    Sequence sequence(env);
    shared_ptr<vint> numLargeItemset = make_shared<vint>();
    shared_ptr<ClassInfo> cls = make_shared<ClassInfo>();
    shared_ptr<Partition> partition = make_shared<Partition>();
    shared_ptr<InvertDatabase> invdb = make_shared<InvertDatabase>(env);
//    shared_ptr<ItemBufferManager> ibm = make_shared<ItemBufferManager>();

    sequence.set_args(args);
//    sequence.set_ibm(ibm);
    sequence.set_cls(cls);
    sequence.set_invdb(invdb);
    sequence.set_partition(partition);
    sequence.set_num_large_dataset(numLargeItemset);

    cls->setArgs(args);

    partition->set_args(args);

    invdb->set_cls(cls);
    invdb->setArgs(args);
    invdb->setPartition(partition);

    ostringstream &summary = env.summary;
    ostringstream &logger = env.logger;
    ostringstream &seqstrm = env.seqstrm;

    cls->init();

    int external_block_size =
            args->num_partitions + (args->total_trans_count + args->num_partitions - 1) / args->num_partitions;
    invdb->init(external_block_size);
    sequence.set_num_large_dataset(numLargeItemset);

    partition->init();
    partition->partition_alloc();

    int i;
    sequence.read_files();

    if (args->use_maxgap) {
        invdb->init_buffer(cls->get_num_class(), numLargeItemset->at(0));
    }

    sequence.newSeq();

    summary << "SPADE";

    if (args->use_hash) {
        summary << " USEHASH";
    }
    summary << " " << args->dataf << " " << args->min_support_per_class << " " << args->min_support << " "
            << args->num_intersect << " " << args->pruning_type << " " << args->L2pruning << " " << args->prepruning
            << " " << args->postpruning;
    if (args->use_window) {
        summary << " " << args->use_window << " " << args->max_gap;
    } else {
        summary << " 0";
        if (args->use_maxgap) {
            summary << " " << args->max_gap;
        } else {
            summary << " -1";
        }
    }

    summary << " " << args->min_gap << " " << args->max_iset_len << " " << args->max_seq_len;

    for (i = 0; i < numLargeItemset->size(); i++) {
        summary << " " << numLargeItemset->at(i);
    }
    summary << endl;
    result_t result;
    result.summary = summary.str();
    result.logger = logger.str();
    result.seqstrm = seqstrm.str();
    result.nsequences = args->total_trans_count;

    return result;
}

result_t sequenceWrapper(const string &s, shared_ptr<Env>& envptr) {
    args_t args_ = parse(s);
    Env env;
    shared_ptr<SpadeArguments> args = make_shared<SpadeArguments>();
    args->parse_args(args_.argc, args_.argv);
    if (envptr == nullptr) {
        return sequenceFunc(env, args);
    } else {
        return sequenceFunc(*envptr, args);
    }
}