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
struct IntField {
    i64 row_count;
    i64 col_count;
    i64 cri;
    i64 cci;
    std::vector<i64>* values;
    friend bool operator==(const IntField& l, const IntField& r) {
        return (l.row_count == r.row_count) && (l.col_count == r.col_count) && (l.cri == r.cri) && (l.cci == r.cci);
    }
};

namespace std {
    template<>
    struct hash<IntField> {
        std::size_t operator()(const IntField& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.row_count) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.col_count) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.cri) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.cci) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

struct Delta {
    i64 dr;
    i64 dc;
    friend bool operator==(const Delta& l, const Delta& r) {
        return (l.dr == r.dr) && (l.dc == r.dc);
    }
};

namespace std {
    template<>
    struct hash<Delta> {
        std::size_t operator()(const Delta& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.dr) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.dc) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

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

    if (i == 1ll) {
        return "["s;
    };

    if (i == 2ll) {
        return "]"s;
    };

    if (i == 3ll) {
        return "#"s;
    };
    err("int_to_char"s);
}

void print_int_field(IntField* f, std::string dir) {
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

            if (ri == f->cri && f->cci == ci) {
                s = s + ""s + dir;
            }
            else {
                s = s + ""s + int_to_char(v);
            };
        }
        printf("%s\n", (s).c_str());
    }
    printf("\n");
}

i64 char_to_int(std::string c) {

    if (c == "."s) {
        return 0ll;
    };

    if (c == "@"s) {
        return 0ll;
    };

    if (c == "["s) {
        return 1ll;
    };

    if (c == "]"s) {
        return 2ll;
    };

    if (c == "#"s) {
        return 3ll;
    };
    err("char_to_int"s);
}

std::string char_to_new_chars(std::string c) {

    if (c == "."s) {
        return ".."s;
    };

    if (c == "@"s) {
        return "@."s;
    };

    if (c == "O"s) {
        return "[]"s;
    };

    if (c == "#"s) {
        return "##"s;
    };
    err("char_to_new_chars"s);
}

IntField* lines_to_int_field(std::vector<std::string>* lines) {
    IntField* f;
    std::string s;
    i64 ri;
    i64 ci;
    i64 v;
    f = create_int_field(len(lines), len(str_to_chars(lines->at(0ll)))*2ll);

    auto __expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        s = ""s;

        auto __expr1 = str_to_chars(lines__it0);
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            std::string expr__it1 = (*__expr1)[expr__it1__idx];
            s = s + char_to_new_chars(expr__it1);
        }
        ri = lines__it0__idx;

        auto __expr2 = str_to_chars(s);
        for (int expr__it2__idx = 0; expr__it2__idx < __expr2->size(); expr__it2__idx++) {
            std::string expr__it2 = (*__expr2)[expr__it2__idx];
            ci = expr__it2__idx;

            if (expr__it2 == "@"s) {
                f->cri = ri;
                f->cci = ci;
            };
            v = char_to_int(expr__it2);
            set_int_field_value(f, ri, ci, v);
        }
    }
    return f;
}

Delta* get_delta(std::string s) {
    i64 dr;
    i64 dc;
    Delta* d;
    dr = 0ll;
    dc = 0ll;

    if (s == "<"s) {
        dc = 0ll - 1ll;
    };

    if (s == ">"s) {
        dc = 1ll;
    };

    if (s == "v"s) {
        dr = 1ll;
    };

    if (s == "^"s) {
        dr = 0ll - 1ll;
    };
    d = new Delta();
    d->dr = dr;
    d->dc = dc;
    return d;
}

void move_many(IntField* f, std::vector<Coords*>* box_indexes, std::string dir) {
    Delta* d;
    Coords* c;
    d = get_delta(dir);

    auto __expr0 = box_indexes;
    for (int box_indexes__it0__idx = 0; box_indexes__it0__idx < __expr0->size(); box_indexes__it0__idx++) {
        Coords* box_indexes__it0 = (*__expr0)[box_indexes__it0__idx];
        set_int_field_value(f, box_indexes__it0->ri, box_indexes__it0->ci, 0ll);
        set_int_field_value(f, box_indexes__it0->ri, box_indexes__it0->ci + 1ll, 0ll);
    }

    for (;;) {

        if (len(box_indexes) == 0ll) {
            break;
        };
        c = pop(box_indexes);

        if (dir == ">"s) {
            set_int_field_value(f, c->ri, c->ci + 1ll, 1ll);
            set_int_field_value(f, c->ri, c->ci + 2ll, 2ll);
            continue;
        };

        if (dir == "<"s) {
            set_int_field_value(f, c->ri, c->ci - 1ll, 1ll);
            set_int_field_value(f, c->ri, c->ci - 0ll, 2ll);
            continue;
        };

        if (dir == "^"s) {
            set_int_field_value(f, c->ri - 1ll, c->ci, 1ll);
            set_int_field_value(f, c->ri - 1ll, c->ci + 1ll, 2ll);
            continue;
        };

        if (dir == "v"s) {
            set_int_field_value(f, c->ri + 1ll, c->ci, 1ll);
            set_int_field_value(f, c->ri + 1ll, c->ci + 1ll, 2ll);
            continue;
        };
    }
}

