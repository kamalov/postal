#include "prelude.cpp"

//// prelude start
template <typename T>i64 array_size(shared_vector<T> a);
template <typename T>void array_push(shared_vector<T> a, T element);
template <typename T>T array_pop(shared_vector<T> a);
template <typename T>void array_quick_sort(shared_vector<T> a);
template <typename T>void array_push_front(shared_vector<T> a, T element);
template <typename T>T array_pop_front(shared_vector<T> a);
template <typename T>T array_last(shared_vector<T> a);
template <typename T>void array_set_size(shared_vector<T> a, i64 new_size);
template <typename T>i64 array_contains(shared_vector<T> a, T value);
template <typename T>void array_remove_at(shared_vector<T> a, i64 index);
template <typename T>void array_remove(shared_vector<T> a, T value);
template <typename T>i64 array_index_of(shared_vector<T> a, T value);
template <typename T>shared_vector<T> array_slice(shared_vector<T> a, i64 from, i64 to);
template <typename K, typename V>void map_add(shared_map<K, V> map, K key, V value);
template <typename K, typename V>V map_get_value(shared_map<K, V> map, K key);
template <typename K, typename V>void map_add_or_update(shared_map<K, V> map, K key, V value);
template <typename K, typename V>i64 map_has_key(shared_map<K, V> map, K key);
template <typename K, typename V>void map_remove(shared_map<K, V> map, K key);
template <typename K, typename V>i64 map_size(shared_map<K, V> map);
template <typename K, typename V>shared_vector<K> map_keys(shared_map<K, V> map);
shared_vector<std::string> string_split(std::string s, std::string by);
i64 string_to_integer(std::string s);
shared_vector<std::string> string_to_chars(std::string s);
i64 string_contains(std::string s, std::string subs);
i64 string_size(std::string s);
std::string string_remove(std::string s, std::string r);
std::string string_array_join(shared_vector<std::string> a, std::string delimiter);
std::string integer_to_string(i64 i);
void error(std::string s);
std::string read_line();
shared_vector<std::string> read_string_lines_from_file(std::string filename);
//// prelude end
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

shared_pointer<Coords> new_coords(i64 ri, i64 ci) {
    shared_pointer<Coords> c;
    c = create_shared_pointer<Coords >();
    c->ri = ri;
    c->ci = ci;
    return c;
}

struct IntField {
    i64 row_count;
    i64 col_count;
    shared_pointer<Coords> start;
    shared_pointer<Coords> finish;
    shared_vector<i64> values;
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

shared_pointer<IntField> create_int_field(i64 row_count, i64 col_count) {
    shared_pointer<IntField> f;
    f = create_shared_pointer<IntField >();
    f->row_count = row_count;
    f->col_count = col_count;
    f->values = create_shared_vector<i64>();
    array_set_size(f->values, row_count*col_count);
    return f;
}

i64 get_int_field_value(shared_pointer<IntField> f, i64 row_index, i64 col_index) {
    return f->values->at(row_index*f->col_count + col_index);
}

void set_int_field_value(shared_pointer<IntField> f, i64 row_index, i64 col_index, i64 value) {
    f->values->at(row_index*f->col_count + col_index) = value;
}

bool is_valid_field_index(shared_pointer<IntField> f, i64 row_index, i64 col_index) {
    return row_index >= 0ll && row_index < f->row_count && col_index >= 0ll && col_index < f->col_count;
}

std::string int_to_char(i64 i) {

    if (i == 0ll) {
        return "."s;
    };

    if (i == 0ll-1ll) {
        return "#"s;
    };
    return integer_to_string(i);
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
        return 0ll-1ll;
    };
    error("char_to_int"s);
}

void print_int_field(shared_pointer<IntField> f) {
    i64 ri;
    std::string s;
    i64 ci;
    i64 v;

    auto __expr0 = create_range(0ll, (f->row_count-1ll));
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;
        s = ""s;

        auto __expr1 = create_range(0ll, (f->col_count-1ll));
        for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            v = get_int_field_value(f, ri, ci);
            s = s + int_to_char(v);
        }
        printf("%s\n", (s).c_str());
    }
    printf("\n");
}

