#include "prelude.cpp"

/// prelude template <typename T>i64 array_size(shared_vector<T> a);
/// prelude template <typename T>void array_push(shared_vector<T> a, T element);
/// prelude template <typename T>T array_pop(shared_vector<T> a);
/// prelude template <typename T>void array_quick_sort(shared_vector<T> a);
/// prelude template <typename T>void array_push_front(shared_vector<T> a, T element);
/// prelude template <typename T>T array_pop_front(shared_vector<T> a, T element);
/// prelude template <typename T>T array_last(shared_vector<T> a);
/// prelude template <typename T>void array_set_size(shared_vector<T> a, i64 new_size);
/// prelude template <typename T>i64 array_contains(shared_vector<T> a, T value);
/// prelude template <typename T>void array_remove_at(shared_vector<T> a, i64 index);
/// prelude template <typename T>void array_remove(shared_vector<T> a, T value);
/// prelude template <typename T>i64 array_index_of(shared_vector<T> a, T value);
/// prelude template <typename T>shared_vector<T> array_slice(shared_vector<T> a, i64 from, i64 to);
/// prelude template <typename K, typename V>void map_add(shared_map<K, V> map, K key, V value);
/// prelude template <typename K, typename V>V map_get_value(shared_map<K, V> map, K key);
/// prelude template <typename K, typename V>void map_add_or_update(shared_map<K, V> map, K key, V value);
/// prelude template <typename K, typename V>i64 map_has_key(shared_map<K, V> map, K key);
/// prelude template <typename K, typename V>void map_remove(shared_map<K, V> map, K key);
/// prelude template <typename K, typename V>i64 map_size(shared_map<K, V> map);
/// prelude template <typename K, typename V>shared_vector<K> map_keys(shared_map<K, V> map);
/// prelude shared_vector<std::string> string_split(std::string s, std::string by);
/// prelude i64 string_to_integer(std::string s);
/// prelude shared_vector<std::string> string_to_chars(std::string s);
/// prelude i64 string_contains(std::string s, std::string subs);
/// prelude i64 string_size(std::string s);
/// prelude std::string string_remove(std::string s, std::string r);
/// prelude std::string string_array_join(shared_vector<std::string> a, std::string delimiter);
/// prelude std::string integer_to_string(i64 i);
/// prelude void error(std::string s);
/// prelude std::string read_line();
/// prelude shared_vector<std::string> read_string_lines_from_file(std::string filename);
////
//// prelude end
////
struct Int_Array {
    shared_vector<i64> values;
};


shared_pointer<Int_Array> clone_int_array(shared_pointer<Int_Array> arr) {
    shared_pointer<Int_Array> r;
    r = create_shared_pointer<Int_Array >();
    r->values = create_shared_vector<i64>();

    auto __expr0 = arr->values;
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        array_push(r->values, expr__it0);
    }
    return r;
}

struct Graph {
    shared_vector<std::string> names;
    shared_map<std::string, i64> vertex_names_to_index;
    shared_map<i64, shared_pointer<Int_Array>> edges;
    shared_vector<shared_pointer<Int_Array>> cons;
    shared_vector<i64> matrix;
};


void set_matrix(shared_pointer<Graph> g, i64 a, i64 b, i64 v) {
    g->matrix->at(a*1000ll + b) = v;
}

i64 get_matrix(shared_pointer<Graph> g, i64 a, i64 b) {
    return g->matrix->at(a*1000ll + b);
}

std::string vals_to_str(shared_pointer<Graph> g, shared_vector<i64> vals) {
    std::string s;
    s = ""s;

    auto __expr0 = vals;
    for (i64 vals__it0__idx = 0; vals__it0__idx < (i64)__expr0->size(); vals__it0__idx++) {
        i64 vals__it0 = (*__expr0)[vals__it0__idx];

        if (s != ""s) {
            s = s + ","s;
        };
        s = s + g->names->at(vals__it0);
    }
    return s;
}

i64 ensure_vertex(shared_pointer<Graph> g, std::string vertex_name) {
    i64 idx;

    if (map_has_key(g->vertex_names_to_index, vertex_name)) {
        return map_get_value(g->vertex_names_to_index, vertex_name);
    };
    idx = array_size(g->names);
    array_push(g->names, vertex_name);
    map_add(g->vertex_names_to_index, vertex_name, idx);
    return idx;
}

