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
////     +---+---+
////     | ^ | A |
//// +---+---+---+
//// | < | v | > |
//// +---+---+---+
struct Navi {
    i64 from;
    i64 to;
    shared_vector<i64> path;
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
    error("error"s);
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
    return string_to_integer(c);
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
    return integer_to_string(i);
}

std::string path_to_str(shared_vector<i64> p) {
    std::string s;
    s = ""s;

    auto __expr0 = p;
    for (i64 p__it0__idx = 0; p__it0__idx < (i64)__expr0->size(); p__it0__idx++) {
        i64 p__it0 = (*__expr0)[p__it0__idx];
        s = s + int_to_char(p__it0);
    }
    return s;
}

shared_vector<i64> str_to_path(std::string s) {
    shared_vector<i64> r;
    r = create_shared_vector<i64>();

    auto __expr0 = string_to_chars(s);
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        std::string expr__it0 = (*__expr0)[expr__it0__idx];
        array_push(r, char_to_int(expr__it0));
    }
    return r;
}

void print_navi(shared_pointer<Navi> n) {
    std::string s;
    printf("%lld\n", (i64)(n->from + n->to));
    s = "    "s;

    auto __expr0 = n->path;
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        s = s + int_to_char(expr__it0) + " "s;
    }
    printf("%s\n", (s).c_str());
}

shared_vector<std::string> get_paths_str_array() {
    shared_vector<std::string> l;
    l = create_shared_vector<std::string>();
    array_push(l, "A^ <"s);
    array_push(l, "A> v"s);
    array_push(l, "Av <v v<"s);
    array_push(l, "A< v<<"s);
    array_push(l, "^> >v v>"s);
    array_push(l, "^v v"s);
    array_push(l, "^< v<"s);
    array_push(l, ">v <"s);
    array_push(l, ">< <<"s);
    array_push(l, "v< <"s);
    //// +---+---+---+
    //// | 7 | 8 | 9 |
    //// +---+---+---+
    //// | 4 | 5 | 6 |
    //// +---+---+---+
    //// | 1 | 2 | 3 |
    //// +---+---+---+
    ////     | 0 | A |
    ////     +---+---+
    array_push(l, "A0 <"s);
    array_push(l, "A1 ^<<"s);
    array_push(l, "A2 <^ ^<"s);
    array_push(l, "A3 ^"s);
    array_push(l, "A4 ^^<<"s);
    array_push(l, "A5 <^^ ^^<"s);
    array_push(l, "A6 ^^"s);
    array_push(l, "A7 ^^^<< <^<^^ <^^<^ <^^^< ^<<^^ ^<^<^ ^<^^< ^^<<^ ^^<^<"s);
    array_push(l, "A8 <^^^ ^^^<"s);
    array_push(l, "A9 ^^^"s);
    array_push(l, "01 ^<"s);
    array_push(l, "02 ^"s);
    array_push(l, "03 ^>"s);
    array_push(l, "04 ^^<"s);
    array_push(l, "05 ^^"s);
    array_push(l, "06 ^^>"s);
    array_push(l, "07 ^^^<"s);
    array_push(l, "08 ^^^"s);
    array_push(l, "09 >^^^"s);
    array_push(l, "12 >"s);
    array_push(l, "13 >>"s);
    array_push(l, "14 ^"s);
    array_push(l, "15 >^ ^>"s);
    array_push(l, "16 >>^ ^>>"s);
    array_push(l, "17 ^^"s);
    array_push(l, "18 ^^> >^^"s);
    array_push(l, "19 >>^^ ^^>>"s);
    array_push(l, "23 >"s);
    array_push(l, "24 <^ ^<"s);
    array_push(l, "25 ^"s);
    array_push(l, "26 ^> >^"s);
    array_push(l, "27 <^^ ^^<"s);
    array_push(l, "28 ^^"s);
    array_push(l, "29 >^^ ^^>"s);
    array_push(l, "34 <^< <<^ ^<<"s);
    array_push(l, "35 <^ ^<"s);
    array_push(l, "36 ^"s);
    array_push(l, "37 <<^^ ^^<<"s);
    array_push(l, "38 <^^ ^^<"s);
    array_push(l, "39 ^^"s);
    array_push(l, "45 >"s);
    array_push(l, "46 >>"s);
    array_push(l, "47 ^"s);
    array_push(l, "48 ^> >^"s);
    array_push(l, "49 ^>> >>^"s);
    array_push(l, "56 >"s);
    array_push(l, "57 <^ ^<"s);
    array_push(l, "58 ^"s);
    array_push(l, "59 ^> >^"s);
    array_push(l, "67 <<^ ^<<"s);
    array_push(l, "68 <^ ^<"s);
    array_push(l, "69 ^"s);
    array_push(l, "78 >"s);
    array_push(l, "79 >>"s);
    array_push(l, "89 >"s);
    return l;
}

