#include "prelude.h"

struct A {
    i64 i;
    friend bool operator==(const A& l, const A& r) {
        return (l.i == r.i);
    }
};

namespace std {
    template<>
    struct hash<A> {
        std::size_t operator()(const A& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.i) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

_sv_<_sp_<A>> test() {
    _sv_<_sp_<A>> arr;
    _sp_<A> a;
    arr = _svi_<_sp_<A>>();
    arr_set_len(arr, 1000000ll);

    auto __expr0 = arr;
    for (auto arr__it0__idx = 0; arr__it0__idx < __expr0->size(); arr__it0__idx++) {
        _sp_<A> arr__it0 = (*__expr0)[arr__it0__idx];
        a = _spi_<A >();
        (a->i) = arr__it0__idx;
        arr->at(arr__it0__idx) = a;
    }
    return arr;
}

void test1() {
    i64 i;
    _sv_<_sp_<A>> a;
    i = 0ll;

    for (;;) {
        i = (i + 1ll);

        if ((i > 100ll)) {
            break;
        };
        a = test();
        printf("%lld %lld\n", static_cast<i64>(i), static_cast<i64>((arr_last(a)->i)));
    }
}

void run() {
    test1();
    printf("%s\n", ("done"s).c_str());
    readln();
    //b = test()
}


