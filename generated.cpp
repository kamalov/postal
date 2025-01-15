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
i64 f1(i64 a) {
    //log(a)
    return a;
}

struct A {
    _sv_<i64> b;
};


_sp_<A> get_a() {
    _sp_<A> a;
    a = _spi_<A >();
    (a->b) = _svi_<i64>();
    return a;
}

void run() {
    _sp_<A> a;
    i64 c;
    i64 f;
    //a = 5
    //if not a do log('not a')
    a = get_a();
    push((a->b), 5ll);
    //b = [int]
    c = (-1ll);
    f = 1ll;

    if (((!(-1ll))-1ll)) {
        c = (1ll + (a->b)->at(0ll));
    };
    printf("%lld\n", static_cast<i64>(c));
}


