#include "prelude.h"

/// prelude template <typename T>i64 len(std::vector<T>* a);
/// prelude template <typename T>void push(std::vector<T>* a, T elem);
/// prelude template <typename T>T pop(std::vector<T>* a);
/// prelude template <typename T>void sort(std::vector<T>* a);
/// prelude template <typename T>void arr_push_front(std::vector<T>* a, T elem);
/// prelude template <typename T>T arr_pop_front(std::vector<T>* a, T elem);
/// prelude template <typename T>T arr_last(std::vector<T>* a);
/// prelude template <typename T>void arr_set_len(std::vector<T>* a, i64 new_len);
/// prelude template <typename T>i64 arr_contains(std::vector<T>* a, T value);
/// prelude template <typename T>void arr_remove_at(std::vector<T>* a, i64 index);
/// prelude template <typename T>void arr_remove(std::vector<T>* a, T value);
/// prelude template <typename T>i64 arr_index_of(std::vector<T>* a, T value);
/// prelude template <typename T>std::vector<T>* arr_slice(std::vector<T>* a, i64 from, i64 to);
/// prelude template <typename K, typename V>void map_add(universal_hashmap<K, V>* map, K key, V value);
/// prelude template <typename K, typename V>void map_add_or_update(universal_hashmap<K, V>* map, K key, V value);
/// prelude template <typename K, typename V>i64 map_has_key(universal_hashmap<K, V>* map, K key);
/// prelude template <typename K, typename V>V map_get_value(universal_hashmap<K, V>* map, K key);
/// prelude template <typename K, typename V>void map_remove(universal_hashmap<K, V>* map, K key);
/// prelude template <typename K, typename V>i64 map_len(universal_hashmap<K, V>* map);
/// prelude template <typename K, typename V>std::vector<K>* map_keys(universal_hashmap<K, V>* map);
/// prelude std::vector<std::string>* str_split(std::string s, std::string by);
/// prelude i64 str_to_int(std::string s);
/// prelude std::vector<std::string>* str_to_chars(std::string s);
/// prelude i64 str_contains(std::string s, std::string subs);
/// prelude i64 str_len(std::string s);
/// prelude std::string str_remove(std::string s, std::string r);
/// prelude std::string str_arr_join(std::vector<std::string>* a, std::string delimiter);
/// prelude std::string int_to_str(i64 i);
/// prelude void err(std::string s);
/// prelude std::string readln();
/// prelude std::vector<std::string>* read_string_lines_from_file(std::string filename);
struct Lock {
    std::vector<i64>* vals;
};


struct Key {
    std::vector<i64>* vals;
};


struct Data {
    std::vector<Lock*>* locks;
    std::vector<Key*>* keys;
};


std::string int_array_to_str(std::vector<i64>* a) {
    std::string s;
    s = ""s;

    auto __expr0 = a;
    for (i64 a__it0__idx = 0; a__it0__idx < __expr0->size(); a__it0__idx++) {
        i64 a__it0 = (*__expr0)[a__it0__idx];

        if (s != ""s) {
            s = s + ","s;
        };
        s = s + int_to_str(a__it0);
    }
    return s;
}

void process(Data* d, std::vector<std::string>* lines) {
    std::vector<std::string>* chars;
    std::vector<i64>* vals;
    i64 ci;
    i64 ri;
    Lock* lock;
    Key* key;
    chars = str_to_chars(str_arr_join(lines, ""s));
    vals = new std::vector<i64>();

    if (chars->at(0ll) == "#"s) {

        auto __expr0 = create_range(0ll, 4ll);
        for (i64 expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
            i64 expr__it0 = (*__expr0)[expr__it0__idx];
            ci = expr__it0;

            auto __expr1 = create_range(1ll, 6ll);
            for (i64 expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
                i64 expr__it1 = (*__expr1)[expr__it1__idx];
                ri = expr__it1;

                if (chars->at(ri*5ll + ci) == "."s) {
                    break;
                };
            }
            push(vals, ri - 1ll);
        }
        lock = new Lock();
        lock->vals = vals;
        push(d->locks, lock);
    }
    else {

        auto __expr2 = create_range(0ll, 4ll);
        for (i64 expr__it2__idx = 0; expr__it2__idx < __expr2->size(); expr__it2__idx++) {
            i64 expr__it2 = (*__expr2)[expr__it2__idx];
            ci = expr__it2;
            ri = 5ll;

            for (;;) {

                if (chars->at(ri*5ll + ci) == "."s) {
                    break;
                };
                ri = ri - 1ll;
            }
            push(vals, 5ll - ri);
        }
        key = new Key();
        key->vals = vals;
        push(d->keys, key);
    };
}

i64 is_match(Lock* lock, Key* key) {

    auto __expr0 = create_range(0ll, 4ll);
    for (i64 expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];

        if (lock->vals->at(expr__it0) + key->vals->at(expr__it0) > 5ll) {
            return 0ll;
        };
    }
    return 1ll;
}

void run() {
    std::vector<std::string>* lines;
    Data* d;
    std::vector<std::string>* b;
    i64 total;
    Lock* lock;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    d = new Data();
    d->locks = new std::vector<Lock*>();
    d->keys = new std::vector<Key*>();
    b = new std::vector<std::string>();

    auto __expr0 = lines;
    for (i64 lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];

        if (lines__it0 == ""s) {
            process(d, b);
            b = new std::vector<std::string>();
        }
        else {
            push(b, lines__it0);
        };
    }
    process(d, b);
    printf("%s\n", ("locks"s).c_str());

    auto __expr1 = d->locks;
    for (i64 expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
        Lock* expr__it1 = (*__expr1)[expr__it1__idx];
        printf("%s\n", (int_array_to_str(expr__it1->vals)).c_str());
    }
    printf("%s\n", ("\nkeys"s).c_str());

    auto __expr2 = d->keys;
    for (i64 expr__it2__idx = 0; expr__it2__idx < __expr2->size(); expr__it2__idx++) {
        Key* expr__it2 = (*__expr2)[expr__it2__idx];
        printf("%s\n", (int_array_to_str(expr__it2->vals)).c_str());
    }
    total = 0ll;

    auto __expr3 = d->locks;
    for (i64 expr__it3__idx = 0; expr__it3__idx < __expr3->size(); expr__it3__idx++) {
        Lock* expr__it3 = (*__expr3)[expr__it3__idx];
        lock = expr__it3;

        auto __expr4 = d->keys;
        for (i64 expr__it4__idx = 0; expr__it4__idx < __expr4->size(); expr__it4__idx++) {
            Key* expr__it4 = (*__expr4)[expr__it4__idx];
            total = total + is_match(lock, expr__it4);
        }
    }
    printf("%s %lld\n", ("matches"s).c_str(), static_cast<i64>(total));
}


