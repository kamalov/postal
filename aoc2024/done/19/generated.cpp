#include "prelude.h"

/// prelude template <typename T>i64 len(std::vector<T>* a);
/// prelude template <typename T>void push(std::vector<T>* a, T elem);
/// prelude template <typename T>T pop(std::vector<T>* a);
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
struct Part {
    std::vector<i64>* vals;
};


struct Data {
    std::vector<Part*>* parts;
};


std::vector<i64>* str_to_int_array(std::string s) {
    std::vector<i64>* r;
    std::vector<std::string>* chars;
    i64 i;
    r = new std::vector<i64>();
    chars = str_to_chars("bgruw"s);

    auto __expr0 = str_to_chars(s);
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        std::string expr__it0 = (*__expr0)[expr__it0__idx];
        i = arr_index_of(chars, expr__it0);
        push(r, i);
    }
    return r;
}

std::string int_array_to_str(std::vector<i64>* p) {
    std::vector<std::string>* chars;
    std::string s;
    chars = str_to_chars("bgruw"s);
    s = ""s;

    auto __expr0 = p;
    for (int p__it0__idx = 0; p__it0__idx < __expr0->size(); p__it0__idx++) {
        i64 p__it0 = (*__expr0)[p__it0__idx];
        s = s + chars->at(p__it0);
    }
    return s;
}

std::vector<Part*>* parse_parts(std::string line) {
    std::string s;
    std::vector<std::string>* a;
    std::vector<Part*>* parts;
    Part* p;
    s = str_remove(line, " "s);
    a = str_split(s, ","s);
    arr_sort(a);
    parts = new std::vector<Part*>();

    auto __expr0 = a;
    for (int a__it0__idx = 0; a__it0__idx < __expr0->size(); a__it0__idx++) {
        std::string a__it0 = (*__expr0)[a__it0__idx];
        p = new Part();
        p->vals = str_to_int_array(a__it0);
        push(parts, p);
    }
    return parts;
}

i64 matches(std::vector<i64>* target, i64 index, std::vector<i64>* v) {
    i64 i;

    auto __expr0 = v;
    for (int v__it0__idx = 0; v__it0__idx < __expr0->size(); v__it0__idx++) {
        i64 v__it0 = (*__expr0)[v__it0__idx];
        i = index + v__it0__idx;

        if (i >= len(target)) {
            return 0ll;
        };

        if (v__it0 != target->at(i)) {
            return 0ll;
        };
    }
    return 1ll;
}

struct Counter {
    i64 count;
    std::vector<std::string>* lines;
    std::vector<i64>* visited;
    friend bool operator==(const Counter& l, const Counter& r) {
        return (l.count == r.count);
    }
};

namespace std {
    template<>
    struct hash<Counter> {
        std::size_t operator()(const Counter& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.count) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

i64 check(Data* d, std::vector<i64>* target, i64 index, Counter* counter, std::string s) {
    i64 r;
    std::vector<i64>* p;
    i64 i;
    i64 v;
    std::string ns;
    r = 0ll;

    if (index >= len(target)) {
        counter->count = counter->count + 1ll;
        //log('found', s)
        return 1ll;
    };

    auto __expr0 = d->parts;
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        Part* expr__it0 = (*__expr0)[expr__it0__idx];
        p = expr__it0->vals;

        if (matches(target, index, p)) {
            //log('matches', index, int_array_to_str(p))
            i = index*1000ll + expr__it0__idx;
            v = counter->visited->at(i);

            if (v == 0ll - 1ll) {
                continue;
            };
            ns = s + ", "s + int_array_to_str(p);

            if (v > 0ll) {
                r = r + v;
            }
            else {
                v = check(d, target, index + len(p), counter, ns);

                if (v > 0ll) {
                    counter->visited->at(i) = v;
                    r = r + v;
                }
                else {
                    counter->visited->at(i) = 0ll - 1ll;
                };
            };
        };
    }
    return r;
}

void run() {
    std::vector<std::string>* lines;
    Data* d;
    i64 i;
    i64 total;
    std::vector<i64>* target;
    Counter* c;
    i64 r;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    d = new Data();
    d->parts = parse_parts(lines->at(0ll));

    auto __expr0 = d->parts;
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        Part* expr__it0 = (*__expr0)[expr__it0__idx];
        printf("%s\n", (int_array_to_str(expr__it0->vals)).c_str());
    }
    printf("\n");
    i = arr_index_of(lines, ""s);
    lines = arr_slice(lines, i + 1ll, len(lines));
    total = 0ll;

    auto __expr1 = lines;
    for (int lines__it1__idx = 0; lines__it1__idx < __expr1->size(); lines__it1__idx++) {
        std::string lines__it1 = (*__expr1)[lines__it1__idx];
        target = str_to_int_array(lines__it1);
        printf("%s\n", (lines__it1).c_str());
        c = new Counter();
        c->lines = new std::vector<std::string>();
        c->visited = new std::vector<i64>();
        arr_set_len(c->visited, 1000000ll);
        r = check(d, target, 0ll, c, ""s);
        printf("%lld\n", static_cast<i64>(r));
        //total = total + c.count
        total = total + r;
        //for c.visited do log(it)
    }
    printf("%s %lld\n", ("total"s).c_str(), static_cast<i64>(total));
}


