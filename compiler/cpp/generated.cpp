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
//// user type
struct FooType {
    i64 i;
    double r;
    std::string s;
    bool b;
    shared_vector<i64> integer_array;
    shared_vector<shared_pointer<FooType>> foo_array;
    shared_map<std::string, i64> string_to_integer_map;
    shared_map<shared_pointer<FooType>, shared_pointer<FooType>> foo_to_string_map;
    friend bool operator==(const FooType& l, const FooType& r) {
        return (l.i == r.i) && (l.r == r.r) && (l.s == r.s) && (l.b == r.b);
    }
};

namespace std {
    template<>
    struct hash<FooType> {
        std::size_t operator()(const FooType& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.i) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<double>{}(rec.r) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<std::string>{}(rec.s) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<bool>{}(rec.b) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

//// example function
bool example_function(i64 p1, std::string p2) {
    i64 i;
    double r;
    std::string s;
    bool b;
    i64 v;
    i64 range_item;
    i64 range_index;
    shared_vector<i64> integer_array;
    shared_pointer<FooType> f1;
    shared_pointer<FooType> f2;
    shared_map<std::string, shared_pointer<FooType>> some_map;
    shared_pointer<FooType> f;
    i = 1000ll;
    r = 1000.1;
    s = "some"s + " "s + "value"s;
    b = true || false;
    //// if/else

    if ((i == 1000ll || i > 1000ll || r < 1000.1) && (b || !r)) {
        v = 1ll;
    }
    else if (s == "some"s) {
        //// all variables are in function scope (like old C or JavaScript)
        //// no local scopes (yet)
        v = 2ll;
    }
    else {
        v = 3ll;
    };
    //// single line condition

    if (i > 1000ll) {
        i = i + 1ll;
    };
    //// endless loop

    for (;;) {
        i = i + 1ll;

        if (i < 2000ll) {
            continue;
        };
        //// continue cycle

        if (i > 2000ll) {
            break;
        };
        //// break loop
    }
    //// iterate over range (both sides included)

    auto __expr0 = create_range(10ll, 15ll);
    for (i64 expr__it0__idx = 0; expr__it0__idx < (i64)__expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        range_item = expr__it0;
        range_index = expr__it0__idx;
        printf("%s %lld %lld\n", ("iterating over range"s).c_str(), (i64)(expr__it0__idx), (i64)(expr__it0));
    }
    //// array 
    integer_array = create_shared_vector<i64>();
    //// adding into array (std functions declarations are inside 'prelude.post' file)
    array_push(integer_array, 5ll);
    array_push(integer_array, 6ll);
    array_push(integer_array, 7ll);
    integer_array->at(0ll) = 1ll;
    //// iterating over array

    auto __expr1 = integer_array;
    for (i64 integer_array__it1__idx = 0; integer_array__it1__idx < (i64)__expr1->size(); integer_array__it1__idx++) {
        i64 integer_array__it1 = (*__expr1)[integer_array__it1__idx];
        printf("%s %lld %lld\n", ("iterating over integer array"s).c_str(), (i64)(integer_array__it1__idx), (i64)(integer_array__it1));
    }
    //// creating FooType objects
    f1 = create_shared_pointer<FooType >();
    f1->i = 1ll;
    f1->s = "one"s;
    f2 = create_shared_pointer<FooType >();
    f2->r = 2.0;
    f2->s = "two"s;
    //// string to FooType map
    some_map = create_shared_map<std::string, shared_pointer<FooType>>();
    map_add(some_map, "one"s, f1);
    map_add(some_map, "two"s, f2);
    //// iterating over map

    auto __expr2 = map_keys(some_map);
    for (i64 expr__it2__idx = 0; expr__it2__idx < (i64)__expr2->size(); expr__it2__idx++) {
        std::string expr__it2 = (*__expr2)[expr__it2__idx];
        f = map_get_value(some_map, expr__it2);
        printf("%s %s %s\n", ("iterating over map"s).c_str(), (expr__it2).c_str(), (f->s).c_str());
    }
    //// return 
    return true;
}

//// entry point
void run() {
    bool b;
    b = example_function(1ll, "2"s);
    printf("%s\n", ("Hello, World!"s).c_str());
}

//// more examples in 'examples' and 'test' folders