void add_vertex_edge(shared_pointer<Graph> g, i64 a, i64 b) {
    shared_pointer<Int_Array> to;

    if (map_has_key(g->edges, a)) {
        to = map_get_value(g->edges, a);
        array_push(to->values, b);
    }
    else {
        to = create_shared_pointer<Int_Array >();
        to->values = create_shared_vector<i64>();
        array_push(to->values, b);
        map_add(g->edges, a, to);
    };
}

void add_edge(shared_pointer<Graph> g, std::string s1, std::string s2) {
    i64 a;
    i64 b;
    a = ensure_vertex(g, s1);
    b = ensure_vertex(g, s2);
    set_matrix(g, a, b, 1ll);
    set_matrix(g, b, a, 1ll);
    add_vertex_edge(g, a, b);
    add_vertex_edge(g, b, a);
}

shared_vector<shared_pointer<Int_Array>> next_cons(shared_pointer<Graph> g) {
    shared_vector<shared_pointer<Int_Array>> result;
    shared_pointer<Int_Array> con;
    i64 candidate;
    i64 is_valid;
    shared_pointer<Int_Array> new_con;
    result = create_shared_vector<shared_pointer<Int_Array>>();

    auto __expr0 = g->cons;
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        shared_pointer<Int_Array> expr__it0 = (*__expr0)[expr__it0__idx];
        con = expr__it0;

        auto __expr1 = create_range(array_last(con->values), array_size(g->names)-1ll);
        for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            candidate = expr__it1;
            is_valid = 1ll;

            auto __expr2 = con->values;
            for (i64 expr__it2__idx = 0; expr__it2__idx < (i64)__expr2->size(); expr__it2__idx++) {
                i64 expr__it2 = (*__expr2)[expr__it2__idx];

                if (get_matrix(g, expr__it2, candidate) == 0ll) {
                    is_valid = 0ll;
                    break;
                };
            }

            if (is_valid) {
                new_con = clone_int_array(con);
                array_push(new_con->values, candidate);
                array_push(result, new_con);
            };
        }
    }
    return result;
}

void run() {
    shared_vector<std::string> lines;
    shared_pointer<Graph> g;
    shared_vector<std::string> p;
    shared_pointer<Int_Array> a;
    shared_vector<shared_pointer<Int_Array>> next;
    shared_vector<i64> vals;
    shared_vector<std::string> sa;
    std::string s;
    lines = read_string_lines_from_file("D:/src/postal/examples/aoc2024/23/input.txt"s);
    g = create_shared_pointer<Graph >();
    g->names = create_shared_vector<std::string>();
    g->vertex_names_to_index = create_shared_map<std::string, i64>();
    g->edges = create_shared_map<i64, shared_pointer<Int_Array>>();
    g->cons = create_shared_vector<shared_pointer<Int_Array>>();
    g->matrix = create_shared_vector<i64>();
    array_set_size(g->matrix, 1000000ll);

    auto __expr0 = lines;
    for (i64 lines__it0__idx = 0; lines__it0__idx < (i64)__expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        p = string_split(lines__it0, "-"s);
        add_edge(g, p->at(0ll), p->at(1ll));
    }

    auto __expr1 = g->names;
    for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
        std::string expr__it1 = (*__expr1)[expr__it1__idx];
        a = create_shared_pointer<Int_Array >();
        a->values = create_shared_vector<i64>();
        array_push(a->values, expr__it1__idx);
        array_push(g->cons, a);
    }

    auto __expr2 = create_range(1ll, 13ll);
    for (i64 expr__it2__idx = 0; expr__it2__idx < (i64)__expr2->size(); expr__it2__idx++) {
        i64 expr__it2 = (*__expr2)[expr__it2__idx];
        next = next_cons(g);
        printf("%s %lld %s %lld\n", ("rank"s).c_str(), (i64)(expr__it2), ("count"s).c_str(), (i64)(array_size(next)));

        if (array_size(next) == 0ll) {
            break;
        };
        g->cons = next;
    }
    vals = g->cons->at(0ll)->values;
    sa = create_shared_vector<std::string>();

    auto __expr3 = vals;
    for (i64 vals__it3__idx = 0; vals__it3__idx < (i64)__expr3->size(); vals__it3__idx++) {
        i64 vals__it3 = (*__expr3)[vals__it3__idx];
        array_push(sa, g->names->at(vals__it3));
    }
    array_quick_sort(sa);
    s = string_array_join(sa, ","s);
    printf("%s %lld %s\n", ("done"s).c_str(), (i64)(array_size(vals)), (vals_to_str(g, vals)).c_str());
    printf("%s %lld %s\n", ("done"s).c_str(), (i64)(array_size(vals)), (s).c_str());
}

