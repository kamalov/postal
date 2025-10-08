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

i64 get_count(shared_pointer<IntField> f) {
    i64 count;
    i64 ri;
    i64 ci;
    i64 v;
    count = 0ll;

    auto __expr0 = create_range(0ll, f->row_count - 1ll);
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;

        auto __expr1 = create_range(0ll, f->col_count - 1ll);
        for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
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
    error("int_to_char"s);
}

struct Data {
    shared_pointer<IntField> f;
    shared_pointer<IntField> visited;
    shared_pointer<IntField> weights;
    shared_vector<shared_pointer<Coords>> visited_by_steps;
    shared_map<i64, i64> fallen;
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

void print_int_field(shared_pointer<Data> d, i64 step) {
    shared_pointer<IntField> f;
    i64 ri;
    std::string s;
    i64 ci;
    i64 v;
    i64 fv;
    std::string ns;
    f = d->f;

    auto __expr0 = create_range(0ll, (f->row_count - 1ll));
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;
        s = ""s;

        auto __expr1 = create_range(0ll, (f->col_count - 1ll));
        for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            v = get_int_field_value(f, ri, ci);

            if (v == 0ll - 1ll) {
                fv = map_get_value(d->fallen, ri*1000ll + ci);
                ns = "/"s;
                //// if fv <= step
                ////     ns = "#"
                //// else
                ////     if get_int_field_value(d.visited, ri, ci)
                ////         ns = "O"
                ////     else
                ////         ns = "?"
                ////     end
                //// end
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

shared_pointer<Data> create_field(shared_vector<std::string> lines, i64 size, i64 steps) {
    shared_pointer<IntField> f;
    shared_map<i64, i64> fallen;
    shared_vector<shared_pointer<Coords>> visited_by_steps;
    shared_pointer<IntField> weights;
    i64 i;
    shared_vector<std::string> p;
    i64 ci;
    i64 ri;
    shared_pointer<Data> d;
    f = create_int_field(size, size);
    fallen = create_shared_map<i64, i64>();
    visited_by_steps = create_shared_vector<shared_pointer<Coords>>();
    array_push(visited_by_steps, new_coords(1000ll, 1000ll));
    weights = create_int_field(size, size);
    i = 0ll;

    auto __expr0 = lines;
    for (i64 lines__it0__idx = 0; lines__it0__idx < (i64)__expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        i = i + 1ll;

        if (i > steps) {
            break;
        };
        p = string_split(lines__it0, ","s);
        ci = string_to_integer(p->at(0ll));
        ri = string_to_integer(p->at(1ll));
        map_add(fallen, ri*1000ll + ci, lines__it0__idx + 1ll);
        set_int_field_value(f, ri, ci, 0ll - 1ll);
        ////set_int_field_value(weights, ri, ci, ri + ci)
        array_push(visited_by_steps, new_coords(ri, ci));
    }
    d = create_shared_pointer<Data >();
    d->f = f;
    d->visited = create_int_field(size, size);
    d->weights = weights;
    d->visited_by_steps = visited_by_steps;
    d->fallen = fallen;
    d->min_len = 1000000ll;
    return d;
}

i64 is_fallen(shared_pointer<Data> d, i64 ri, i64 ci, i64 step) {
    i64 key;
    key = ri*1000ll + ci;
    ////if map_has_key(d.fallen, key) then return map_get_value(d.fallen, key) <= step
    return map_has_key(d->fallen, key);
}

void next(shared_pointer<Data> d, i64 ri, i64 ci, i64 step) {
    i64 rest;
    i64 v;
    i64 w;
    rest = d->f->row_count - ri - 1ll + d->f->col_count - ci - 1ll;

    if (step + rest > d->min_len) {
        //// print(ri, ci)
        //// print_int_field(d, step)
        //// readln()
        return;
    };

    if (!is_valid_field_index(d->f, ri, ci)) {
        return;
    };

    if (is_fallen(d, ri, ci, step)) {
        return;
    };
    v = get_int_field_value(d->f, ri, ci);
    //// if step > 0
    ////     c = d.visited_by_steps[step]
    ////     if get_int_field_value(d.visited, c.ri, c.ci) then return
    //// end

    if (ri == d->f->row_count - 1ll && ci == d->f->col_count - 1ll) {

        if (step < d->min_len) {
            d->min_len = step;
            ////print_int_field(d, step)
            ////print(step)
            ////readln()
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
        ////print("if w >= step", w, step)
        return;
    };
    set_int_field_value(d->visited, ri, ci, 1ll);
    d->count = d->count + 1ll;
    ////if d.count mod 1000000 = 0
    ////print(d.count)
    ////print(step)
    ////print_int_field(d, step)
    ////readln()
    ////end
    next(d, ri, ci + 1ll, step + 1ll);
    next(d, ri + 1ll, ci, step + 1ll);
    next(d, ri, ci - 1ll, step + 1ll);
    next(d, ri - 1ll, ci, step + 1ll);
    set_int_field_value(d->visited, ri, ci, 0ll);
}

void run() {
    shared_vector<std::string> lines;
    shared_pointer<Data> d;
    i64 i;
    lines = read_string_lines_from_file("./input.txt"s);
    ////d = create_field(lines, 7, 12)
    d = create_field(lines, 71ll, 1024ll);
    ////print_int_field(d, 0)
    i = array_size(lines);
    ////i = 1024

    for (;;) {
        i = i - 1ll;
        d = create_field(lines, 71ll, i);
        next(d, 0ll, 0ll, 0ll);
        printf("%s %lld %s %lld\n", ("falls count"s).c_str(), (i64)(i), ("min_len"s).c_str(), (i64)(d->min_len));

        if (d->min_len != 1000000ll) {
            printf("%s\n", (lines->at(i)).c_str());
            break;
        };
        ////readln()
    }
    ////print("done", d.min_len)
}

