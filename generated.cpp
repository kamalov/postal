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
    i64 v;

    if (d->ip >= len(d->instructions)) {
        v = 0ll;
    }
    else {
        v = d->instructions->at(d->ip + 1ll);
    };
    printf("%s %lld %s %lld %s %lld %s %lld %s %lld %s %lld %s %s\n", ("A="s).c_str(), static_cast<i64>(d->a), ("B="s).c_str(), static_cast<i64>(d->b), ("C="s).c_str(), static_cast<i64>(d->c), ("ip="s).c_str(), static_cast<i64>(d->ip), ("value="s).c_str(), static_cast<i64>(v), ("combo="s).c_str(), static_cast<i64>(get_combo_value(d, v)), ("out:"s).c_str(), (d->out).c_str());
    //log('instructions:', len(d.instructions))
    //log('out:', d.out)
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
        /// adv

        if (opcode == 0ll) {
            print1(d, "adv"s);
            d->a = d->a/exp(2ll, combo_value);
            d->ip = d->ip + 2ll;
            continue;
        };
        /// bxl 

        if (opcode == 1ll) {
            print(d);
            printf("%s %s %lld %s %lld %s %lld\n", ("bxl"s).c_str(), ("B = B ^ value ::"s).c_str(), static_cast<i64>(d->b^value), ("="s).c_str(), static_cast<i64>(d->b), ("^"s).c_str(), static_cast<i64>(value));
            d->b = d->b^value;
            d->ip = d->ip + 2ll;
            print(d);
            //readln()
            continue;
        };
        /// bst 

        if (opcode == 2ll) {
            print(d);
            d->b = combo_value % 8ll;
            printf("%s %lld %s %lld %s\n", ("bst: B = combo % 8 ::"s).c_str(), static_cast<i64>(d->b), ("="s).c_str(), static_cast<i64>(combo_value), ("% 8"s).c_str());
            d->ip = d->ip + 2ll;
            print(d);
            printf("\n");
            //readln()
            continue;
        };
        /// jnz

        if (opcode == 3ll) {
            print1(d, "jnz"s);

            if (d->a > 0ll) {
                d->ip = value;
            }
            else {
                d->ip = d->ip + 2ll;
            };
            //readln()
            continue;
        };
        /// bxc

        if (opcode == 4ll) {
            print1(d, "bxc"s);
            d->b = d->b^d->c;
            d->ip = d->ip + 2ll;
            //readln()
            continue;
        };
        /// out

        if (opcode == 5ll) {
            print1(d, "out"s);
            v = combo_value % 8ll;

            if (str_len(d->out) > 0ll) {
                d->out = d->out + ","s;
            };
            d->out = d->out + int_to_str(v);
            d->ip = d->ip + 2ll;
            //readln()
            continue;
        };
        /// bdv

        if (opcode == 6ll) {
            print1(d, "bdv"s);
            d->b = d->a/exp(2ll, combo_value);
            d->ip = d->ip + 2ll;
            //readln()
            continue;
        };
        /// cdv

        if (opcode == 7ll) {
            print(d);
            d->c = d->a/exp(2ll, combo_value);
            printf("%s %s %lld %s %lld %s %lld\n", ("cdv"s).c_str(), ("C = A div 2^combo ::"s).c_str(), static_cast<i64>(d->c), ("="s).c_str(), static_cast<i64>(d->a), ("div 2^"s).c_str(), static_cast<i64>(combo_value));
            d->ip = d->ip + 2ll;
            print(d);
            //readln()
            continue;
        };
    }
}

void run() {
    std::vector<std::string>* lines;
    Data* d;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    d = create_data(lines);
    printf("%s\n", (lines->at(4ll)).c_str());
    execute(d);
    printf("%s %s\n", ("\ndone"s).c_str(), (d->out).c_str());
}


