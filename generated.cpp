#include "prelude.h"

/// prelude template <typename T>i64 len(_sv_<T> a);
/// prelude template <typename T>void push(_sv_<T> a, T elem);
/// prelude template <typename T>T pop(_sv_<T> a);
/// prelude template <typename T>void sort(_sv_<T> a);
/// prelude template <typename T>void arr_push_front(_sv_<T> a, T elem);
/// prelude template <typename T>T arr_pop_front(_sv_<T> a, T elem);
/// prelude template <typename T>T arr_last(_sv_<T> a);
/// prelude template <typename T>void arr_set_len(_sv_<T> a, i64 new_len);
/// prelude template <typename T>i64 arr_contains(_sv_<T> a, T value);
/// prelude template <typename T>void arr_remove_at(_sv_<T> a, i64 index);
/// prelude template <typename T>void arr_remove(_sv_<T> a, T value);
/// prelude template <typename T>i64 arr_index_of(_sv_<T> a, T value);
/// prelude template <typename T>_sv_<T> arr_slice(_sv_<T> a, i64 from, i64 to);
/// prelude template <typename K, typename V>void map_add(_sm_<K, V> map, K key, V value);
/// prelude template <typename K, typename V>V map_get_value(_sm_<K, V> map, K key);
/// prelude template <typename K, typename V>void map_add_or_update(_sm_<K, V> map, K key, V value);
/// prelude template <typename K, typename V>i64 map_has_key(_sm_<K, V> map, K key);
/// prelude template <typename K, typename V>void map_remove(_sm_<K, V> map, K key);
/// prelude template <typename K, typename V>i64 map_len(_sm_<K, V> map);
/// prelude template <typename K, typename V>_sv_<K> map_keys(_sm_<K, V> map);
/// prelude _sv_<std::string> str_split(std::string s, std::string by);
/// prelude i64 str_to_int(std::string s);
/// prelude _sv_<std::string> str_to_chars(std::string s);
/// prelude i64 str_contains(std::string s, std::string subs);
/// prelude i64 str_len(std::string s);
/// prelude std::string str_remove(std::string s, std::string r);
/// prelude std::string str_arr_join(_sv_<std::string> a, std::string delimiter);
/// prelude std::string int_to_str(i64 i);
/// prelude void err(std::string s);
/// prelude std::string readln();
/// prelude _sv_<std::string> read_string_lines_from_file(std::string filename);
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
    _sp_<A> a;
    _sp_<A> b;
    a = _spi_<A >();
    a->v = 5ll;
    b = _spi_<A >();
    b->v = 5ll;
    printf("%s %lld\n", ("test equals"s).c_str(), static_cast<i64>(a == b));
}

void test_array() {
    _sv_<_sp_<A>> aa;
    _sp_<A> a;
    _sp_<A> b;
    _sv_<_sp_<A>> slice;
    aa = _svi_<_sp_<A>>();
    a = _spi_<A >();
    a->v = 5ll;
    push(aa, a);
    b = _spi_<A >();
    b->v = 5ll;
    push(aa, b);
    printf("%s %lld\n", ("object array contains A {v: 5} ="s).c_str(), static_cast<i64>(arr_contains(aa, b)));
    printf("%s %lld\n", ("object array len ="s).c_str(), static_cast<i64>(len(aa)));
    printf("%s %lld\n", ("object array first elem v field value ="s).c_str(), static_cast<i64>(aa->at(0ll)->v));
    a->v = 6ll;
    printf("%s %lld\n", ("object array contains A {v: 6} ="s).c_str(), static_cast<i64>(arr_contains(aa, a)));
    printf("%s %lld\n", ("object array contains A {v: 5} ="s).c_str(), static_cast<i64>(arr_contains(aa, b)));
    slice = arr_slice(aa, 0ll, 0ll);
    printf("%s %lld\n", ("arr slice first elem value"s).c_str(), static_cast<i64>(slice->at(0ll)->v));
}

void test_map() {
    _sm_<i64, i64> i;
    _sm_<_sp_<A>, i64> m;
    _sp_<A> a;
    _sp_<A> b;
    i = _smi_<i64, i64>();
    map_add(i, 1ll, 10ll);
    map_add(i, 2ll, 11ll);
    printf("%s %lld\n", ("test simple map get value"s).c_str(), static_cast<i64>(map_get_value(i, 1ll)));
    m = _smi_<_sp_<A>, i64>();
    a = _spi_<A >();
    a->v = 5ll;
    map_add(m, a, 15ll);
    b = _spi_<A >();
    b->v = 5ll;
    printf("%s %lld\n", ("test shared map get value"s).c_str(), static_cast<i64>(map_get_value(m, b)));
    a->v = 6ll;
    printf("%s %lld\n", ("test shared map has key"s).c_str(), static_cast<i64>(map_has_key(m, a)));

    auto __expr0 = map_keys(i);
    for (i64 expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        printf("%s %lld %lld\n", ("simple map key"s).c_str(), static_cast<i64>(expr__it0__idx), static_cast<i64>(expr__it0));
    }

    auto __expr1 = map_keys(m);
    for (i64 expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
        _sp_<A> expr__it1 = (*__expr1)[expr__it1__idx];
        printf("%s %lld %lld\n", ("shared map key"s).c_str(), static_cast<i64>(expr__it1__idx), static_cast<i64>(expr__it1->v));
    }
}

void test_str() {
    std::string s;
    _sv_<std::string> a;
    s = "1 2 3"s;
    a = _svi_<std::string>();

    auto __expr0 = str_split(s, " "s);
    for (i64 expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        std::string expr__it0 = (*__expr0)[expr__it0__idx];
        push(a, expr__it0);
        printf("%s %s %s\n", (s).c_str(), ("split"s).c_str(), (expr__it0).c_str());
    }
    printf("%s %s\n", ("str_arr_join"s).c_str(), (str_arr_join(a, " + "s)).c_str());
}

void test_size() {
    _sv_<_sp_<A>> arr;
    i64 i;
    _sp_<A> a;
    arr = _svi_<_sp_<A>>();
    arr_set_len(arr, 1000000ll);
    i = 0ll;
    printf("%s\n", ("arr set"s).c_str());
    readln();

    for (;;) {

        if (i == len(arr)) {
            break;
        };
        a = _spi_<A >();
        a->v = i;
        arr->at(i) = a;
        i = i + 1ll;
    }
}

void run() {
    i64 i;
    // test_equals()
    // test_array()
    // test_map()
    //test_str()
    i = 0ll;

    for (;;) {
        i = i + 1ll;
        test_size();
        printf("%lld\n", static_cast<i64>(i));
        readln();

        if (i == 1000ll) {
            break;
        };
    }
    readln();
}


