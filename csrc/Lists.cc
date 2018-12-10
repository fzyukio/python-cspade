#include "common.h"
#include "Lists.h"
#include "Array.h"
#include "Eqclass.h"
#include "Itemset.h"

template<class T>
ListNodes<T>::ListNodes(T item, shared_ptr<ListNodes<T>> next) {
    theItem = item;
    theNext = next;
}

template<class T>
ListNodes<T>::~ListNodes() {
    theNext = nullptr;
    theItem = nullptr;
}

template<class T>
Lists<T>::Lists() {
    theHead = 0;
    theLast = 0;
    theSize = 0;
}

//only listnodes are deleted, if node->item() is a pointer to some object
//that object is *not* deleted
template<class T>
Lists<T>::~Lists() = default;

//listnodes are deleted, if node->item() is a pointer to some object
//that object is *also*  deleted
template<class T>
void Lists<T>::clear() {
    theHead = nullptr;
    theLast = nullptr;
    theSize = 0;
}


template<class T>
void Lists<T>::append(T item) {
    shared_ptr<ListNodes<T>> node;

    theSize++;
    node.reset(new ListNodes<T>(item, 0));

    if (theHead == nullptr) {
        theHead = node;
        theLast = node;
    } else {
        theLast->set_next(node);
        theLast = node;
    }
}


template<class T>
void Lists<T>::prepend(T item) {
    shared_ptr<ListNodes<T>> node;

    theSize++;
    node.reset(new ListNodes<T>(item, 0));

    if (theHead == 0) {
        theHead = node;
        theLast = node;
    } else {
        node->set_next(theHead);
        theHead = node;
    }
}

template
class Lists<int *>;

template
class Lists<Array_S>;

template
class Lists<shared_ptr<Itemset>>;

template
class Lists<Eqclass_S>;
