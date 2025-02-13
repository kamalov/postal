#include "prelude.cpp"

struct B {
    i64 i;
    friend bool operator==(const B& l, const B& r) {
        return (l.i == r.i);
    }
};

namespace std {
    template<>
    struct hash<B> {
        std::size_t operator()(const B& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.i) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

struct A {
    shared_pointer<B> b;
};


void run() {
    shared_pointer<A> a;
    a = create_shared_pointer<A >();
    printf("%s %lld\n", ("done"s).c_str(), (i64)(4ll));
}


