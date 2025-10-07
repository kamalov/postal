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
i64 next(i64 number) {
    i64 two24;
    i64 n;
    i64 v;
    two24 = 1ll << 24ll;
    n = number;
    v = n << 6ll;
    n = n^v;
    n = n % two24;
    v = n >> 5ll;
    n = n^v;
    n = n % two24;
    v = n << 11ll;
    n = n^v;
    n = n % two24;
    return n;
}

std::string int_arr_to_str(shared_vector<i64> a) {
    std::string s;
    s = ""s;

    auto __expr0 = a;
    for (i64 a__it0__idx = 0; a__it0__idx < (i64)__expr0->size(); a__it0__idx++) {
        i64 a__it0 = (*__expr0)[a__it0__idx];

        if (s != ""s) {
            s = s + ","s;
        };
        s = s + integer_to_string(a__it0);
    }
    return s;
}

i64 get_number(shared_vector<i64> last4, i64 base) {
    i64 den;
    i64 v;

    if (array_size(last4) != 4ll) {
        error("get_number "s + integer_to_string(array_size(last4)));
    };
    den = 1ll;
    v = 0ll;

    auto __expr0 = last4;
    for (i64 last4__it0__idx = 0; last4__it0__idx < (i64)__expr0->size(); last4__it0__idx++) {
        i64 last4__it0 = (*__expr0)[last4__it0__idx];
        v = v + den*(last4__it0 + 9ll);
        den = den*base;
    }
    return v;
}

shared_vector<i64> int_to_arr(i64 n, i64 base) {
    shared_vector<i64> r;
    i64 v;
    i64 rem;
    r = create_shared_vector<i64>();
    v = n;

    for (;;) {
        rem = v % base;
        array_push(r, rem-9ll);
        v = v/base;

        if (v == 0ll) {
            break;
        };
    }
    return r;
}

void run() {
    shared_vector<std::string> lines;
    i64 deep;
    i64 t;
    shared_vector<i64> acc;
    i64 base;
    shared_vector<i64> blocked;
    std::string line;
    i64 v;
    i64 prev;
    shared_vector<i64> last4;
    i64 rem;
    i64 diff;
    i64 n;
    i64 max_v;
    lines = read_string_lines_from_file("D:/src/postal/examples/aoc2024/22/input.txt"s);
    deep = 2000ll;
    t = 0ll;
    acc = create_shared_vector<i64>();
    base = 20ll;
    array_set_size(acc, base*base*base*base);
    blocked = create_shared_vector<i64>();
    array_set_size(blocked, base*base*base*base);

    auto __expr0 = lines;
    for (i64 lines__it0__idx = 0; lines__it0__idx < (i64)__expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        line = lines__it0;

        if (line == ""s) {
            break;
        };
        v = string_to_integer(line);
        prev = v % 10ll;
        printf("%lld\n", (i64)(v));

        auto __expr1 = create_range(0ll, array_size(blocked)-1ll);
        for (i64 expr__it1__idx = 0; expr__it1__idx < (i64)__expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            blocked->at(expr__it1) = 0ll;
        }
        last4 = create_shared_vector<i64>();

        auto __expr2 = create_range(1ll, deep);
        for (i64 expr__it2__idx = 0; expr__it2__idx < (i64)__expr2->size(); expr__it2__idx++) {
            i64 expr__it2 = (*__expr2)[expr__it2__idx];
            v = next(v);
            rem = v % 10ll;
            diff = rem-prev;
            prev = rem;
            array_push(last4, diff);

            if (array_size(last4) > 4ll) {
                array_pop_front(last4);
            };

            if (array_size(last4) == 4ll) {
                ////log(line, rem, int_arr_to_str(last4))
                n = get_number(last4, base);
                ////log(line, rem, int_arr_to_str(last4), "|", int_arr_to_str(int_to_arr(n, base)))

                if (blocked->at(n)) {
                    continue;
                };
                acc->at(n) = acc->at(n) + rem;
                blocked->at(n) = 1ll;
            };
            ////log(v, rem, "(", diff,")")
        }
    }
    max_v = 0ll;

    auto __expr3 = acc;
    for (i64 acc__it3__idx = 0; acc__it3__idx < (i64)__expr3->size(); acc__it3__idx++) {
        i64 acc__it3 = (*__expr3)[acc__it3__idx];

        if (acc__it3 > max_v) {
            max_v = acc__it3;
        };
    }

    auto __expr4 = acc;
    for (i64 acc__it4__idx = 0; acc__it4__idx < (i64)__expr4->size(); acc__it4__idx++) {
        i64 acc__it4 = (*__expr4)[acc__it4__idx];

        if (acc__it4 == max_v) {
            printf("%lld %s %s\n", (i64)(acc__it4), ("max seq"s).c_str(), (int_arr_to_str(int_to_arr(acc__it4__idx, base))).c_str());
        };
    }
    printf("%s %lld\n", ("\ndone"s).c_str(), (i64)(max_v));
}

