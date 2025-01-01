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
i64 exp(i64 a, i64 e) {
    i64 r;
    r = 1ll;

    auto __expr0 = create_range(1ll, e);
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
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
    std::vector<i64>* instructions;
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

i64 get_combo_value(Data* d, i64 operand) {

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
    err("get_combo_value opcode "s + int_to_str(operand));
}

void print(Data* d) {
    printf("%s %lld %s %lld %s %lld\n", ("A="s).c_str(), static_cast<i64>(d->a), ("B="s).c_str(), static_cast<i64>(d->b), ("C="s).c_str(), static_cast<i64>(d->c));
    //log('instructions:', len(d.instructions))
    //log('out:', d.out)
}

void print_params(Data* d) {
    i64 op;
    i64 v;

    if (d->ip >= len(d->instructions)) {
        op = 0ll;
        v = 0ll;
    }
    else {
        op = d->instructions->at(d->ip);
        v = d->instructions->at(d->ip + 1ll);
    };
    printf("%s %lld %s %lld %s %lld %s %s\n", ("op="s).c_str(), static_cast<i64>(op), ("value="s).c_str(), static_cast<i64>(v), ("combo="s).c_str(), static_cast<i64>(get_combo_value(d, v)), ("out:"s).c_str(), (d->out).c_str());
}

Data* create_data(std::vector<std::string>* lines) {
    Data* d;
    d = new Data();
    d->a = str_to_int(str_split(lines->at(0ll), " "s)->at(2ll));
    d->b = str_to_int(str_split(lines->at(1ll), " "s)->at(2ll));
    d->c = str_to_int(str_split(lines->at(2ll), " "s)->at(2ll));
    d->instructions = new std::vector<i64>();

    auto __expr0 = str_split(str_split(lines->at(4ll), " "s)->at(1ll), ","s);
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        std::string expr__it0 = (*__expr0)[expr__it0__idx];
        push(d->instructions, str_to_int(expr__it0));
    }
    return d;
}

void print1(Data* d, std::string cmd) {
    return;
    printf("%s\n", (cmd).c_str());
    print(d);
}

void execute(Data* d) {
    i64 opcode;
    i64 operand;
    i64 value;
    i64 combo_value;
    i64 v;

    for (;;) {

        if (d->ip >= len(d->instructions)) {
            break;
        };
        opcode = d->instructions->at(d->ip);
        operand = d->instructions->at(d->ip + 1ll);
        value = operand;
        combo_value = get_combo_value(d, operand);
        //print(d)
        //print_params(d)

        if (opcode == 0ll) {
            /// adv
            //log('adv', 'A >> combo |', d.a, '>>', combo_value, '=', d.a rshift 3)
            d->a = d->a >> combo_value;
            d->ip = d->ip + 2ll;
        }
        else if (opcode == 1ll) {
            /// bxl
            //log('bxl', 'B xor value => B |', d.b, 'xor', value, '=>', d.b^value)
            d->b = d->b^value;
            d->ip = d->ip + 2ll;
        }
        else if (opcode == 2ll) {
            /// bst 
            //log('bst: A(', d.a, ') % 8 => B(', d.a % 8, ')')
            d->b = combo_value % 8ll;
            d->ip = d->ip + 2ll;
        }
        else if (opcode == 3ll) {
            //log('ret ret ret ret ret ret ret ret ret ret ret ret ret ret ret ret ret ret ret ret ret')
            /// jnz

            if (d->a > 0ll) {
                d->ip = value;
            }
            else {
                d->ip = d->ip + 2ll;
            };
        }
        else if (opcode == 4ll) {
            /// bxc
            //log('bxc: B xor C => B(', d.b, 'xor', d.c, '=>', d.b ^ d.c)
            d->b = d->b^d->c;
            d->ip = d->ip + 2ll;
        }
        else if (opcode == 5ll) {
            /// out
            v = combo_value % 8ll;
            //log('out: combo % 8:', combo_value, '% 8', v)

            if (str_len(d->out) > 0ll) {
                d->out = d->out + ","s;
            };
            d->out = d->out + int_to_str(v);
            d->ip = d->ip + 2ll;
        }
        else if (opcode == 6ll) {
            //log('opcode = 6')
            /// bdv
            d->b = d->a >> combo_value;
            d->ip = d->ip + 2ll;
        }
        else if (opcode == 7ll) {
            /// cdv
            d->c = d->a >> combo_value;
            //log('cdv', 'A >> combo => C |', d.a, '>>', combo_value, '=', d.c)
            d->ip = d->ip + 2ll;
        }
        else {
            err("err"s);
        };
        //print(d)
        //log()
        //readln()
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

void try_find(std::vector<i64>* targets, i64 a, i64 index, std::vector<i64>* res) {
    i64 target;
    i64 a_new;

    if (index >= len(targets)) {
        return;
    };
    target = targets->at(index);

    auto __expr0 = create_range(0ll, 7ll);
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        a_new = a*8ll + expr__it0;

        if (test(a_new) == target) {

            if (index == len(targets) - 1ll) {
                printf("%s %lld\n", ("found "s).c_str(), static_cast<i64>(a_new));
                push(res, a_new);
            };
            try_find(targets, a_new, index + 1ll, res);
        };
    }
}

void generate(Data* d) {
    std::vector<i64>* targets;
    std::vector<std::string>* arr;
    std::vector<i64>* res;
    targets = new std::vector<i64>();
    arr = str_split("2,4,1,5,7,5,1,6,0,3,4,3,5,5,3,0"s, ","s);

    for (;;) {

        if (len(arr) == 0ll) {
            break;
        };
        push(targets, str_to_int(pop(arr)));
    }
    res = new std::vector<i64>();
    try_find(targets, 0ll, 0ll, res);
    arr_sort(res);
    printf("%s %lld\n", ("reses"s).c_str(), static_cast<i64>(len(res)));
    printf("%s %lld\n", ("min"s).c_str(), static_cast<i64>(res->at(0ll)));
}

void run() {
    std::vector<std::string>* lines;
    Data* d;
    std::string s;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    d = create_data(lines);
    s = str_split(lines->at(4ll), " "s)->at(1ll);
    s = str_remove(s, ","s);
    //target = str_to_int(s)
    print(d);
    execute(d);
    print(d);
    generate(d);
    printf("%s %s\n", ("\nout="s).c_str(), (d->out).c_str());
}


