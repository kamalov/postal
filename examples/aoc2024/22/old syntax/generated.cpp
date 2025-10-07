#include "prelude.h"

/// prelude template <typename T>i64 len(std::vector<T>* a);
/// prelude template <typename T>void push(std::vector<T>* a, T elem);
/// prelude template <typename T>T pop(std::vector<T>* a);
/// prelude template <typename T>void arr_push_front(std::vector<T>* a, T elem);
/// prelude template <typename T>T arr_pop_front(std::vector<T>* a, T elem);
/// prelude template <typename T>T arr_last(std::vector<T>* a);
/// prelude template <typename T>void arr_set_len(std::vector<T>* a, i64 new_len);
/// prelude template <typename T>void arr_sort(std::vector<T>* a);
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
i64 next(i64 number) {
    i64 two24;
    i64 n;
    i64 v;
    two24 = 1ll << 24ll;
    n = number;
    v = n << 6ll;
    n = n^v;
    n = n % two24;
    v = n >> 5ll;
    n = n^v;
    n = n % two24;
    v = n << 11ll;
    n = n^v;
    n = n % two24;
    return n;
}

std::string int_arr_to_str(std::vector<i64>* a) {
    std::string s;
    s = ""s;

    auto __expr0 = a;
    for (int a__it0__idx = 0; a__it0__idx < __expr0->size(); a__it0__idx++) {
        i64 a__it0 = (*__expr0)[a__it0__idx];

        if (s != ""s) {
            s = s + ","s;
        };
        s = s + int_to_str(a__it0);
    }
    return s;
}

i64 get_number(std::vector<i64>* last4, i64 base) {
    i64 den;
    i64 v;

    if (len(last4) != 4ll) {
        err("get_number "s + int_to_str(len(last4)));
    };
    den = 1ll;
    v = 0ll;

    auto __expr0 = last4;
    for (int last4__it0__idx = 0; last4__it0__idx < __expr0->size(); last4__it0__idx++) {
        i64 last4__it0 = (*__expr0)[last4__it0__idx];
        v = v + den*(last4__it0 + 9ll);
        den = den*base;
    }
    return v;
}

std::vector<i64>* int_to_arr(i64 n, i64 base) {
    std::vector<i64>* r;
    i64 v;
    i64 rem;
    r = new std::vector<i64>();
    v = n;

    for (;;) {
        rem = v % base;
        push(r, rem - 9ll);
        v = v/base;

        if (v == 0ll) {
            break;
        };
    }
    return r;
}

void run() {
    std::vector<std::string>* lines;
    i64 deep;
    i64 t;
    std::vector<i64>* acc;
    i64 base;
    std::vector<i64>* blocked;
    std::string line;
    i64 v;
    i64 prev;
    std::vector<i64>* last4;
    i64 rem;
    i64 diff;
    i64 n;
    i64 max_v;
    lines = read_string_lines_from_file("D:/src/postal/examples/aoc2024/22/old syntax/input.txt"s);
    deep = 2000ll;
    t = 0ll;
    acc = new std::vector<i64>();
    base = 20ll;
    arr_set_len(acc, base*base*base*base);
    blocked = new std::vector<i64>();
    arr_set_len(blocked, base*base*base*base);

    auto __expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        line = lines__it0;

        if (line == ""s) {
            break;
        };
        v = str_to_int(line);
        prev = v % 10ll;
        printf("%lld\n", static_cast<i64>(v));

        auto __expr1 = create_range(0ll, len(blocked) - 1ll);
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            blocked->at(expr__it1) = 0ll;
        }
        last4 = new std::vector<i64>();

        auto __expr2 = create_range(1ll, deep);
        for (int expr__it2__idx = 0; expr__it2__idx < __expr2->size(); expr__it2__idx++) {
            i64 expr__it2 = (*__expr2)[expr__it2__idx];
            v = next(v);
            rem = v % 10ll;
            diff = rem - prev;
            prev = rem;
            push(last4, diff);

            if (len(last4) > 4ll) {
                arr_pop_front(last4);
            };

            if (len(last4) == 4ll) {
                //log(line, rem, int_arr_to_str(last4))
                n = get_number(last4, base);
                //log(line, rem, int_arr_to_str(last4), '|', int_arr_to_str(int_to_arr(n, base)))

                if (blocked->at(n)) {
                    continue;
                };
                acc->at(n) = acc->at(n) + rem;
                blocked->at(n) = 1ll;
            };
            //log(v, rem, '(', diff,')')
        }
    }
    max_v = 0ll;

    auto __expr3 = acc;
    for (int acc__it3__idx = 0; acc__it3__idx < __expr3->size(); acc__it3__idx++) {
        i64 acc__it3 = (*__expr3)[acc__it3__idx];

        if (acc__it3 > max_v) {
            max_v = acc__it3;
        };
    }

    auto __expr4 = acc;
    for (int acc__it4__idx = 0; acc__it4__idx < __expr4->size(); acc__it4__idx++) {
        i64 acc__it4 = (*__expr4)[acc__it4__idx];

        if (acc__it4 == max_v) {
            printf("%lld %s %s\n", static_cast<i64>(acc__it4), ("max seq"s).c_str(), (int_arr_to_str(int_to_arr(acc__it4__idx, base))).c_str());
        };
    }
    printf("%s %lld\n", ("\ndone"s).c_str(), static_cast<i64>(max_v));
}


