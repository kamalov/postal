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
i64 exp(i64 a, i64 e) {
    i64 r;
    r = 1ll;

    auto __expr0 = create_range(1ll, e);
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        r = r*a;
    }
    return r;
}

struct Data {
    i64 a;
    i64 b;
    i64 c;
    i64 ip;
    shared_vector<i64> instructions;
    std::string out;
    friend bool operator==(const Data& l, const Data& r) {
        return (l.a == r.a) && (l.b == r.b) && (l.c == r.c) && (l.ip == r.ip) && (l.out == r.out);
    }
};

namespace std {
    template<>
    struct hash<Data> {
        std::size_t operator()(const Data& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.a) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.b) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.c) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.ip) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<std::string>{}(rec.out) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

i64 get_combo_value(shared_pointer<Data> d, i64 operand) {

    if (operand <= 3ll) {
        return operand;
    };

    if (operand == 4ll) {
        return d->a;
    };

    if (operand == 5ll) {
        return d->b;
    };

    if (operand == 6ll) {
        return d->c;
    };

    if (operand == 7ll) {
        return 7ll;
    };
    error("get_combo_value opcode "s + integer_to_string(operand));
}

void print_data(shared_pointer<Data> d) {
    printf("%s %lld %s %lld %s %lld\n", ("A="s).c_str(), (i64)(d->a), ("B="s).c_str(), (i64)(d->b), ("C="s).c_str(), (i64)(d->c));
    ////print("instructions:", array_size(d.instructions))
    ////print("out:", d.out)
}

void print_params(shared_pointer<Data> d) {
    i64 op;
    i64 v;

    if (d->ip >= array_size(d->instructions)) {
        op = 0ll;
        v = 0ll;
    }
    else {
        op = d->instructions->at(d->ip);
        v = d->instructions->at(d->ip + 1ll);
    };
    printf("%s %lld %s %lld %s %lld %s %s\n", ("op="s).c_str(), (i64)(op), ("value="s).c_str(), (i64)(v), ("combo="s).c_str(), (i64)(get_combo_value(d, v)), ("out:"s).c_str(), (d->out).c_str());
}

shared_pointer<Data> create_data(shared_vector<std::string> lines) {
    shared_pointer<Data> d;
    d = create_shared_pointer<Data >();
    d->a = string_to_integer(string_split(lines->at(0ll), " "s)->at(2ll));
    d->b = string_to_integer(string_split(lines->at(1ll), " "s)->at(2ll));
    d->c = string_to_integer(string_split(lines->at(2ll), " "s)->at(2ll));
    d->instructions = create_shared_vector<i64>();

    auto __expr0 = string_split(string_split(lines->at(4ll), " "s)->at(1ll), ","s);
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        std::string expr__it0 = (*__expr0)[expr__it0__idx];
        array_push(d->instructions, string_to_integer(expr__it0));
    }
    return d;
}

void execute(shared_pointer<Data> d) {
    i64 opcode;
    i64 operand;
    i64 value;
    i64 combo_value;
    i64 v;

    for (;;) {

        if (d->ip >= array_size(d->instructions)) {
            break;
        };
        opcode = d->instructions->at(d->ip);
        operand = d->instructions->at(d->ip + 1ll);
        value = operand;
        combo_value = get_combo_value(d, operand);
        ////print(d)
        ////print_params(d)

        if (opcode == 0ll) {
            ///// adv
            ////print("adv", "A >> combo |", d.a, ">>", combo_value, "=", d.a rshift 3)
            d->a = d->a >> combo_value;
            d->ip = d->ip + 2ll;
        }
        else if (opcode == 1ll) {
            ///// bxl
            ////print("bxl", "B xor value => B |", d.b, "xor", value, "=>", d.bbit_xorvalue)
            d->b = d->b^value;
            d->ip = d->ip + 2ll;
        }
        else if (opcode == 2ll) {
            ///// bst 
            ////print("bst: A(", d.a, ") mod 8 => B(", d.a mod 8, ")")
            d->b = combo_value % 8ll;
            d->ip = d->ip + 2ll;
        }
        else if (opcode == 3ll) {
            ////print("return return return return return return return return return return return return return return return return return return return return return")
            ///// jnz

            if (d->a > 0ll) {
                d->ip = value;
            }
            else {
                d->ip = d->ip + 2ll;
            };
        }
        else if (opcode == 4ll) {
            ///// bxc
            ////print("bxc: B xor C => B(", d.b, "xor", d.c, "=>", d.b bit_xor d.c)
            d->b = d->b^d->c;
            d->ip = d->ip + 2ll;
        }
        else if (opcode == 5ll) {
            ///// out
            v = combo_value % 8ll;
            ////print("out: combo mod 8:", combo_value, "mod 8", v)

            if (string_size(d->out) > 0ll) {
                d->out = d->out + ","s;
            };
            d->out = d->out + integer_to_string(v);
            d->ip = d->ip + 2ll;
        }
        else if (opcode == 6ll) {
            ////print("opcode = 6")
            ///// bdv
            d->b = d->a >> combo_value;
            d->ip = d->ip + 2ll;
        }
        else if (opcode == 7ll) {
            ///// cdv
            d->c = d->a >> combo_value;
            ////print("cdv", "A >> combo => C |", d.a, ">>", combo_value, "=", d.c)
            d->ip = d->ip + 2ll;
        }
        else {
            error("err"s);
        };
        ////print(d)
        ////print()
        ////readln()
    }
}

i64 test(i64 a) {
    i64 b;
    i64 c;
    b = a % 8ll;
    b = b^5ll;
    c = a >> b;
    b = b^6ll;
    b = b^c;
    b = b % 8ll;
    return b;
}

void try_find(shared_vector<i64> targets, i64 a, i64 target_index, shared_vector<i64> res) {
    i64 target;
    i64 a_new;

    if (target_index >= array_size(targets)) {
        return;
    };
    target = targets->at(target_index);

    auto __expr0 = create_range(0ll, 7ll);
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        a_new = a*8ll + expr__it0;

        if (test(a_new) == target) {

            if (target_index == array_size(targets) - 1ll) {
                printf("%s %lld\n", ("found "s).c_str(), (i64)(a_new));
                array_push(res, a_new);
            };
            try_find(targets, a_new, target_index + 1ll, res);
        };
    }
}

void generate(shared_pointer<Data> d) {
    shared_vector<i64> targets;
    shared_vector<std::string> arr;
    shared_vector<i64> res;
    targets = create_shared_vector<i64>();
    arr = string_split("2,4,1,5,7,5,1,6,0,3,4,3,5,5,3,0"s, ","s);

    for (;;) {

        if (array_size(arr) == 0ll) {
            break;
        };
        array_push(targets, string_to_integer(array_pop(arr)));
    }
    res = create_shared_vector<i64>();
    try_find(targets, 0ll, 0ll, res);
    array_quick_sort(res);
    printf("%s %lld\n", ("reses"s).c_str(), (i64)(array_size(res)));
    printf("%s %lld\n", ("min"s).c_str(), (i64)(res->at(0ll)));
}

void run() {
    shared_vector<std::string> lines;
    shared_pointer<Data> d;
    std::string s;
    lines = read_string_lines_from_file("./input.txt"s);
    d = create_data(lines);
    s = string_split(lines->at(4ll), " "s)->at(1ll);
    s = string_remove(s, ","s);
    ////target = string_to_integer(s)
    print_data(d);
    execute(d);
    print_data(d);
    generate(d);
    printf("%s %s\n", ("\nout="s).c_str(), (d->out).c_str());
}

