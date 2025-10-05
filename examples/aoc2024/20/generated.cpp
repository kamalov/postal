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
struct Coords {
    i64 ri;
    i64 ci;
    friend bool operator==(const Coords& l, const Coords& r) {
        return (l.ri == r.ri) && (l.ci == r.ci);
    }
};

namespace std {
    template<>
    struct hash<Coords> {
        std::size_t operator()(const Coords& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.ri) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.ci) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

Coords* new_coords(i64 ri, i64 ci) {
    Coords* c;
    c = new Coords();
    c->ri = ri;
    c->ci = ci;
    return c;
}

struct IntField {
    i64 row_count;
    i64 col_count;
    Coords* start;
    Coords* finish;
    std::vector<i64>* values;
    friend bool operator==(const IntField& l, const IntField& r) {
        return (l.row_count == r.row_count) && (l.col_count == r.col_count);
    }
};

namespace std {
    template<>
    struct hash<IntField> {
        std::size_t operator()(const IntField& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.row_count) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.col_count) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

IntField* create_int_field(i64 row_count, i64 col_count) {
    IntField* f;
    f = new IntField();
    f->row_count = row_count;
    f->col_count = col_count;
    f->values = new std::vector<i64>();
    arr_set_len(f->values, row_count*col_count);
    return f;
}

i64 get_int_field_value(IntField* f, i64 row_index, i64 col_index) {
    return f->values->at(row_index*f->col_count + col_index);
}

void set_int_field_value(IntField* f, i64 row_index, i64 col_index, i64 value) {
    f->values->at(row_index*f->col_count + col_index) = value;
}

i64 is_valid_field_index(IntField* f, i64 row_index, i64 col_index) {
    return row_index >= 0ll && row_index < f->row_count && col_index >= 0ll && col_index < f->col_count;
}

std::string int_to_char(i64 i) {

    if (i == 0ll) {
        return "."s;
    };

    if (i == 0ll - 1ll) {
        return "#"s;
    };
    return int_to_str(i);
}

i64 char_to_int(std::string c) {

    if (c == "."s) {
        return 0ll;
    };

    if (c == "S"s) {
        return 0ll;
    };

    if (c == "E"s) {
        return 0ll;
    };

    if (c == "#"s) {
        return 0ll - 1ll;
    };
    err("char_to_int"s);
}

void print_int_field(IntField* f) {
    i64 ri;
    std::string s;
    i64 ci;
    i64 v;

    auto __expr0 = create_range(0ll, (f->row_count - 1ll));
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;
        s = ""s;

        auto __expr1 = create_range(0ll, (f->col_count - 1ll));
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            v = get_int_field_value(f, ri, ci);
            s = s + int_to_char(v);
        }
        printf("%s\n", (s).c_str());
    }
    printf("\n");
}

IntField* lines_to_int_field(std::vector<std::string>* lines) {
    IntField* f;
    i64 ri;
    i64 ci;
    i64 v;
    f = create_int_field(len(lines), len(str_to_chars(lines->at(0ll))));
    f->start = new Coords();
    f->finish = new Coords();

    auto __expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        ri = lines__it0__idx;

        auto __expr1 = str_to_chars(lines__it0);
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            std::string expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1__idx;

            if (expr__it1 == "S"s) {
                f->start->ri = ri;
                f->start->ci = ci;
            };

            if (expr__it1 == "E"s) {
                f->finish->ri = ri;
                f->finish->ci = ci;
            };
            v = char_to_int(expr__it1);
            set_int_field_value(f, ri, ci, v);
        }
    }
    return f;
}

void prepare(IntField* f, i64 ri, i64 ci, i64 cost) {
    i64 v;

    if (is_valid_field_index(f, ri, ci) == 0ll) {
        return;
    };
    v = get_int_field_value(f, ri, ci);

    if (v == 0ll - 1ll || v > 0ll) {
        return;
    };
    set_int_field_value(f, ri, ci, cost);
    prepare(f, ri + 1ll, ci, cost + 1ll);
    prepare(f, ri - 1ll, ci, cost + 1ll);
    prepare(f, ri, ci + 1ll, cost + 1ll);
    prepare(f, ri, ci - 1ll, cost + 1ll);
}

i64 abs_val(i64 i) {

    if (i < 0ll) {
        return 0ll - i;
    };
    return i;
}

