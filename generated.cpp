#include "prelude.h"

/// prelude template <typename T>i64 len(_sv_<T > a);
/// prelude template <typename T>void push(_sv_<T > a, T elem);
/// prelude template <typename T>T pop(_sv_<T > a);
/// prelude template <typename T>void sort(_sv_<T > a);
/// prelude template <typename T>void arr_push_front(_sv_<T > a, T elem);
/// prelude template <typename T>T arr_pop_front(_sv_<T > a, T elem);
/// prelude template <typename T>T arr_last(_sv_<T > a);
/// prelude template <typename T>void arr_set_len(_sv_<T > a, i64 new_len);
/// prelude template <typename T>i64 arr_contains(_sv_<T > a, T value);
/// prelude template <typename T>void arr_remove_at(_sv_<T > a, i64 index);
/// prelude template <typename T>void arr_remove(_sv_<T > a, T value);
/// prelude template <typename T>i64 arr_index_of(_sv_<T > a, T value);
/// prelude template <typename T>_sv_<T > arr_slice(_sv_<T > a, i64 from, i64 to);
/// prelude template <typename K, typename V>void map_add(_sm_<K, V > map, K key, V value);
/// prelude template <typename K, typename V>V map_get_value(_sm_<K, V > map, K key);
/// prelude template <typename K, typename V>void map_add_or_update(_sm_<K, V > map, K key, V value);
/// prelude template <typename K, typename V>i64 map_has_key(_sm_<K, V > map, K key);
/// prelude template <typename K, typename V>void map_remove(_sm_<K, V > map, K key);
/// prelude template <typename K, typename V>i64 map_len(_sm_<K, V > map);
/// prelude template <typename K, typename V>_sv_<K > map_keys(_sm_<K, V > map);
// fn str_split(s: str, by: str) [str] external
// fn str_to_int(s: str) int external
// fn str_to_chars(s: str) [str] external
// fn str_contains(s: str, subs: str) int external
// fn str_len(s: str) int external
// fn str_remove(s: str, r: str) str external
// fn str_arr_join(a: [str], delimiter: str) str external
// fn int_to_str(i: int) str external
// fn err(s: str) external
// fn readln() str external   
// fn read_string_lines_from_file(filename: str) [str] external
// fn len<T>(a: [T]) int external
// fn push<T>(a: [T], elem: T) external
struct A {
    i64 v;
    friend bool operator==(const A& l, const A& r) {
        return (l.v == r.v);
    }
};

namespace std {
    template<>
    struct hash<A> {
        std::size_t operator()(const A& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.v) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

void test_equals() {
    _sp_<A > a;
    _sp_<A > b;
    a = _spi_<A >();
    a->v = 5ll;
    b = _spi_<A >();
    b->v = 5ll;
    printf("%s %lld\n", ("test equals"s).c_str(), static_cast<i64>(a == b));
}

void test_array() {
    _sv_<_sp_<A > > aa;
    _sp_<A > a;
    _sp_<A > b;
    aa = _svi_<_sp_<A > >();
    a = _spi_<A >();
    a->v = 5ll;
    push(aa, a);
    b = _spi_<A >();
    b->v = 5ll;
    printf("%s %lld\n", ("object array contains A {v: 5} ="s).c_str(), static_cast<i64>(arr_contains(aa, b)));
    printf("%s %lld\n", ("object array len ="s).c_str(), static_cast<i64>(len(aa)));
    printf("%s %lld\n", ("object array first elem v field value ="s).c_str(), static_cast<i64>(aa->at(0ll)->v));
    a->v = 6ll;
    printf("%s %lld\n", ("object array contains A {v: 6} ="s).c_str(), static_cast<i64>(arr_contains(aa, a)));
    printf("%s %lld\n", ("object array contains A {v: 5} ="s).c_str(), static_cast<i64>(arr_contains(aa, b)));
}

void test_map() {
    _sm_<i64, i64 > i;
    _sm_<_sp_<A>, i64 > m;
    _sp_<A > a;
    _sp_<A > b;
    i = _smi_<i64, i64 >();
    map_add(i, 1ll, 10ll);
    printf("%s %lld\n", ("test simple map get value"s).c_str(), static_cast<i64>(map_get_value(i, 1ll)));
    m = _smi_<_sp_<A>, i64 >();
    a = _spi_<A >();
    a->v = 5ll;
    map_add(m, a, 15ll);
    b = _spi_<A >();
    b->v = 5ll;
    printf("%s %lld\n", ("test shared map get value"s).c_str(), static_cast<i64>(map_get_value(m, b)));
    a->v = 6ll;
    printf("%s %lld\n", ("test shared map has key"s).c_str(), static_cast<i64>(map_has_key(m, a)));
}

void run() {
    test_equals();
    test_array();
    test_map();
}


