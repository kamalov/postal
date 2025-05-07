#include "prelude.cpp"

/// prelude template <typename T>i64 size(shared_vector<T> a);
/// prelude template <typename T>void push(shared_vector<T> a, T element);
/// prelude template <typename T>T pop(shared_vector<T> a);
/// prelude template <typename T>void array_sort(shared_vector<T> a);
/// prelude template <typename T>void array_push_front(shared_vector<T> a, T element);
/// prelude template <typename T>T array_pop_front(shared_vector<T> a, T element);
/// prelude template <typename T>T array_last(shared_vector<T> a);
/// prelude template <typename T>void array_set_size(shared_vector<T> a, i64 new_size);
/// prelude template <typename T>i64 array_contains(shared_vector<T> a, T value);
/// prelude template <typename T>void array_remove_at(shared_vector<T> a, i64 index);
/// prelude template <typename T>void array_remove(shared_vector<T> a, T value);
/// prelude template <typename T>i64 array_index_of(shared_vector<T> a, T value);
/// prelude template <typename T>shared_vector<T> array_slice(shared_vector<T> a, i64 from, i64 to);
/// prelude template <typename K, typename V>void map_add(shared_map<K, V> map, K key, V value);
/// prelude template <typename K, typename V>V map_get_value(shared_map<K, V> map, K key);
/// prelude template <typename K, typename V>void map_add_or_update(shared_map<K, V> map, K key, V value);
/// prelude template <typename K, typename V>i64 map_has_key(shared_map<K, V> map, K key);
/// prelude template <typename K, typename V>void map_remove(shared_map<K, V> map, K key);
/// prelude template <typename K, typename V>i64 map_size(shared_map<K, V> map);
/// prelude template <typename K, typename V>shared_vector<K> map_keys(shared_map<K, V> map);
/// prelude shared_vector<std::string> string_split(std::string s, std::string by);
/// prelude i64 string_to_integer(std::string s);
/// prelude shared_vector<std::string> string_to_chars(std::string s);
/// prelude i64 string_contains(std::string s, std::string subs);
/// prelude i64 string_size(std::string s);
/// prelude std::string string_remove(std::string s, std::string r);
/// prelude std::string string_array_join(shared_vector<std::string> a, std::string delimiter);
/// prelude std::string integer_to_string(i64 i);
/// prelude void error(std::string s);
/// prelude std::string read_line();
/// prelude shared_vector<std::string> read_string_lines_from_file(std::string filename);
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
    shared_pointer<A> a;
    shared_pointer<A> b;
    a = create_shared_pointer<A >();
    a->v = 5ll;
    b = create_shared_pointer<A >();
    b->v = 5ll;
    printf("%s %d\n", ("test equals"s).c_str(), a == b);
}

void test_array() {
    shared_vector<shared_pointer<A>> aa;
    shared_pointer<A> a;
    shared_pointer<A> b;
    shared_vector<shared_pointer<A>> slice;
    aa = create_shared_vector<shared_pointer<A>>();
    a = create_shared_pointer<A >();
    a->v = 5ll;
    push(aa, a);
    b = create_shared_pointer<A >();
    b->v = 5ll;
    push(aa, b);
    printf("%s %lld\n", ("object array contains A {v: 5} ="s).c_str(), (i64)(array_contains(aa, b)));
    printf("%s %lld\n", ("object array len ="s).c_str(), (i64)(size(aa)));
    printf("%s %lld\n", ("object array first elem v field value ="s).c_str(), (i64)(aa->at(0ll)->v));
    a->v = 6ll;
    printf("%s %lld\n", ("object array contains A {v: 6} ="s).c_str(), (i64)(array_contains(aa, a)));
    printf("%s %lld\n", ("object array contains A {v: 5} ="s).c_str(), (i64)(array_contains(aa, b)));
    slice = array_slice(aa, 0ll, 0ll);
    printf("%s %lld\n", ("arr slice first elem value"s).c_str(), (i64)(slice->at(0ll)->v));
}

void test_map() {
    shared_map<i64, i64> i;
    shared_map<shared_pointer<A>, i64> m;
    shared_pointer<A> a;
    shared_pointer<A> b;
    i = create_shared_map<i64, i64>();
    map_add(i, 1ll, 10ll);
    map_add(i, 2ll, 11ll);
    printf("%s %lld\n", ("test simple map get value"s).c_str(), (i64)(map_get_value(i, 1ll)));
    m = create_shared_map<shared_pointer<A>, i64>();
    a = create_shared_pointer<A >();
    a->v = 5ll;
    map_add(m, a, 15ll);
    b = create_shared_pointer<A >();
    b->v = 5ll;
    printf("%s %lld\n", ("test shared map get value"s).c_str(), (i64)(map_get_value(m, b)));
    a->v = 6ll;
    printf("%s %lld\n", ("test shared map has key"s).c_str(), (i64)(map_has_key(m, a)));

    auto __expr0 = map_keys(i);
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        printf("%s %lld %lld\n", ("simple map key"s).c_str(), (i64)(expr__it0__idx), (i64)(expr__it0));
    }

    auto __expr1 = map_keys(m);
    for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
        shared_pointer<A> expr__it1 = (*__expr1)[expr__it1__idx];
        printf("%s %lld %lld\n", ("shared map key"s).c_str(), (i64)(expr__it1__idx), (i64)(expr__it1->v));
    }
}

void test_string() {
    std::string s;
    shared_vector<std::string> a;
    s = "1 2 3"s;
    a = create_shared_vector<std::string>();

    auto __expr0 = string_split(s, " "s);
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        std::string expr__it0 = (*__expr0)[expr__it0__idx];
        push(a, expr__it0);
        printf("%s %s %s\n", (s).c_str(), ("split"s).c_str(), (expr__it0).c_str());
    }
    printf("%s %s\n", ("str_arr_join"s).c_str(), (string_array_join(a, " + "s)).c_str());
}

void test_size() {
    shared_vector<shared_pointer<A>> arr;
    i64 i;
    shared_pointer<A> a;
    arr = create_shared_vector<shared_pointer<A>>();
    array_set_size(arr, 1000000ll);
    i = 0ll;
    printf("%s\n", ("array set"s).c_str());
    //read_line()

    for (;;) {

        if (i == size(arr)) {
            break;
        };
        a = create_shared_pointer<A >();
        a->v = i;
        arr->at(i) = a;
        i = i + 1ll;
    }
}

void run() {
    i64 i;
    test_equals();
    test_array();
    test_map();
    test_string();
    i = 0ll;

    for (;;) {
        i = i + 1ll;
        test_size();
        printf("%lld\n", (i64)(i));
        //read_line()

        if (i == 10ll) {
            break;
        };
    }
    // read_line()
}