shared_vector<shared_pointer<Navi>> create_navis(shared_vector<std::string> lines) {
    shared_vector<shared_pointer<Navi>> navis;
    shared_vector<std::string> p;
    i64 from;
    i64 to;
    shared_vector<std::string> a;
    shared_pointer<Navi> forward;
    shared_pointer<Navi> backward;
    navis = create_shared_vector<shared_pointer<Navi>>();

    auto __expr0 = lines;
    for (i64 lines__it0__idx = 0; lines__it0__idx < (i64)__expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        p = string_split(lines__it0, " "s);
        from = char_to_int(string_to_chars(p->at(0ll))->at(0ll));
        to = char_to_int(string_to_chars(p->at(0ll))->at(1ll));

        auto __expr1 = create_range(1ll, array_size(p)-1ll);
        for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            a = string_to_chars(p->at(expr__it1));
            forward = create_shared_pointer<Navi >();
            forward->from = from;
            forward->to = to;
            forward->path = create_shared_vector<i64>();
            backward = create_shared_pointer<Navi >();
            backward->from = to;
            backward->to = from;
            backward->path = create_shared_vector<i64>();

            auto __expr2 = a;
            for (i64 a__it2__idx = 0; a__it2__idx < (i64)__expr2->size(); a__it2__idx++) {
                std::string a__it2 = (*__expr2)[a__it2__idx];
                array_push(forward->path, char_to_int(a__it2));
                array_push_front(backward->path, char_to_int(reverse_dir(a__it2)));
            }
            forward->path_str = path_to_str(forward->path);
            backward->path_str = path_to_str(backward->path);
            array_push(navis, forward);
            array_push(navis, backward);
        }
    }
    return navis;
}

struct Data {
    shared_vector<shared_pointer<Navi>> navis;
    shared_vector<i64> empty_path;
};


std::string transform_stack(shared_pointer<Data> d, shared_vector<i64> target_path, i64 from_, i64 index_, std::string transformed_) {
    i64 from;
    shared_vector<std::string> t;
    i64 current_index;
    std::string s;
    i64 to;
    from = from_;
    t = create_shared_vector<std::string>();
    array_push(t, transformed_);
    current_index = index_;

    for (;;) {

        if (current_index >= array_size(target_path)) {
            //// if map_has_key(res, transformed) = 0 
            ////     map_add(res, transformed, 0)
            //// end
            s = string_array_join(t, ""s);
            array_set_size(t, 0ll);
            return s;
        };
        to = target_path->at(current_index);

        if (from == to) {
            ////transform(d, target_path, to, index + 1, transformed + "A", res)
            from = to;
            current_index = current_index + 1ll;
            array_push(t, "A"s);
        }
        else {

            auto __expr0 = d->navis;
            for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
                shared_pointer<Navi> expr__it0 = (*__expr0)[expr__it0__idx];

                if (expr__it0->from == from && expr__it0->to == to) {
                    ////print("item.from = from and item.to", from, to, s)
                    ////transform(d, target_path, to, index + 1, transformed + item.path_str + "A", res)
                    from = to;
                    current_index = current_index + 1ll;
                    array_push(t, expr__it0->path_str + "A"s);
                    break;
                };
            }
        };
    }
}

void transform(shared_pointer<Data> d, shared_vector<i64> target_path, i64 from, i64 target_index, std::string transformed, shared_vector<std::string> res) {
    i64 to;
    i64 c;

    if (target_index >= array_size(target_path)) {
        array_push(res, transformed);
        return;
    };
    to = target_path->at(target_index);

    if (from == to) {
        transform(d, target_path, to, target_index + 1ll, transformed + "A"s, res);
    }
    else {
        c = 0ll;

        auto __expr0 = d->navis;
        for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
            shared_pointer<Navi> expr__it0 = (*__expr0)[expr__it0__idx];

            if (expr__it0->from == from && expr__it0->to == to) {
                ////if c > 0 then print(item.path_str)
                ////print("item.from = from and item.to", from, to, s)
                transform(d, target_path, to, target_index + 1ll, transformed + expr__it0->path_str + "A"s, res);
                ////c = c + 1
            };
        }
    };
}

