#include "prelude.h"

/// prelude template <typename T>i64 len(std::vector<T>* a);
/// prelude template <typename T>void push(std::vector<T>* a, T elem);
/// prelude template <typename T>T pop(std::vector<T>* a);
/// prelude template <typename T>void arr_push_front(std::vector<T>* a, T elem);
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
//     +---+---+
//     | ^ | A |
// +---+---+---+
// | < | v | > |
// +---+---+---+
struct Navi {
    i64 from;
    i64 to;
    std::vector<i64>* path;
    std::string path_str;
    friend bool operator==(const Navi& l, const Navi& r) {
        return (l.from == r.from) && (l.to == r.to) && (l.path_str == r.path_str);
    }
};

namespace std {
    template<>
    struct hash<Navi> {
        std::size_t operator()(const Navi& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.from) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.to) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<std::string>{}(rec.path_str) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

std::string reverse_dir(std::string dir) {

    if (dir == "^"s) {
        return "v"s;
    };

    if (dir == ">"s) {
        return "<"s;
    };

    if (dir == "<"s) {
        return ">"s;
    };

    if (dir == "v"s) {
        return "^"s;
    };
    err("err"s);
}

i64 char_to_int(std::string c) {

    if (c == "A"s) {
        return 10ll;
    };

    if (c == ">"s) {
        return 11ll;
    };

    if (c == "v"s) {
        return 12ll;
    };

    if (c == "<"s) {
        return 13ll;
    };

    if (c == "^"s) {
        return 14ll;
    };
    return str_to_int(c);
}

std::string int_to_char(i64 i) {

    if (i == 10ll) {
        return "A"s;
    };

    if (i == 11ll) {
        return ">"s;
    };

    if (i == 12ll) {
        return "v"s;
    };

    if (i == 13ll) {
        return "<"s;
    };

    if (i == 14ll) {
        return "^"s;
    };
    return int_to_str(i);
}

std::string path_to_str(std::vector<i64>* p) {
    std::string s;
    s = ""s;

    auto __expr0 = p;
    for (int p__it0__idx = 0; p__it0__idx < __expr0->size(); p__it0__idx++) {
        i64 p__it0 = (*__expr0)[p__it0__idx];
        s = s + int_to_char(p__it0);
    }
    return s;
}

std::vector<i64>* str_to_path(std::string s) {
    std::vector<i64>* r;
    r = new std::vector<i64>();

    auto __expr0 = str_to_chars(s);
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        std::string expr__it0 = (*__expr0)[expr__it0__idx];
        push(r, char_to_int(expr__it0));
    }
    return r;
}

void print_navi(Navi* n) {
    std::string s;
    printf("%lld\n", static_cast<i64>(n->from + n->to));
    s = "    "s;

    auto __expr0 = n->path;
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        s = s + int_to_char(expr__it0) + " "s;
    }
    printf("%s\n", (s).c_str());
}

