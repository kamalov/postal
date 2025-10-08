#include "prelude.h"

/// external template <typename T>i64 len(std::vector<T>* a);
/// external template <typename T>void push(std::vector<T>* a, T elem);
/// external template <typename T>T pop(std::vector<T>* a);
/// external template <typename T>void arr_set_len(std::vector<T>* a, i64 new_len);
/// external template <typename T>void arr_sort(std::vector<T>* a);
/// external template <typename T>i64 arr_contains(std::vector<T>* a, T value);
/// external template <typename T>void arr_remove_at(std::vector<T>* a, i64 index);
/// external template <typename T>void arr_remove(std::vector<T>* a, T value);
/// external template <typename T>i64 arr_index_of(std::vector<T>* a, T value);
/// external template <typename T>std::vector<T>* arr_slice(std::vector<T>* a, i64 from, i64 to);
/// external template <typename K, typename V>void map_add(universal_hashmap<K, V>* map, K key, V value);
/// external template <typename K, typename V>void map_add_or_update(universal_hashmap<K, V>* map, K key, V value);
/// external template <typename K, typename V>i64 map_has_key(universal_hashmap<K, V>* map, K key);
/// external template <typename K, typename V>V map_get_value(universal_hashmap<K, V>* map, K key);
/// external template <typename K, typename V>void map_remove(universal_hashmap<K, V>* map, K key);
/// external template <typename K, typename V>i64 map_len(universal_hashmap<K, V>* map);
/// external template <typename K, typename V>std::vector<K>* map_keys(universal_hashmap<K, V>* map);
/// external std::vector<std::string>* str_split(std::string s, std::string by);
/// external i64 str_to_int(std::string s);
/// external std::vector<std::string>* str_to_chars(std::string s);
/// external i64 str_contains(std::string s, std::string subs);
/// external i64 str_len(std::string s);
/// external std::string str_remove(std::string s, std::string r);
/// external std::string str_arr_join(std::vector<std::string>* a, std::string delimiter);
/// external std::string int_to_str(i64 i);
/// external void err(std::string s);
/// external std::string readln();
/// external std::vector<std::string>* read_string_lines_from_file(std::string filename);
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

i64 get_count(IntField* f) {
    i64 count;
    i64 ri;
    i64 ci;
    i64 v;
    count = 0ll;

    auto __expr0 = create_range(0ll, f->row_count - 1ll);
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;

        auto __expr1 = create_range(0ll, f->col_count - 1ll);
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            v = get_int_field_value(f, ri, ci);

            if (v > 0ll) {
                count = count + 1ll;
            };
        }
    }
    return count;
}

std::string int_to_char(i64 i) {

    if (i == 0ll) {
        return "."s;
    };

    if (i == 0ll - 1ll) {
        return "#"s;
    };
    err("int_to_char"s);
}

struct Data {
    IntField* f;
    IntField* visited;
    IntField* weights;
    std::vector<Coords*>* visited_by_steps;
    universal_hashmap<i64, i64>* fallen;
    i64 min_len;
    i64 count;
    friend bool operator==(const Data& l, const Data& r) {
        return (l.min_len == r.min_len) && (l.count == r.count);
    }
};

namespace std {
    template<>
    struct hash<Data> {
        std::size_t operator()(const Data& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.min_len) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.count) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

void print_int_field(Data* d, i64 step) {
    IntField* f;
    i64 ri;
    std::string s;
    i64 ci;
    i64 v;
    i64 fv;
    std::string ns;
    f = d->f;

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

            if (v == 0ll - 1ll) {
                fv = map_get_value(d->fallen, ri*1000ll + ci);
                ns = "/"s;
                // if fv <= step {
                //     ns = '#'
                // } else {
                //     if get_int_field_value(d.visited, ri, ci) {
                //         ns = 'O'
                //     } else {
                //         ns = '?'
                //     }
                // }
            };

            if (v == 0ll) {

                if (get_int_field_value(d->visited, ri, ci)) {
                    ns = "O"s;
                }
                else {
                    ns = "."s;
                };
            };
            s = s + ns;
        }
        printf("%s\n", (s).c_str());
    }
    printf("\n");
}