i64 process(shared_pointer<Data> d, shared_vector<std::string> s, i64 deep) {
    shared_vector<std::string> src;
    i64 iter;
    shared_vector<std::string> res;
    shared_vector<i64> p;
    i64 l;
    i64 min_len;
    src = s;

    auto __expr0 = create_range(1ll, deep);
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        iter = expr__it0;
        res = create_shared_vector<std::string>();

        auto __expr1 = src;
        for (i64 src__it1__idx = 0; src__it1__idx < (i64)__expr1->size(); src__it1__idx++) {
            std::string src__it1 = (*__expr1)[src__it1__idx];
            p = str_to_path(src__it1);
            l = array_size(res);
            transform(d, p, char_to_int("A"s), 0ll, ""s, res);
            array_set_size(p, 0ll);

            if (iter > 3ll) {
                printf("%s %lld\n", (src__it1).c_str(), (i64)(array_size(res)));
            };
        }
        src = res;
        printf("%lld\n", (i64)(array_size(src)));
    }
    min_len = 1000000ll;

    auto __expr2 = src;
    for (i64 src__it2__idx = 0; src__it2__idx < (i64)__expr2->size(); src__it2__idx++) {
        std::string src__it2 = (*__expr2)[src__it2__idx];
        l = string_size(src__it2);

        if (l < min_len) {
            min_len = l;
        };
    }
    return min_len;
}

shared_vector<i64> get_path(shared_pointer<Data> d, i64 from, i64 to) {
    std::string s;

    auto __expr0 = d->navis;
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        shared_pointer<Navi> expr__it0 = (*__expr0)[expr__it0__idx];

        if (expr__it0->from == from && expr__it0->to == to) {
            return expr__it0->path;
        };
    }
    s = "get_path from "s + int_to_char(from) + " to "s + int_to_char(to);
    error(s);
}

i64 test_single(shared_pointer<Data> d, shared_vector<i64> input_path, i64 level, i64 max_level, shared_vector<std::string> acc, shared_vector<i64> visited) {
    i64 from;
    i64 total;
    i64 to;
    i64 vi;
    i64 r;
    shared_vector<i64> next_path;
    from = char_to_int("A"s);
    total = 0ll;

    if (level == max_level) {
        total = array_size(input_path);

        if (level > 0ll) {
            total = total + 1ll;
        };
    }
    else {

        if (input_path == d->empty_path) {
            total = total + 1ll;
        }
        else {

            auto __expr0 = input_path;
            for (i64 input_path__it0__idx = 0; input_path__it0__idx < (i64)__expr0->size(); input_path__it0__idx++) {
                i64 input_path__it0 = (*__expr0)[input_path__it0__idx];
                to = input_path__it0;
                vi = level*10000ll + from*100ll + to;

                if (visited->at(vi) > 0ll) {
                    r = visited->at(vi);
                }
                else {

                    if (from == to) {
                        r = test_single(d, d->empty_path, level + 1ll, max_level, acc, visited);
                    }
                    else {
                        next_path = get_path(d, from, to);
                        r = test_single(d, next_path, level + 1ll, max_level, acc, visited);
                    };
                    visited->at(vi) = r;
                };
                total = total + r;
                from = to;
            }

            if (level > 0ll) {
                to = char_to_int("A"s);
                vi = level*10000ll + from*100ll + to;

                if (visited->at(vi) > 0ll) {
                    r = visited->at(vi);
                }
                else {

                    if (from == to) {
                        r = test_single(d, d->empty_path, level + 1ll, max_level, acc, visited);
                    }
                    else {
                        next_path = get_path(d, from, to);
                        r = test_single(d, next_path, level + 1ll, max_level, acc, visited);
                    };
                    visited->at(vi) = r;
                };
                total = total + r;
            };
        };
    };
    //// acc[level] = acc[level] + path_to_str(input_path)
    //// if level > 0 
    ////     acc[level] = acc[level] + "A"
    //// end
    return total;
}

