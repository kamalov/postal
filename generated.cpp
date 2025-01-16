#include "prelude.h"

struct A {
    shared_vector<i64> i;
};


shared_vector<shared_pointer<A>> test() {
    shared_vector<shared_pointer<A>> arr;
    shared_pointer<A> a;
    arr = create_shared_vector<shared_pointer<A>>();
    arr_set_len(arr, 1000000ll);

    auto __expr0 = arr;
    for (i64 arr__it0__idx = 0; arr__it0__idx < (i64)__expr0->size(); arr__it0__idx++) {
        shared_pointer<A> arr__it0 = (*__expr0)[arr__it0__idx];
        a = create_shared_pointer<A >();
        (a->i) = create_shared_vector<i64>();
        push((a->i), arr__it0__idx);
        arr->at(arr__it0__idx) = a;
    }
    return arr;
}

void test1() {
    i64 i;
    shared_vector<shared_pointer<A>> a;
    i = 0ll;

    for (;;) {
        i = (i + 1ll);

        if ((i > 100ll)) {
            break;
        };
        a = test();
        printf("%lld %lld\n", (i64)(i), (i64)((arr_last(a)->i)->at(0ll)));
    }
}

void run() {
    test1();
    printf("%s\n", ("done"s).c_str());
    readln();
    //b = test()
}