Data* create_field(std::vector<std::string>* lines, i64 size, i64 steps) {
    IntField* f;
    universal_hashmap<i64, i64>* fallen;
    std::vector<Coords*>* visited_by_steps;
    IntField* weights;
    i64 i;
    std::vector<std::string>* p;
    i64 ci;
    i64 ri;
    Data* d;
    f = create_int_field(size, size);
    fallen = new universal_hashmap<i64, i64>();
    visited_by_steps = new std::vector<Coords*>();
    push(visited_by_steps, new_coords(1000ll, 1000ll));
    weights = create_int_field(size, size);
    i = 0ll;

    auto __expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        i = i + 1ll;

        if (i > steps) {
            break;
        };
        p = str_split(lines__it0, ","s);
        ci = str_to_int(p->at(0ll));
        ri = str_to_int(p->at(1ll));
        map_add(fallen, ri*1000ll + ci, lines__it0__idx + 1ll);
        set_int_field_value(f, ri, ci, 0ll - 1ll);
        //set_int_field_value(weights, ri, ci, ri + ci)
        push(visited_by_steps, new_coords(ri, ci));
    }
    d = new Data();
    d->f = f;
    d->visited = create_int_field(size, size);
    d->weights = weights;
    d->visited_by_steps = visited_by_steps;
    d->fallen = fallen;
    d->min_len = 1000000ll;
    return d;
}

i64 is_fallen(Data* d, i64 ri, i64 ci, i64 step) {
    i64 key;
    key = ri*1000ll + ci;
    //if map_has_key(d.fallen, key) do ret map_get_value(d.fallen, key) <= step
    return map_has_key(d->fallen, key);
}

void next(Data* d, i64 ri, i64 ci, i64 step) {
    i64 rest;
    i64 v;
    i64 w;
    rest = d->f->row_count - ri - 1ll + d->f->col_count - ci - 1ll;

    if (step + rest > d->min_len) {
        // log(ri, ci)
        // print_int_field(d, step)
        // readln()
        return;
    };

    if (is_valid_field_index(d->f, ri, ci) == 0ll) {
        return;
    };

    if (is_fallen(d, ri, ci, step)) {
        return;
    };
    v = get_int_field_value(d->f, ri, ci);
    // if step > 0 {
    //     c = d.visited_by_steps[step]
    //     if get_int_field_value(d.visited, c.ri, c.ci) do ret
    // }

    if (ri == d->f->row_count - 1ll && ci == d->f->col_count - 1ll) {

        if (step < d->min_len) {
            d->min_len = step;
            //print_int_field(d, step)
            //log(step)
            //readln()
        };
        return;
    };

    if (get_int_field_value(d->visited, ri, ci)) {
        return;
    };
    w = get_int_field_value(d->weights, ri, ci);

    if (w == 0ll || step < w) {
        set_int_field_value(d->weights, ri, ci, step);
    }
    else {
        //log('if w >= step', w, step)
        return;
    };
    set_int_field_value(d->visited, ri, ci, 1ll);
    d->count = d->count + 1ll;
    //if d.count mod 1000000 = 0 {
    //log(d.count)
    //log(step)
    //print_int_field(d, step)
    //readln()
    //}
    next(d, ri, ci + 1ll, step + 1ll);
    next(d, ri + 1ll, ci, step + 1ll);
    next(d, ri, ci - 1ll, step + 1ll);
    next(d, ri - 1ll, ci, step + 1ll);
    set_int_field_value(d->visited, ri, ci, 0ll);
}

void run() {
    std::vector<std::string>* lines;
    Data* d;
    i64 i;
    lines = read_string_lines_from_file("./input.txt"s);
    //d = create_field(lines, 7, 12)
    d = create_field(lines, 71ll, 1024ll);
    //print_int_field(d, 0)
    i = len(lines);
    //i = 1024

    for (;;) {
        i = i - 1ll;
        d = create_field(lines, 71ll, i);
        next(d, 0ll, 0ll, 0ll);
        printf("%s %lld %s %lld\n", ("falls count"s).c_str(), static_cast<i64>(i), ("min_len"s).c_str(), static_cast<i64>(d->min_len));

        if (d->min_len != 1000000ll) {
            printf("%s\n", (lines->at(i)).c_str());
            break;
        };
        //readln()
    }
    //log('done', d.min_len)
}