i64 test_mult(shared_pointer<Data> d, shared_vector<i64> input_path, i64 level, i64 max_level, shared_vector<std::string> acc, shared_vector<i64> visited) {
    i64 from;
    i64 total;
    i64 to;
    i64 vi;
    i64 r;
    i64 test_r;
    from = char_to_int("A"s);
    total = 0ll;

    if (level == max_level) {
        total = array_size(input_path);

        if (level > 0ll) {
            total = total + 1ll;
        };
    }
    else {

        if (input_path == d->empty_path) {
            total = total + 1ll;
        }
        else {

            auto __expr0 = input_path;
            for (i64 input_path__it0__idx = 0; input_path__it0__idx < (i64)__expr0->size(); input_path__it0__idx++) {
                i64 input_path__it0 = (*__expr0)[input_path__it0__idx];
                to = input_path__it0;
                vi = level*10000ll + from*100ll + to;

                if (visited->at(vi) > 0ll) {
                    r = visited->at(vi);
                }
                else {

                    if (from == to) {
                        r = test_mult(d, d->empty_path, level + 1ll, max_level, acc, visited);
                    }
                    else {
                        r = 0ll;

                        auto __expr1 = d->navis;
                        for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
                            shared_pointer<Navi> expr__it1 = (*__expr1)[expr__it1__idx];

                            if (expr__it1->from == from && expr__it1->to == to) {
                                test_r = test_mult(d, expr__it1->path, level + 1ll, max_level, acc, visited);

                                if (r == 0ll || test_r < r) {
                                    r = test_r;
                                };
                            };
                        }
                    };
                    visited->at(vi) = r;
                };
                total = total + r;
                from = to;
            }

            if (level > 0ll) {
                to = char_to_int("A"s);
                vi = level*10000ll + from*100ll + to;

                if (visited->at(vi) > 0ll) {
                    r = visited->at(vi);
                }
                else {

                    if (from == to) {
                        r = test_mult(d, d->empty_path, level + 1ll, max_level, acc, visited);
                    }
                    else {
                        r = 0ll;

                        auto __expr2 = d->navis;
                        for (i64 expr__it2__idx = 0; expr__it2__idx < (i64)__expr2->size(); expr__it2__idx++) {
                            shared_pointer<Navi> expr__it2 = (*__expr2)[expr__it2__idx];

                            if (expr__it2->from == from && expr__it2->to == to) {
                                test_r = test_mult(d, expr__it2->path, level + 1ll, max_level, acc, visited);

                                if (r == 0ll || test_r < r) {
                                    r = test_r;
                                };
                            };
                        }
                    };
                    visited->at(vi) = r;
                };
                total = total + r;
            };
        };
    };
    //// acc[level] = acc[level] + path_to_str(input_path)
    //// if level > 0 
    ////     acc[level] = acc[level] + "A"
    //// end
    return total;
}

void run() {
    shared_vector<std::string> lines;
    shared_pointer<Data> d;
    i64 deep;
    i64 total;
    i64 t1;
    i64 t2;
    shared_vector<std::string> acc;
    i64 v;
    shared_vector<i64> visited;
    shared_vector<i64> visited_mult;
    i64 ts;
    i64 tm;
    ////lines = read_string_lines_from_file("D:/src/postal/examples/aoc2024/21/input.txt")
    lines = read_string_lines_from_file("./input.txt"s);
    d = create_shared_pointer<Data >();
    d->empty_path = create_shared_vector<i64>();
    d->navis = create_navis(get_paths_str_array());
    deep = 26ll;
    total = 0ll;
    //// for lines 
    ////     s = item
    ////     if s = "" then break
    ////     //print(s)
    ////     v = string_to_integer(string_remove(s, "A"))
    ////     arr = [string]
    ////     array_push(arr, s)
    ////     r = process(d, arr, deep)    
    ////     //print(v, r, v*r)
    ////     total = total + v*r
    //// end
    printf("%s\n", ("\nfast"s).c_str());
    t1 = 0ll;
    t2 = 0ll;
    acc = create_shared_vector<std::string>();

    auto __expr0 = lines;
    for (i64 lines__it0__idx = 0; lines__it0__idx < (i64)__expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];

        if (lines__it0 == ""s) {
            break;
        };
        v = string_to_integer(string_remove(lines__it0, "A"s));
        ////acc = [string]
        ////array_set_size(acc, deep + 1)
        visited = create_shared_vector<i64>();
        visited_mult = create_shared_vector<i64>();
        array_set_size(visited, 1000000ll);
        array_set_size(visited_mult, 1000000ll);
        ts = test_single(d, str_to_path(lines__it0), 0ll, deep, acc, visited);
        tm = test_mult(d, str_to_path(lines__it0), 0ll, deep, acc, visited_mult);
        ////for acc then print(item)
        printf("%s %s %lld %s %lld\n", (lines__it0).c_str(), ("single"s).c_str(), (i64)(ts), ("mult"s).c_str(), (i64)(tm));
        t1 = t1 + ts*v;
        t2 = t2 + tm*v;
    }
    printf("%s %lld\n", ("\ndone good"s).c_str(), (i64)(total));
    printf("%s %s %lld %s %lld\n", ("\ndone fast"s).c_str(), ("single"s).c_str(), (i64)(t1), ("mult"s).c_str(), (i64)(t2));
}

