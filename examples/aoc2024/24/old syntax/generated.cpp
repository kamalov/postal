#include "prelude.h"

/// prelude template <typename T>i64 len(std::vector<T>* a);
/// prelude template <typename T>void push(std::vector<T>* a, T elem);
/// prelude template <typename T>T pop(std::vector<T>* a);
/// prelude template <typename T>void sort(std::vector<T>* a);
/// prelude template <typename T>void arr_push_front(std::vector<T>* a, T elem);
/// prelude template <typename T>T arr_pop_front(std::vector<T>* a, T elem);
/// prelude template <typename T>T arr_last(std::vector<T>* a);
/// prelude template <typename T>void arr_set_len(std::vector<T>* a, i64 new_len);
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
    universal_hashmap<std::string, i64>* values;
    std::vector<Expr*>* exprs;
    std::vector<std::string>* mappings;
};


std::string int_to_str_padded(i64 i) {
    std::string r;
    r = int_to_str(i);

    if (i < 10ll) {
        r = "0"s + r;
    };
    return r;
}

i64 starts_with(std::string s, std::string a) {
    return str_to_chars(s)->at(0ll) == a;
}

std::string expr_to_str(Expr* e) {
    return e->op + ": "s + e->left + " "s + e->op + " "s + e->right + " -> "s + e->reg;
}

std::string wrap_reg(std::string s) {
    std::string a;
    a = str_to_chars(s)->at(0ll);

    if (a == "x"s || a == "y"s || a == "z"s) {
        return s;
    };
    return "_"s + s;
}

Data* create_data(std::vector<std::string>* lines) {
    Data* r;
    Expr* e;
    std::vector<std::string>* p;
    std::string t;
    r = new Data();
    r->values = new universal_hashmap<std::string, i64>();
    r->exprs = new std::vector<Expr*>();
    r->mappings = new std::vector<std::string>();

    auto __expr0 = lines;
    for (i64 lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        e = new Expr();
        p = str_split(lines__it0, " -> "s);
        e->reg = arr_last(p);
        p = str_split(p->at(0ll), " "s);
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
        push(r->exprs, e);
    }
    return r;
}

void update_reg(Data* d, std::string from, std::string to) {
    Expr* e;
    push(d->mappings, to + " <- "s + from);

    auto __expr0 = d->exprs;
    for (i64 expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        Expr* expr__it0 = (*__expr0)[expr__it0__idx];
        e = expr__it0;

        if (e->left == from) {
            e->left = to;
        };

        if (e->right == from) {
            //log(from)
            e->right = e->left;
            e->left = to;
        };

        if (e->reg == from && starts_with(e->reg, "z"s) == 0ll) {
            e->reg = to;
        };
    }
}

void update_regs(Data* d) {
    i64 i;
    std::string new_reg;

    auto __expr0 = d->exprs;
    for (i64 expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        Expr* expr__it0 = (*__expr0)[expr__it0__idx];

        if (starts_with(expr__it0->left, "x"s)) {
            i = str_to_int(str_remove(expr__it0->left, "x"s));

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
    std::vector<std::string>* lines;
    Data* d;
    std::vector<std::string>* out;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    d = create_data(lines);
    update_regs(d);
    out = new std::vector<std::string>();

    auto __expr0 = d->exprs;
    for (i64 expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        Expr* expr__it0 = (*__expr0)[expr__it0__idx];
        push(out, expr_to_str(expr__it0));
    }
    sort(out);

    auto __expr1 = out;
    for (i64 out__it1__idx = 0; out__it1__idx < __expr1->size(); out__it1__idx++) {
        std::string out__it1 = (*__expr1)[out__it1__idx];
        printf("%s\n", (out__it1).c_str());
    }
    sort(d->mappings);
    printf("%s\n", ("\nmappings\n"s).c_str());

    auto __expr2 = d->mappings;
    for (i64 expr__it2__idx = 0; expr__it2__idx < __expr2->size(); expr__it2__idx++) {
        std::string expr__it2 = (*__expr2)[expr__it2__idx];
        printf("%s\n", (expr__it2).c_str());
    }
    printf("%s\n", ("done 1"s).c_str());
}


