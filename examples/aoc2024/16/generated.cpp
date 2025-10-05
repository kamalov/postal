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
    Coords* start;
    Coords* finish;
    i64 cost;
    std::vector<i64>* values;
    Coords* north;
    Coords* south;
    Coords* west;
    Coords* east;
    friend bool operator==(const IntField& l, const IntField& r) {
        return (l.row_count == r.row_count) && (l.col_count == r.col_count) && (l.cost == r.cost);
    }
};

namespace std {
    template<>
    struct hash<IntField> {
        std::size_t operator()(const IntField& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.row_count) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.col_count) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.cost) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

struct VisitedCosts {
    i64 dummy;
    IntField* north;
    IntField* south;
    IntField* west;
    IntField* east;
    IntField* best;
    i64 count;
    friend bool operator==(const VisitedCosts& l, const VisitedCosts& r) {
        return (l.dummy == r.dummy) && (l.count == r.count);
    }
};

namespace std {
    template<>
    struct hash<VisitedCosts> {
        std::size_t operator()(const VisitedCosts& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.dummy) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.count) + 0x9e3779b9 + (h << 6) + (h >> 2);
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
        return "/"s;
    };
    err("int_to_char"s);
}

void print_int_field(IntField* f, IntField* visited) {
    i64 ri;
    std::string s;
    i64 ci;
    i64 v;
    i64 vis_value;

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
            vis_value = get_int_field_value(visited, ri, ci);

            if (vis_value != 0ll) {
                s = s + "O"s;
                //int_to_str(vis_value)
            }
            else {
                s = s + int_to_char(v);
            };
            //  if ri = f.cri and f.cci = ci {
            //      s = s + '@' + dir
            //  } else {
            //      s = s + '' + int_to_char(v)
            //  }
        }
        printf("%s\n", (s).c_str());
    }
    printf("\n");
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

IntField* lines_to_int_field(std::vector<std::string>* lines) {
    IntField* f;
    i64 ri;
    i64 ci;
    i64 v;
    f = create_int_field(len(lines), len(str_to_chars(lines->at(0ll))));
    f->start = new Coords();
    f->finish = new Coords();
    f->north = new_coords(0ll - 1ll, 0ll);
    f->south = new_coords(1ll, 0ll);
    f->west = new_coords(0ll, 0ll - 1ll);
    f->east = new_coords(0ll, 1ll);
    f->cost = 1000000000ll;

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
                //log('finish', ri, ci)
            };
            v = char_to_int(expr__it1);
            set_int_field_value(f, ri, ci, v);
        }
    }
    return f;
}

Coords* rotate_left(IntField* f, Coords* dir) {

    if (dir == f->north) {
        return f->west;
    };

    if (dir == f->west) {
        return f->south;
    };

    if (dir == f->south) {
        return f->east;
    };

    if (dir == f->east) {
        return f->north;
    };
    err("rotate_left"s);
}

i64 get_visited_cost(IntField* f, VisitedCosts* vis, i64 ri, i64 ci, Coords* dir) {

    if (dir == f->north) {
        return get_int_field_value(vis->north, ri, ci);
    };

    if (dir == f->south) {
        return get_int_field_value(vis->south, ri, ci);
    };

    if (dir == f->west) {
        return get_int_field_value(vis->west, ri, ci);
    };

    if (dir == f->east) {
        return get_int_field_value(vis->east, ri, ci);
    };
    err("get_visited_cost"s);
}

void set_visited_cost(IntField* f, VisitedCosts* vis, i64 ri, i64 ci, Coords* dir, i64 value) {

    if (dir == f->north) {
        set_int_field_value(vis->north, ri, ci, value);
        return;
    };

    if (dir == f->south) {
        set_int_field_value(vis->south, ri, ci, value);
        return;
    };

    if (dir == f->west) {
        set_int_field_value(vis->west, ri, ci, value);
        return;
    };

    if (dir == f->east) {
        set_int_field_value(vis->east, ri, ci, value);
        return;
    };
    err("set_visited_cost"s);
}