std::vector<std::string>* get_paths_str_array() {
    std::vector<std::string>* l;
    l = new std::vector<std::string>();
    push(l, "A^ <"s);
    push(l, "A> v"s);
    //push(l, 'Av <v v<')
    push(l, "Av <v"s);
    push(l, "A< v<<"s);
    //push(l, '^> >v v>')
    push(l, "^> >v"s);
    push(l, "^v v"s);
    push(l, "^< v<"s);
    push(l, ">v <"s);
    push(l, ">< <<"s);
    push(l, "v< <"s);
    // +---+---+---+
    // | 7 | 8 | 9 |
    // +---+---+---+
    // | 4 | 5 | 6 |
    // +---+---+---+
    // | 1 | 2 | 3 |
    // +---+---+---+
    //     | 0 | A |
    //     +---+---+
    push(l, "A0 <"s);
    push(l, "A1 ^<<"s);
    //push(l, 'A2 <^ ^<')
    push(l, "A2 <^"s);
    push(l, "A3 ^"s);
    push(l, "A4 ^^<<"s);
    //push(l, 'A5 <^^ ^^<')
    push(l, "A5 <^^"s);
    push(l, "A6 ^^"s);
    //push(l, 'A7 <^<^^ <^^<^ <^^^< ^<<^^ ^<^<^ ^<^^< ^^<<^ ^^<^< ^^^<<')
    push(l, "A7 ^^^<<"s);
    //push(l, 'A8 <^^^ ^^^<')
    push(l, "A8 <^^^"s);
    push(l, "A9 ^^^"s);
    push(l, "01 ^<"s);
    push(l, "02 ^"s);
    push(l, "03 ^>"s);
    push(l, "04 ^^<"s);
    push(l, "05 ^^"s);
    push(l, "06 ^^>"s);
    push(l, "07 ^^^<"s);
    push(l, "08 ^^^"s);
    push(l, "09 ^^^>"s);
    push(l, "12 >"s);
    push(l, "13 >>"s);
    push(l, "14 ^"s);
    //push(l, '15 >^ ^>')
    push(l, "15 >^"s);
    push(l, "16 >>^ ^>>"s);
    push(l, "17 ^^"s);
    //push(l, '18 ^^> >^^')
    push(l, "18 >^^"s);
    //push(l, '19 >>^^ ^^>>')
    push(l, "19 >>^^"s);
    push(l, "23 >"s);
    //push(l, '24 <^ ^<')
    push(l, "24 <^"s);
    push(l, "25 ^"s);
    //push(l, '26 ^> >^')
    push(l, "26 >^"s);
    //push(l, '27 <^^ ^^<')
    push(l, "27 <^^"s);
    push(l, "28 ^^"s);
    //push(l, '29 >^^ ^^>')
    push(l, "29 >^^"s);
    //    push(l, '34 <<^ ^<<')
    push(l, "34 <<^"s);
    //push(l, '35 <^ ^<')
    push(l, "35 <^"s);
    push(l, "36 ^"s);
    //push(l, '37 <<^^ ^^<<')
    push(l, "37 <<^^"s);
    //push(l, '38 <^^ ^^<')
    push(l, "38 <^^"s);
    push(l, "39 ^^"s);
    push(l, "45 >"s);
    push(l, "46 >>"s);
    push(l, "47 ^"s);
    //push(l, '48 ^> >^')
    push(l, "48 >^"s);
    //push(l, '49 ^>> >>^')
    push(l, "49 >>^"s);
    push(l, "56 >"s);
    //push(l, '57 <^ ^<')
    push(l, "57 <^"s);
    push(l, "58 ^"s);
    //push(l, '59 ^> >^')
    push(l, "59 >^"s);
    //push(l, '67 <<^ ^<<')
    push(l, "67 <<^"s);
    //push(l, '68 <^ ^<')
    push(l, "68 <^"s);
    push(l, "69 ^"s);
    push(l, "78 >"s);
    push(l, "79 >>"s);
    push(l, "89 >"s);
    return l;
}

std::vector<Navi*>* create_navis(std::vector<std::string>* lines) {
    std::vector<Navi*>* navis;
    std::vector<std::string>* p;
    i64 from;
    i64 to;
    std::vector<std::string>* a;
    Navi* forward;
    Navi* backward;
    navis = new std::vector<Navi*>();

    auto __expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        p = str_split(lines__it0, " "s);
        from = char_to_int(str_to_chars(p->at(0ll))->at(0ll));
        to = char_to_int(str_to_chars(p->at(0ll))->at(1ll));

        auto __expr1 = create_range(1ll, len(p) - 1ll);
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            a = str_to_chars(p->at(expr__it1));
            forward = new Navi();
            forward->from = from;
            forward->to = to;
            forward->path = new std::vector<i64>();
            backward = new Navi();
            backward->from = to;
            backward->to = from;
            backward->path = new std::vector<i64>();

            auto __expr2 = a;
            for (int a__it2__idx = 0; a__it2__idx < __expr2->size(); a__it2__idx++) {
                std::string a__it2 = (*__expr2)[a__it2__idx];
                push(forward->path, char_to_int(a__it2));
                arr_push_front(backward->path, char_to_int(reverse_dir(a__it2)));
            }
            forward->path_str = path_to_str(forward->path);
            backward->path_str = path_to_str(backward->path);
            push(navis, forward);
            push(navis, backward);
        }
    }
    return navis;
}