i64 try_add_boxes(IntField* f, universal_hashmap<Coords*, i64>* boxes, Coords* box, i64 dr) {
    i64 ri;
    i64 ci;
    i64 l;
    i64 r;

    if (map_has_key(boxes, box)) {
        return 1ll;
    };
    map_add(boxes, box, 0ll);
    ri = box->ri + dr;
    ci = box->ci;
    l = get_int_field_value(f, ri, ci);
    r = get_int_field_value(f, ri, ci + 1ll);

    if (l == 3ll || r == 3ll) {
        return 0ll;
    };

    if (l == 0ll && r == 0ll) {
        return 1ll;
    };

    if (l == 1ll) {
        return try_add_boxes(f, boxes, new_coords(ri, ci), dr);
    };

    if (l == 2ll) {

        if (try_add_boxes(f, boxes, new_coords(ri, ci - 1ll), dr) == 0ll) {
            return 0ll;
        };
    };

    if (r == 1ll) {

        if (try_add_boxes(f, boxes, new_coords(ri, ci + 1ll), dr) == 0ll) {
            return 0ll;
        };
    };
    return 1ll;
}

void move_next(IntField* f, std::string dir) {
    Delta* d;
    i64 ri;
    i64 ci;
    i64 v;
    std::vector<Coords*>* box_indexes;
    universal_hashmap<Coords*, i64>* boxes;
    Coords* c;
    std::vector<Coords*>* coords;
    d = get_delta(dir);
    ri = f->cri + d->dr;
    ci = f->cci + d->dc;
    v = get_int_field_value(f, ri, ci);

    if (v == 3ll) {
        return;
    };

    if (v == 0ll) {
        set_int_field_value(f, f->cri, f->cci, 0ll);
        f->cri = ri;
        f->cci = ci;
        return;
    };

    if (dir == "<"s || dir == ">"s) {
        box_indexes = new std::vector<Coords*>();

        for (;;) {
            v = get_int_field_value(f, ri, ci);

            if (v == 3ll) {
                return;
            };

            if (v == 0ll) {
                move_many(f, box_indexes, dir);
                f->cri = f->cri + d->dr;
                f->cci = f->cci + d->dc;
                return;
            };

            if (v == 1ll) {
                push(box_indexes, new_coords(ri, ci));
            };
            ri = ri + d->dr;
            ci = ci + d->dc;
        }
        return;
    };

    if (dir == "^"s || dir == "v"s) {
        boxes = new universal_hashmap<Coords*, i64>();
        v = get_int_field_value(f, ri, ci);

        if (v == 1ll) {
            c = new_coords(ri, ci);
        }
        else {
            c = new_coords(ri, ci - 1ll);
        };

        if (try_add_boxes(f, boxes, c, d->dr) == 0ll) {
            return;
        };
        coords = map_keys(boxes);
        move_many(f, coords, dir);
        f->cri = f->cri + d->dr;
        f->cci = f->cci + d->dc;
        return;
    };
}

i64 get_total(IntField* f) {
    i64 total;
    i64 ri;
    i64 ci;
    i64 v;
    total = 0ll;

    auto __expr0 = create_range(0ll, (f->row_count - 1ll));
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;

        auto __expr1 = create_range(0ll, (f->col_count - 1ll));
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            v = get_int_field_value(f, ri, ci);

            if (v == 1ll) {
                total = total + ri*100ll + ci;
            };
        }
    }
    return total;
}

void run() {
    std::vector<std::string>* lines;
    i64 i;
    IntField* f;
    std::string commands_str;
    std::string s;
    std::vector<std::string>* commands;
    std::string c;
    std::string next;
    Delta* delta;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    i = arr_index_of(lines, ""s);
    f = lines_to_int_field(arr_slice(lines, 0ll, i - 1ll));
    print_int_field(f, "@"s);
    commands_str = str_arr_join(arr_slice(lines, i + 1ll, len(lines)), ""s);
    //log(commands)
    s = ""s;
    commands = str_to_chars(commands_str);

    auto __expr0 = create_range(0ll, len(commands) - 1ll);
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        c = commands->at(expr__it0);

        if (expr__it0 >= len(commands) - 1ll) {
            next = "n"s;
        }
        else {
            next = commands->at(expr__it0 + 1ll);
        };
        delta = get_delta(c);
        move_next(f, c);
        // print_int_field(f, next)
        // log('moved', c)
        // if str_len(s) > 50 do s = ''
        // s = s + '' + c
        // log(s)
        // readln()
    }
    print_int_field(f, "@"s);
    printf("\n");
    printf("%s %lld\n", ("total"s).c_str(), static_cast<i64>(get_total(f)));
}


