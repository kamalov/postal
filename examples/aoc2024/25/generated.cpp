#include "prelude.cpp"

/// prelude template <typename T>i64 array_size(shared_vector<T> a);
/// prelude template <typename T>void array_push(shared_vector<T> a, T element);
/// prelude template <typename T>T array_pop(shared_vector<T> a);
/// prelude template <typename T>void array_quick_sort(shared_vector<T> a);
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
////
//// prelude end
////
struct Lock {
    shared_vector<i64> vals;
};


struct Key {
    shared_vector<i64> vals;
};


struct Data {
    shared_vector<shared_pointer<Lock>> locks;
    shared_vector<shared_pointer<Key>> keys;
};


std::string integer_array_to_string(shared_vector<i64> a) {
    std::string s;
    s = ""s;

    auto __expr0 = a;
    for (i64 a__it0__idx = 0; a__it0__idx < (i64)__expr0->size(); a__it0__idx++) {
        i64 a__it0 = (*__expr0)[a__it0__idx];

        if (s != ""s) {
            s = s + ","s;
        };
        s = s + integer_to_string(a__it0);
    }
    return s;
}

void process(shared_pointer<Data> d, shared_vector<std::string> lines) {
    shared_vector<std::string> chars;
    shared_vector<i64> vals;
    i64 ci;
    i64 ri;
    shared_pointer<Lock> lock;
    shared_pointer<Key> key;
    chars = string_to_chars(string_array_join(lines, ""s));
    vals = create_shared_vector<i64>();

    if (chars->at(0ll) == "#"s) {

        auto __expr0 = create_range(0ll, 4ll);
        for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
            i64 expr__it0 = (*__expr0)[expr__it0__idx];
            ci = expr__it0;
            ri = 0ll;

            auto __expr1 = create_range(1ll, 6ll);
            for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
                i64 expr__it1 = (*__expr1)[expr__it1__idx];
                ri = expr__it1;

                if (chars->at(ri*5ll + ci) == "."s) {
                    break;
                };
            }
            array_push(vals, ri-1ll);
        }
        lock = create_shared_pointer<Lock >();
        lock->vals = vals;
        array_push(d->locks, lock);
    }
    else {

        auto __expr2 = create_range(0ll, 4ll);
        for (i64 expr__it2__idx = 0; expr__it2__idx < (i64)__expr2->size(); expr__it2__idx++) {
            i64 expr__it2 = (*__expr2)[expr__it2__idx];
            ci = expr__it2;
            ri = 5ll;

            for (;;) {

                if (chars->at(ri*5ll + ci) == "."s) {
                    break;
                };
                ri = ri-1ll;
            }
            array_push(vals, 5ll-ri);
        }
        key = create_shared_pointer<Key >();
        key->vals = vals;
        array_push(d->keys, key);
    };
}

i64 is_match(shared_pointer<Lock> lock, shared_pointer<Key> key) {

    auto __expr0 = create_range(0ll, 4ll);
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];

        if (lock->vals->at(expr__it0) + key->vals->at(expr__it0) > 5ll) {
            return 0ll;
        };
    }
    return 1ll;
}

void run() {
    shared_vector<std::string> lines;
    shared_pointer<Data> d;
    shared_vector<std::string> b;
    i64 total;
    shared_pointer<Lock> lock;
    lines = read_string_lines_from_file("D:/src/postal/examples/aoc2024/input.txt"s);
    d = create_shared_pointer<Data >();
    d->locks = create_shared_vector<shared_pointer<Lock>>();
    d->keys = create_shared_vector<shared_pointer<Key>>();
    b = create_shared_vector<std::string>();

    auto __expr0 = lines;
    for (i64 lines__it0__idx = 0; lines__it0__idx < (i64)__expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];

        if (lines__it0 == ""s) {
            process(d, b);
            b = create_shared_vector<std::string>();
        }
        else {
            array_push(b, lines__it0);
        };
    }
    process(d, b);
    printf("%s\n", ("locks"s).c_str());

    auto __expr1 = d->locks;
    for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
        shared_pointer<Lock> expr__it1 = (*__expr1)[expr__it1__idx];
        printf("%s\n", (integer_array_to_string(expr__it1->vals)).c_str());
    }
    printf("%s\n", ("\nkeys"s).c_str());

    auto __expr2 = d->keys;
    for (i64 expr__it2__idx = 0; expr__it2__idx < (i64)__expr2->size(); expr__it2__idx++) {
        shared_pointer<Key> expr__it2 = (*__expr2)[expr__it2__idx];
        printf("%s\n", (integer_array_to_string(expr__it2->vals)).c_str());
    }
    total = 0ll;

    auto __expr3 = d->locks;
    for (i64 expr__it3__idx = 0; expr__it3__idx < (i64)__expr3->size(); expr__it3__idx++) {
        shared_pointer<Lock> expr__it3 = (*__expr3)[expr__it3__idx];
        lock = expr__it3;

        auto __expr4 = d->keys;
        for (i64 expr__it4__idx = 0; expr__it4__idx < (i64)__expr4->size(); expr__it4__idx++) {
            shared_pointer<Key> expr__it4 = (*__expr4)[expr__it4__idx];
            total = total + is_match(lock, expr__it4);
        }
    }
    printf("%s %lld\n", ("matches"s).c_str(), (i64)(total));
}