i64 try_update_visited_cost(IntField* f, VisitedCosts* vis, i64 ri, i64 ci, Coords* dir, i64 cost) {
    IntField* vf;
    i64 v;

    if (dir == f->north) {
        vf = vis->north;
    };

    if (dir == f->south) {
        vf = vis->south;
    };

    if (dir == f->west) {
        vf = vis->west;
    };

    if (dir == f->east) {
        vf = vis->east;
    };
    v = get_int_field_value(vf, ri, ci);

    if (v == 0ll || cost <= v) {
        set_int_field_value(vf, ri, ci, cost);
        return 1ll;
    };
    return 0ll;
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

            if (v == 1ll) {
                count = count + 1ll;
            };
        }
    }
    return count;
}

void update_best(IntField* best, IntField* visited) {
    i64 ri;
    i64 ci;
    i64 v;

    auto __expr0 = create_range(0ll, visited->row_count - 1ll);
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;

        auto __expr1 = create_range(0ll, visited->col_count - 1ll);
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            v = get_int_field_value(visited, ri, ci);

            if (v == 1ll) {
                set_int_field_value(best, ri, ci, 1ll);
            };
        }
    }
}

i64 try_visit(IntField* f, IntField* visited, i64 ri, i64 ci, i64 cost, Coords* dir, VisitedCosts* visited_costs) {
    i64 v;
    Coords* rl;
    Coords* rr;
    i64 r1;
    i64 r2;
    i64 r3;

    if (cost > f->cost) {
        return 1ll;
    };

    if (get_int_field_value(visited, ri, ci) == 1ll) {
        return 1ll;
    };
    v = get_int_field_value(f, ri, ci);

    if (v == 0ll - 1ll) {
        return 0ll;
    };

    if (ri == f->finish->ri && ci == f->finish->ci) {
        f->cost = cost;
        //print_int_field(f, visited)
        printf("%s %lld\n", ("found"s).c_str(), static_cast<i64>(cost));

        if (cost == 90460ll) {
            update_best(visited_costs->best, visited);
        };
        return 1ll;
    };
    set_int_field_value(visited, ri, ci, 1ll);
    // visited_costs.count = visited_costs.count + 1
    // if visited_costs.count % 50000000 = 0 {
    //     print_int_field(f, visited)
    //     log('at step', visited_costs.count)
    //     readln()
    // }
    rl = rotate_left(f, dir);
    rr = rotate_left(f, rotate_left(f, rl));
    //if try_update_visited_cost(f, visited_costs, ri, ci, dir, cost) {
    r1 = try_visit(f, visited, ri + dir->ri, ci + dir->ci, cost + 1ll, dir, visited_costs);
    //}

    if (try_update_visited_cost(f, visited_costs, ri, ci, rl, cost + 1000ll)) {
        r2 = try_visit(f, visited, ri + rl->ri, ci + rl->ci, cost + 1001ll, rl, visited_costs);
    };

    if (try_update_visited_cost(f, visited_costs, ri, ci, rr, cost + 1000ll)) {
        r3 = try_visit(f, visited, ri + rr->ri, ci + rr->ci, cost + 1001ll, rr, visited_costs);
    };
    set_int_field_value(visited, ri, ci, 0ll);

    if (r1 + r2 + r3 == 0ll) {
        set_int_field_value(f, ri, ci, 0ll - 1ll);
        return 0ll;
    };
    return 1ll;
}

VisitedCosts* create_visited_costs(IntField* f) {
    VisitedCosts* c;
    c = new VisitedCosts();
    c->north = create_int_field(f->row_count, f->col_count);
    c->south = create_int_field(f->row_count, f->col_count);
    c->west = create_int_field(f->row_count, f->col_count);
    c->east = create_int_field(f->row_count, f->col_count);
    c->best = create_int_field(f->row_count, f->col_count);
    return c;
}

void run() {
    std::vector<std::string>* lines;
    IntField* f;
    IntField* visited;
    VisitedCosts* visited_costs;
    i64 total;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    f = lines_to_int_field(lines);
    visited = create_int_field(f->row_count, f->col_count);
    visited_costs = create_visited_costs(f);
    try_visit(f, visited, f->start->ri, f->start->ci, 0ll, f->east, visited_costs);
    //print_int_field(f, visited)
    total = get_count(visited_costs->best) + 1ll;
    printf("%s %lld %s %lld\n", ("done"s).c_str(), static_cast<i64>(f->cost), ("steps"s).c_str(), static_cast<i64>(total));
}


