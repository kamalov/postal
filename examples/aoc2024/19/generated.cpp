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
struct Part {
    shared_vector<i64> vals;
};


struct Data {
    shared_vector<shared_pointer<Part>> parts;
};


shared_vector<i64> str_to_int_array(std::string s) {
    shared_vector<i64> r;
    shared_vector<std::string> chars;
    i64 i;
    r = create_shared_vector<i64>();
    chars = string_to_chars("bgruw"s);

    auto __expr0 = string_to_chars(s);
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        std::string expr__it0 = (*__expr0)[expr__it0__idx];
        i = array_index_of(chars, expr__it0);
        array_push(r, i);
    }
    return r;
}

std::string int_array_to_str(shared_vector<i64> p) {
    shared_vector<std::string> chars;
    std::string s;
    chars = string_to_chars("bgruw"s);
    s = ""s;

    auto __expr0 = p;
    for (i64 p__it0__idx = 0; p__it0__idx < (i64)__expr0->size(); p__it0__idx++) {
        i64 p__it0 = (*__expr0)[p__it0__idx];
        s = s + chars->at(p__it0);
    }
    return s;
}

shared_vector<shared_pointer<Part>> parse_parts(std::string line) {
    std::string s;
    shared_vector<std::string> a;
    shared_vector<shared_pointer<Part>> parts;
    shared_pointer<Part> p;
    s = string_remove(line, " "s);
    a = string_split(s, ","s);
    array_quick_sort(a);
    parts = create_shared_vector<shared_pointer<Part>>();

    auto __expr0 = a;
    for (i64 a__it0__idx = 0; a__it0__idx < (i64)__expr0->size(); a__it0__idx++) {
        std::string a__it0 = (*__expr0)[a__it0__idx];
        p = create_shared_pointer<Part >();
        p->vals = str_to_int_array(a__it0);
        array_push(parts, p);
    }
    return parts;
}

i64 matches(shared_vector<i64> target, i64 target_index, shared_vector<i64> v) {
    i64 i;

    auto __expr0 = v;
    for (i64 v__it0__idx = 0; v__it0__idx < (i64)__expr0->size(); v__it0__idx++) {
        i64 v__it0 = (*__expr0)[v__it0__idx];
        i = target_index + v__it0__idx;

        if (i >= array_size(target)) {
            return 0ll;
        };

        if (v__it0 != target->at(i)) {
            return 0ll;
        };
    }
    return 1ll;
}

struct Counter {
    i64 count;
    shared_vector<std::string> lines;
    shared_vector<i64> visited;
    friend bool operator==(const Counter& l, const Counter& r) {
        return (l.count == r.count);
    }
};

namespace std {
    template<>
    struct hash<Counter> {
        std::size_t operator()(const Counter& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.count) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

i64 check(shared_pointer<Data> d, shared_vector<i64> target, i64 target_index, shared_pointer<Counter> counter, std::string s) {
    i64 r;
    shared_vector<i64> p;
    i64 i;
    i64 v;
    std::string ns;
    r = 0ll;

    if (target_index >= array_size(target)) {
        counter->count = counter->count + 1ll;
        ////print("found", s)
        return 1ll;
    };

    auto __expr0 = d->parts;
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        shared_pointer<Part> expr__it0 = (*__expr0)[expr__it0__idx];
        p = expr__it0->vals;

        if (matches(target, target_index, p)) {
            ////print("matches", target_index, int_array_to_str(p))
            i = target_index*1000ll + expr__it0__idx;
            v = counter->visited->at(i);

            if (v == 0ll - 1ll) {
                continue;
            };
            ns = s + ", "s + int_array_to_str(p);

            if (v > 0ll) {
                r = r + v;
            }
            else {
                v = check(d, target, target_index + array_size(p), counter, ns);

                if (v > 0ll) {
                    counter->visited->at(i) = v;
                    r = r + v;
                }
                else {
                    counter->visited->at(i) = 0ll - 1ll;
                };
            };
        };
    }
    return r;
}

void run() {
    shared_vector<std::string> lines;
    shared_pointer<Data> d;
    i64 i;
    i64 total;
    shared_vector<i64> target;
    shared_pointer<Counter> c;
    i64 r;
    lines = read_string_lines_from_file("./input.txt"s);
    d = create_shared_pointer<Data >();
    d->parts = parse_parts(lines->at(0ll));

    auto __expr0 = d->parts;
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        shared_pointer<Part> expr__it0 = (*__expr0)[expr__it0__idx];
        printf("%s\n", (int_array_to_str(expr__it0->vals)).c_str());
    }
    printf("\n");
    i = array_index_of(lines, ""s);
    lines = array_slice(lines, i + 1ll, array_size(lines));
    total = 0ll;

    auto __expr1 = lines;
    for (i64 lines__it1__idx = 0; lines__it1__idx < (i64)__expr1->size(); lines__it1__idx++) {
        std::string lines__it1 = (*__expr1)[lines__it1__idx];
        target = str_to_int_array(lines__it1);
        printf("%s\n", (lines__it1).c_str());
        c = create_shared_pointer<Counter >();
        c->lines = create_shared_vector<std::string>();
        c->visited = create_shared_vector<i64>();
        array_set_size(c->visited, 1000000ll);
        r = check(d, target, 0ll, c, ""s);
        printf("%lld\n", (i64)(r));
        ////total = total + c.count
        total = total + r;
        ////for c.visited then print(item)
    }
    printf("%s %lld\n", ("total"s).c_str(), (i64)(total));
}

