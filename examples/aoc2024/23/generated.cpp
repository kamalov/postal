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
struct Int_Array {
    std::vector<i64>* values;
};


Int_Array* clone_int_array(Int_Array* arr) {
    Int_Array* r;
    r = new Int_Array();
    r->values = new std::vector<i64>();

    auto __expr0 = arr->values;
    for (i64 expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        push(r->values, expr__it0);
    }
    return r;
}

struct Graph {
    std::vector<std::string>* names;
    universal_hashmap<std::string, i64>* vertex_names_to_index;
    universal_hashmap<i64, Int_Array*>* edges;
    std::vector<Int_Array*>* cons;
    std::vector<i64>* matrix;
};


void set_matrix(Graph* g, i64 a, i64 b, i64 v) {
    g->matrix->at(a*1000ll + b) = v;
}

i64 get_matrix(Graph* g, i64 a, i64 b) {
    return g->matrix->at(a*1000ll + b);
}

std::string vals_to_str(Graph* g, std::vector<i64>* vals) {
    std::string s;
    s = ""s;

    auto __expr0 = vals;
    for (i64 vals__it0__idx = 0; vals__it0__idx < __expr0->size(); vals__it0__idx++) {
        i64 vals__it0 = (*__expr0)[vals__it0__idx];

        if (s != ""s) {
            s = s + ","s;
        };
        s = s + g->names->at(vals__it0);
    }
    return s;
}

i64 ensure_vertex(Graph* g, std::string vertex_name) {
    i64 index;

    if (map_has_key(g->vertex_names_to_index, vertex_name)) {
        return map_get_value(g->vertex_names_to_index, vertex_name);
    };
    index = len(g->names);
    push(g->names, vertex_name);
    map_add(g->vertex_names_to_index, vertex_name, index);
    return index;
}

void add_vertex_edge(Graph* g, i64 a, i64 b) {
    Int_Array* to;

    if (map_has_key(g->edges, a)) {
        to = map_get_value(g->edges, a);
        push(to->values, b);
    }
    else {
        to = new Int_Array();
        to->values = new std::vector<i64>();
        push(to->values, b);
        map_add(g->edges, a, to);
    };
}

void add_edge(Graph* g, std::string s1, std::string s2) {
    i64 a;
    i64 b;
    a = ensure_vertex(g, s1);
    b = ensure_vertex(g, s2);
    set_matrix(g, a, b, 1ll);
    set_matrix(g, b, a, 1ll);
    add_vertex_edge(g, a, b);
    add_vertex_edge(g, b, a);
}

std::vector<Int_Array*>* next_cons(Graph* g) {
    std::vector<Int_Array*>* result;
    Int_Array* con;
    i64 candidate;
    i64 is_valid;
    Int_Array* new_con;
    result = new std::vector<Int_Array*>();

    auto __expr0 = g->cons;
    for (i64 expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        Int_Array* expr__it0 = (*__expr0)[expr__it0__idx];
        con = expr__it0;

        auto __expr1 = create_range(arr_last(con->values), len(g->names) - 1ll);
        for (i64 expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            candidate = expr__it1;
            is_valid = 1ll;

            auto __expr2 = con->values;
            for (i64 expr__it2__idx = 0; expr__it2__idx < __expr2->size(); expr__it2__idx++) {
                i64 expr__it2 = (*__expr2)[expr__it2__idx];

                if (get_matrix(g, expr__it2, candidate) == 0ll) {
                    is_valid = 0ll;
                    break;
                };
            }

            if (is_valid) {
                new_con = clone_int_array(con);
                push(new_con->values, candidate);
                push(result, new_con);
            };
        }
    }
    return result;
}

void run() {
    std::vector<std::string>* lines;
    Graph* g;
    std::vector<std::string>* p;
    Int_Array* a;
    std::vector<Int_Array*>* next;
    std::vector<i64>* vals;
    std::vector<std::string>* sa;
    std::string s;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    g = new Graph();
    g->names = new std::vector<std::string>();
    g->vertex_names_to_index = new universal_hashmap<std::string, i64>();
    g->edges = new universal_hashmap<i64, Int_Array*>();
    g->cons = new std::vector<Int_Array*>();
    g->matrix = new std::vector<i64>();
    arr_set_len(g->matrix, 1000000ll);

    auto __expr0 = lines;
    for (i64 lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        p = str_split(lines__it0, "-"s);
        add_edge(g, p->at(0ll), p->at(1ll));
    }

    auto __expr1 = g->names;
    for (i64 expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
        std::string expr__it1 = (*__expr1)[expr__it1__idx];
        a = new Int_Array();
        a->values = new std::vector<i64>();
        push(a->values, expr__it1__idx);
        push(g->cons, a);
    }

    auto __expr2 = create_range(1ll, 13ll);
    for (i64 expr__it2__idx = 0; expr__it2__idx < __expr2->size(); expr__it2__idx++) {
        i64 expr__it2 = (*__expr2)[expr__it2__idx];
        next = next_cons(g);
        printf("%s %lld %s %lld\n", ("rank"s).c_str(), static_cast<i64>(expr__it2), ("count"s).c_str(), static_cast<i64>(len(next)));

        if (len(next) == 0ll) {
            break;
        };
        g->cons = next;
    }
    vals = g->cons->at(0ll)->values;
    sa = new std::vector<std::string>();

    auto __expr3 = vals;
    for (i64 vals__it3__idx = 0; vals__it3__idx < __expr3->size(); vals__it3__idx++) {
        i64 vals__it3 = (*__expr3)[vals__it3__idx];
        push(sa, g->names->at(vals__it3));
    }
    arr_sort(sa);
    s = str_arr_join(sa, ","s);
    printf("%s %lld %s\n", ("done"s).c_str(), static_cast<i64>(len(vals)), (vals_to_str(g, vals)).c_str());
    printf("%s %lld %s\n", ("done"s).c_str(), static_cast<i64>(len(vals)), (s).c_str());
}


