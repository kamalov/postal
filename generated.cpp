#include "prelude.h"

struct A {
    i64 b;
    shared_vector<i64> i;
    friend bool operator==(const A& l, const A& r) {
        return (l.b == r.b);
    }
};

namespace std {
    template<>
    struct hash<A> {
        std::size_t operator()(const A& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.b) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

shared_vector<shared_pointer<A>> test() {
    shared_vector<shared_pointer<A>> arr;
    shared_pointer<A> a;
    arr = create_shared_vector<shared_pointer<A>>();
    arr_set_len(arr, 1ll);

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
    // arr = [A]
    // arr_set_len(arr, 10)
    // for arr {
    //     a = A {}
    //     arr[idx] = a
    // }
    printf("%s\n", ("done"s).c_str());
    //readln()
}