i64 check_cheat(IntField* f, i64 ri, i64 ci, i64 ri1, i64 ci1, std::vector<i64>* cheat_count) {
    i64 v;
    i64 v1;
    i64 l;
    i64 diff;

    if (is_valid_field_index(f, ri1, ci1) == 0ll) {
        return 0ll;
    };
    v = get_int_field_value(f, ri, ci);
    v1 = get_int_field_value(f, ri1, ci1);

    if (v1 == 0ll - 1ll) {
        return 0ll;
    };
    l = abs_val(ri - ri1) + abs_val(ci - ci1);
    diff = v1 - v - l;

    if (diff > 0ll) {
        cheat_count->at(diff) = cheat_count->at(diff) + 1ll;

        if (diff >= 100ll) {
            //     log('cheat=', diff, 'from', ri, ci, 'to', ri1, ci1)
            return 1ll;
        };
    };
    return 0ll;
}

i64 try_cheat(IntField* f) {
    i64 r;
    std::vector<i64>* cheat_count;
    i64 max;
    i64 ri;
    i64 ci;
    i64 v;
    i64 dr;
    i64 dc;
    i64 l;
    r = 0ll;
    cheat_count = new std::vector<i64>();
    arr_set_len(cheat_count, 10000ll);
    max = 20ll;

    auto __expr0 = create_range(0ll, (f->row_count - 1ll));
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;

        auto __expr1 = create_range(0ll, (f->col_count - 1ll));
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            v = get_int_field_value(f, ri, ci);

            if (v == 0ll - 1ll) {
                continue;
            };

            auto __expr2 = create_range(0ll, max);
            for (int expr__it2__idx = 0; expr__it2__idx < __expr2->size(); expr__it2__idx++) {
                i64 expr__it2 = (*__expr2)[expr__it2__idx];
                dr = expr__it2;

                auto __expr3 = create_range(0ll, max);
                for (int expr__it3__idx = 0; expr__it3__idx < __expr3->size(); expr__it3__idx++) {
                    i64 expr__it3 = (*__expr3)[expr__it3__idx];
                    dc = expr__it3;
                    l = dr + dc;

                    if (l < 2ll || l > max) {
                        continue;
                    };
                    //if dr <> 0 and dc <> 0 do continue
                    //log('dr dc', dr, dc)

                    if (dr == 0ll) {
                        r = r + check_cheat(f, ri, ci, ri, ci + dc, cheat_count);
                        r = r + check_cheat(f, ri, ci, ri, ci - dc, cheat_count);
                    }
                    else if (dc == 0ll) {
                        r = r + check_cheat(f, ri, ci, ri + dr, ci, cheat_count);
                        r = r + check_cheat(f, ri, ci, ri - dr, ci, cheat_count);
                    }
                    else {
                        r = r + check_cheat(f, ri, ci, ri + dr, ci + dc, cheat_count);
                        r = r + check_cheat(f, ri, ci, ri + dr, ci - dc, cheat_count);
                        r = r + check_cheat(f, ri, ci, ri - dr, ci + dc, cheat_count);
                        r = r + check_cheat(f, ri, ci, ri - dr, ci - dc, cheat_count);
                    };
                }
            }
        }
    }

    auto __expr4 = cheat_count;
    for (int cheat_count__it4__idx = 0; cheat_count__it4__idx < __expr4->size(); cheat_count__it4__idx++) {
        i64 cheat_count__it4 = (*__expr4)[cheat_count__it4__idx];
        //if it > 0 do log('cheat', idx, 'count', it)

        if (cheat_count__it4 > 0ll) {
            printf("%s %lld %s %lld\n", ("count"s).c_str(), static_cast<i64>(cheat_count__it4), ("cheat"s).c_str(), static_cast<i64>(cheat_count__it4__idx));
        };
    }
    return r;
}

void run() {
    std::vector<std::string>* lines;
    IntField* f;
    i64 t;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    f = lines_to_int_field(lines);
    //print_int_field(f)
    prepare(f, f->start->ri, f->start->ci, 1ll);
    //print_int_field(f)
    printf("%s %lld\n", ("max"s).c_str(), static_cast<i64>(get_int_field_value(f, f->finish->ri, f->finish->ci)));
    t = try_cheat(f);
    printf("%s %lld\n", ("done"s).c_str(), static_cast<i64>(t));
}


