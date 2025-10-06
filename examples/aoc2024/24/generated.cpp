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
struct Expr {
    std::string left;
    std::string op;
    std::string right;
    std::string reg;
    friend bool operator==(const Expr& l, const Expr& r) {
        return (l.left == r.left) && (l.op == r.op) && (l.right == r.right) && (l.reg == r.reg);
    }
};

namespace std {
    template<>
    struct hash<Expr> {
        std::size_t operator()(const Expr& rec) const {
            size_t h = 0;
            h ^= std::hash<std::string>{}(rec.left) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<std::string>{}(rec.op) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<std::string>{}(rec.right) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<std::string>{}(rec.reg) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

struct Data {
    shared_map<std::string, i64> values;
    shared_vector<shared_pointer<Expr>> exprs;
    shared_vector<std::string> mappings;
};


std::string int_to_str_padded(i64 i) {
    std::string r;
    r = integer_to_string(i);

    if (i < 10ll) {
        r = "0"s + r;
    };
    return r;
}

bool starts_with(std::string s, std::string a) {
    return string_to_chars(s)->at(0ll) == a;
}

std::string expr_to_str(shared_pointer<Expr> e) {
    return e->op + ": "s + e->left + " "s + e->op + " "s + e->right + " -> "s + e->reg;
}

std::string wrap_reg(std::string s) {
    std::string a;
    a = string_to_chars(s)->at(0ll);

    if (a == "x"s || a == "y"s || a == "z"s) {
        return s;
    };
    return "_"s + s;
}

shared_pointer<Data> create_data(shared_vector<std::string> lines) {
    shared_pointer<Data> r;
    shared_pointer<Expr> e;
    shared_vector<std::string> p;
    std::string t;
    r = create_shared_pointer<Data >();
    r->values = create_shared_map<std::string, i64>();
    r->exprs = create_shared_vector<shared_pointer<Expr>>();
    r->mappings = create_shared_vector<std::string>();

    auto __expr0 = lines;
    for (i64 lines__it0__idx = 0; lines__it0__idx < (i64)__expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        e = create_shared_pointer<Expr >();
        p = string_split(lines__it0, " -> "s);
        e->reg = array_last(p);
        p = string_split(p->at(0ll), " "s);
        e->left = p->at(0ll);
        e->op = p->at(1ll);
        e->right = p->at(2ll);

        if (starts_with(e->left, "y"s)) {
            t = e->left;
            e->left = e->right;
            e->right = t;
        };
        e->left = wrap_reg(e->left);
        e->right = wrap_reg(e->right);
        e->reg = wrap_reg(e->reg);
        array_push(r->exprs, e);
    }
    return r;
}

void update_reg(shared_pointer<Data> d, std::string from, std::string to) {
    shared_pointer<Expr> e;
    array_push(d->mappings, to + " <- "s + from);

    auto __expr0 = d->exprs;
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        shared_pointer<Expr> expr__it0 = (*__expr0)[expr__it0__idx];
        e = expr__it0;

        if (e->left == from) {
            e->left = to;
        };

        if (e->right == from) {
            ////log(from)
            e->right = e->left;
            e->left = to;
        };

        if (e->reg == from && !starts_with(e->reg, "z"s)) {
            e->reg = to;
        };
    }
}

void update_regs(shared_pointer<Data> d) {
    i64 i;
    std::string new_reg;

    auto __expr0 = d->exprs;
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        shared_pointer<Expr> expr__it0 = (*__expr0)[expr__it0__idx];

        if (starts_with(expr__it0->left, "x"s)) {
            i = string_to_integer(string_remove(expr__it0->left, "x"s));

            if (expr__it0->op == "xor"s) {
                new_reg = "sum_half_"s + int_to_str_padded(i);
                update_reg(d, expr__it0->reg, new_reg);
            };

            if (expr__it0->op == "and"s) {
                new_reg = "carry_half_"s + int_to_str_padded(i);
                update_reg(d, expr__it0->reg, new_reg);
            };
        };
    }
}

void run() {
    shared_vector<std::string> lines;
    shared_pointer<Data> d;
    shared_vector<std::string> out;
    lines = read_string_lines_from_file("D:/src/postal/examples/aoc2024/24/input.txt"s);
    d = create_data(lines);
    update_regs(d);
    out = create_shared_vector<std::string>();

    auto __expr0 = d->exprs;
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        shared_pointer<Expr> expr__it0 = (*__expr0)[expr__it0__idx];
        array_push(out, expr_to_str(expr__it0));
    }
    array_quick_sort(out);

    auto __expr1 = out;
    for (i64 out__it1__idx = 0; out__it1__idx < (i64)__expr1->size(); out__it1__idx++) {
        std::string out__it1 = (*__expr1)[out__it1__idx];
        printf("%s\n", (out__it1).c_str());
    }
    array_quick_sort(d->mappings);
    printf("%s\n", ("\nmappings\n"s).c_str());

    auto __expr2 = d->mappings;
    for (i64 expr__it2__idx = 0; expr__it2__idx < (i64)__expr2->size(); expr__it2__idx++) {
        std::string expr__it2 = (*__expr2)[expr__it2__idx];
        printf("%s\n", (expr__it2).c_str());
    }
    printf("%s\n", ("done 1"s).c_str());
}