shared_pointer<IntField> lines_to_int_field(shared_vector<std::string> lines) {
    shared_pointer<IntField> f;
    i64 ri;
    i64 ci;
    i64 v;
    f = create_int_field(array_size(lines), array_size(string_to_chars(lines->at(0ll))));
    f->start = create_shared_pointer<Coords >();
    f->finish = create_shared_pointer<Coords >();

    auto __expr0 = lines;
    for (i64 lines__it0__idx = 0; lines__it0__idx < (i64)__expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        ri = lines__it0__idx;

        auto __expr1 = string_to_chars(lines__it0);
        for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
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

void prepare(shared_pointer<IntField> f, i64 ri, i64 ci, i64 cost) {
    i64 v;

    if (!is_valid_field_index(f, ri, ci)) {
        return;
    };
    v = get_int_field_value(f, ri, ci);

    if (v == 0ll-1ll || v > 0ll) {
        return;
    };
    set_int_field_value(f, ri, ci, cost);
    prepare(f, ri + 1ll, ci, cost + 1ll);
    prepare(f, ri-1ll, ci, cost + 1ll);
    prepare(f, ri, ci + 1ll, cost + 1ll);
    prepare(f, ri, ci-1ll, cost + 1ll);
}

i64 abs_val(i64 i) {

    if (i < 0ll) {
        return 0ll-i;
    };
    return i;
}

i64 check_cheat(shared_pointer<IntField> f, i64 ri, i64 ci, i64 ri1, i64 ci1, shared_vector<i64> cheat_count) {
    i64 v;
    i64 v1;
    i64 l;
    i64 diff;

    if (!is_valid_field_index(f, ri1, ci1)) {
        return 0ll;
    };
    v = get_int_field_value(f, ri, ci);
    v1 = get_int_field_value(f, ri1, ci1);

    if (v1 == 0ll-1ll) {
        return 0ll;
    };
    l = abs_val(ri-ri1) + abs_val(ci-ci1);
    diff = v1-v-l;

    if (diff > 0ll) {
        cheat_count->at(diff) = cheat_count->at(diff) + 1ll;

        if (diff >= 100ll) {
            ////     print("cheat=", diff, "from", ri, ci, "to", ri1, ci1)
            return 1ll;
        };
    };
    return 0ll;
}

i64 try_cheat(shared_pointer<IntField> f) {
    i64 r;
    shared_vector<i64> cheat_count;
    i64 max;
    i64 ri;
    i64 ci;
    i64 v;
    i64 dr;
    i64 dc;
    i64 l;
    r = 0ll;
    cheat_count = create_shared_vector<i64>();
    array_set_size(cheat_count, 10000ll);
    max = 20ll;

    auto __expr0 = create_range(0ll, (f->row_count-1ll));
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;

        auto __expr1 = create_range(0ll, (f->col_count-1ll));
        for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            v = get_int_field_value(f, ri, ci);

            if (v == 0ll-1ll) {
                continue;
            };

            auto __expr2 = create_range(0ll, max);
            for (i64 expr__it2__idx = 0; expr__it2__idx < (i64)__expr2->size(); expr__it2__idx++) {
                i64 expr__it2 = (*__expr2)[expr__it2__idx];
                dr = expr__it2;

                auto __expr3 = create_range(0ll, max);
                for (i64 expr__it3__idx = 0; expr__it3__idx < (i64)__expr3->size(); expr__it3__idx++) {
                    i64 expr__it3 = (*__expr3)[expr__it3__idx];
                    dc = expr__it3;
                    l = dr + dc;

                    if (l < 2ll || l > max) {
                        continue;
                    };
                    ////if dr <> 0 and dc <> 0 then continue
                    ////print("dr dc", dr, dc)

                    if (dr == 0ll) {
                        r = r + check_cheat(f, ri, ci, ri, ci + dc, cheat_count);
                        r = r + check_cheat(f, ri, ci, ri, ci-dc, cheat_count);
                    }
                    else if (dc == 0ll) {
                        r = r + check_cheat(f, ri, ci, ri + dr, ci, cheat_count);
                        r = r + check_cheat(f, ri, ci, ri-dr, ci, cheat_count);
                    }
                    else {
                        r = r + check_cheat(f, ri, ci, ri + dr, ci + dc, cheat_count);
                        r = r + check_cheat(f, ri, ci, ri + dr, ci-dc, cheat_count);
                        r = r + check_cheat(f, ri, ci, ri-dr, ci + dc, cheat_count);
                        r = r + check_cheat(f, ri, ci, ri-dr, ci-dc, cheat_count);
                    };
                }
            }
        }
    }

    auto __expr4 = cheat_count;
    for (i64 cheat_count__it4__idx = 0; cheat_count__it4__idx < (i64)__expr4->size(); cheat_count__it4__idx++) {
        i64 cheat_count__it4 = (*__expr4)[cheat_count__it4__idx];
        ////if item > 0 then print("cheat", index, "count", item)

        if (cheat_count__it4 > 0ll) {
            printf("%s %lld %s %lld\n", ("count"s).c_str(), (i64)(cheat_count__it4), ("cheat"s).c_str(), (i64)(cheat_count__it4__idx));
        };
    }
    return r;
}

void run() {
    shared_vector<std::string> lines;
    shared_pointer<IntField> f;
    i64 t;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    f = lines_to_int_field(lines);
    ////print_int_field(f)
    prepare(f, f->start->ri, f->start->ci, 1ll);
    ////print_int_field(f)
    printf("%s %lld\n", ("max"s).c_str(), (i64)(get_int_field_value(f, f->finish->ri, f->finish->ci)));
    t = try_cheat(f);
    printf("%s %lld\n", ("done"s).c_str(), (i64)(t));
}

