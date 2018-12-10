#ifndef __LISTS_H
#define __LISTS_H

#include "common.h"

typedef int (*CMP_FUNC)(void *, void *);

template<class T>
class ListNodes {
private:
    shared_ptr<ListNodes<T>> theNext;
    T theItem;

public:
    ListNodes(T item, shared_ptr<ListNodes<T>> next);

    ~ListNodes();

    shared_ptr<ListNodes<T>> next() {
        return theNext;
    }

    void set_next(shared_ptr<ListNodes<T>> next) {
        theNext = next;
    }

    T& item() {
        return theItem;
    }

    void set_item(T item) {
        theItem = item;
    }
};

template<class T>
class Lists {
private:
    shared_ptr<ListNodes<T>> theHead;
    shared_ptr<ListNodes<T>> theLast;
    int theSize;

public:

    Lists();

    ~Lists();

    void clear();

    shared_ptr<ListNodes<T>> head() {
        return theHead;
    };

    shared_ptr<ListNodes<T>> last() {
        return theLast;
    };

    int size() {
        return theSize;
    };

    void append(T item);

    void prepend(T item);

    T find(T item, CMP_FUNC cmpare);
};

#endif// __LISTS_H