struct Data {
    std::vector<Navi*>* navis;
};


std::string transform(Data* d, std::vector<i64>* target_path, i64 from_, i64 index_, std::string transformed_) {
    i64 from;
    std::vector<std::string>* t;
    i64 index;
    std::string s;
    i64 to;
    from = from_;
    t = new std::vector<std::string>();
    push(t, transformed_);
    index = index_;

    for (;;) {

        if (index >= len(target_path)) {
            // if map_has_key(res, transformed) = 0 {
            //     map_add(res, transformed, 0)
            // }
            s = str_arr_join(t, ""s);
            arr_set_len(t, 0ll);
            return s;
        };
        to = target_path->at(index);

        if (from == to) {
            //transform(d, target_path, to, index + 1, transformed + 'A', res)
            from = to;
            index = index + 1ll;
            push(t, "A"s);
        }
        else {

            auto __expr0 = d->navis;
            for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
                Navi* expr__it0 = (*__expr0)[expr__it0__idx];

                if (expr__it0->from == from && expr__it0->to == to) {
                    //log('it.from = from and it.to', from, to, s)
                    //transform(d, target_path, to, index + 1, transformed + it.path_str + 'A', res)
                    from = to;
                    index = index + 1ll;
                    push(t, expr__it0->path_str + "A"s);
                    break;
                };
            }
        };
    }
}

void transform1(Data* d, std::vector<i64>* target_path, i64 from, i64 index, std::string transformed, std::vector<std::string>* res) {
    i64 to;
    i64 c;

    if (index >= len(target_path)) {
        push(res, transformed);
        return;
    };
    to = target_path->at(index);

    if (from == to) {
        transform1(d, target_path, to, index + 1ll, transformed + "A"s, res);
    }
    else {
        c = 0ll;

        auto __expr0 = d->navis;
        for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
            Navi* expr__it0 = (*__expr0)[expr__it0__idx];

            if (expr__it0->from == from && expr__it0->to == to) {

                if (c > 0ll) {
                    printf("%s\n", (expr__it0->path_str).c_str());
                };
                //log('it.from = from and it.to', from, to, s)
                transform1(d, target_path, to, index + 1ll, transformed + expr__it0->path_str + "A"s, res);
                c = c + 1ll;
            };
        }
    };
}

i64 process(Data* d, std::string s) {
    std::string src;
    i64 iteration;
    std::vector<i64>* p;
    std::string res;
    src = s;
    //log(s)

    auto __expr0 = create_range(1ll, 20ll);
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        //log('it', it)
        //res = #[str, int]
        iteration = expr__it0;

        if (iteration > 2ll) {
            printf("%s %lld\n", ("it"s).c_str(), static_cast<i64>(iteration));
        };
        //log(it)
        //if iteration > 2 do log(it)
        //r = [str]
        p = str_to_path(src);
        res = transform(d, p, char_to_int("A"s), 0ll, ""s);

        if (iteration > 2ll) {
            // for r do log(it, str_len(it))
            // log()
        };
        //for r do push(res, it)
        src = res;

        if (iteration > 10ll) {
            printf("%lld\n", static_cast<i64>(str_len(res)));
        };
        //arr_set_len(res, 0)
    }
    printf("%s %lld\n", ("len"s).c_str(), static_cast<i64>(str_len(src)));
    //arr_set_len(src, 0)
    return str_len(src);
}

void run() {
    std::vector<std::string>* lines;
    Data* d;
    i64 total;
    std::string s;
    i64 v;
    i64 r;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    d = new Data();
    d->navis = create_navis(get_paths_str_array());
    total = 0ll;

    auto __expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        s = lines__it0;

        if (s == ""s) {
            break;
        };
        //log(s)
        v = str_to_int(str_remove(s, "A"s));
        r = process(d, s);
        //log(v, r, v*r)
        total = total + v*r;
    }
    //log('\ndone')
    printf("%s %lld\n", ("\ndone"s).c_str(), static_cast<i64>(total));
}


